use std::env;
use std::fs;
use std::path::Path;

const RELEASE_NAME: &str = "DoxRain Cherokee";
const ENGINE_VERSION: &str = "0.1.0";

fn print_help() {
    println!("DoxRain Cherokee");
    println!("Windows-first DoxRain interpreter");
    println!();
    println!("Usage:");
    println!("  doxrain <file.dox>       Run a DoxRain program");
    println!("  doxrain --check <file>   Check that a source file can be read");
    println!("  doxrain --version        Show the release identity");
    println!("  doxrain --help           Show this help");
}

fn interpolate(value: &str, variables: &[(String, String)]) -> String {
    let mut output = value.to_owned();
    for (name, replacement) in variables {
        output = output.replace(&format!("{{{name}}}"), replacement);
    }
    output
}

fn run_source(source: &str) -> Result<(), String> {
    let mut variables: Vec<(String, String)> = Vec::new();
    for (index, original) in source.lines().enumerate() {
        let line_number = index + 1;
        let line = original.trim();
        if line.is_empty() || line.starts_with('#') || line.starts_with("//") {
            continue;
        }
        if let Some(rest) = line.strip_prefix("let ") {
            let (name, value) = rest.split_once('=').ok_or_else(|| {
                format!("line {line_number}: expected `let name = value`")
            })?;
            let name = name.trim();
            if name.is_empty() || !name.chars().next().is_some_and(|c| c.is_ascii_alphabetic() || c == '_') {
                return Err(format!("line {line_number}: invalid variable name"));
            }
            let value = evaluate(value.trim(), &variables)
                .map_err(|message| format!("line {line_number}: {message}"))?;
            variables.retain(|(existing, _)| existing != name);
            variables.push((name.to_owned(), value));
        } else if let Some(rest) = line.strip_prefix("show ") {
            let value = evaluate(rest.trim(), &variables)
                .map_err(|message| format!("line {line_number}: {message}"))?;
            println!("{value}");
        } else if line.starts_with("pkg ") || line.starts_with("use ") || line.starts_with("fn ") || line == "}" || line.ends_with('{') || line.starts_with("if ") || line.starts_with("for ") {
            return Err(format!("line {line_number}: this Cherokee milestone does not yet execute blocks/functions; use `let` and `show` for now"));
        } else {
            return Err(format!("line {line_number}: unknown statement"));
        }
    }
    Ok(())
}

fn evaluate(text: &str, variables: &[(String, String)]) -> Result<String, String> {
    let text = interpolate(text, variables);
    let text = text.trim();
    if text.len() >= 2 && text.starts_with('"') && text.ends_with('"') {
        return Ok(text[1..text.len() - 1].to_owned());
    }
    if let Some((left, operator, right)) = split_operation(text) {
        let a: f64 = left.trim().parse().map_err(|_| "left side is not a number".to_owned())?;
        let b: f64 = right.trim().parse().map_err(|_| "right side is not a number".to_owned())?;
        let result = match operator {
            '+' => a + b,
            '-' => a - b,
            '*' => a * b,
            '/' if b != 0.0 => a / b,
            '/' => return Err("division by zero".to_owned()),
            _ => unreachable!(),
        };
        return Ok(if result.fract() == 0.0 { (result as i64).to_string() } else { result.to_string() });
    }
    if text.parse::<f64>().is_ok() || text == "true" || text == "false" {
        return Ok(text.to_owned());
    }
    variables.iter().find(|(name, _)| name == text)
        .map(|(_, value)| value.clone())
        .ok_or_else(|| format!("unknown value `{text}`"))
}

fn split_operation(text: &str) -> Option<(&str, char, &str)> {
    for operator in ['+', '-', '*', '/'] {
        if let Some(position) = text.find(operator) {
            if position > 0 && position + 1 < text.len() {
                return Some((&text[..position], operator, &text[position + 1..]));
            }
        }
    }
    None
}

fn main() {
    let arguments: Vec<String> = env::args().skip(1).collect();
    match arguments.as_slice() {
        [] | [flag] if flag == "--help" || flag == "-h" => print_help(),
        [flag] if flag == "--version" || flag == "-V" => println!("{RELEASE_NAME} (engine {ENGINE_VERSION})"),
        [file] => execute_file(file, false),
        [flag, file] if flag == "--check" => execute_file(file, true),
        _ => {
            eprintln!("Invalid arguments. Use `doxrain --help`.");
            std::process::exit(2);
        }
    }
}

fn execute_file(file: &str, check_only: bool) {
    if !Path::new(file).is_file() {
        eprintln!("DoxRain: file not found: {file}");
        std::process::exit(1);
    }
    let source = match fs::read_to_string(file) {
        Ok(source) => source,
        Err(error) => {
            eprintln!("DoxRain: cannot read {file}: {error}");
            std::process::exit(1);
        }
    };
    match if check_only { check_source(&source) } else { run_source(&source) } {
        Ok(()) if check_only => println!("OK: {file}"),
        Ok(()) => {},
        Err(error) => {
            eprintln!("DoxRain: {error}");
            std::process::exit(1);
        }
    }
}

fn check_source(source: &str) -> Result<(), String> {
    for (index, line) in source.lines().enumerate() {
        let line = line.trim();
        if line.is_empty() || line.starts_with('#') || line.starts_with("//") { continue; }
        if !(line.starts_with("let ") || line.starts_with("show ")) {
            return Err(format!("line {}: unsupported statement in Windows milestone", index + 1));
        }
    }
    Ok(())
}

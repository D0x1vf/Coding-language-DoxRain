use std::collections::{HashMap, HashSet};
use std::io::{self, Write};

#[derive(Clone, Debug)]
enum Value {
    Int(i64),
    Float(f64),
    Bool(bool),
    Str(String),
    Void,
}

impl Value {
    fn to_bool(&self) -> bool {
        match self {
            Value::Bool(b) => *b,
            Value::Int(i) => *i != 0,
            Value::Float(f) => *f != 0.0,
            Value::Str(s) => !s.is_empty(),
            Value::Void => false,
        }
    }

    fn to_number(&self) -> f64 {
        match self {
            Value::Int(i) => *i as f64,
            Value::Float(f) => *f,
            Value::Bool(b) => if *b { 1.0 } else { 0.0 },
            Value::Str(s) => s.trim().parse::<f64>().unwrap_or(0.0),
            Value::Void => 0.0,
        }
    }

    fn to_string(&self) -> String {
        match self {
            Value::Int(i) => i.to_string(),
            Value::Float(f) => f.to_string(),
            Value::Bool(b) => b.to_string(),
            Value::Str(s) => s.clone(),
            Value::Void => String::new(),
        }
    }
}

#[derive(Clone, Debug)]
enum Expr {
    Literal(Value),
    Var(String),
    Binary(Box<Expr>, String, Box<Expr>),
    Call(String, Vec<Expr>),
}

#[derive(Clone, Debug)]
enum Stmt {
    Let { name: String, expr: Expr },
    Show(Expr),
    UseLib(String),
    If { cond: Expr, body: Vec<Stmt> },
    For { var: String, start: Expr, end: Expr, body: Vec<Stmt> },
    Fn { name: String, params: Vec<String>, body: Vec<Stmt> },
    ExprStmt(Expr),
    PkgInstall(String),
    PkgRemove(String),
    PkgList,
}

#[derive(Clone, Debug)]
enum Token {
    Ident(String),
    Number(String),
    StringLit(String),
    Symbol(String),
    Keyword(String),
}

fn is_ident_start(c: char) -> bool {
    c.is_ascii_alphabetic() || c == '_'
}

fn is_ident_char(c: char) -> bool {
    c.is_ascii_alphanumeric() || c == '_'
}

fn lex(src: &str) -> Vec<Token> {
    let mut tokens = Vec::new();
    let mut chars = src.chars().peekable();

    while let Some(c) = chars.next() {
        if c.is_whitespace() {
            continue;
        } else if c == '"' {
            let mut s = String::new();
            while let Some(&nc) = chars.peek() {
                chars.next();
                if nc == '"' {
                    break;
                }
                s.push(nc);
            }
            tokens.push(Token::StringLit(s));
        } else if is_ident_start(c) {
            let mut ident = String::new();
            ident.push(c);
            while let Some(&nc) = chars.peek() {
                if is_ident_char(nc) {
                    chars.next();
                    ident.push(nc);
                } else {
                    break;
                }
            }
            let kw = match ident.as_str() {
                "let" | "show" | "use" | "if" | "for" | "in" | "fn" | "pkg" => {
                    Some(Token::Keyword(ident.clone()))
                }
                _ => None,
            };
            if let Some(k) = kw {
                tokens.push(k);
            } else {
                tokens.push(Token::Ident(ident));
            }
        } else if c.is_ascii_digit() {
            let mut num = String::new();
            num.push(c);
            let mut has_dot = false;
            while let Some(&nc) = chars.peek() {
                if nc.is_ascii_digit() {
                    chars.next();
                    num.push(nc);
                } else if nc == '.' && !has_dot {
                    has_dot = true;
                    chars.next();
                    num.push(nc);
                } else {
                    break;
                }
            }
            tokens.push(Token::Number(num));
        } else {
            let mut sym = c.to_string();
            if let Some(&nc) = chars.peek() {
                let two = format!("{}{}", c, nc);
                if ["==", "!=", ">=", "<=", ".."].contains(&two.as_str()) {
                    sym = two;
                    chars.next();
                }
            }
            tokens.push(Token::Symbol(sym));
        }
    }

    tokens
}

struct Parser {
    tokens: Vec<Token>,
    pos: usize,
}

impl Parser {
    fn new(tokens: Vec<Token>) -> Self {
        Self { tokens, pos: 0 }
    }

    fn peek(&self) -> Option<&Token> {
        self.tokens.get(self.pos)
    }

    fn next(&mut self) -> Option<Token> {
        if self.pos < self.tokens.len() {
            let t = self.tokens[self.pos].clone();
            self.pos += 1;
            Some(t)
        } else {
            None
        }
    }

    fn match_symbol(&mut self, s: &str) -> bool {
        if let Some(Token::Symbol(sym)) = self.peek() {
            if sym == s {
                self.next();
                return true;
            }
        }
        false
    }

    fn match_keyword(&mut self, k: &str) -> bool {
        if let Some(Token::Keyword(kw)) = self.peek() {
            if kw == k {
                self.next();
                return true;
            }
        }
        false
    }

    fn parse_program(&mut self) -> Vec<Stmt> {
        let mut stmts = Vec::new();
        while self.pos < self.tokens.len() {
            if let Some(stmt) = self.parse_stmt() {
                stmts.push(stmt);
            } else {
                break;
            }
        }
        stmts
    }

    fn parse_block(&mut self) -> Vec<Stmt> {
        let mut stmts = Vec::new();
        if !self.match_symbol("{") {
            return stmts;
        }
        while self.pos < self.tokens.len() {
            if self.match_symbol("}") {
                break;
            }
            if let Some(stmt) = self.parse_stmt() {
                stmts.push(stmt);
            } else {
                break;
            }
        }
        stmts
    }

    fn parse_stmt(&mut self) -> Option<Stmt> {
        if self.match_keyword("let") {
            self.parse_let()
        } else if self.match_keyword("show") {
            Some(Stmt::Show(self.parse_expr()))
        } else if self.match_keyword("use") {
            self.parse_use()
        } else if self.match_keyword("if") {
            self.parse_if()
        } else if self.match_keyword("for") {
            self.parse_for()
        } else if self.match_keyword("fn") {
            self.parse_fn()
        } else if self.match_keyword("pkg") {
            self.parse_pkg()
        } else {
            Some(Stmt::ExprStmt(self.parse_expr()))
        }
    }

    fn parse_let(&mut self) -> Option<Stmt> {
        let name = match self.next()? {
            Token::Ident(n) => n,
            _ => return None,
        };
        if !self.match_symbol("=") {
            return None;
        }
        let expr = self.parse_expr();
        Some(Stmt::Let { name, expr })
    }

    fn parse_use(&mut self) -> Option<Stmt> {
        let name = match self.next()? {
            Token::Ident(n) => n,
            _ => return None,
        };
        Some(Stmt::UseLib(name))
    }

    fn parse_if(&mut self) -> Option<Stmt> {
        let cond = self.parse_expr();
        let body = self.parse_block();
        Some(Stmt::If { cond, body })
    }

    fn parse_for(&mut self) -> Option<Stmt> {
        let var = match self.next()? {
            Token::Ident(n) => n,
            _ => return None,
        };
        if !self.match_keyword("in") {
            return None;
        }
        let start = self.parse_expr();
        if !self.match_symbol("..") {
            return None;
        }
        let end = self.parse_expr();
        let body = self.parse_block();
        Some(Stmt::For { var, start, end, body })
    }

    fn parse_fn(&mut self) -> Option<Stmt> {
        let name = match self.next()? {
            Token::Ident(n) => n,
            _ => return None,
        };
        if !self.match_symbol("(") {
            return None;
        }
        let mut params = Vec::new();
        loop {
            if self.match_symbol(")") {
                break;
            }
            if let Some(Token::Ident(p)) = self.next() {
                params.push(p);
                if self.match_symbol(")") {
                    break;
                }
                let _ = self.match_symbol(",");
            } else {
                break;
            }
        }
        let body = self.parse_block();
        Some(Stmt::Fn { name, params, body })
    }

    fn parse_pkg(&mut self) -> Option<Stmt> {
        let sub = match self.next()? {
            Token::Ident(s) => s,
            _ => return None,
        };
        match sub.as_str() {
            "list" => Some(Stmt::PkgList),
            "install" => {
                let name = match self.next()? {
                    Token::Ident(n) => n,
                    _ => return None,
                };
                Some(Stmt::PkgInstall(name))
            }
            "remove" => {
                let name = match self.next()? {
                    Token::Ident(n) => n,
                    _ => return None,
                };
                Some(Stmt::PkgRemove(name))
            }
            _ => None,
        }
    }

    fn parse_expr(&mut self) -> Expr {
        self.parse_equality()
    }

    fn parse_equality(&mut self) -> Expr {
        let mut expr = self.parse_comparison();
        while let Some(Token::Symbol(op)) = self.peek() {
            if op == "==" || op == "!=" {
                let op = if let Token::Symbol(s) = self.next().unwrap() { s } else { unreachable!() };
                let right = self.parse_comparison();
                expr = Expr::Binary(Box::new(expr), op, Box::new(right));
            } else {
                break;
            }
        }
        expr
    }

    fn parse_comparison(&mut self) -> Expr {
        let mut expr = self.parse_term();
        while let Some(Token::Symbol(op)) = self.peek() {
            if ["<", ">", "<=", ">="].contains(&op.as_str()) {
                let op = if let Token::Symbol(s) = self.next().unwrap() { s } else { unreachable!() };
                let right = self.parse_term();
                expr = Expr::Binary(Box::new(expr), op, Box::new(right));
            } else {
                break;
            }
        }
        expr
    }

    fn parse_term(&mut self) -> Expr {
        let mut expr = self.parse_factor();
        while let Some(Token::Symbol(op)) = self.peek() {
            if op == "+" || op == "-" {
                let op = if let Token::Symbol(s) = self.next().unwrap() { s } else { unreachable!() };
                let right = self.parse_factor();
                expr = Expr::Binary(Box::new(expr), op, Box::new(right));
            } else {
                break;
            }
        }
        expr
    }

    fn parse_factor(&mut self) -> Expr {
        let mut expr = self.parse_unary();
        while let Some(Token::Symbol(op)) = self.peek() {
            if op == "*" || op == "/" {
                let op = if let Token::Symbol(s) = self.next().unwrap() { s } else { unreachable!() };
                let right = self.parse_unary();
                expr = Expr::Binary(Box::new(expr), op, Box::new(right));
            } else {
                break;
            }
        }
        expr
    }

    fn parse_unary(&mut self) -> Expr {
        if let Some(Token::Symbol(op)) = self.peek() {
            if op == "-" {
                let _ = self.next();
                let right = self.parse_unary();
                return Expr::Binary(
                    Box::new(Expr::Literal(Value::Int(0))),
                    "-".to_string(),
                    Box::new(right),
                );
            }
        }
        self.parse_primary()
    }

    fn parse_primary(&mut self) -> Expr {
        match self.next() {
            Some(Token::Number(n)) => {
                if let Ok(i) = n.parse::<i64>() {
                    Expr::Literal(Value::Int(i))
                } else if let Ok(f) = n.parse::<f64>() {
                    Expr::Literal(Value::Float(f))
                } else {
                    Expr::Literal(Value::Str(n))
                }
            }
            Some(Token::StringLit(s)) => Expr::Literal(Value::Str(s)),
            Some(Token::Ident(name)) => {
                if self.match_symbol("(") {
                    let mut args = Vec::new();
                    if !self.match_symbol(")") {
                        loop {
                            let arg = self.parse_expr();
                            args.push(arg);
                            if self.match_symbol(")") {
                                break;
                            }
                            let _ = self.match_symbol(",");
                        }
                    }
                    Expr::Call(name, args)
                } else {
                    Expr::Var(name)
                }
            }
            Some(Token::Symbol(sym)) if sym == "(".to_string() => {
                let expr = self.parse_expr();
                let _ = self.match_symbol(")");
                expr
            }
            _ => Expr::Literal(Value::Void),
        }
    }
}

#[derive(Clone)]
struct Function {
    params: Vec<String>,
    body: Vec<Stmt>,
}

#[derive(Clone)]
struct Package {
    name: String,
    version: String,
    description: String,
}

struct Env {
    scopes: Vec<HashMap<String, Value>>,
    funcs: HashMap<String, Function>,
    available_pkgs: HashMap<String, Package>,
    installed_pkgs: HashSet<String>,
    loaded_libs: HashSet<String>,
}

impl Env {
    fn new() -> Self {
        let mut global = HashMap::new();
        global.insert("PI".into(), Value::Float(3.14159));
        global.insert("E".into(), Value::Float(2.71828));
        global.insert("VERSION".into(), Value::Str("4.0.0-rs".into()));

        let mut available_pkgs = HashMap::new();
        available_pkgs.insert(
            "discord".into(),
            Package {
                name: "discord".into(),
                version: "0.1.0".into(),
                description: "Discord bot utilities".into(),
            },
        );
        available_pkgs.insert(
            "http".into(),
            Package {
                name: "http".into(),
                version: "0.1.0".into(),
                description: "HTTP client utilities".into(),
            },
        );

        Self {
            scopes: vec![global],
            funcs: HashMap::new(),
            available_pkgs,
            installed_pkgs: HashSet::new(),
            loaded_libs: HashSet::new(),
        }
    }

    fn push_scope(&mut self) {
        self.scopes.push(HashMap::new());
    }

    fn pop_scope(&mut self) {
        self.scopes.pop();
    }

    fn set_var(&mut self, name: &str, val: Value) {
        if let Some(scope) = self.scopes.last_mut() {
            scope.insert(name.to_string(), val);
        }
    }

    fn get_var(&self, name: &str) -> Option<Value> {
        for scope in self.scopes.iter().rev() {
            if let Some(v) = scope.get(name) {
                return Some(v.clone());
            }
        }
        None
    }

    fn define_fn(&mut self, name: &str, func: Function) {
        self.funcs.insert(name.to_string(), func);
    }

    fn get_fn(&self, name: &str) -> Option<Function> {
        self.funcs.get(name).cloned()
    }

    fn install_pkg(&mut self, name: &str) {
        if self.installed_pkgs.contains(name) {
            println!("[pkg] {} already installed", name);
            return;
        }
        if let Some(pkg) = self.available_pkgs.get(name) {
            println!(
                "[pkg] installing {} v{} - {}",
                pkg.name, pkg.version, pkg.description
            );
            self.installed_pkgs.insert(name.to_string());
        } else {
            println!("[pkg] unknown package: {}", name);
        }
    }

    fn remove_pkg(&mut self, name: &str) {
        if self.installed_pkgs.remove(name) {
            println!("[pkg] removed {}", name);
            self.loaded_libs.remove(name);
        } else {
            println!("[pkg] {} is not installed", name);
        }
    }

    fn list_pkgs(&self) {
        println!("[pkg] available packages:");
        for (name, pkg) in &self.available_pkgs {
            let installed = if self.installed_pkgs.contains(name) {
                "installed"
            } else {
                "not installed"
            };
            println!(
                "  - {} v{} ({}) [{}]",
                pkg.name, pkg.version, pkg.description, installed
            );
        }
    }

    fn load_lib(&mut self, name: &str) {
        if !self.installed_pkgs.contains(name) {
            println!("[lib] {} not installed, installing now...", name);
            self.install_pkg(name);
        }
        if self.installed_pkgs.contains(name) {
            if self.loaded_libs.contains(name) {
                println!("[lib] {} already loaded", name);
            } else {
                println!("[lib] loading {}...", name);
                self.loaded_libs.insert(name.to_string());
            }
        }
    }
}

fn eval_expr(env: &mut Env, expr: &Expr) -> Value {
    match expr {
        Expr::Literal(v) => v.clone(),
        Expr::Var(name) => env.get_var(name).unwrap_or(Value::Void),
        Expr::Binary(left, op, right) => {
            let l = eval_expr(env, left);
            let r = eval_expr(env, right);
            match op.as_str() {
                "+" => Value::Float(l.to_number() + r.to_number()),
                "-" => Value::Float(l.to_number() - r.to_number()),
                "*" => Value::Float(l.to_number() * r.to_number()),
                "/" => {
                    let rv = r.to_number();
                    if rv == 0.0 {
                        println!("[error] division by zero");
                        Value::Void
                    } else {
                        Value::Float(l.to_number() / rv)
                    }
                }
                "==" => Value::Bool(l.to_string() == r.to_string()),
                "!=" => Value::Bool(l.to_string() != r.to_string()),
                ">" => Value::Bool(l.to_number() > r.to_number()),
                "<" => Value::Bool(l.to_number() < r.to_number()),
                ">=" => Value::Bool(l.to_number() >= r.to_number()),
                "<=" => Value::Bool(l.to_number() <= r.to_number()),
                _ => Value::Void,
            }
        }
        Expr::Call(name, args) => {
            if name == "input" {
                let prompt = if !args.is_empty() {
                    eval_expr(env, &args[0]).to_string()
                } else {
                    String::new()
                };
                print!("{}", prompt);
                io::stdout().flush().ok();
                let mut buf = String::new();
                io::stdin().read_line(&mut buf).ok();
                Value::Str(buf.trim_end().to_string())
            } else {
                if let Some(func) = env.get_fn(name) {
                    if func.params.len() != args.len() {
                        println!("[error] wrong arg count for {}", name);
                        return Value::Void;
                    }
                    env.push_scope();
                    for (p, a) in func.params.iter().zip(args.iter()) {
                        let v = eval_expr(env, a);
                        env.set_var(p, v);
                    }
                    for stmt in &func.body {
                        exec_stmt(env, stmt);
                    }
                    env.pop_scope();
                    Value::Void
                } else {
                    println!("[error] unknown function {}", name);
                    Value::Void
                }
            }
        }
    }
}

fn interpolate(env: &Env, s: &str) -> String {
    let mut result = String::new();
    let mut chars = s.chars().peekable();
    while let Some(c) = chars.next() {
        if c == '{' {
            let mut name = String::new();
            while let Some(&nc) = chars.peek() {
                chars.next();
                if nc == '}' {
                    break;
                }
                name.push(nc);
            }
            if let Some(v) = env.get_var(&name) {
                result.push_str(&v.to_string());
            } else {
                result.push_str(&format!("{{{}}}", name));
            }
        } else {
            result.push(c);
        }
    }
    result
}

fn exec_block(env: &mut Env, body: &[Stmt]) {
    env.push_scope();
    for stmt in body {
        exec_stmt(env, stmt);
    }
    env.pop_scope();
}

fn exec_stmt(env: &mut Env, stmt: &Stmt) {
    match stmt {
        Stmt::Let { name, expr } => {
            let v = eval_expr(env, expr);
            env.set_var(name, v);
        }
        Stmt::Show(expr) => {
            let v = eval_expr(env, expr);
            let s = v.to_string();
            let out = interpolate(env, &s);
            println!("{}", out);
        }
        Stmt::UseLib(name) => {
            env.load_lib(name);
        }
        Stmt::If { cond, body } => {
            let c = eval_expr(env, cond);
            if c.to_bool() {
                exec_block(env, body);
            }
        }
        Stmt::For { var, start, end, body } => {
            let s = eval_expr(env, start).to_number() as i64;
            let e = eval_expr(env, end).to_number() as i64;
            for i in s..e {
                env.push_scope();
                env.set_var(var, Value::Int(i));
                for stmt in body {
                    exec_stmt(env, stmt);
                }
                env.pop_scope();
            }
        }
        Stmt::Fn { name, params, body } => {
            let func = Function {
                params: params.clone(),
                body: body.clone(),
            };
            env.define_fn(name, func);
        }
        Stmt::ExprStmt(expr) => {
            let _ = eval_expr(env, expr);
        }
        Stmt::PkgInstall(name) => {
            env.install_pkg(name);
        }
        Stmt::PkgRemove(name) => {
            env.remove_pkg(name);
        }
        Stmt::PkgList => {
            env.list_pkgs();
        }
    }
}

fn main() {
    let src = r#"
pkg list

pkg install discord
use discord

fn greet(name) {
    show "Hello, {name}"
}

show "Welcome to DoxRain-RS {VERSION}"

let x = 10
let y = 5
let result = x * y + 2
show "Result = {result}"

if result > 20 {
    show "Big result!"
}

for i in 0..3 {
    show "Loop {i}"
}

let username = input("Enter your name: ")
greet(username)

pkg remove discord
pkg list
"#;

    let tokens = lex(src);
    let mut parser = Parser::new(tokens);
    let program = parser.parse_program();

    let mut env = Env::new();
    for stmt in &program {
        exec_stmt(&mut env, stmt);
    }
}

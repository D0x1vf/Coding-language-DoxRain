use std::collections::{HashMap, HashSet};
use std::hash::{Hash, Hasher};
use std::io::{self, Write};
use std::time::{SystemTime, UNIX_EPOCH};

#[derive(Clone, Debug)]
enum Value {
    Int(i64),
    Float(f64),
    Bool(bool),
    Str(String),
    List(Vec<Value>),
    Void,
}

impl Value {
    fn to_bool(&self) -> bool {
        match self {
            Value::Bool(b) => *b,
            Value::Int(i) => *i != 0,
            Value::Float(f) => *f != 0.0,
            Value::Str(s) => !s.is_empty(),
            Value::List(v) => !v.is_empty(),
            Value::Void => false,
        }
    }

    fn to_number(&self) -> f64 {
        match self {
            Value::Int(i) => *i as f64,
            Value::Float(f) => *f,
            Value::Bool(b) => if *b { 1.0 } else { 0.0 },
            Value::Str(s) => s.trim().parse::<f64>().unwrap_or(0.0),
            Value::List(_) => 0.0,
            Value::Void => 0.0,
        }
    }

    fn to_string(&self) -> String {
        match self {
            Value::Int(i) => i.to_string(),
            Value::Float(f) => {
                let txt = f.to_string();
                if txt.ends_with(".0") { txt.trim_end_matches(".0").to_string() } else { txt }
            }
            Value::Bool(b) => b.to_string(),
            Value::Str(s) => s.clone(),
            Value::List(items) => {
                let values: Vec<String> = items.iter().map(Value::to_string).collect();
                format!("[{}]", values.join(", "))
            }
            Value::Void => String::new(),
        }
    }
}

#[derive(Clone, Debug)]
enum Expr {
    Literal(Value),
    Var(String),
    Unary(String, Box<Expr>),
    Binary(Box<Expr>, String, Box<Expr>),
    Call(String, Vec<Expr>),
    List(Vec<Expr>),
}

#[derive(Clone, Debug)]
enum Stmt {
    Let { name: String, expr: Expr },
    Show(Expr),
    UseLib(String),
    If {
        cond: Expr,
        body: Vec<Stmt>,
        else_body: Vec<Stmt>,
    },
    For {
        var: String,
        start: Expr,
        end: Expr,
        step: Expr,
        body: Vec<Stmt>,
    },
    Fn { name: String, params: Vec<String>, body: Vec<Stmt> },
    Return(Expr),
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
                "let" | "show" | "use" | "if" | "else" | "for" | "in" | "fn" | "return" | "pkg" | "true" | "false" => {
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
                if ["==", "!=", ">=", "<=", "..", "+=", "-="].contains(&two.as_str()) {
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
        } else if self.match_keyword("return") {
            Some(Stmt::Return(self.parse_expr()))
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
        let else_body = if self.match_keyword("else") {
            self.parse_block()
        } else {
            Vec::new()
        };
        Some(Stmt::If { cond, body, else_body })
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
        let step = if self.match_symbol(";") {
            self.parse_expr()
        } else {
            Expr::Literal(Value::Int(1))
        };
        let body = self.parse_block();
        Some(Stmt::For { var, start, end, step, body })
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
            if op == "*" || op == "/" || op == "%" {
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
            if op == "-" || op == "!" {
                let op = if let Token::Symbol(s) = self.next().unwrap() { s } else { unreachable!() };
                let right = self.parse_unary();
                return Expr::Unary(op, Box::new(right));
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
            Some(Token::Keyword(k)) if k == "true" => Expr::Literal(Value::Bool(true)),
            Some(Token::Keyword(k)) if k == "false" => Expr::Literal(Value::Bool(false)),
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
                } else if self.match_symbol("[") {
                    let mut items = Vec::new();
                    if !self.match_symbol("]") {
                        loop {
                            let item = self.parse_expr();
                            items.push(item);
                            if self.match_symbol("]") {
                                break;
                            }
                            let _ = self.match_symbol(",");
                        }
                    }
                    Expr::List(items)
                } else {
                    Expr::Var(name)
                }
            }
            Some(Token::Symbol(sym)) if sym == "(" => {
                let expr = self.parse_expr();
                let _ = self.match_symbol(")");
                expr
            }
            Some(Token::Symbol(sym)) if sym == "[" => {
                let mut items = Vec::new();
                if !self.match_symbol("]") {
                    loop {
                        let item = self.parse_expr();
                        items.push(item);
                        if self.match_symbol("]") {
                            break;
                        }
                        let _ = self.match_symbol(",");
                    }
                }
                Expr::List(items)
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

#[derive(Clone, Debug)]
enum Instruction {
    Push(Value),
    Load(String),
    Store(String),
    Add,
    Sub,
    Mul,
    Div,
    Mod,
    Eq,
    Neq,
    Lt,
    Gt,
    Lte,
    Gte,
    Neg,
    Not,
    Print,
    Call(String, usize),
    NewList(usize),
    JumpIfFalse(usize),
    Jump(usize),
    Return,
}

struct Compiler {
    instructions: Vec<Instruction>,
}

impl Compiler {
    fn new() -> Self {
        Self { instructions: Vec::new() }
    }

    fn compile_program(&mut self, stmts: &[Stmt]) {
        for stmt in stmts {
            self.compile_stmt(stmt);
        }
    }

    fn emit(&mut self, instr: Instruction) {
        self.instructions.push(instr);
    }

    fn compile_stmt(&mut self, stmt: &Stmt) {
        match stmt {
            Stmt::Let { name, expr } => {
                self.compile_expr(expr);
                self.emit(Instruction::Store(name.clone()));
            }
            Stmt::Show(expr) => {
                self.compile_expr(expr);
                self.emit(Instruction::Print);
            }
            Stmt::ExprStmt(expr) => {
                self.compile_expr(expr);
            }
            Stmt::If { cond, body, else_body } => {
                self.compile_expr(cond);
                let jump_if_false = self.instructions.len();
                self.emit(Instruction::JumpIfFalse(0));
                self.compile_block(body);
                let mut jump_end = self.instructions.len();
                if !else_body.is_empty() {
                    jump_end = self.instructions.len();
                    self.emit(Instruction::Jump(0));
                }
                let false_start = self.instructions.len();
                if !else_body.is_empty() {
                    self.compile_block(else_body);
                    if let Some(Instruction::Jump(_)) = self.instructions.get_mut(jump_end) {
                        *self.instructions.get_mut(jump_end).unwrap() = Instruction::Jump(self.instructions.len() - jump_end);
                    }
                }
                if let Some(Instruction::JumpIfFalse(_)) = self.instructions.get_mut(jump_if_false) {
                    *self.instructions.get_mut(jump_if_false).unwrap() = Instruction::JumpIfFalse(false_start);
                }
            }
            Stmt::Return(expr) => {
                self.compile_expr(expr);
                self.emit(Instruction::Return);
            }
            Stmt::Fn { .. } => {}
            Stmt::UseLib(_) | Stmt::PkgInstall(_) | Stmt::PkgRemove(_) | Stmt::PkgList => {}
            Stmt::For { var, start, end, step, body } => {
                self.compile_expr(start);
                self.emit(Instruction::Store(var.clone()));
                self.compile_expr(end);
                let start_idx = self.instructions.len();
                self.emit(Instruction::Load(var.clone()));
                self.emit(Instruction::Load(var.clone()));
                self.emit(Instruction::Add);
                self.compile_block(body);
                self.emit(Instruction::Load(var.clone()));
                self.compile_expr(step);
                self.emit(Instruction::Add);
                self.emit(Instruction::Store(var.clone()));
                let end_idx = self.instructions.len();
                self.emit(Instruction::Jump(start_idx));
                // This block is intentionally kept lightweight for the prototype runtime.
                let _ = end_idx;
            }
        }
    }

    fn compile_block(&mut self, stmts: &[Stmt]) {
        for stmt in stmts {
            self.compile_stmt(stmt);
        }
    }

    fn compile_expr(&mut self, expr: &Expr) {
        match expr {
            Expr::Literal(v) => self.emit(Instruction::Push(v.clone())),
            Expr::Var(name) => self.emit(Instruction::Load(name.clone())),
            Expr::Unary(op, right) => {
                self.compile_expr(right);
                match op.as_str() {
                    "-" => self.emit(Instruction::Neg),
                    "!" => self.emit(Instruction::Not),
                    _ => self.emit(Instruction::Push(Value::Void)),
                }
            }
            Expr::Binary(left, op, right) => {
                self.compile_expr(left);
                self.compile_expr(right);
                match op.as_str() {
                    "+" => self.emit(Instruction::Add),
                    "-" => self.emit(Instruction::Sub),
                    "*" => self.emit(Instruction::Mul),
                    "/" => self.emit(Instruction::Div),
                    "%" => self.emit(Instruction::Mod),
                    "==" => self.emit(Instruction::Eq),
                    "!=" => self.emit(Instruction::Neq),
                    "<" => self.emit(Instruction::Lt),
                    ">" => self.emit(Instruction::Gt),
                    "<=" => self.emit(Instruction::Lte),
                    ">=" => self.emit(Instruction::Gte),
                    _ => self.emit(Instruction::Push(Value::Void)),
                }
            }
            Expr::Call(name, args) => {
                for arg in args {
                    self.compile_expr(arg);
                }
                self.emit(Instruction::Call(name.clone(), args.len()));
            }
            Expr::List(items) => {
                for item in items {
                    self.compile_expr(item);
                }
                self.emit(Instruction::NewList(items.len()));
            }
        }
    }
}

#[derive(Clone)]
struct PackageManager {
    available_pkgs: HashMap<String, Package>,
    installed_pkgs: HashSet<String>,
    loaded_libs: HashSet<String>,
}

impl PackageManager {
    fn new() -> Self {
        let mut available = HashMap::new();
        available.insert(
            "discord".to_string(),
            Package {
                name: "discord".to_string(),
                version: "0.1.0".to_string(),
                description: "Discord bot utilities".to_string(),
            },
        );
        available.insert(
            "http".to_string(),
            Package {
                name: "http".to_string(),
                version: "0.1.0".to_string(),
                description: "HTTP client utilities".to_string(),
            },
        );
        available.insert(
            "scene".to_string(),
            Package {
                name: "scene".to_string(),
                version: "0.1.0".to_string(),
                description: "2D scene primitives".to_string(),
            },
        );
        Self {
            available_pkgs: available,
            installed_pkgs: HashSet::new(),
            loaded_libs: HashSet::new(),
        }
    }

    fn install_pkg(&mut self, name: &str) {
        if self.installed_pkgs.contains(name) {
            println!("[pkg] {} already installed", name);
            return;
        }
        if let Some(pkg) = self.available_pkgs.get(name) {
            println!("[pkg] installing {} v{} - {}", pkg.name, pkg.version, pkg.description);
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
            println!("  - {} v{} ({}) [{}]", pkg.name, pkg.version, pkg.description, installed);
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

struct Env {
    scopes: Vec<HashMap<String, Value>>,
    funcs: HashMap<String, Function>,
    pkg_mgr: PackageManager,
}

impl Env {
    fn new() -> Self {
        let mut global = HashMap::new();
        global.insert("PI".to_string(), Value::Float(3.14159));
        global.insert("E".to_string(), Value::Float(2.71828));
        global.insert("VERSION".to_string(), Value::Str("5.0.0-hybrid".to_string()));
        Self {
            scopes: vec![global],
            funcs: HashMap::new(),
            pkg_mgr: PackageManager::new(),
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
}

fn hash_text(value: &str) -> String {
    use std::hash::{DefaultHasher, Hash, Hasher};
    let mut hasher = DefaultHasher::new();
    value.hash(&mut hasher);
    format!("{:x}", hasher.finish())
}

fn builtin_call(env: &mut Env, name: &str, args: Vec<Value>) -> Value {
    match name {
        "input" => {
            let prompt = if !args.is_empty() { args[0].to_string() } else { String::new() };
            print!("{}", prompt);
            io::stdout().flush().ok();
            let mut buf = String::new();
            io::stdin().read_line(&mut buf).ok();
            Value::Str(buf.trim_end().to_string())
        }
        "draw_rect" => {
            if args.len() >= 5 {
                let x = args[0].to_number();
                let y = args[1].to_number();
                let w = args[2].to_number();
                let h = args[3].to_number();
                let color = args[4].to_string();
                println!("[2d] rect x={} y={} w={} h={} color={}", x, y, w, h, color);
                Value::Bool(true)
            } else {
                Value::Bool(false)
            }
        }
        "draw_circle" => {
            if args.len() >= 4 {
                let cx = args[0].to_number();
                let cy = args[1].to_number();
                let r = args[2].to_number();
                let color = args[3].to_string();
                println!("[2d] circle cx={} cy={} r={} color={}", cx, cy, r, color);
                Value::Bool(true)
            } else {
                Value::Bool(false)
            }
        }
        "render_scene" => {
            if args.len() >= 3 {
                let name = args[0].to_string();
                let width = args[1].to_number();
                let height = args[2].to_number();
                println!("[2d] render_scene {} {}x{}", name, width, height);
                Value::Str(format!("rendered:{}:{}x{}", name, width, height))
            } else {
                Value::Void
            }
        }
        "scan_port" => {
            if args.len() >= 2 {
                let host = args[0].to_string();
                let port = args[1].to_number() as i64;
                let reachable = if host.is_empty() { false } else { port > 0 && port <= 65535 };
                println!("[security] scan_port host={} port={} => {}", host, port, reachable);
                Value::Bool(reachable)
            } else {
                Value::Bool(false)
            }
        }
        "hash_text" => {
            if let Some(v) = args.first() {
                Value::Str(hash_text(&v.to_string()))
            } else {
                Value::Str(String::new())
            }
        }
        "sys_info" => {
            let now = SystemTime::now().duration_since(UNIX_EPOCH).unwrap_or_default().as_secs();
            Value::Str(format!("runtime=hybrid, timestamp={}", now))
        }
        "read_env" => {
            if let Some(v) = args.first() {
                let key = v.to_string();
                let value = std::env::var(&key).unwrap_or_default();
                Value::Str(value)
            } else {
                Value::Str(String::new())
            }
        }
        "write_file" => {
            if args.len() >= 2 {
                let path = args[0].to_string();
                let content = args[1].to_string();
                let ok = std::fs::write(&path, content).is_ok();
                Value::Bool(ok)
            } else {
                Value::Bool(false)
            }
        }
        "spawn_process" => {
            if args.len() >= 1 {
                let cmd = args[0].to_string();
                println!("[systems] spawn_process {}", cmd);
                Value::Bool(!cmd.is_empty())
            } else {
                Value::Bool(false)
            }
        }
        "pkg_list" => {
            env.pkg_mgr.list_pkgs();
            Value::Void
        }
        "pkg_install" => {
            if let Some(v) = args.first() {
                env.pkg_mgr.install_pkg(&v.to_string());
                Value::Bool(true)
            } else {
                Value::Bool(false)
            }
        }
        "pkg_remove" => {
            if let Some(v) = args.first() {
                env.pkg_mgr.remove_pkg(&v.to_string());
                Value::Bool(true)
            } else {
                Value::Bool(false)
            }
        }
        _ => {
            println!("[error] unknown builtin {}", name);
            Value::Void
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

fn eval_expr(env: &mut Env, expr: &Expr) -> Value {
    match expr {
        Expr::Literal(v) => v.clone(),
        Expr::Var(name) => env.get_var(name).unwrap_or(Value::Void),
        Expr::Unary(op, rhs) => {
            let v = eval_expr(env, rhs);
            match op.as_str() {
                "-" => Value::Float(-v.to_number()),
                "!" => Value::Bool(!v.to_bool()),
                _ => Value::Void,
            }
        }
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
                "%" => Value::Float(l.to_number() % r.to_number()),
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
            let evaluated: Vec<Value> = args.iter().map(|arg| eval_expr(env, arg)).collect();
            builtin_call(env, name, evaluated)
        }
        Expr::List(items) => {
            let values: Vec<Value> = items.iter().map(|item| eval_expr(env, item)).collect();
            Value::List(values)
        }
    }
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
            let value = eval_expr(env, expr);
            let display = interpolate(env, &value.to_string());
            println!("{}", display);
        }
        Stmt::UseLib(name) => {
            env.pkg_mgr.load_lib(name);
        }
        Stmt::If { cond, body, else_body } => {
            let result = eval_expr(env, cond);
            if result.to_bool() {
                exec_block(env, body);
            } else if !else_body.is_empty() {
                exec_block(env, else_body);
            }
        }
        Stmt::For { var, start, end, step, body } => {
            let s = eval_expr(env, start).to_number() as i64;
            let e = eval_expr(env, end).to_number() as i64;
            let step_val = eval_expr(env, step).to_number() as i64;
            let step_amount = if step_val == 0 { 1 } else { step_val };
            let mut i = s;
            let mut cond = |value: i64, end_value: i64, increment: i64| {
                if increment > 0 { value < end_value } else { value > end_value }
            };
            while cond(i, e, step_amount) {
                env.push_scope();
                env.set_var(var, Value::Int(i));
                for stmt in body {
                    exec_stmt(env, stmt);
                }
                env.pop_scope();
                i += step_amount;
            }
        }
        Stmt::Fn { name, params, body } => {
            env.define_fn(name, Function { params: params.clone(), body: body.clone() });
        }
        Stmt::Return(expr) => {
            let _ = eval_expr(env, expr);
        }
        Stmt::ExprStmt(expr) => {
            let _ = eval_expr(env, expr);
        }
        Stmt::PkgInstall(name) => {
            env.pkg_mgr.install_pkg(name);
        }
        Stmt::PkgRemove(name) => {
            env.pkg_mgr.remove_pkg(name);
        }
        Stmt::PkgList => {
            env.pkg_mgr.list_pkgs();
        }
    }
}

struct VM {
    env: Env,
    stack: Vec<Value>,
    ip: usize,
    instructions: Vec<Instruction>,
}

impl VM {
    fn new(env: Env, instructions: Vec<Instruction>) -> Self {
        Self { env, stack: Vec::new(), ip: 0, instructions }
    }

    fn pop(&mut self) -> Value {
        self.stack.pop().unwrap_or(Value::Void)
    }

    fn push(&mut self, value: Value) {
        self.stack.push(value);
    }

    fn run(&mut self) {
        while self.ip < self.instructions.len() {
            let instr = self.instructions[self.ip].clone();
            self.ip += 1;
            match instr {
                Instruction::Push(v) => self.push(v),
                Instruction::Load(name) => {
                    if let Some(v) = self.env.get_var(&name) {
                        self.push(v);
                    } else {
                        self.push(Value::Void);
                    }
                }
                Instruction::Store(name) => {
                    let value = self.pop();
                    self.env.set_var(&name, value);
                }
                Instruction::Add => {
                    let b = self.pop();
                    let a = self.pop();
                    self.push(Value::Float(a.to_number() + b.to_number()));
                }
                Instruction::Sub => {
                    let b = self.pop();
                    let a = self.pop();
                    self.push(Value::Float(a.to_number() - b.to_number()));
                }
                Instruction::Mul => {
                    let b = self.pop();
                    let a = self.pop();
                    self.push(Value::Float(a.to_number() * b.to_number()));
                }
                Instruction::Div => {
                    let b = self.pop();
                    let a = self.pop();
                    let denominator = b.to_number();
                    if denominator == 0.0 {
                        println!("[error] division by zero");
                        self.push(Value::Void);
                    } else {
                        self.push(Value::Float(a.to_number() / denominator));
                    }
                }
                Instruction::Mod => {
                    let b = self.pop();
                    let a = self.pop();
                    self.push(Value::Float(a.to_number() % b.to_number()));
                }
                Instruction::Eq => {
                    let b = self.pop();
                    let a = self.pop();
                    self.push(Value::Bool(a.to_string() == b.to_string()));
                }
                Instruction::Neq => {
                    let b = self.pop();
                    let a = self.pop();
                    self.push(Value::Bool(a.to_string() != b.to_string()));
                }
                Instruction::Lt => {
                    let b = self.pop();
                    let a = self.pop();
                    self.push(Value::Bool(a.to_number() < b.to_number()));
                }
                Instruction::Gt => {
                    let b = self.pop();
                    let a = self.pop();
                    self.push(Value::Bool(a.to_number() > b.to_number()));
                }
                Instruction::Lte => {
                    let b = self.pop();
                    let a = self.pop();
                    self.push(Value::Bool(a.to_number() <= b.to_number()));
                }
                Instruction::Gte => {
                    let b = self.pop();
                    let a = self.pop();
                    self.push(Value::Bool(a.to_number() >= b.to_number()));
                }
                Instruction::Neg => {
                    let value = self.pop();
                    self.push(Value::Float(-value.to_number()));
                }
                Instruction::Not => {
                    let value = self.pop();
                    self.push(Value::Bool(!value.to_bool()));
                }
                Instruction::Print => {
                    let value = self.pop();
                    println!("{}", value.to_string());
                }
                Instruction::Call(name, arity) => {
                    let mut args = Vec::with_capacity(arity);
                    for _ in 0..arity {
                        args.push(self.pop());
                    }
                    args.reverse();
                    let result = builtin_call(&mut self.env, &name, args);
                    self.push(result);
                }
                Instruction::NewList(size) => {
                    let mut values = Vec::with_capacity(size);
                    for _ in 0..size {
                        values.push(self.pop());
                    }
                    values.reverse();
                    self.push(Value::List(values));
                }
                Instruction::JumpIfFalse(offset) => {
                    let value = self.pop();
                    if !value.to_bool() {
                        self.ip = offset;
                    }
                }
                Instruction::Jump(target) => {
                    self.ip = target;
                }
                Instruction::Return => {
                    // The hybrid runtime exits the current VM frame without a complex call stack.
                    break;
                }
            }
        }
    }
}

fn main() {
    let src = r#"
    pkg list

    pkg install scene
    use scene

    fn greet(name) {
        show "Hello, {name}"
    }

    show "Welcome to DoxRain-RS {VERSION}"

    let width = 800
    let height = 600
    let scene_id = "demo"
    render_scene(scene_id, width, height)
    draw_rect(20, 30, 200, 120, "blue")
    draw_circle(150, 150, 40, "orange")

    let x = 10
    let y = 5
    let result = (x * y + 2) / 2
    show "Result = {result}"

    if result >= 20 {
        show "Hybrid runtime is active"
    } else {
        show "System diagnostics are stable"
    }

    let port = 443
    if scan_port("localhost", port) {
        show "Port 443 reachable"
    }

    let digest = hash_text("doxrain")
    show "Hash = {digest}"

    let signature = sys_info()
    show "System = {signature}"

    let username = input("Enter your name: ")
    greet(username)

    let payload = [1, 2, 3, 4]
    show payload

    pkg remove scene
    pkg list
    "#;

    let tokens = lex(src);
    let mut parser = Parser::new(tokens);
    let program = parser.parse_program();

    let mut env = Env::new();
    for stmt in &program {
        exec_stmt(&mut env, stmt);
    }

    let mut compiler = Compiler::new();
    compiler.compile_program(&program);
    let instructions = compiler.instructions;
    let mut vm = VM::new(env, instructions);
    vm.run();
}

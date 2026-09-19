# DoxRain Cherokee 1.0.0 — 教程（中文）

## 发布版本

DoxRain Cherokee 1.0.0 是 DoxRain 语言的第一个 Windows 优先基础版本。目标系统为 Windows 7 及更高版本，计划提供 MSVC x86 和 x64 构建。Linux 支持计划在未来加入。

项目首先面向 2D 开发，之后扩展到 3D/AAA、进攻以外的防御性网络安全工具，以及外部和嵌入式编程。第一阶段使用解释器，之后再加入字节码和原生编译。

当前发布版本还没有附带已编译的 ZIP 文件。因此它是开发基础版本，不是完整的生产级框架。

## 源代码和语法

Rust 原型 `doxrain_fullcode_Prototype.rs` 包含词法分析器、解析器、AST、运行环境、解释器、包管理器，以及实验性的编译器/虚拟机方向。Cherokee 分支定义 Windows 主机和框架架构。

```dox
let name = "DoxRain"
let answer = 40 + 2
show "Hello, {name}!"
show answer

if answer >= 42 {
    show "正确"
} else {
    show "请重试"
}

for i in 0..3 {
    show "项目 {i}"
}
```

语言支持整数、浮点数、布尔值、字符串、列表和 `Void`。运算符包括 `+`、`-`、`*`、`/`、`%`、`==`、`!=`、`<`、`>`、`<=` 和 `>=`。

```dox
fn greet(user) {
    show "欢迎，{user}"
}

greet("Rain")
```

## 包和构建

```dox
pkg list
pkg install scene
use scene
pkg remove scene
```

```bat
cargo build --release --target x86_64-pc-windows-msvc
cargo build --release --target i686-pc-windows-msvc
```

在声明兼容性之前，应测试 Windows 7 SP1、Windows 10 和 Windows 11。第一版不包含 Linux 支持。

## 限制

语言、编译器、Cherokee、更新器和领域 API 仍在开发中。网络安全功能必须只用于防御目的，并且必须获得系统所有者授权。

# DoxRain Cherokee 1.0.0 — Tutorial (español)

## Lanzamiento

DoxRain Cherokee 1.0.0 es la primera base de DoxRain orientada a Windows. Su objetivo es Windows 7 y versiones posteriores, con compilaciones MSVC de 32 y 64 bits. La compatibilidad con Linux está prevista para el futuro.

El proyecto comienza con desarrollo 2D y después apunta a 3D/AAA, herramientas de ciberseguridad defensiva y programación externa o embebida. El intérprete es el primer backend; el bytecode y la compilación nativa llegarán más adelante.

La versión todavía no contiene archivos ZIP compilados. Por tanto, es una base de desarrollo y no un framework de producción terminado.

## Código y sintaxis

El prototipo Rust `doxrain_fullcode_Prototype.rs` incluye lexer, parser, AST, entorno, intérprete, gestor de paquetes y una dirección experimental de compilador/VM. Cherokee define el host y el framework de Windows.

```dox
let name = "DoxRain"
let answer = 40 + 2
show "Hello, {name}!"
show answer

if answer >= 42 {
    show "Correcto"
} else {
    show "Inténtalo de nuevo"
}

for i in 0..3 {
    show "Elemento {i}"
}
```

Hay números, booleanos, cadenas, listas y `Void`. Los operadores incluyen `+`, `-`, `*`, `/`, `%`, `==`, `!=`, `<`, `>`, `<=` y `>=`.

```dox
fn greet(user) {
    show "Bienvenido, {user}"
}

greet("Rain")
```

## Paquetes y compilación

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

Prueba Windows 7 SP1, Windows 10 y Windows 11 antes de anunciar compatibilidad. Linux no forma parte de la primera versión.

## Limitaciones

El lenguaje, el compilador, Cherokee, el actualizador y las API especializadas siguen en desarrollo. Las herramientas de ciberseguridad deben utilizarse únicamente con fines defensivos, legales y autorizados.

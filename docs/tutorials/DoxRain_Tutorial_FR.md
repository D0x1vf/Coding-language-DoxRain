# DoxRain Cherokee 1.0.0 — Tutoriel (français)

## Version

DoxRain Cherokee 1.0.0 est la première base de DoxRain conçue d’abord pour Windows. Elle vise Windows 7 et les versions ultérieures, avec des compilations MSVC x86 et x64. La prise en charge de Linux est prévue ultérieurement.

Le projet commence par le développement 2D, puis vise la 3D/AAA, les outils de cybersécurité défensive ainsi que la programmation externe et embarquée. L’interpréteur est le premier moteur d’exécution; le bytecode et la compilation native viendront plus tard.

Aucun fichier ZIP compilé n’est encore joint à la release. Il s’agit donc d’une base de développement, pas encore d’un framework de production terminé.

## Code et syntaxe

Le prototype Rust `doxrain_fullcode_Prototype.rs` contient un lexer, un parseur, un AST, un environnement d’exécution, un interpréteur, un gestionnaire de paquets et une direction expérimentale compilateur/VM. Cherokee définit l’hôte Windows et l’architecture du framework.

```dox
let name = "DoxRain"
let answer = 40 + 2
show "Hello, {name}!"
show answer

if answer >= 42 {
    show "Correct"
} else {
    show "Réessayez"
}

for i in 0..3 {
    show "Élément {i}"
}
```

Les valeurs comprennent les nombres, les booléens, les chaînes, les listes et `Void`. Les opérateurs comprennent `+`, `-`, `*`, `/`, `%`, `==`, `!=`, `<`, `>`, `<=` et `>=`.

```dox
fn greet(user) {
    show "Bienvenue, {user}"
}

greet("Rain")
```

## Paquets et compilation

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

Testez Windows 7 SP1, Windows 10 et Windows 11 avant d’annoncer la compatibilité. Linux ne fait pas partie de la première version.

## Limites

Le langage, le compilateur, Cherokee, l’updater et les API spécialisées sont encore en développement. Les fonctions de cybersécurité doivent rester défensives, légales et autorisées.

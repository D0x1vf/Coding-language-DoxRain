🌑 DoxRain‑RS — Language Tutorial

A beginner‑friendly guide to writing programs in the DoxRain language.

DoxRain‑RS is a compact scripting language powered by a Rust interpreter.
This tutorial teaches you the full language syntax, features, and behavior.

---

📌 1. Variables

Use let to create variables.

let x = 10
let name = "DoxRain"


Variables can store:

• integers
• floats
• booleans
• strings


---

📌 2. Showing Output

Use show to print values.

show "Hello world"
show x


String interpolation

You can embed variables inside strings using {var}.

let user = "Rain"
show "Welcome, {user}"


---

📌 3. Expressions

DoxRain supports:

• + addition
• - subtraction
• * multiplication
• / division
• comparisons: == != > < >= <=


Examples:

let result = 10 * 5 + 2
show result

if result > 20 {
    show "Big result!"
}


---

📌 4. Input

Use the built‑in input() function.

let name = input("Enter your name: ")
show "Hello, {name}"


---

📌 5. If Statements

Basic conditional:

if x > 5 {
    show "x is big"
}


Blocks use { ... }.

---

📌 6. Loops

DoxRain supports for loops with ranges:

for i in 0..3 {
    show "Loop {i}"
}


This prints:

Loop 0
Loop 1
Loop 2


---

📌 7. Functions

Define functions using fn.

fn greet(name) {
    show "Hello, {name}"
}

greet("Rain")


Functions support parameters and scoped variables.

---

📌 8. Using Libraries

Load a library with use.

use discord


If the library is not installed, DoxRain automatically installs it.

---

📌 9. Package Manager

DoxRain includes a built‑in package manager with three commands:

List packages

pkg list


Install a package

pkg install discord


Remove a package

pkg remove discord


Packages included by default:

• discord — Discord bot utilities
• http — HTTP client utilities


---

📌 10. Full Example Program

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


---

📌 11. Notes

• Variables are dynamically typed
• Functions do not return values yet
• Blocks create new scopes
• Division by zero prints an error
• Unknown variables evaluate to Void
• Unknown functions print an error


---

🌑 End of Tutorial

This file covers the entire DoxRain‑RS language as implemented in the Rust engine.

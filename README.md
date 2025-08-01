# DoxRain Programming Language

**The Future of Programming - Simple, Powerful, and Natural**

DoxRain is a modern programming language designed for simplicity and rapid development. With its natural language-inspired syntax, built-in web capabilities, and comprehensive package ecosystem, DoxRain makes programming accessible to everyone while remaining powerful enough for complex applications.

## 🌟 Key Features

### 🎯 Natural Language Syntax
Write code that reads like English:
```doxrain
show "Hello, World!"
var name = "Alice"
input "What's your name? "
if $name == "Alice"
    show "Nice to meet you!"
end
```

### 🧮 Built-in Mathematics
Perform calculations naturally:
```doxrain
var radius = 5
$radius ^ 2
show "Area: "
show $result
```

### 🌐 Web Development Ready
Create web applications effortlessly:
```doxrain
import dox-web
webserver 8080
show "Server running at http://localhost:8080"
```

### 📦 Integrated Package Manager
Install and use packages seamlessly:
```doxrain
import dox-math
import dox-http
import dox-json
```

### 🔧 Interactive Development
Test code instantly with the built-in REPL:
```bash
./doxrain
dox> var x = 10
dox> show $x
10
```

### 🎨 Language Server Protocol (LSP)
Get intelligent code completion and error detection in your favorite editor.

## 🚀 Quick Start

### Installation

```bash
# Clone the repository
git clone https://github.com/yourusername/doxrain.git
cd doxrain

# Compile DoxRain
g++ -std=c++17 -pthread -o doxrain doxrain.cpp

# Run your first program
echo 'show "Hello, DoxRain!"' > hello.dox
./doxrain hello.dox
```

### Your First Program

Create a file called `hello.dox`:
```doxrain
show "Welcome to DoxRain!"
input "What's your name? "
show "Hello, "
show $input
show "! Welcome to the future of programming!"
```

Run it:
```bash
./doxrain hello.dox
```

## 📚 Learning DoxRain

### Complete Tutorial
Start with our comprehensive tutorial that demonstrates all language features in a single program:
```bash
./doxrain complete_doxrain_tutorial.dox
```

See [TUTORIAL_README.md](TUTORIAL_README.md) for the complete learning guide.

### Interactive Learning
Launch the REPL for hands-on experimentation:
```bash
./doxrain
```

### Documentation
Explore detailed tutorials in the `tutorials/` directory:
- [Getting Started](tutorials/01_getting_started.md)
- [Variables and Data Types](tutorials/02_variables.md)
- [Functions](tutorials/09_functions.md)
- [Web Development](tutorials/13_web_servers.md)
- [Complete Projects](tutorials/19_projects.md)

## 🏗️ Language Architecture

### Core Components
- **Native C++ Implementation** - High performance and reliability
- **Single Binary Architecture** - Compiler, interpreter, and tools in one executable
- **Built-in Package Manager** - No external dependency management needed
- **Integrated LSP Server** - Smart editor features out of the box
- **REPL Environment** - Interactive development and testing

### Package Ecosystem
- **dox-math** - Advanced mathematical operations
- **dox-web** - Web framework and server capabilities
- **dox-http** - HTTP client for API integration
- **dox-json** - JSON parsing and serialization

## 💻 Programming Examples

### Simple Calculator
```doxrain
input "Enter first number: "
var a = $input

input "Enter second number: "
var b = $input

$a + $b
show "Sum: "
show $result

$a * $b
show "Product: "
show $result
```

### Web API Server
```doxrain
import dox-web
import dox-json

webserver 3000

show "API server running on port 3000"
show "Endpoints available:"
show "GET /api/users"
show "POST /api/data"
```

### Data Processing
```doxrain
import dox-json

function processData()
    var total = 0
    var count = 0
    
    loop 5
        input "Enter a number: "
        var num = $input
        
        $total + $num
        var total = $result
        
        $count + 1
        var count = $result
    end
    
    $total / $count
    show "Average: "
    show $result
end

processData()
```

### Object-Oriented Programming
```doxrain
class Student
end

class Course
end

function createStudent()
    input "Student name: "
    var name = $input
    
    input "Student ID: "
    var id = $input
    
    show "Student created: "
    show $name
    show " (ID: "
    show $id
    show ")"
end

createStudent()
```

## 🛠️ Development Tools

### REPL (Read-Eval-Print Loop)
Interactive development environment:
```bash
./doxrain
dox> var x = 10
dox> $x * 2
[MATH] 10 * 2 = 20
dox> show $result
20
```

### Package Manager
Built-in package management:
```bash
dox> pkg install dox-math
dox> pkg list
dox> pkg search web
```

### Language Server
Smart editor integration:
```bash
./doxrain --lsp
```

## 🌐 Web Development

DoxRain excels at web development with built-in capabilities:

### HTTP Server
```doxrain
import dox-web
webserver 8080
```

### API Client
```doxrain
import dox-http
api GET https://api.example.com/data
api POST https://api.example.com/users
```

### JSON Processing
```doxrain
import dox-json
# JSON parsing and serialization built-in
```

## 🧪 Testing and Debugging

### Interactive Testing
Use the REPL for rapid testing:
```doxrain
dox> function test()
dox>     show "Testing..."
dox> end
dox> test()
Testing...
```

### Error Handling
Built-in validation and error messages:
```doxrain
var x = 10
var y = 0

if $y == 0
    show "Error: Cannot divide by zero"
end

if $y != 0
    $x / $y
    show "Result: "
    show $result
end
```

## 📊 Performance and Compatibility

### Performance
- **Native C++ implementation** for optimal speed
- **Efficient memory management** with smart pointers
- **Optimized mathematical operations**
- **Fast startup time** with minimal overhead

### Platform Support
- **Linux** (primary platform)
- **macOS** (compatible)
- **Windows** (with WSL or native compilation)

### Requirements
- C++17 compatible compiler
- Standard C++ library
- POSIX-compliant system (for full features)

## 🤝 Contributing

We welcome contributions to DoxRain! Here's how you can help:

### Areas for Contribution
- **Core language features** - Enhance the interpreter
- **Package development** - Create new packages
- **Documentation** - Improve tutorials and guides
- **Tools and utilities** - Build developer tools
- **Testing** - Add test cases and examples

### Development Setup
```bash
git clone https://github.com/yourusername/doxrain.git
cd doxrain
g++ -std=c++17 -pthread -o doxrain doxrain.cpp
./doxrain examples/hello.dox
```

### Guidelines
1. Follow C++17 standards
2. Maintain compatibility with existing code
3. Include tests for new features
4. Update documentation
5. Follow the existing code style

## 📄 License

DoxRain is released under the **MIT License**. See [LICENSE](LICENSE) for details.

This means you can:
- ✅ Use DoxRain commercially
- ✅ Modify and distribute
- ✅ Include in proprietary software
- ✅ Use for any purpose

## 🔗 Resources

### Documentation
- [Complete Tutorial](TUTORIAL_README.md) - Comprehensive learning guide
- [Language Reference](tutorials/) - Detailed feature documentation
- [Examples](examples/) - Sample programs and use cases

### Community
- **GitHub Issues** - Bug reports and feature requests
- **Discussions** - General questions and community support
- **Wiki** - Community-maintained documentation

### Related Projects
- **Editor Extensions** - VSCode, Vim, Emacs support
- **Package Repository** - Community packages
- **Web Framework** - Enhanced web development tools

## 🎯 Roadmap

### Current Version: v3.0.0
- ✅ Complete language implementation
- ✅ Package manager integration
- ✅ LSP server support
- ✅ Web development capabilities
- ✅ Interactive REPL

### Future Plans
- 🔄 Performance optimizations
- 🔄 Extended package ecosystem
- 🔄 Advanced debugging tools
- 🔄 IDE plugin improvements
- 🔄 Cross-platform enhancements

## 💬 Support

### Getting Help
- **Documentation** - Check tutorials and examples first
- **GitHub Issues** - Report bugs or request features
- **Discussions** - Ask questions and share projects
- **Community** - Connect with other developers

### Reporting Issues
When reporting issues, please include:
- DoxRain version
- Operating system
- Sample code that reproduces the issue
- Expected vs. actual behavior

---

**Start your DoxRain journey today!** 

```bash
git clone https://github.com/yourusername/doxrain.git
cd doxrain && make
./doxrain complete_doxrain_tutorial.dox
```

*DoxRain - Programming Made Simple* 🚀

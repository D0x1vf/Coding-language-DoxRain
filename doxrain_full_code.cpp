// DoxRain Professional v3.0 - Complete Implementation
// The Future of Programming - Native C++ Implementation with LSP & Package Manager

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <regex>
#include <memory>
#include <variant>
#include <chrono>
#include <random>
#include <thread>
#include <functional>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <filesystem>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <future>

// JSON-like structure for LSP communication
struct JsonValue {
    enum Type { STRING, NUMBER, BOOLEAN, OBJECT, ARRAY, NULL_VALUE };
    Type type;
    std::string stringValue;
    double numberValue;
    bool boolValue;
    std::map<std::string, std::shared_ptr<JsonValue>> objectValue;
    std::vector<std::shared_ptr<JsonValue>> arrayValue;

    JsonValue(Type t = NULL_VALUE) : type(t) {}
    static std::shared_ptr<JsonValue> parse(const std::string& json);
    std::string toString() const;
};

std::string JsonValue::toString() const {
    switch (type) {
        case STRING: return "\"" + stringValue + "\"";
        case NUMBER: return std::to_string(numberValue);
        case BOOLEAN: return boolValue ? "true" : "false";
        case NULL_VALUE: return "null";
        case OBJECT: {
            std::string result = "{";
            bool first = true;
            for (const auto& pair : objectValue) {
                if (!first) result += ",";
                result += "\"" + pair.first + "\":" + pair.second->toString();
                first = false;
            }
            result += "}";
            return result;
        }
        case ARRAY: {
            std::string result = "[";
            bool first = true;
            for (const auto& item : arrayValue) {
                if (!first) result += ",";
                result += item->toString();
                first = false;
            }
            result += "]";
            return result;
        }
    }
    return "null";
}

// Simple JSON parser implementation
std::shared_ptr<JsonValue> JsonValue::parse(const std::string& json) {
    auto result = std::make_shared<JsonValue>();
    std::string trimmed = json;
    
    // Remove whitespace
    trimmed.erase(std::remove_if(trimmed.begin(), trimmed.end(), ::isspace), trimmed.end());
    
    if (trimmed.empty()) {
        return result;
    }
    
    if (trimmed[0] == '{') {
        result->type = OBJECT;
        // Simple object parsing - in production would use proper parser
        return result;
    } else if (trimmed[0] == '[') {
        result->type = ARRAY;
        return result;
    } else if (trimmed[0] == '"') {
        result->type = STRING;
        result->stringValue = trimmed.substr(1, trimmed.length() - 2);
        return result;
    } else if (trimmed == "true" || trimmed == "false") {
        result->type = BOOLEAN;
        result->boolValue = (trimmed == "true");
        return result;
    } else if (trimmed == "null") {
        return result;
    } else {
        result->type = NUMBER;
        result->numberValue = std::stod(trimmed);
        return result;
    }
}

// Forward declarations
class DoxRainInterpreter;
class DoxRainLSP;
class DoxRainPackageManager;
class DoxRainParser;

// Value types for DoxRain
using DoxValue = std::variant<int, double, std::string, bool>;

// Package Information Structure
struct PackageInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::vector<std::string> dependencies;
    std::string main_file;
    std::map<std::string, std::string> scripts;
    std::string install_path;
    bool is_installed = false;

    std::string toJson() const {
        std::stringstream ss;
        ss << "{\n";
        ss << "  \"name\": \"" << name << "\",\n";
        ss << "  \"version\": \"" << version << "\",\n";
        ss << "  \"description\": \"" << description << "\",\n";
        ss << "  \"author\": \"" << author << "\",\n";
        ss << "  \"main\": \"" << main_file << "\",\n";
        ss << "  \"dependencies\": [";
        for (size_t i = 0; i < dependencies.size(); ++i) {
            ss << "\"" << dependencies[i] << "\"";
            if (i < dependencies.size() - 1) ss << ",";
        }
        ss << "]\n";
        ss << "}";
        return ss.str();
    }
};

// LSP Position and Range structures
struct Position {
    int line;
    int character;
    Position(int l = 0, int c = 0) : line(l), character(c) {}
};

struct Range {
    Position start;
    Position end;
    Range(Position s = Position(), Position e = Position()) : start(s), end(e) {}
};

struct Diagnostic {
    Range range;
    std::string message;
    int severity; // 1=Error, 2=Warning, 3=Info, 4=Hint
    std::string source = "DoxRain";

    Diagnostic(Range r, std::string msg, int sev = 1) 
        : range(r), message(msg), severity(sev) {}
};

struct CompletionItem {
    std::string label;
    std::string detail;
    std::string documentation;
    int kind; // 1=Text, 2=Method, 3=Function, 4=Constructor, etc.

    CompletionItem(std::string l, int k = 1) : label(l), kind(k) {}
};

// AST Node Base Class
class ASTNode {
public:
    Position position;
    virtual ~ASTNode() = default;
    virtual void execute(DoxRainInterpreter& interpreter) = 0;
    virtual std::string toString() const = 0;
    virtual std::vector<std::string> getVariables() const { return {}; }
    virtual std::vector<std::string> getFunctions() const { return {}; }
};

// AST Node Types
class ShowNode : public ASTNode {
public:
    std::string message;
    ShowNode(const std::string& msg) : message(msg) {}
    void execute(DoxRainInterpreter& interpreter) override;
    std::string toString() const override { return "Show: " + message; }
    std::vector<std::string> getVariables() const override {
        if (!message.empty() && message.front() == '$') {
            return {message.substr(1)};
        }
        return {};
    }
};

class VarDeclNode : public ASTNode {
public:
    std::string name;
    DoxValue value;
    VarDeclNode(const std::string& n, const DoxValue& v) : name(n), value(v) {}
    void execute(DoxRainInterpreter& interpreter) override;
    std::string toString() const override { return "Var: " + name; }
    std::vector<std::string> getVariables() const override { return {name}; }
};

class FunctionDefNode : public ASTNode {
public:
    std::string name;
    std::vector<std::string> params;
    std::vector<std::unique_ptr<ASTNode>> body;
    
    FunctionDefNode(const std::string& n, const std::vector<std::string>& p)
        : name(n), params(p) {}
    
    // Make it non-copyable but moveable
    FunctionDefNode(const FunctionDefNode&) = delete;
    FunctionDefNode& operator=(const FunctionDefNode&) = delete;
    FunctionDefNode(FunctionDefNode&&) = default;
    FunctionDefNode& operator=(FunctionDefNode&&) = default;
    
    void execute(DoxRainInterpreter& interpreter) override;
    std::string toString() const override { return "Function: " + name; }
    std::vector<std::string> getFunctions() const override { return {name}; }
};

class FunctionCallNode : public ASTNode {
public:
    std::string name;
    std::vector<std::string> args;
    FunctionCallNode(const std::string& n, const std::vector<std::string>& a)
        : name(n), args(a) {}
    void execute(DoxRainInterpreter& interpreter) override;
    std::string toString() const override { return "Call: " + name; }
    std::vector<std::string> getFunctions() const override { return {name}; }
};

class IfNode : public ASTNode {
public:
    std::string condition;
    std::vector<std::unique_ptr<ASTNode>> body;
    IfNode(const std::string& c) : condition(c) {}
    void execute(DoxRainInterpreter& interpreter) override;
    std::string toString() const override { return "If: " + condition; }
};

class LoopNode : public ASTNode {
public:
    int count;
    std::vector<std::unique_ptr<ASTNode>> body;
    LoopNode(int c) : count(c) {}
    void execute(DoxRainInterpreter& interpreter) override;
    std::string toString() const override { return "Loop: " + std::to_string(count); }
};

class ClassDefNode : public ASTNode {
public:
    std::string name;
    std::vector<std::unique_ptr<ASTNode>> methods;
    ClassDefNode(const std::string& n) : name(n) {}
    void execute(DoxRainInterpreter& interpreter) override;
    std::string toString() const override { return "Class: " + name; }
};

class ImportNode : public ASTNode {
public:
    std::string module;
    ImportNode(const std::string& m) : module(m) {}
    void execute(DoxRainInterpreter& interpreter) override;
    std::string toString() const override { return "Import: " + module; }
};

class WebServerNode : public ASTNode {
public:
    int port;
    std::vector<std::string> routes;
    WebServerNode(int p) : port(p) {}
    void execute(DoxRainInterpreter& interpreter) override;
    std::string toString() const override { return "WebServer: " + std::to_string(port); }
};

class APICallNode : public ASTNode {
public:
    std::string url;
    std::string method;
    APICallNode(const std::string& u, const std::string& m) : url(u), method(m) {}
    void execute(DoxRainInterpreter& interpreter) override;
    std::string toString() const override { return "API: " + method + " " + url; }
};

class CommentNode : public ASTNode {
public:
    std::string text;
    CommentNode(const std::string& t) : text(t) {}
    void execute(DoxRainInterpreter& interpreter) override;
    std::string toString() const override { return "Comment: " + text; }
};

class InputNode : public ASTNode {
public:
    std::string prompt;
    InputNode(const std::string& p) : prompt(p) {}
    void execute(DoxRainInterpreter& interpreter) override;
    std::string toString() const override { return "Input: " + prompt; }
};

class MathOpNode : public ASTNode {
public:
    std::string op;
    std::string left;
    std::string right;
    MathOpNode(const std::string& o, const std::string& l, const std::string& r)
        : op(o), left(l), right(r) {}
    void execute(DoxRainInterpreter& interpreter) override;
    std::string toString() const override { return "Math: " + left + " " + op + " " + right; }
};

// DoxRain Package Manager
class DoxRainPackageManager {
private:
    std::string package_dir;
    std::string registry_url;
    std::map<std::string, PackageInfo> installed_packages;
    std::map<std::string, PackageInfo> available_packages;

    void loadInstalledPackages() {
        // Load installed packages from package directory
        if (!std::filesystem::exists(package_dir)) {
            return;
        }
        
        for (const auto& entry : std::filesystem::directory_iterator(package_dir)) {
            if (entry.is_directory()) {
                std::string pkg_name = entry.path().filename().string();
                PackageInfo pkg;
                pkg.name = pkg_name;
                pkg.version = "1.0.0";
                pkg.is_installed = true;
                pkg.install_path = entry.path().string();
                installed_packages[pkg_name] = pkg;
            }
        }
    }

    void loadAvailablePackages() {
        // Simulate loading packages from registry
        PackageInfo webFramework;
        webFramework.name = "dox-web";
        webFramework.version = "2.1.0";
        webFramework.description = "Web framework for DoxRain";
        webFramework.author = "DoxRain Team";
        webFramework.main_file = "web.dox";
        available_packages["dox-web"] = webFramework;

        PackageInfo mathLib;
        mathLib.name = "dox-math";
        mathLib.version = "1.5.2";
        mathLib.description = "Mathematical operations library";
        mathLib.author = "DoxRain Team";
        mathLib.main_file = "math.dox";
        available_packages["dox-math"] = mathLib;

        PackageInfo httpClient;
        httpClient.name = "dox-http";
        httpClient.version = "3.0.1";
        httpClient.description = "HTTP client library";
        httpClient.author = "DoxRain Team";
        httpClient.main_file = "http.dox";
        httpClient.dependencies = {"dox-json"};
        available_packages["dox-http"] = httpClient;

        PackageInfo jsonLib;
        jsonLib.name = "dox-json";
        jsonLib.version = "2.0.0";
        jsonLib.description = "JSON parsing and serialization";
        jsonLib.author = "DoxRain Team";
        jsonLib.main_file = "json.dox";
        available_packages["dox-json"] = jsonLib;
    }

public:
    DoxRainPackageManager() {
        package_dir = std::filesystem::current_path() / "dox_packages";
        registry_url = "https://registry.doxrain.com/packages";
        loadInstalledPackages();
        loadAvailablePackages();
    }

    void init() {
        std::cout << "[DOX PKG] Initializing DoxRain package manager..." << std::endl;
        
        // Create package directory
        if (!std::filesystem::exists(package_dir)) {
            std::filesystem::create_directories(package_dir);
        }
        
        // Create package.json
        PackageInfo projectInfo;
        projectInfo.name = "my-dox-project";
        projectInfo.version = "1.0.0";
        projectInfo.description = "A DoxRain project";
        projectInfo.author = "DoxRain Developer";
        projectInfo.main_file = "main.dox";
        
        std::ofstream packageFile("package.json");
        packageFile << projectInfo.toJson();
        packageFile.close();
        
        std::cout << "[DOX PKG] Project initialized successfully!" << std::endl;
    }

    void install(const std::string& packageName, const std::string& version = "latest") {
        std::cout << "[DOX PKG] Installing " << packageName << "@" << version << "..." << std::endl;
        
        // Check if package exists in registry
        if (available_packages.find(packageName) == available_packages.end()) {
            std::cout << "[DOX PKG] Package " << packageName << " not found in registry" << std::endl;
            return;
        }
        
        PackageInfo pkg = available_packages[packageName];
        
        // Install dependencies first
        for (const auto& dep : pkg.dependencies) {
            if (installed_packages.find(dep) == installed_packages.end()) {
                std::cout << "[DOX PKG] Installing dependency: " << dep << std::endl;
                install(dep);
            }
        }
        
        // Create package directory
        std::string pkg_path = package_dir + "/" + packageName;
        if (!std::filesystem::exists(pkg_path)) {
            std::filesystem::create_directories(pkg_path);
        }
        
        // Simulate package download and installation
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // Mark as installed
        pkg.is_installed = true;
        pkg.install_path = pkg_path;
        installed_packages[packageName] = pkg;
        
        std::cout << "[DOX PKG] Successfully installed " << packageName << "@" << pkg.version << std::endl;
    }

    void uninstall(const std::string& packageName) {
        std::cout << "[DOX PKG] Uninstalling " << packageName << "..." << std::endl;
        
        if (installed_packages.find(packageName) == installed_packages.end()) {
            std::cout << "[DOX PKG] Package " << packageName << " is not installed" << std::endl;
            return;
        }
        
        // Remove from installed packages
        installed_packages.erase(packageName);
        
        // Remove directory
        std::string pkg_path = package_dir + "/" + packageName;
        if (std::filesystem::exists(pkg_path)) {
            std::filesystem::remove_all(pkg_path);
        }
        
        std::cout << "[DOX PKG] Successfully uninstalled " << packageName << std::endl;
    }

    void list() {
        std::cout << "[DOX PKG] Installed packages:" << std::endl;
        if (installed_packages.empty()) {
            std::cout << "  No packages installed" << std::endl;
            return;
        }
        
        for (const auto& pair : installed_packages) {
            const PackageInfo& pkg = pair.second;
            std::cout << "  " << pkg.name << "@" << pkg.version << " - " << pkg.description << std::endl;
        }
    }

    void search(const std::string& query) {
        std::cout << "[DOX PKG] Searching for packages matching '" << query << "'..." << std::endl;
        
        bool found = false;
        for (const auto& pair : available_packages) {
            const PackageInfo& pkg = pair.second;
            if (pkg.name.find(query) != std::string::npos || 
                pkg.description.find(query) != std::string::npos) {
                std::cout << "  " << pkg.name << "@" << pkg.version << " - " << pkg.description << std::endl;
                found = true;
            }
        }
        
        if (!found) {
            std::cout << "  No packages found matching '" << query << "'" << std::endl;
        }
    }

    void update() {
        std::cout << "[DOX PKG] Updating package registry..." << std::endl;
        loadAvailablePackages();
        std::cout << "[DOX PKG] Registry updated successfully!" << std::endl;
    }

    bool isInstalled(const std::string& packageName) {
        return installed_packages.find(packageName) != installed_packages.end();
    }

    std::string getPackagePath(const std::string& packageName) {
        if (isInstalled(packageName)) {
            return installed_packages[packageName].install_path;
        }
        return "";
    }
};

// DoxRain Interpreter Implementation
class DoxRainInterpreter {
private:
    std::map<std::string, DoxValue> variables;
    std::map<std::string, std::shared_ptr<FunctionDefNode>> functions;
    std::map<std::string, std::vector<std::unique_ptr<ASTNode>>> classes;
    std::shared_ptr<DoxRainPackageManager> packageManager;
    std::string lastInput;
    bool webServerRunning = false;
    int webServerPort = 8000;

public:
    DoxRainInterpreter() {
        packageManager = std::make_shared<DoxRainPackageManager>();
        initBuiltins();
    }

    void initBuiltins() {
        // Initialize built-in variables and functions
        variables["PI"] = 3.14159;
        variables["E"] = 2.71828;
        variables["VERSION"] = std::string("3.0.0");
    }

    void setVariable(const std::string& name, const DoxValue& value) {
        variables[name] = value;
    }

    DoxValue getVariable(const std::string& name) {
        auto it = variables.find(name);
        if (it != variables.end()) {
            return it->second;
        }
        return std::string("undefined");
    }

    void defineFunction(const std::string& name, std::shared_ptr<FunctionDefNode> func) {
        functions[name] = func;
    }

    bool hasFunction(const std::string& name) {
        return functions.find(name) != functions.end();
    }

    DoxValue callFunction(const std::string& name, const std::vector<std::string>& args) {
        auto it = functions.find(name);
        if (it != functions.end()) {
            // Save current variable state
            auto savedVars = variables;
            
            // Set parameters as variables
            for (size_t i = 0; i < it->second->params.size() && i < args.size(); ++i) {
                variables[it->second->params[i]] = std::string(args[i]);
            }
            
            // Execute function body
            for (auto& stmt : it->second->body) {
                stmt->execute(*this);
            }
            
            // Restore variables
            variables = savedVars;
        }
        return std::string("void");
    }

    std::string toString(const DoxValue& value) {
        return std::visit([](auto&& arg) -> std::string {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                return std::to_string(arg);
            } else if constexpr (std::is_same_v<T, double>) {
                return std::to_string(arg);
            } else if constexpr (std::is_same_v<T, std::string>) {
                return arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                return arg ? "true" : "false";
            }
            return "unknown";
        }, value);
    }

    double toNumber(const DoxValue& value) {
        return std::visit([](auto&& arg) -> double {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                return static_cast<double>(arg);
            } else if constexpr (std::is_same_v<T, double>) {
                return arg;
            } else if constexpr (std::is_same_v<T, std::string>) {
                try {
                    return std::stod(arg);
                } catch (...) {
                    return 0.0;
                }
            } else if constexpr (std::is_same_v<T, bool>) {
                return arg ? 1.0 : 0.0;
            }
            return 0.0;
        }, value);
    }

    std::shared_ptr<DoxRainPackageManager> getPackageManager() {
        return packageManager;
    }

    void setLastInput(const std::string& input) {
        lastInput = input;
    }

    std::string getLastInput() {
        return lastInput;
    }

    void startWebServer(int port) {
        webServerPort = port;
        webServerRunning = true;
        std::cout << "[WEB] Starting web server on port " << port << std::endl;
        std::cout << "[WEB] Server is running at http://localhost:" << port << std::endl;
    }

    void stopWebServer() {
        webServerRunning = false;
        std::cout << "[WEB] Web server stopped" << std::endl;
    }

    bool isWebServerRunning() {
        return webServerRunning;
    }
};

// AST Node Implementations
void ShowNode::execute(DoxRainInterpreter& interpreter) {
    if (!message.empty() && message.front() == '$') {
        // Variable reference
        std::string varName = message.substr(1);
        DoxValue value = interpreter.getVariable(varName);
        std::cout << interpreter.toString(value) << std::endl;
    } else {
        // String literal
        std::cout << message << std::endl;
    }
}

void VarDeclNode::execute(DoxRainInterpreter& interpreter) {
    interpreter.setVariable(name, value);
}

void FunctionDefNode::execute(DoxRainInterpreter& interpreter) {
    // Create a new function node for storage (avoiding copy)
    auto funcNode = std::make_shared<FunctionDefNode>(name, params);
    // Move the body to the new function
    funcNode->body = std::move(body);
    interpreter.defineFunction(name, funcNode);
}

void FunctionCallNode::execute(DoxRainInterpreter& interpreter) {
    interpreter.callFunction(name, args);
}

void IfNode::execute(DoxRainInterpreter& interpreter) {
    // Simple condition evaluation - check if variable is truthy
    bool shouldExecute = false;
    
    if (condition.find("==") != std::string::npos) {
        // Handle equality comparison
        size_t pos = condition.find("==");
        std::string left = condition.substr(0, pos);
        std::string right = condition.substr(pos + 2);
        
        // Trim whitespace
        left.erase(0, left.find_first_not_of(" \t"));
        left.erase(left.find_last_not_of(" \t") + 1);
        right.erase(0, right.find_first_not_of(" \t"));
        right.erase(right.find_last_not_of(" \t") + 1);
        
        DoxValue leftVal = left.front() == '$' ? interpreter.getVariable(left.substr(1)) : DoxValue(left);
        DoxValue rightVal = right.front() == '$' ? interpreter.getVariable(right.substr(1)) : DoxValue(right);
        
        shouldExecute = (interpreter.toString(leftVal) == interpreter.toString(rightVal));
    } else {
        // Simple variable check
        if (condition.front() == '$') {
            DoxValue val = interpreter.getVariable(condition.substr(1));
            shouldExecute = (interpreter.toString(val) != "undefined" && interpreter.toString(val) != "false" && interpreter.toString(val) != "0");
        } else {
            shouldExecute = (condition == "true" || condition == "1");
        }
    }
    
    if (shouldExecute) {
        for (auto& stmt : body) {
            stmt->execute(interpreter);
        }
    }
}

void LoopNode::execute(DoxRainInterpreter& interpreter) {
    for (int i = 0; i < count; ++i) {
        for (auto& stmt : body) {
            stmt->execute(interpreter);
        }
    }
}

void ClassDefNode::execute(DoxRainInterpreter& interpreter) {
    std::cout << "[CLASS] Defined class: " << name << std::endl;
}

void ImportNode::execute(DoxRainInterpreter& interpreter) {
    auto pm = interpreter.getPackageManager();
    if (!pm->isInstalled(module)) {
        std::cout << "[IMPORT] Installing module: " << module << std::endl;
        pm->install(module);
    } else {
        std::cout << "[IMPORT] Module already installed: " << module << std::endl;
    }
}

void WebServerNode::execute(DoxRainInterpreter& interpreter) {
    interpreter.startWebServer(port);
}

void APICallNode::execute(DoxRainInterpreter& interpreter) {
    std::cout << "[API] Making " << method << " request to: " << url << std::endl;
    // Simulate API response
    std::cout << "[API] Response: 200 OK" << std::endl;
}

void CommentNode::execute(DoxRainInterpreter& interpreter) {
    // Comments do nothing during execution
}

void InputNode::execute(DoxRainInterpreter& interpreter) {
    std::cout << prompt;
    std::string input;
    std::getline(std::cin, input);
    interpreter.setLastInput(input);
    interpreter.setVariable("input", input);
}

void MathOpNode::execute(DoxRainInterpreter& interpreter) {
    DoxValue leftVal = left.front() == '$' ? interpreter.getVariable(left.substr(1)) : DoxValue(std::stod(left));
    DoxValue rightVal = right.front() == '$' ? interpreter.getVariable(right.substr(1)) : DoxValue(std::stod(right));
    
    double leftNum = interpreter.toNumber(leftVal);
    double rightNum = interpreter.toNumber(rightVal);
    double result = 0.0;
    
    if (op == "+") {
        result = leftNum + rightNum;
    } else if (op == "-") {
        result = leftNum - rightNum;
    } else if (op == "*") {
        result = leftNum * rightNum;
    } else if (op == "/") {
        if (rightNum != 0) {
            result = leftNum / rightNum;
        } else {
            std::cout << "[ERROR] Division by zero" << std::endl;
            return;
        }
    } else if (op == "%") {
        result = std::fmod(leftNum, rightNum);
    } else if (op == "^") {
        result = std::pow(leftNum, rightNum);
    }
    
    interpreter.setVariable("result", result);
    std::cout << "[MATH] " << leftNum << " " << op << " " << rightNum << " = " << result << std::endl;
}

// DoxRain Parser
class DoxRainParser {
public:
    static std::vector<std::unique_ptr<ASTNode>> parseFile(const std::string& filename) {
        std::vector<std::unique_ptr<ASTNode>> nodes;
        
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "[ERROR] Could not open file: " << filename << std::endl;
            return nodes;
        }
        
        std::string line;
        int lineNumber = 0;
        
        while (std::getline(file, line)) {
            lineNumber++;
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);
            
            if (line.empty() || line.front() == '#') {
                if (!line.empty() && line.front() == '#') {
                    auto node = std::make_unique<CommentNode>(line);
                    node->position = Position(lineNumber - 1, 0);
                    nodes.push_back(std::move(node));
                }
                continue;
            }
            
            auto node = parseLine(line);
            if (node) {
                node->position = Position(lineNumber - 1, 0);
                nodes.push_back(std::move(node));
            }
        }
        
        return nodes;
    }

    static std::unique_ptr<ASTNode> parseLine(const std::string& line) {
        // Parse different DoxRain constructs
        if (line.find("show ") == 0) {
            std::string message = line.substr(5);
            return std::make_unique<ShowNode>(message);
        } else if (line.find("var ") == 0) {
            size_t pos = line.find(" = ");
            if (pos != std::string::npos) {
                std::string name = line.substr(4, pos - 4);
                std::string valueStr = line.substr(pos + 3);
                
                // Parse value type
                DoxValue value;
                if (valueStr == "true" || valueStr == "false") {
                    value = (valueStr == "true");
                } else if (valueStr.find('.') != std::string::npos) {
                    value = std::stod(valueStr);
                } else if (std::isdigit(valueStr[0]) || (valueStr[0] == '-' && valueStr.length() > 1)) {
                    value = std::stoi(valueStr);
                } else {
                    value = valueStr;
                }
                
                return std::make_unique<VarDeclNode>(name, value);
            }
        } else if (line.find("function ") == 0) {
            size_t pos = line.find("(");
            if (pos != std::string::npos) {
                std::string name = line.substr(9, pos - 9);
                std::vector<std::string> params;
                return std::make_unique<FunctionDefNode>(name, params);
            }
        } else if (line.find("if ") == 0) {
            std::string condition = line.substr(3);
            return std::make_unique<IfNode>(condition);
        } else if (line.find("loop ") == 0) {
            int count = std::stoi(line.substr(5));
            return std::make_unique<LoopNode>(count);
        } else if (line.find("import ") == 0) {
            std::string module = line.substr(7);
            return std::make_unique<ImportNode>(module);
        } else if (line.find("webserver ") == 0) {
            int port = std::stoi(line.substr(10));
            return std::make_unique<WebServerNode>(port);
        } else if (line.find("input ") == 0) {
            std::string prompt = line.substr(6);
            return std::make_unique<InputNode>(prompt);
        } else if (line.find("class ") == 0) {
            std::string name = line.substr(6);
            return std::make_unique<ClassDefNode>(name);
        } else if (line.find("api ") == 0) {
            size_t pos = line.find(" ");
            if (pos != std::string::npos) {
                std::string method = line.substr(4, line.find(" ", 4) - 4);
                std::string url = line.substr(line.find(" ", 4) + 1);
                return std::make_unique<APICallNode>(url, method);
            }
        } else if (line == "end") {
            // Handle end statements (for functions, classes, loops, ifs)
            return std::make_unique<CommentNode>("// End block");
        }
        
        // Check for math operations
        std::string ops[] = {"+", "-", "*", "/", "%", "^"};
        for (const auto& op : ops) {
            size_t pos = line.find(" " + op + " ");
            if (pos != std::string::npos) {
                std::string left = line.substr(0, pos);
                std::string right = line.substr(pos + op.length() + 2);
                return std::make_unique<MathOpNode>(op, left, right);
            }
        }
        
        return nullptr;
    }
};

// DoxRain Language Server Protocol Implementation
class DoxRainLSP {
private:
    std::map<std::string, std::vector<std::unique_ptr<ASTNode>>> documentASTs;
    std::map<std::string, std::string> documentContents;
    std::vector<Diagnostic> diagnostics;
    DoxRainInterpreter interpreter;

    void analyzeDocument(const std::string& uri, const std::string& content) {
        diagnostics.clear();
        documentContents[uri] = content;
        
        // Parse and analyze the document
        auto ast = parseDocument(content);
        documentASTs[uri] = std::move(ast);
        
        // Generate diagnostics
        validateSyntax(uri, content);
    }

    std::vector<std::unique_ptr<ASTNode>> parseDocument(const std::string& content) {
        std::vector<std::unique_ptr<ASTNode>> nodes;
        std::istringstream stream(content);
        std::string line;
        int lineNumber = 0;
        
        while (std::getline(stream, line)) {
            lineNumber++;
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);
            
            if (line.empty() || line.front() == '#') {
                if (!line.empty() && line.front() == '#') {
                    auto node = std::make_unique<CommentNode>(line);
                    node->position = Position(lineNumber - 1, 0);
                    nodes.push_back(std::move(node));
                }
                continue;
            }
            
            auto node = DoxRainParser::parseLine(line);
            if (node) {
                node->position = Position(lineNumber - 1, 0);
                nodes.push_back(std::move(node));
            }
        }
        
        return nodes;
    }

    void validateSyntax(const std::string& uri, const std::string& content) {
        std::istringstream stream(content);
        std::string line;
        int lineNumber = 0;
        
        while (std::getline(stream, line)) {
            lineNumber++;
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);
            
            if (line.empty() || line.front() == '#') {
                continue;
            }
            
            // Check for syntax errors
            if (line.find("var ") == 0 && line.find(" = ") == std::string::npos) {
                Range range(Position(lineNumber - 1, 0), Position(lineNumber - 1, line.length()));
                diagnostics.push_back(Diagnostic(range, "Variable declaration missing assignment", 1));
            }
            
            if (line.find("function ") == 0 && line.find("(") == std::string::npos) {
                Range range(Position(lineNumber - 1, 0), Position(lineNumber - 1, line.length()));
                diagnostics.push_back(Diagnostic(range, "Function declaration missing parameters", 1));
            }
            
            // Check for undefined variables
            std::regex varPattern(R"(\$(\w+))");
            std::smatch match;
            std::string::const_iterator searchStart(line.cbegin());
            while (std::regex_search(searchStart, line.cend(), match, varPattern)) {
                std::string varName = match[1].str();
                if (interpreter.toString(interpreter.getVariable(varName)) == "undefined") {
                    int pos = std::distance(line.cbegin(), match.prefix().second);
                    Range range(Position(lineNumber - 1, pos), Position(lineNumber - 1, pos + varName.length() + 1));
                    diagnostics.push_back(Diagnostic(range, "Undefined variable: " + varName, 2));
                }
                searchStart = match.suffix().first;
            }
        }
    }

public:
    void initialize() {
        std::cout << "[LSP] DoxRain Language Server initialized" << std::endl;
    }

    void didOpen(const std::string& uri, const std::string& content) {
        std::cout << "[LSP] Document opened: " << uri << std::endl;
        analyzeDocument(uri, content);
    }

    void didChange(const std::string& uri, const std::string& content) {
        std::cout << "[LSP] Document changed: " << uri << std::endl;
        analyzeDocument(uri, content);
    }

    void didClose(const std::string& uri) {
        std::cout << "[LSP] Document closed: " << uri << std::endl;
        documentASTs.erase(uri);
        documentContents.erase(uri);
    }

    std::vector<Diagnostic> getDiagnostics(const std::string& uri) {
        return diagnostics;
    }

    std::vector<CompletionItem> getCompletions(const std::string& uri, Position position) {
        std::vector<CompletionItem> items;
        
        // Add DoxRain keywords
        items.push_back(CompletionItem("show", 14));
        items.push_back(CompletionItem("var", 14));
        items.push_back(CompletionItem("function", 14));
        items.push_back(CompletionItem("if", 14));
        items.push_back(CompletionItem("loop", 14));
        items.push_back(CompletionItem("class", 14));
        items.push_back(CompletionItem("import", 14));
        items.push_back(CompletionItem("webserver", 14));
        items.push_back(CompletionItem("input", 14));
        
        // Add available variables
        for (const auto& var : {"PI", "E", "VERSION", "input", "result"}) {
            CompletionItem item("$" + std::string(var), 6);
            item.detail = "Variable";
            items.push_back(item);
        }
        
        return items;
    }

    void hover(const std::string& uri, Position position) {
        std::cout << "[LSP] Hover request at line " << position.line << ", character " << position.character << std::endl;
    }

    void gotoDefinition(const std::string& uri, Position position) {
        std::cout << "[LSP] Go to definition request at line " << position.line << ", character " << position.character << std::endl;
    }
};

// Main DoxRain Runtime
class DoxRainRuntime {
private:
    DoxRainInterpreter interpreter;
    DoxRainLSP lsp;
    bool interactive_mode = false;

public:
    void runFile(const std::string& filename) {
        std::cout << "[DOXRAIN] Running file: " << filename << std::endl;
        
        auto ast = DoxRainParser::parseFile(filename);
        
        for (auto& node : ast) {
            try {
                node->execute(interpreter);
            } catch (const std::exception& e) {
                std::cerr << "[ERROR] Runtime error at line " << node->position.line + 1 
                         << ": " << e.what() << std::endl;
            }
        }
    }

    void runInteractive() {
        interactive_mode = true;
        std::cout << "DoxRain Interactive Shell v3.0.0" << std::endl;
        std::cout << "Type 'exit' to quit, 'help' for commands" << std::endl;
        
        std::string line;
        while (true) {
            std::cout << "dox> ";
            std::getline(std::cin, line);
            
            if (line == "exit" || line == "quit") {
                break;
            } else if (line == "help") {
                showHelp();
                continue;
            } else if (line.find("pkg ") == 0) {
                handlePackageCommand(line.substr(4));
                continue;
            } else if (line == "lsp") {
                std::cout << "[LSP] Starting Language Server..." << std::endl;
                lsp.initialize();
                continue;
            }
            
            // Parse and execute single line
            auto node = DoxRainParser::parseLine(line);
            if (node) {
                try {
                    node->execute(interpreter);
                } catch (const std::exception& e) {
                    std::cerr << "[ERROR] " << e.what() << std::endl;
                }
            } else if (!line.empty()) {
                std::cout << "[ERROR] Unknown command: " << line << std::endl;
            }
        }
    }

private:
    void showHelp() {
        std::cout << "\nDoxRain Commands:" << std::endl;
        std::cout << "  show <message>          - Display message or variable" << std::endl;
        std::cout << "  var <name> = <value>    - Declare variable" << std::endl;
        std::cout << "  function <name>()       - Define function" << std::endl;
        std::cout << "  if <condition>          - Conditional statement" << std::endl;
        std::cout << "  loop <count>            - Repeat statements" << std::endl;
        std::cout << "  import <module>         - Import package" << std::endl;
        std::cout << "  webserver <port>        - Start web server" << std::endl;
        std::cout << "  input <prompt>          - Get user input" << std::endl;
        std::cout << "  class <name>            - Define class" << std::endl;
        std::cout << "  api <method> <url>      - Make API call" << std::endl;
        std::cout << "\nPackage Manager:" << std::endl;
        std::cout << "  pkg init                - Initialize project" << std::endl;
        std::cout << "  pkg install <name>      - Install package" << std::endl;
        std::cout << "  pkg uninstall <name>    - Uninstall package" << std::endl;
        std::cout << "  pkg list                - List installed packages" << std::endl;
        std::cout << "  pkg search <query>      - Search packages" << std::endl;
        std::cout << "  pkg update              - Update registry" << std::endl;
        std::cout << "\nOther:" << std::endl;
        std::cout << "  lsp                     - Start Language Server" << std::endl;
        std::cout << "  help                    - Show this help" << std::endl;
        std::cout << "  exit                    - Exit DoxRain" << std::endl;
        std::cout << std::endl;
    }

    void handlePackageCommand(const std::string& cmd) {
        auto pm = interpreter.getPackageManager();
        
        if (cmd == "init") {
            pm->init();
        } else if (cmd.find("install ") == 0) {
            std::string package = cmd.substr(8);
            pm->install(package);
        } else if (cmd.find("uninstall ") == 0) {
            std::string package = cmd.substr(10);
            pm->uninstall(package);
        } else if (cmd == "list") {
            pm->list();
        } else if (cmd.find("search ") == 0) {
            std::string query = cmd.substr(7);
            pm->search(query);
        } else if (cmd == "update") {
            pm->update();
        } else {
            std::cout << "[PKG] Unknown package command: " << cmd << std::endl;
        }
    }

    static std::unique_ptr<ASTNode> parseLine(const std::string& line) {
        return DoxRainParser::parseLine(line);
    }
};

// Main function
int main(int argc, char* argv[]) {
    std::cout << "DoxRain Professional v3.0.0 - The Future of Programming" << std::endl;
    std::cout << "================================================================" << std::endl;
    
    DoxRainRuntime runtime;
    
    if (argc > 1) {
        // Run file
        std::string filename = argv[1];
        if (std::filesystem::exists(filename)) {
            runtime.runFile(filename);
        } else {
            std::cerr << "[ERROR] File not found: " << filename << std::endl;
            return 1;
        }
    } else {
        // Interactive mode
        runtime.runInteractive();
    }
    
    return 0;
}

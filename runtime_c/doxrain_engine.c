#include "include/doxrain_engine.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_VARS 128
#define MAX_NAME 64
#define MAX_VALUE 512

typedef struct { char name[MAX_NAME]; char value[MAX_VALUE]; } Variable;
typedef struct { Variable vars[MAX_VARS]; size_t count; const char *file; } Context;

static void trim(char *text) {
    size_t start = 0, length = strlen(text);
    while (start < length && isspace((unsigned char)text[start])) start++;
    while (length > start && isspace((unsigned char)text[length - 1])) text[--length] = '\0';
    if (start) memmove(text, text + start, length - start + 1);
}

static int line_error(Context *ctx, size_t line, const char *code, const char *message, const char *source) {
    fprintf(stderr, "error[%s]: %s\n --> %s:%zu:1\n  |\n%zu | %s\n  | ^\n", code, message, ctx->file, line, line, source);
    return 1;
}

static const char *lookup(Context *ctx, const char *name) {
    for (size_t i = ctx->count; i > 0; i--) if (strcmp(ctx->vars[i - 1].name, name) == 0) return ctx->vars[i - 1].value;
    return NULL;
}

static int valid_name(const char *name) {
    if (!name[0] || !(isalpha((unsigned char)name[0]) || name[0] == '_')) return 0;
    for (size_t i = 1; name[i]; i++) if (!(isalnum((unsigned char)name[i]) || name[i] == '_')) return 0;
    return 1;
}

static int number(const char *text, double *out) {
    char *end = NULL; *out = strtod(text, &end);
    while (end && isspace((unsigned char)*end)) end++;
    return end && end != text && *end == '\0';
}

static int evaluate(Context *ctx, char *text, char *out, size_t out_size, size_t line, const char *source) {
    trim(text);
    if (text[0] == '"') {
        size_t length = strlen(text);
        if (length < 2 || text[length - 1] != '"') return line_error(ctx, line, "E101", "unterminated string literal", source);
        text[length - 1] = '\0';
        snprintf(out, out_size, "%s", text + 1);
        for (size_t i = 0; out[i]; i++) {
            if (out[i] == '{') {
                char name[MAX_NAME] = {0}; size_t j = i + 1, n = 0;
                while (out[j] && out[j] != '}' && n + 1 < sizeof(name)) name[n++] = out[j++];
                if (out[j] != '}') return line_error(ctx, line, "E102", "unterminated interpolation; expected `}`", source);
                name[n] = '\0'; const char *value = lookup(ctx, name);
                if (!value) return line_error(ctx, line, "E103", "interpolation refers to an unknown variable", source);
                char expanded[MAX_VALUE]; snprintf(expanded, sizeof(expanded), "%.*s%s%s", (int)i, out, value, out[j + 1] ? out + j + 1 : "");
                snprintf(out, out_size, "%s", expanded);
            }
        }
        return 0;
    }
    const char *variable = lookup(ctx, text);
    if (variable) { snprintf(out, out_size, "%s", variable); return 0; }
    double left, right; char op = 0; char lhs[128], rhs[128];
    if (sscanf(text, " %127[^+*/-] %c %127s", lhs, &op, rhs) == 3 && strchr("+-*/", op)) {
        trim(lhs); trim(rhs);
        if (!number(lhs, &left) || !number(rhs, &right)) return line_error(ctx, line, "E104", "arithmetic operands must be numbers", source);
        if (op == '/' && right == 0) return line_error(ctx, line, "E105", "division by zero", source);
        double result = op == '+' ? left + right : op == '-' ? left - right : op == '*' ? left * right : left / right;
        snprintf(out, out_size, "%.15g", result); return 0;
    }
    if (number(text, &left) || strcmp(text, "true") == 0 || strcmp(text, "false") == 0) { snprintf(out, out_size, "%s", text); return 0; }
    return line_error(ctx, line, "E106", "unknown value or expression", source);
}

static int compile_line(Context *ctx, char *line_text, size_t line, const char *source, int check_only) {
    trim(line_text);
    if (!line_text[0] || line_text[0] == '#' || (line_text[0] == '/' && line_text[1] == '/')) return 0;
    if (strncmp(line_text, "let ", 4) == 0) {
        char *equals = strchr(line_text + 4, '=');
        if (!equals) return line_error(ctx, line, "E107", "expected `let name = value`", source);
        *equals = '\0'; char *name = line_text + 4; char value[MAX_VALUE]; trim(name);
        if (!valid_name(name)) return line_error(ctx, line, "E108", "invalid variable name", source);
        if (evaluate(ctx, equals + 1, value, sizeof(value), line, source)) return 1;
        if (check_only) return 0;
        if (ctx->count >= MAX_VARS) return line_error(ctx, line, "E109", "variable table is full", source);
        snprintf(ctx->vars[ctx->count].name, MAX_NAME, "%s", name); snprintf(ctx->vars[ctx->count].value, MAX_VALUE, "%s", value); ctx->count++;
        return 0;
    }
    if (strncmp(line_text, "show ", 5) == 0) {
        char value[MAX_VALUE]; if (evaluate(ctx, line_text + 5, value, sizeof(value), line, source)) return 1;
        if (!check_only) puts(value); return 0;
    }
    return line_error(ctx, line, "E110", "statement is not implemented by the Cherokee 0.5.0 C engine yet", source);
}

int doxrain_run_file(const char *path, int check_only) {
    FILE *file = fopen(path, "rb");
    if (!file) { fprintf(stderr, "error[E100]: cannot open source file `%s`\n", path); return 1; }
    Context ctx = {0}; ctx.file = path; char buffer[2048]; size_t line = 0; int failed = 0;
    while (fgets(buffer, sizeof(buffer), file)) { line++; char original[2048]; snprintf(original, sizeof(original), "%s", buffer); if (compile_line(&ctx, buffer, line, original, check_only)) { failed = 1; break; } }
    fclose(file);
    if (!failed && check_only) printf("OK: %s\n", path);
    return failed;
}

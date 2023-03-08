#pragma once

#ifdef USE_MODULES
import <vector>;
#else
#include <vector>
#endif

#include "symbol_table.hpp"
#include "Lexer.hpp"

NAMESPACE_BEGIN(script::detail)

NAMESPACE_BEGIN(Syntax)

enum struct Type : u8
{
    keyword,
    variable,
    character,
    value, // only string

    error,
};

NAMESPACE_END(Syntax)

struct Node
{
    Syntax::Type type;
    string_pool_view value;
};

struct ast_node
{
    u8 size;
    Node node[MAX_LINE_TOKEN];
};

struct Parser
{
public:
    const Lexer& lexer;
    symbol_table symbols;

public:
    Parser(const Lexer& lexer) : lexer{lexer} {}

    auto parse_global_scope(const line_payload& payload) const noexcept -> bool;

    auto parse_script_scope(const line_payload& payload) const noexcept -> bool;

    auto parse_line(const line_payload& payload,
        bool is_script_scope) const noexcept -> bool;

    auto generate_ast() noexcept -> std::vector<ast_node>;
};

NAMESPACE_END(script::detail)

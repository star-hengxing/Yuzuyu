#include <algorithm>

#include <fast_io.h>

#include "Parser.hpp"

using namespace script::detail;
using Type = Token::Type;

NAMESPACE_BEGIN(script::detail)

auto Parser::parse_global_scope(const line_payload& payload) const noexcept -> bool
{
    const auto [size, tokens] = payload;
    if (size == 2)
    {
        // exp -> keyword string
        const Type rule[]{Type::keyword, Type::string};
        const Type exp[]{tokens[0].type, tokens[1].type};
        return std::ranges::equal(exp, rule);
    }
    else if (size == 3)
    {
        // exp -> identifier = string
        const Type rule[]{Type::identifier, Type::assign, Type::string};
        const Type exp[]{tokens[0].type, tokens[1].type, tokens[2].type};
        return std::ranges::equal(exp, rule);
    }

    return false;
}

auto Parser::parse_script_scope(const line_payload& payload) const noexcept -> bool
{
    const auto [size, tokens] = payload;
    if (size == 1)
    {
        // exp -> string
        return tokens[0].type == Type::string
               || tokens[0].type == Type::keyword;
    }
    else if (size == 2)
    {
        // exp -> identifier string
        const Type rule1[]{Type::identifier, Type::string};
        // exp -> keyword string
        const Type rule2[]{Type::keyword, Type::string};
        const Type exp[]{tokens[0].type, tokens[1].type};
        return std::ranges::equal(exp, rule1) || std::ranges::equal(exp, rule2);
    }
    else if (size == 3)
    {
        // exp -> keyword identifier string
        const Type rule[]{Type::keyword, Type::identifier, Type::string};
        const Type exp[]{tokens[0].type, tokens[1].type, tokens[2].type};
        return std::ranges::equal(exp, rule);
    }

    return false;
}

auto Parser::parse_line(const line_payload& payload, bool is_script_scope) const noexcept -> bool
{
    if (payload.size == 0)
        return false;

    return is_script_scope ? parse_script_scope(payload) : parse_global_scope(payload);
}

auto Parser::generate_ast() noexcept -> std::vector<ast_node>
{
    auto is_script_scope = false;
    auto ast = std::vector<ast_node>{};
    for (const auto& payload : lexer.tokens)
    {
        if (!parse_line(payload.token, is_script_scope)) [[unlikely]]
        {
            perrln("line ", payload.line, ": syntax error");
            return {};
        }
        // semantic analysis
        const auto size = payload.token.size;
        const auto tokens = payload.token.token;

        if (!is_script_scope) [[unlikely]]
        {
            Symbol symbol;
            symbol.key = tokens[0].string;
            if (size == 2)
            {
                // scene
                is_script_scope = true;
                symbol.value = tokens[1].string;
                // mutiple define
                if (symbol.value.empty()
                    || symbol.key.get_view(lexer.string_pool) != KEYWORDS[0]
                    || !symbols.push_back(symbol, lexer.string_pool))
                {
                    return {};
                }
            }
            else if (size == 3)
            {
                // character
                symbol.value = tokens[2].string;
                // mutiple define
                if (symbol.value.empty() && symbols.contain(symbol.key, lexer.string_pool))
                    return {};

                symbols.table.push_back(symbol);
            }
            else
            {
                // syntax already checked
                global::unreachable();
            }
            // global not generate node, just add symbol in symbol table.
            continue;
        }

        ast_node nodes;
        nodes.size = size;

        if (size == 1)
        {
            if (tokens[0].type == Token::Type::keyword)
            {
                nodes.node[0].type = Syntax::Type::keyword;
            }
            else if (tokens[0].type == Token::Type::string)
            {
                nodes.node[0].type = Syntax::Type::value;
                nodes.node[0].value = tokens[0].string;
            }
            else
            {
                // syntax already checked
                global::unreachable();
            }
        }
        else if (size == 2)
        {
            nodes.node[0].type = Syntax::Type::keyword;
            nodes.node[0].value = tokens[0].string;
            nodes.node[1].type = Syntax::Type::value;
            nodes.node[1].value = tokens[1].string;
            if (tokens[0].type == Token::Type::identifier)
            {
                if (!symbols.contain(tokens[0].string, lexer.string_pool))
                    return {};

                nodes.node[0].type = Syntax::Type::character;
            }
            else
            {
                auto symbol = Symbol{tokens[0].string, tokens[1].string};
                auto is_scene = symbol.key.get_view(lexer.string_pool) == KEYWORDS[0];
                if (is_scene)
                {
                    // mutiple define
                    if (symbol.value.empty()
                        || !symbols.push_back(symbol, lexer.string_pool))
                    {
                        return {};
                    }
                }
            }
        }
        else if (size == 3)
        {
            nodes.node[0].type = Syntax::Type::keyword;
            nodes.node[0].value = tokens[0].string;
            nodes.node[1].type = Syntax::Type::variable;
            nodes.node[1].value = tokens[1].string;
            nodes.node[2].type = Syntax::Type::value;
            nodes.node[2].value = tokens[2].string;
        }
        else
        {
            // syntax already checked
            global::unreachable();
        }

        ast.push_back(nodes);
    }

    return ast;
}

NAMESPACE_END(script::detail)

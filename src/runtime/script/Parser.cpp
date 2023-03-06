#include <fast_io.h>

#include <runtime/helper/range.hpp>
#include "Parser.hpp"

using namespace script::detail;
using Type = Token::Type;

static constexpr std::string_view KEYWORDS[]
{
    "scene",
    "goto",
    "set",
    "end",
};

NAMESPACE_BEGIN()

auto is_keyword(const std::string_view key) noexcept -> bool
{
    for (auto i : KEYWORDS)
    {
        if (key == i)
        {
            return true;
        }
    }
    return false;
}

NAMESPACE_END()

NAMESPACE_BEGIN(script::detail)

auto Parser::parse_global_scope(const line_payload& payload) noexcept -> bool
{
    const auto [size, tokens] = payload;
    if (size != 2 && size != 3)
        return false;

    Type type = tokens[0].type;
    if (size == 2)
    {
        // symbol string
        if (type == Type::symbol && tokens[1].type == Type::string)
        {
            const auto symbol = Symbol{tokens[0].string, tokens[1].string};
            if (!symbol.value.empty()
                && symbol.key.get_view(lexer.string_pool) == KEYWORDS[0])
            {
                return symbols.push_back(symbol, lexer.string_pool);
            }
        }
    }
    else if (size == 3)
    {
        // symbol = string
        if (type == Type::symbol
            && tokens[1].type == Type::assign
            && tokens[2].type == Type::string)
        {
            const auto symbol = Symbol{tokens[0].string, tokens[2].string};
            const auto key = symbol.key.get_view(lexer.string_pool);
            // new character
            if (!is_keyword(key) && !symbol.value.empty()
                && !symbols.contain(symbol.key, lexer.string_pool))
            {
                return symbols.push_back(symbol, lexer.string_pool);
            }
        }
    }

    return false;
}

auto Parser::parse_script_scope(const line_payload& payload) noexcept -> bool
{
    const auto [size, tokens] = payload;
    Type type = tokens[0].type;
    if (size == 1)
    {
        // string
        if (type == Type::string)
            return true;
    }
    else if (size == 2)
    {
        // symbol string
        if (type == Type::symbol && tokens[1].type == Type::string)
        {
            const auto symbol = Symbol{tokens[0].string, tokens[1].string};
            const auto key = symbol.key.get_view(lexer.string_pool);
            if (symbols.contain(symbol.key, lexer.string_pool) || is_keyword(key))
                return true;
        }
    }
    else if (size == 3)
    {
        // symbol symbol string
        if (type == Type::symbol
            && tokens[1].type == Type::symbol
            && tokens[2].type == Type::string)
        {
            return true;
        }
    }

    return false;
}

auto Parser::parse_line(const line_payload& payload, bool is_script_scope) noexcept
    -> bool
{
    if (payload.size == 0)
        return false;

    return is_script_scope ? parse_script_scope(payload) : parse_global_scope(payload);
}

auto Parser::parse() noexcept -> bool
{
    auto is_script_scope = false;
    for (const auto& payload : lexer.tokens)
    {
        if (!parse_line(payload.token, is_script_scope)) [[unlikely]]
        {
            perrln("error from line ", payload.line);
            return false;
        }

        if (!is_script_scope) [[unlikely]]
        {
            // is scene?
            if (payload.token.size == 2)
            {
                is_script_scope = true;
                continue;
            }
        }
    }

    return true;
}

NAMESPACE_END(script::detail)

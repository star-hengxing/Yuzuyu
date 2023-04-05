#include <fast_io.h>

#include <runtime/base/debug.hpp>
#include <runtime/io/io.hpp>
#include "Lexer.hpp"

NAMESPACE_BEGIN(script::detail)

struct Context
{
private:
    using Self = Context;

public:
    const char* current;
    Token::Type type;
    string_pool_view literal;

    auto set(const char* ptr) noexcept -> Self&
    {
        current = ptr;
        return *this;
    }

    auto get() const noexcept -> char
    {
        return *current;
    }

    auto next() noexcept -> Self&
    {
        current += 1;
        return *this;
    }
};

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

NAMESPACE_END(script::detail)

NAMESPACE_BEGIN()

constexpr auto is_letter(char c) noexcept -> bool
{
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

constexpr auto is_number(char c) noexcept -> bool
{
    return c >= '0' && c <= '9';
}

constexpr auto is_alpha(char c) noexcept -> bool
{
    return is_letter(c) || is_number(c);
}

using namespace ::script::detail;

constexpr auto skip_ws(Context* context) noexcept -> void
{
    char c = context->get();
    auto ptr = context->current;

    while (c != 0 && (c == ' ' || c == '\t'))
    {
        ptr += 1;
        c = *ptr;
    }

    context->set(ptr);
}

constexpr auto skip_line(Context* context) noexcept -> usize
{
    auto count = usize{0};
    char c = context->get();
    auto ptr = context->current;

    while (c != 0 && (c == '\n' || c == '\r'))
    {
        ptr += 1;
        c = *ptr;
        if (c == '\n')
        {
            count += 1;
        }
    }
    context->set(ptr);
    return count;
}

constexpr auto skip_comment(Context* context) noexcept -> void
{
    char c = context->get();
    auto ptr = context->current;

    while (c != 0 && (c != '\n' && c != '\r'))
    {
        ptr += 1;
        c = *ptr;
    }
    context->set(ptr);
}

NAMESPACE_END()

NAMESPACE_BEGIN(script::detail)

auto Lexer::parse_string(Context* context) noexcept -> bool
{
    auto size = usize{0};
    const auto start = string_pool.size();

    auto is_escape = false;
    // after "
    auto ptr = context->next().current;
    for (;; ptr += 1)
    {
        char c = *ptr;
        if (c == 0) [[unlikely]]
        {
            context->type = Token::Type::error;
            return false;
        }

        if (!is_escape && c == '\"') [[unlikely]]
        {
            break;
        }

        if (c == '\\') [[unlikely]]
        {
            is_escape = true;
            continue;
        }

        if (is_escape) [[unlikely]]
        {
            is_escape = false;
            if (c == '{' || c == '}')
            {
                size += 1;
                string_pool.push_back('\\');
            }

            if (c != '"' || c != '\\')
            {
                context->type = Token::Type::error;
                return false;
            }
        }

        size += 1;
        string_pool.push_back(c);
    }

    context->set(ptr + 1);
    context->literal = {start, size};
    return true;
}

auto Lexer::parse_identifier(Context* context) noexcept -> bool
{
    auto size = usize{0};
    const auto start = string_pool.size();

    auto is_first = true;
    auto ptr = context->current;

    for (;; size += 1, ptr += 1)
    {
        const char c = *ptr;
        if (c == 0) [[unlikely]]
        {
            break;
        }

        if (!is_first) [[likely]]
        {
            if (!is_alpha(c) && c != '_')
            {
                break;
            }
        }
        else
        {
            is_first = false;
            if (is_number(c) && !is_letter(c) && c != '_')
            {
                context->type = Token::Type::error;
                return false;
            }
        }
        // check valid identifier
        if (size >= MAX_TOKEN_SIZE) [[unlikely]]
        {
            context->type = Token::Type::error;
            return false;
        }

        string_pool.push_back(c);
    }

    context->set(ptr);
    context->literal = {start, size};

    if (is_keyword(context->literal.get_view(string_pool)))
    {
        context->type = Token::Type::keyword;
    }
    return true;
}

auto Lexer::get_token(Context* context) noexcept -> bool
{
    const char c = context->get();
    if (is_letter(c) || c == '_')
    {
        context->type = Token::Type::identifier;
        return parse_identifier(context);
    }
    else if (c == '"')
    {
        context->type = Token::Type::string;
        return parse_string(context);
    }
    else if (c == '=')
    {
        context->next();
        context->type = Token::Type::assign;
        return true;
    }
    else
    {
        context->type = Token::Type::error;
        return false;
    }
}

auto Lexer::parse_line(const char* start, line_payload* payload) noexcept -> bool
{
    payload->size = 0;
    auto context = Context{start, Token::Type::error};

    while (true)
    {
        skip_ws(&context);
        char c = context.get();
        // start a line
        if (c == '#')
        {
            skip_comment(&context);
            c = context.get();
        }
        // next line
        if (c == '\r' || c == '\n')
        {
            skip_line(&context);
            continue;
        }

        if (c == 0) [[unlikely]]
        {
            return true;
        }

        if (!get_token(&context))
            return false;

        payload->token[payload->size] = {context.type, context.literal};
        payload->size += 1;

        if (payload->size >= MAX_LINE_TOKEN)
        {
            return false;
        }
    }

    return true;
}

auto Lexer::tokenization(const char* start) noexcept -> bool
{
    auto line = usize{1};
    auto context = Context{start, Token::Type::error};

    line_payload payload;
    payload.size = 0;

    while (true)
    {
        skip_ws(&context);
        char c = context.get();
        // start a line
        if (c == '#')
        {
            skip_comment(&context);
            c = context.get();
        }
        // next line
        if (c == '\r' || c == '\n')
        {
            const auto count = skip_line(&context);
            if (payload.size != 0)
            {
                tokens.push_back({line, payload});
                payload.size = 0;
            }
            line += count;
            continue;
        }

        if (c == 0) [[unlikely]]
            break;

        if (!get_token(&context))
            break;

        payload.token[payload.size] = {context.type, context.literal};
        payload.size += 1;

        if (payload.size >= MAX_LINE_TOKEN)
        {
            context.type = Token::Type::error;
            break;
        }
    }

    if (context.type == Token::Type::error)
    {
        perrln("error from line ", line);
        return false;
    }

    return true;
}

auto Lexer::parse(const std::string_view filename) noexcept -> bool
{
    if (filename.empty() && !filename.ends_with(".yu"))
        return false;

    auto file = io::file::fixed_buffer{nullptr, 0};

    try
    {
        // add '\0' end of line
        file = io::file::read_to_buffer(filename, true);
        if (file.size == 0 || !file.data)
            return false;
    }
    catch (fast_io::error& e)
    {
        perrln(e);
        return false;
    }
    // remove previous results
    tokens.clear();
    return tokenization(file.data.get());
}

NAMESPACE_END(script::detail)

#pragma once

#ifdef USE_MODULES
import <string_view>;
import <vector>;
#else
#include <string_view>
#include <vector>
#endif

#include "basic.hpp"

NAMESPACE_BEGIN(script::detail)

NAMESPACE_BEGIN(Token)

enum struct Type : u8
{
    identifier = 0,
    keyword,
    string,
    assign,

    error
};

NAMESPACE_END(Token)

struct token_payload
{
    Token::Type type;
    string_pool_view string;
};

struct line_payload
{
    u8 size;
    token_payload token[MAX_LINE_TOKEN];
};

struct Payload
{
    usize line;
    line_payload token;
};

struct Context;

struct Lexer
{
public:
    std::vector<char> string_pool;
    std::vector<Payload> tokens;

protected:
    auto parse_string(Context* context) noexcept -> bool;

    auto parse_identifier(Context* context) noexcept -> bool;

    auto get_token(Context* context) noexcept -> bool;

    auto tokenization(const char* start) noexcept -> bool;

public:
    auto parse_line(const char* start, line_payload* payload) noexcept -> bool;

    auto parse(const std::string_view filename) noexcept -> bool;
};

NAMESPACE_END(script::detail)

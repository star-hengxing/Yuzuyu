#pragma once

#ifdef USE_MODULES
import <vector>;
#else
#include <vector>
#endif

#include "symbol_table.hpp"
#include "Lexer.hpp"
#include "IR.hpp"

NAMESPACE_BEGIN(script::detail)

struct Parser
{
public:
    const Lexer& lexer;
    symbol_table symbols;
    std::vector<IR::Instruction> codes;

protected:
    auto parse_global_scope(const line_payload& payload) noexcept -> bool;

    auto parse_script_scope(const line_payload& payload) noexcept -> bool;

public:
    Parser(const Lexer& lexer) : lexer{lexer} {}

    auto parse_line(const line_payload& payload, bool is_script_scope) noexcept
        -> bool;

    auto parse() noexcept -> bool;
};

NAMESPACE_END(script::detail)

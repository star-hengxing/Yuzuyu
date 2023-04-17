#pragma once

#ifdef USE_MODULES
import <vector>;
#else
#include <vector>
#endif

#include "base.hpp"

NAMESPACE_BEGIN(script::detail)

struct Symbol
{
    string_pool_view key;
    string_pool_view value;
};
// mutiple map
struct symbol_table
{
public:
    std::vector<Symbol> table;

public:
    auto contain(const string_pool_view symbol,
        const std::vector<char>& string_pool) const noexcept -> bool;

    auto find(const string_pool_view symbol,
        const std::vector<char>& string_pool) const noexcept -> Symbol* const;

    auto push_back(const Symbol symbol,
        const std::vector<char>& string_pool) noexcept -> bool;
};

NAMESPACE_END(script::detail)

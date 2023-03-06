#include "symbol_table.hpp"

NAMESPACE_BEGIN(script::detail)

auto symbol_table::contain(const string_pool_view symbol,
    const std::vector<char>& string_pool) const noexcept -> bool
{
    return find(symbol, string_pool) != nullptr;
}

auto symbol_table::find(const string_pool_view symbol,
    const std::vector<char>& string_pool) const noexcept -> Symbol* const
{
    for (auto& entry : table)
    {
        if (entry.key.get_view(string_pool) == symbol.get_view(string_pool))
        {
            return const_cast<Symbol* const>(&entry);
        }
    }
    return nullptr;
}

auto symbol_table::push_back(const Symbol symbol,
    const std::vector<char>& string_pool) noexcept -> bool
{
    for (const auto& entry : table)
    {
        const auto key1 = entry.key.get_view(string_pool);
        const auto key2 = symbol.key.get_view(string_pool);
        const auto value1 = entry.value.get_view(string_pool);
        const auto value2 = symbol.value.get_view(string_pool);
        if (key1 == key2 && value1 == value2)
        {
            return false;
        }
    }

    table.push_back(symbol);
    return true;
}

NAMESPACE_END(script::detail)

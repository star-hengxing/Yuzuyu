#ifdef USE_MODULES
import <functional>;
#else
#include <functional>
#endif

struct Command
{
public:
    using Self = Command;
    using function_type = auto() -> void;
    using key_type = int;

private:
    key_type key = 0;
    std::function<function_type> fptr = nullptr;

public:
    Command() {}

    template <typename T>
    static auto make(key_type k, T function) -> Command
    {
        Command command;
        command.bind(k, function);
        return command;
    }

    template <typename T>
    auto bind(key_type k, T function) -> Self&
    {
        key = k;
        fptr = function;
        return *this;
    }

    auto unbind() -> void
    {
        key = 0;
        fptr = nullptr;
    }

    auto is_same(key_type input) -> bool
    {
        return input == key;
    }

    auto execute() const -> void
    {
        fptr();
    }
};

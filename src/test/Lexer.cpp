#include <boost/ut.hpp>
#include <fast_io.h>

#include <runtime/helper/range.hpp>
#include <runtime/script/Lexer.hpp>

using namespace boost::ut;
using namespace script::detail;

auto main() -> int
{
    "lexer error"_test = [&]
    {
        bool result;
        auto lexer = Lexer{};
        auto payload = line_payload{};

        auto test = [&](const char* str,
            const reflection::source_location& sl = reflection::source_location::current())
        {
            result = lexer.parse_line(str, &payload);
            expect(!result, sl);
            expect(payload.size == 0, sl);
        };

        test("123");
        test("123abcd");
        test("{}");
        test("[]");
        test("()");
        test("\"");

        auto str = std::string{};
        str.resize(MAX_TOKEN_SIZE + 1, 'a');
        test(str.c_str());
    };

    "lexer comment"_test = [&]
    {
        bool result;
        auto lexer = Lexer{};
        auto payload = line_payload{};

        auto test_first = [&](const char* str,
            const reflection::source_location& sl = reflection::source_location::current())
        {
            result = lexer.parse_line(str, &payload);
            expect(result, sl);
            expect(payload.size == 0, sl);
        };

        test_first("");
        test_first("# ");
        test_first("# \n");
        test_first("# \r\n");
        test_first("# hello");
        test_first("# 你好");
        test_first("# こにちは");

        auto test = [&](const char* str,
            const reflection::source_location& sl = reflection::source_location::current())
        {
            result = lexer.parse_line(str, &payload);
            expect(result, sl);
            expect(payload.size == 3, sl);
        };

        test("x = \"x\" # ");
        test("x = \"x\" # \n");
        test("x = \"x\" # \r\n");
        test("x = \"x\" # hello");
        test("x = \"x\" # 你好");
        test("x = \"x\" # こにちは");
    };

    "lexer token"_test = [&]
    {
        bool result;
        auto lexer = Lexer{};
        auto payload = line_payload{};

        auto test = [&](const char* str,
            const reflection::source_location& sl = reflection::source_location::current())
        {
            result = lexer.parse_line(str, &payload);
            expect(result, sl);
            expect(payload.size == 3, sl);
        };

        test("x = x");
        test("_x = _x");
        test("_123 = _123");
        test("x = \"x\"");
        test("\"x\" = x");
    };
}

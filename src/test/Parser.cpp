#include <boost/ut.hpp>

#include <core/base/range.hpp>
#include <function/script/Parser.hpp>

using namespace boost::ut;
using namespace script::detail;

auto main() -> int
{
    "parse global scope error"_test = [&]
    {
        bool result;
        auto lexer = Lexer{};
        auto payload = line_payload{};
        auto parser = Parser{lexer};

        auto test = [&](const char* str,
            const reflection::source_location& sl = reflection::source_location::current())
        {
            expect(lexer.parse_line(str, &payload), sl);
            result = parser.parse_line(payload, false);
            expect(!result, sl);
        };

        test("test =");
        test("= test");
        test("test = test");
        test("\"test\" =");
        test("\"test\" = test");
    };

    "parse script scope error"_test = [&]
    {
        bool result;
        auto lexer = Lexer{};
        auto payload = line_payload{};
        auto parser = Parser{lexer};

        auto test = [&](const char* str,
            const reflection::source_location& sl = reflection::source_location::current())
        {
            expect(lexer.parse_line(str, &payload), sl);
            result = parser.parse_line(payload, true);
            expect(!result, sl);
        };

        test("test =");
        test("= test");
        test("test = test");
        test("\"test\" =");
        test("= \"test\"");
        test("\"test\" = test");
        test("test = \"test\"");
        test("\"test\" \"test\"");
        test("\"test\" \"test\" \"test\"");
        test("\"test\" \"test\" test");
    };
    // exp -> identifier opt string
    // exp -> keyword string
    // opt -> =
    "parse global scope"_test = [&]
    {
        bool result;
        auto lexer = Lexer{};
        auto payload = line_payload{};
        auto parser = Parser{lexer};

        auto test = [&](const char* str,
            const reflection::source_location& sl = reflection::source_location::current())
        {
            expect(lexer.parse_line(str, &payload), sl);
            result = parser.parse_line(payload, false);
            expect(result, sl);
        };

        test("identifier=\"\"");
        test("identifier = \"\"");
        test("scene \"\"");
    };
    // exp -> string
    // exp -> identifier string
    // exp -> keyword string
    // exp -> keyword identifier string
    "parse script scope"_test = [&]
    {
        bool result;
        auto lexer = Lexer{};
        auto payload = line_payload{};
        auto parser = Parser{lexer};

        auto test = [&](const char* str,
            const reflection::source_location& sl = reflection::source_location::current())
        {
            expect(lexer.parse_line(str, &payload), sl);
            result = parser.parse_line(payload, true);
            expect(result, sl);
        };

        test("\"\"");
        test("identifier \"\"");
        test("goto \"other\"");
        test("set variable \"hello\"");
    };
}

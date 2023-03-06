#include <boost/ut.hpp>
#include <fast_io.h>

#include <runtime/helper/range.hpp>
#include <runtime/script/Parser.hpp>

using namespace boost::ut;
using namespace script::detail;

auto main() -> int
{
    "parse global scope error"_test = [&]
    {
        bool result;
        const char* str;
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
        // keyword
        test("goto = \"\"");
        test("set = \"\"");
        test("end = \"\"");
        // scene
        test("scene = \"\"");
        test("scene = \"scene\"");

        test("variable = \"\"");

        str = "scene \"scene\"";
        expect(lexer.parse_line(str, &payload));
        expect(parser.parse_line(payload, false));
        test(str);
        // character
        str = "character = \"character\"";
        expect(lexer.parse_line(str, &payload));
        expect(parser.parse_line(payload, false));
        test(str);
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
        test("\"test\" = test");

        test("test = \"test\"");
        // undefined character
        test("character = \"\"");
    };
}

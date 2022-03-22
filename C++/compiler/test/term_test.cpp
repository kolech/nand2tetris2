#include <filesystem>
#include <fstream>
#include "integer_term.hpp"
#include "gmock/gmock.h"
#include "string_term.hpp"
#include "tokenizer.hpp"
#include "utils.hpp"

using namespace ntt;
using namespace std;
using namespace testing;

namespace fs = std::filesystem;
const fs::path DATA_DIR = fs::path{TEST_DIR} / "data";

class FTerm : public Test {
    public:
        template <typename Parser>
        bool cmp_parser_output(string jack_file, string xml_file) {
            ifstream ifs { DATA_DIR / jack_file};
            Tokenizer tokenizer {ifs};

            fs::path output_file { DATA_DIR / "tmp.xml" };
            {
                ofstream ofs { output_file };
                ofs << Parser(tokenizer).to_xml();
            }

            return Utils::cmpFiles(output_file, DATA_DIR / xml_file);
        }
};

TEST_F(FTerm, ParsesInteger) {
    ASSERT_THAT(cmp_parser_output<IntegerTerm>("integer_term.jack", "integer_term.xml"), Eq(true));
}

TEST_F(FTerm, ParsesString) {
    ASSERT_THAT(cmp_parser_output<StringTerm>("string_term.jack", "string_term.xml"), Eq(true));
}

#ifndef __IF_STATEMENT_H__
#define __IF_STATEMENT_H__

#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <vector>
#include "expression.hpp"
#include "statement.hpp"
#include "token.hpp"
#include "tokenizer.hpp"

namespace ntt {

    class IfStatement : public Statement {
        public:
            IfStatement(Tokenizer&);

            std::string to_xml(size_t level = 0) const override;

        private:
            Token if_;

            Token left_parenthesis_;

            Expression expression_;

            Token right_parenthesis_;

            Token left_brace_;

            using StatementList = std::vector<std::unique_ptr<Statement>>;

            StatementList statements_;

            Token right_brace_;

            struct ElsePart {
                Token else_keyword;
                Token left_parenthesis;
                StatementList statements;
                Token right_parenthesis;
                ElsePart(Tokenizer&);
            };

            std::optional<ElsePart> else_;

            static StatementList parse_statements_(Tokenizer&);

            static std::optional<ElsePart> parse_else_part_(Tokenizer&);

            static void statements_to_xml(std::ostringstream&, const StatementList&, size_t);
    };
}

#endif

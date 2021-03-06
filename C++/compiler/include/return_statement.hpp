#ifndef __RETURN_STATEMENT_H__
#define __RETURN_STATEMENT_H__

#include <optional>
#include <string>
#include "expression.hpp"
#include "statement.hpp"
#include "token.hpp"
#include "tokenizer.hpp"

namespace ntt {

    /* represents a return statement */
    class ReturnStatement : public Statement {
        public:
            ReturnStatement(Tokenizer&);

            std::string to_xml(size_t level = 0) const override;

        private:
            Token return_;
            std::optional<Expression> expression_;
            Token semicolon_;

            static std::optional<Expression> parse_expression_(Tokenizer&);
    };
}

#endif

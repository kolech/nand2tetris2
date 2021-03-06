#ifndef __SUBROUTINE_BODY_H__
#define __SUBROUTINE_BODY_H__

#include <memory>
#include <string>
#include <vector>
#include "jack_fragment.hpp"
#include "token.hpp"
#include "tokenizer.hpp"
#include "statement.hpp"
#include "subroutine_var_dec.hpp"

namespace ntt {

    /* represents a subroutine body */
    class SubroutineBody : public JackFragment {
        public:
            SubroutineBody(Tokenizer&);

            std::string to_xml(size_t level = 0) const override;

        private:
            Token left_brace_;

            std::vector<SubroutineVarDec> var_decs_;

            using Statements = std::vector<std::unique_ptr<Statement>>;
            Statements statements_;

            Token right_brace_;

            static std::vector<SubroutineVarDec> parse_var_decs_(Tokenizer&);

            static Statements parse_statements_(Tokenizer&);
    };
}

#endif

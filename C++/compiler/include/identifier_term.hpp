#ifndef __IDENTIFIER_TERM_H__
#define __IDENTIFIER_TERM_H__

#include <string>
#include "term.hpp"
#include "token.hpp"
#include "tokenizer.hpp"

namespace ntt {

    class IdentifierTerm : public Term {
        public:
            IdentifierTerm(Tokenizer& tokenizer);

            std::string to_xml() const override;

        private:
            Token token_;
    };
}

#endif
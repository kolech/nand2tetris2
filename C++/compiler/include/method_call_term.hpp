#ifndef __METHOD_CALL_TERM_H__
#define __METHOD_CALL_TERM_H__

#include <string>
#include "function_call.hpp"
#include "token.hpp"
#include "tokenizer.hpp"
#include "expression_list.hpp"

namespace ntt {

    /* represents a class method call term */
    class MethodCallTerm : public Term {
        public:
            MethodCallTerm(Tokenizer& tokenizer);

            std::string to_xml(size_t level = 0) const override;

        private:
            Token var_name_;
            Token dot_;
            FunctionCall call_;
    };
}

#endif

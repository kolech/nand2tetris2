#include "integer_term.hpp"

namespace ntt {

    IntegerTerm::IntegerTerm(Tokenizer& tokenizer)
        : token_(tokenizer.consume_integer()) {}

    Term::Type IntegerTerm::get_type() const {
        return Term::Type::INTEGER;
    }

    std::string IntegerTerm::to_xml(size_t level) const {
        return Term::to_xml_(token_, level);
    }
}

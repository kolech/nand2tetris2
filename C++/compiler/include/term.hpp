#ifndef __TERM_H__
#define __TERM_H__

#include <string>
#include "jack_fragment.hpp"
#include "token.hpp"

namespace ntt {

    /* A base class for the various terms of Jack */
    class Term : public JackFragment {
        public:
            virtual std::string to_xml(size_t level = 0) const override = 0;

            virtual ~Term() = default;

        protected:
            /*
                Return a <term> xml element whose content is the xml
                representation of the supplied token.
            */
            static std::string to_xml_(const Token&, size_t level = 0);
    };
}

#endif

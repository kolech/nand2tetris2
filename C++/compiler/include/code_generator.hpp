#ifndef __CODE_GENERATOR_H__
#define __CODE_GENERATOR_H__

#include <cstdint>
#include <memory>
#include <string>
#include "types.hpp"
#include "symbol_table.hpp"

namespace ntt {

    class CodeGenerator {
        public:
            /* add the parameters in the symbol table and return their count */
            uint16_t compile(const ParameterList& param_list);

            /* add the parameters in the symbol table and return their count */
            uint16_t compile(const SubroutineVarDec& var_dec);
          
            /* compile the expression and return the corresponding vm code */
            void compile(const Expression& expression);

            /* compile the term and return the corresponding vm code */
            void compile(const std::unique_ptr<Term>& term);

            void compile(const ArrayTerm& term);

            void compile(const IdentifierTerm& term);

            void compile(const IntegerTerm& term);

            void compile(const KeywordTerm& term);


            void compile(const StringTerm& term);

            const SymbolTable& symbol_table() const;

            const std::vector<std::string>& vm_commands() const;

        private:

            static std::string segment(const SymbolKind& kind);

            SymbolTable symbol_table_;

            std::vector<std::string> vm_commands_;
    };
}

#endif

#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <fstream>
#include <string>
#include <unordered_set>

namespace ntt {

	enum class TokenType { UNKNOWN };

	class Token {
		public:
			explicit Token(const std::string& value,
                           const TokenType& type = TokenType::UNKNOWN)
                           : value_(value), type_(type) {}

			static Token parse(std::ifstream& ifs);
            
            std::string value() const;

            TokenType type() const;

		private:
			std::string value_;
			TokenType type_;
	};
}


#endif

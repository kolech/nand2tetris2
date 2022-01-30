#include <fstream>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include "parser.hpp"
#include "syntax_tree.hpp"
#include "token.hpp"
#include <iostream>

namespace ntt {

    /*
        term            : integerConstant | stringConstant | keywordConstant | varName |
                          varName '[' expresion ']' | subroutineCall | '(' expression ')' | unaryOp term
        keywordConstant : 'true' | 'false' | 'null' | 'this'
        subroutineCall  : subroutineName '(' expressionList ')' |
                          (className | varName) '.' subroutineName '(' expressionList ')'
        unaryOP         : '-' | '~'
        subroutineName  : identifier
        varName         : identifier
        className       : identifier
    */
    Tree Parser::parse_term() {
        if(!tokenizer.has_token())
            throw NoTokenErr();

        static std::unordered_set<std::string> keywords {"true", "false", "null", "this"};
        
        auto tree = std::make_unique<SyntaxTree>("term");

        auto token = tokenizer.get();
        switch(token.type()) {
            case TokenType::INTEGER:
            case TokenType::STRING:
                tree->add_child(std::make_unique<Leaf>(token));
            break;

            case TokenType::KEYWORD:
                if(!keywords.count(token.value()))
                    throw std::runtime_error("invalid keyword constant");
                else
                    tree->add_child(std::make_unique<Leaf>(token));
            break;

            case TokenType::IDENTIFIER:
                tree->add_child(std::make_unique<Leaf>(token));
                if(tokenizer.has_token()) {
                    if(tokenizer.peek().value() == "[") { // Array expression
                        tree->add_child(std::make_unique<Leaf>(tokenizer.get()));   // [
                        tree->add_child(parse_exp()); // exp
                        tree->add_child(std::make_unique<Leaf>(tokenizer.consume_symbol("]"))); // ]
                    }
                    else if(tokenizer.peek().value() == "(") { // subroutine call
                        tree->add_child(std::make_unique<Leaf>(tokenizer.get()));   // (
                        tree->add_child(parse_exp_list()); // exp list
                        tree->add_child(std::make_unique<Leaf>(tokenizer.consume_symbol(")"))); // )
                    }
                    else if(tokenizer.peek().value() == ".") { // method call
                        tree->add_child(std::make_unique<Leaf>(tokenizer.get()));   // .
                        tree->add_child(std::make_unique<Leaf>(tokenizer.consume_identifier()));    // method name
                        tree->add_child(std::make_unique<Leaf>(tokenizer.consume_symbol("(")));   // (
                        tree->add_child(parse_exp_list()); // exp list
                        tree->add_child(std::make_unique<Leaf>(tokenizer.consume_symbol(")"))); // )
                    }
                }
            break;

            case TokenType::SYMBOL:
                if(token.value() == "(") {
                    tree->add_child(std::make_unique<Leaf>(token)); // (
                    tree->add_child(parse_exp());   // exp
                    tree->add_child(std::make_unique<Leaf>(tokenizer.get())); // )
                }
                else if (token.value() == "-" || token.value() == "~") {
                    tree->add_child(std::make_unique<Leaf>(token)); // unaryOp
                    tree->add_child(parse_term());   // term 
                }
                else
                    throw std::runtime_error("invalid symbol token");
            break;

            default:
                return nullptr;
        }

        return tree;
    }

    /*
        expression : term (op term)*
        op : '+' |  '-' |  '*' |  '/' |  '&' |  '|' |  '<' |  '>' |  '='
    */

    Tree Parser::parse_exp() {
        static std::unordered_set<std::string> ops {
            "+" ,  "-" ,  "*" ,  "/" ,  "&" ,  "|" ,  "<" ,  ">" ,  "="
        };

        if(!tokenizer.has_token())
            throw NoTokenErr();

        auto tree = std::make_unique<SyntaxTree>("expression");
        tree->add_child(parse_term());

        while(true) {
            if(!tokenizer.has_token())
                break;

            const auto& token = tokenizer.peek();
            if(!ops.count(token.value()))
                break;

            tree->add_child(std::make_unique<Leaf>(tokenizer.get()));
            tree->add_child(parse_term());
        }

        return tree;
    }

    /*
        expressionList : (expression (',' expression)*)?
    */
    Tree Parser::parse_exp_list() {
        if(!tokenizer.has_token())
            throw NoTokenErr();

        auto tree = std::make_unique<SyntaxTree>("expressionList");
        /*
            expressionList is evaluated in the context of subroutine call; hence,
            by checking the value of the next token, it is possible to determine
            wether the expression list is empty or not.
        */
        if(tokenizer.peek().value() != ")") {
            tree->add_child(parse_exp());
            while(tokenizer.has_token() && tokenizer.peek().value() == ",") {
                tree->add_child(std::make_unique<Leaf>(tokenizer.get()));   // add ,
                tree->add_child(parse_exp());
            }
        }

        return tree;
    }

    /*
        letStatement : 'let' varName ('[' expression ']')? '=' expression ';'
        varName      : identifier
    */
    Tree Parser::parse_let_statement() {
        if(!tokenizer.has_token())
            throw NoTokenErr();

        auto tree = std::make_unique<SyntaxTree>("letStatement");

        tree->add_child(std::make_unique<Leaf>(tokenizer.consume_keyword({"let"})));
        tree->add_child(std::make_unique<Leaf>(tokenizer.consume_identifier()));
        if(tokenizer.peek().value() == "[") {
            tree->add_child(std::make_unique<Leaf>(tokenizer.get()));   // [
            tree->add_child(parse_exp());   // expression
            tree->add_child(std::make_unique<Leaf>(tokenizer.consume_symbol("]")));   // ]
        }
        tree->add_child(std::make_unique<Leaf>(tokenizer.consume_symbol("=")));   // =
        tree->add_child(parse_exp());   // expression
        tree->add_child(std::make_unique<Leaf>(tokenizer.consume_symbol(";")));   // ; 

        return tree;
    }

    /*
        doStatement : 'do' subroutineCall ';'
    */
    Tree Parser::parse_do_statement() {
        if(!tokenizer.has_token())
            throw NoTokenErr();

        auto tree = std::make_unique<SyntaxTree>("doStatement");
        tree->add_child(std::make_unique<Leaf>(tokenizer.consume_keyword({"do"})));   // do
        tree->add_child(std::make_unique<Leaf>(tokenizer.consume_identifier()));   // identifier 

        if(tokenizer.peek().value() == "(") { // subroutine call
            tree->add_child(std::make_unique<Leaf>(tokenizer.get()));   // (
            tree->add_child(parse_exp_list()); // exp list
            tree->add_child(std::make_unique<Leaf>(tokenizer.consume_symbol(")"))); // )
        }
        else if(tokenizer.peek().value() == ".") { // method call
            tree->add_child(std::make_unique<Leaf>(tokenizer.get()));   // .
            tree->add_child(std::make_unique<Leaf>(tokenizer.consume_identifier()));    // method name
            tree->add_child(std::make_unique<Leaf>(tokenizer.consume_symbol("(")));   // (
            tree->add_child(parse_exp_list()); // exp list
            tree->add_child(std::make_unique<Leaf>(tokenizer.consume_symbol(")"))); // )
        }
        else
            throw std::runtime_error("invalid token");

        tree->add_child(std::make_unique<Leaf>(tokenizer.consume_symbol(";"))); // ; 

        return tree;
    }


    /*
        returnStatement : 'return' expression? ';'
    */
    Tree Parser::parse_return_statement() {
        if(!tokenizer.has_token())
            throw NoTokenErr();

        auto tree = std::make_unique<SyntaxTree>("returnStatement");
        tree->add_child(std::make_unique<Leaf>(tokenizer.consume_keyword({"return"})));   // return 
        if(tokenizer.peek().value() != ";")
            tree->add_child(parse_exp());
        tree->add_child(std::make_unique<Leaf>(tokenizer.consume_symbol(";")));   // ; 

        return tree;
    }


    /*
    ifStatement : 'if' '(' expression ')' '{' statements '}'
                  ('else' '{' statements '}')?
    */
    Tree Parser::parse_if_statement() {
        if(!tokenizer.has_token())
            throw NoTokenErr();

        auto tree = std::make_unique<SyntaxTree>("ifStatement");

        tree->add_child(std::make_unique<Leaf>(tokenizer.consume_keyword({"if"})));   // if 
        tree->add_child(std::make_unique<Leaf>(tokenizer.consume_symbol("(")));   // ( 
        tree->add_child(parse_exp());   // expression
        tree->add_child(std::make_unique<Leaf>(tokenizer.consume_symbol(")"))); // )
        tree->add_child(std::make_unique<Leaf>(tokenizer.consume_symbol("{")));   // {

        auto stats_tree = parse_statements();
        if(stats_tree != nullptr)
            tree->add_child(std::move(stats_tree));
        tree->add_child(std::make_unique<Leaf>(tokenizer.consume_symbol("}")));   // } 

        if(tokenizer.has_token() && tokenizer.peek().value() == "else") {
            tree->add_child(std::make_unique<Leaf>(tokenizer.get()));   // else 
            tree->add_child(std::make_unique<Leaf>(tokenizer.consume_symbol("{")));   // {

            auto stats_tree = parse_statements();
            if(stats_tree != nullptr)
                tree->add_child(std::move(stats_tree));
            tree->add_child(std::make_unique<Leaf>(tokenizer.consume_symbol("}")));   // } 
        }

        return tree;
    }

    /*
       whileStatement : 'while' '(' expression ')' '{' statements '}' 
    */
    Tree Parser::parse_while_statement() {
        if(!tokenizer.has_token())
            throw NoTokenErr();

        auto tree = std::make_unique<SyntaxTree>("whileStatement");

        tree->add_child(std::make_unique<Leaf>(tokenizer.consume_keyword({"while"})));  // while
        tree->add_child(std::make_unique<Leaf>(tokenizer.consume_symbol("("))); // (
        tree->add_child(parse_exp());   // expression
        tree->add_child(std::make_unique<Leaf>(tokenizer.consume_symbol(")"))); // )
        tree->add_child(std::make_unique<Leaf>(tokenizer.consume_symbol("{"))); // {
        auto stats_tree = parse_statements();   // statements
        if(stats_tree != nullptr)
            tree->add_child(std::move(stats_tree)); 
        tree->add_child(std::make_unique<Leaf>(tokenizer.consume_symbol("}"))); // }

        return tree;
    }

    /*
        statements : statement*
    */
    Tree Parser::parse_statements() {
        /*
            statements are evaluated in the context of a block, that is, inside a brace pair ({}).
            Therefore, continue evaluating the token stream until '}' is encountered.
        */
        if(tokenizer.peek().value() != "}") {
            auto tree = std::make_unique<SyntaxTree>("statements");
            while(tokenizer.peek().value() != "}") {
                tree->add_child(parse_statement()); // parse statements
            }
            return tree;
        }

        return nullptr;
    }

    /*
        statement  : letStatement | ifStatement | whileStatement | doStatement | returnStatement
    */
    Tree Parser::parse_statement() {
        const static std::unordered_map<std::string, Tree(Parser::*)()> parsers {
            {"let", &Parser::parse_let_statement},
            {"if", &Parser::parse_if_statement},
            {"return", &Parser::parse_return_statement},
            {"do", &Parser::parse_do_statement},
            {"while", &Parser::parse_while_statement}
        };

        auto token = tokenizer.consume_keyword({"let", "if", "return", "do", "while"});
        tokenizer.put(token);

        auto parser = parsers.at(token.value());
        return (this->*parser)();
    }

    /*
        varDec  : 'var' type varName(','varName)*';'
        type    : 'int' | 'char' | 'boolean' | className
        varName : identifier
        className : identifier

    */
    Tree Parser::parse_var_dec() {
        if(!tokenizer.has_token())
            throw NoTokenErr();

        auto tree = std::make_unique<SyntaxTree>("varDec");
        tree->add_child(std::make_unique<Leaf>(tokenizer.consume_keyword({"var"})));    // var
        tree->add_child(std::make_unique<Leaf>(tokenizer.consume_type()));   // type 
        tree->add_child(std::make_unique<Leaf>(tokenizer.consume_identifier()));   //varName 

        while(tokenizer.has_token() && tokenizer.peek().value() != ";") {
            tree->add_child(std::make_unique<Leaf>(tokenizer.consume_symbol(",")));     // ,
            tree->add_child(std::make_unique<Leaf>(tokenizer.consume_identifier()));   //varName 
        }
        tree->add_child(std::make_unique<Leaf>(tokenizer.consume_symbol(";")));     // ;

        return tree;
    }
}

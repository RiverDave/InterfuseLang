#ifndef LEXER_H
#define LEXER_H
#include "AST.h"
#include "IteratorInterface.h"
#include "Token.h"
#include <fstream>
#include <optional>
#include <stack>
extern std::string _global_file_path;


class Lexer final {
private:
public:
    std::string input;
    IteratorWrapper _position;
    const char *file_name;
    bool verbose;

    std::stack<Token> assert_stk;


    explicit Lexer(const std::fstream &, const bool=false);

    [[nodiscard]]
    Token get_next_token();
    inline void move_itr_bounds();

    Token handle_operator(char current, char next, TOKEN_TYPE current_op, TOKEN_TYPE next_op);
    Token handle_matched_token(char, char, TOKEN_TYPE, TOKEN_TYPE);
    Token handle_reversed_matched_token(char, char, TOKEN_TYPE, TOKEN_TYPE);

    std::optional<char> peek_next_char();
    [[nodiscard]] Token invalidToken(std::string value = "\0") {
        return Token{INVALID, value, TokenLocation{this->_position.getLocation()}};
    };

    Token checkKeywordFromMap(
            const std::string &keyword,
            const std::unordered_map<std::string, TOKEN_TYPE> &mp);
};

//To be integrated with the parser(BISON)

static Lexer *lexerInstance = nullptr;


#endif

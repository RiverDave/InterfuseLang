#ifndef LEXER_H
#define LEXER_H
#include "AST.h"
#include "Token.h"
#include "IteratorInterface.h"
#include <fstream>
#include <optional>

extern const char *_global_file_path;


class Lexer final {
private:
public:
    std::string input;
    IteratorInterface _position;
    const char *file_name;

    explicit Lexer(const std::fstream &);

    [[nodiscard]]
    Token get_next_token();

    std::optional<char> peek_next_char();
    [[nodiscard]] Token invalidToken(std::string value = "\0") {
        return Token{INVALID, value, TokenLocation{this->_position.getLocation()}};
    };

    inline Token checkKeywordFromMap(
            const std::string &keyword,
            const std::unordered_map<std::string, TOKEN_TYPE> &mp);
};

//To be integrated with the parser(BISON)

static Lexer* lexerInstance = nullptr;
extern "C" int yylex();


#endif

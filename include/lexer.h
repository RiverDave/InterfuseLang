#ifndef LEXER_H
#define LEXER_H
#include "./AST.h"
#include "./Token.h"
#include <fstream>
#include <optional>
#include <vector>

extern const char* _global_file_name;

class Lexer final {
private:
public:
    std::string input;
    std::string::iterator _position;
    const char* file_name;

    explicit Lexer(const std::string &);
    explicit Lexer(const std::fstream &);

    [[nodiscard]]
    Token get_next_token();

    std::optional<char> check_next_char();

    //helps to classify each sub_token type
    //no discard is used to ensure that the return value is used
    //otherwise a warning will be issued
    [[nodiscard]]
    std::vector<Token> tokenize();
};

//To be integrated with the parser(BISON)

static Lexer *lexerInstance = nullptr;
extern "C" int yylex();


#endif

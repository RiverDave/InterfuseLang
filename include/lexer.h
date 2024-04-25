#ifndef LEXER_H
#define LEXER_H
#include "./Token.h"
#include "./AST.h"
#include <fstream>
#include <vector>





class Lexer final {
private:





public:

  std::string input;
  std::string::iterator _position;

  explicit Lexer(const std::string&);
  explicit Lexer(const std::fstream&);

  [[nodiscard]]
  Token get_next_token();

  //helps to classify each sub_token type
  //no discard is used to ensure that the return value is used
  //otherwise a warning will be issued
  [[nodiscard]]
  std::vector<Token> tokenize();
};

//To be integrated with the parser(BISON)

static Lexer* lexerInstance = nullptr;
extern "C" int yylex();


#endif

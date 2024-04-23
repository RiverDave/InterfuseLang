#ifndef LEXER_H
#define LEXER_H
#include "./Token.h"
#include "./AST.h"
#include <fstream>
#include <vector>





class Lexer final {
private:

  std::string input;
  std::string::iterator _position;

  // std::optional<Token> peek();
  // void consume();


public:


  explicit Lexer(const std::string&);
  explicit Lexer(const std::fstream&);
  Token get_next_token();

  //helps to classify each sub_token type
  //no discard is used to ensure that the return value is used
  //otherwise a warning will be issued
  [[nodiscard]]
  std::vector<Token> tokenize();

  //used to classify each sub_token type
  //inspired in pinecone
  void setup() noexcept;
};

//To be integrated with the parser(BISON)

static Lexer* lexerInstance = nullptr;
extern "C" int yylex();


#endif

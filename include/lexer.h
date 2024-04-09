#ifndef LEXER_H
#define LEXER_H
#include "./Token.h"
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>




class Lexer final {
private:


public:
  Token get_next_token();

  Lexer(const std::string_view);

  const std::string_view input;

  std::string_view::iterator _position;

  //Should tokenize input
  std::vector<Token> tokenize();

  //used to classify each sub_token type
  //inspired in pinecone
  void setup() noexcept;
};
#endif

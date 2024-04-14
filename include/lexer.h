#ifndef LEXER_H
#define LEXER_H
#include "./Token.h"
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>




class Lexer final {
private:

  const std::string_view input;
  std::string_view::iterator _position;
  Token get_next_token();
  // std::optional<Token> peek();
  // void consume();


public:


  explicit Lexer(const std::string_view);

  //helps to classify each sub_token type
  //no discard is used to ensure that the return value is used
  //otherwise a warning will be issued
  [[nodiscard]]
  std::vector<Token> tokenize();

  //used to classify each sub_token type
  //inspired in pinecone
  void setup() noexcept;
};
#endif

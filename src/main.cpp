#include <iostream>
#include <string>
#include "../include/Lexer.h"
#include "../include/Token.h"

int main(){

  // lex test
  const std::string text {
    "@rochi = 23; @num=45;  "
  };

  Lexer lex{text};
  std::vector<Token> tokens = lex.tokenize();

  for(auto &e : tokens){
    std::cout << e;
  }

  return 0;
}

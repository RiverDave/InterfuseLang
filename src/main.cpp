#include <iostream>
#include <string>
#include "../include/Lexer.h"
#include "../include/Token.h"

int main(){

  // input test
  const std::string text {
    "'langosta'"
    // "langosta"
    // "do while if else true false if @param"
  };

  Lexer lex{text};
  std::vector<Token> tokens = lex.tokenize();

  for(auto &e : tokens){
    std::cout << e;
  }

  return 0;
}

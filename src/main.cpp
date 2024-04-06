#include <iostream>
#include <string>
#include "../include/Lexer.h"
#include "../include/Token.h"

int main(){

  // lex test
  const std::string text {
    "$num : Int = 32"
  };



  Lexer lex;
  std::vector<Token> tokens = lex.tokenize(text);

  for(Token &e : tokens){
    std::cout << e;
  }


  return 0;
}

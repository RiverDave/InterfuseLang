#include "../include/Lexer.h"
#include "../include/Token.h"
#include <iostream>
#include <string>

int main() {

  // input test
  const std::string srccode = R"(
  # This is a single comment
  @var = 10
  @var3 = 23

  ```
  This is a huge comment

)";

  Lexer lex{srccode};
  std::vector<Token> tokens = lex.tokenize();

  //Ignore logging spaces
  for (Token &e : tokens) {
    if(e.getType() == TOKEN_TYPE::SPACE){
      continue;
    }
    std::cout << e;
  }

  return 0;
}

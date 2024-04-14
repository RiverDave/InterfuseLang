#include "../include/Lexer.h"
#include "../include/Token.h"
#include <iostream>
#include <string>

int main() {

  // input test
  const std::string srccode = R"(
# This is a comment
@var = "Main"
@name = "John"

@x = 10 * 10
)";

  //move so that it's not copied
  Lexer lex{std::move(srccode)};
  std::vector<Token> tokens = lex.tokenize();
  std::cout << "Input size: " << srccode.size() << "\n";

  // Ignore logging spaces since they bloat the output
  for (auto &e : tokens) {
    if (e.getType() == TOKEN_TYPE::SPACE) {
      continue;
    }
    std::cout << e;
  }

  return 0;
}

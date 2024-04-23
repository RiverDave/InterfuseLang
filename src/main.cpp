#include "../include/Lexer.h"
#include "../include/Token.h"
#include <fstream>
#include <iostream>
#include <string>

extern int yyparse();
extern void initializeLexer();

int main() {

//   // input test
//   const std::string srccode = R"(
// # This is a comment
// @var = "Main"
// @name = "John"
//
// {
//
//   procedure main() {
//     if (true) {
//       provide @name
//     }
// }
//
// @x = 10 * 10
// # This is another comment
// )";

  initializeLexer();
  yyparse();

  return 0;
}

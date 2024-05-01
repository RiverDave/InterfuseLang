#include "../include/Lexer.h"
#include "../include/Token.h"
#include <fstream>
#include <iostream>
#include <string>

extern int yyparse();
extern void initializeLexer();

int main() {

  // std::string src{"@var : int = 23;"};

  // initializeLexer();
  yyparse();

  return 0;
}

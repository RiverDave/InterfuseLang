#include "../include/AST.h"
#include "../include/IR.h"
#include "../include/Lexer.h"

extern int yyparse();
extern void initializeLexer();
extern NBlock *programBlock;

int main() {

  if (yyparse() == 0) {
    try {

      CodeGenContext context;
      context.emitIR(*programBlock);
      context.setTarget();
      context.runCode();

    } catch (const std::exception &e) {
      std::cout << e.what() << std::endl;
    }

  } else {

    std::cout << "Parsing failed" << std::endl;
  }

  return 0;
}

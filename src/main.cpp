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
      context.setTargets();
      context.emitIR(*programBlock);
      context.runCode();

    } catch (const std::runtime_error &e) {
      std::cerr << e.what() << std::flush;
    }

  } else {

    std::cerr << "Parsing failed" << std::endl;
  }

  return 0;
}


#include "../include/Lexer.h"
#include "../include/AST.h"
#include "../include/IR.h"

extern int yyparse();
extern void initializeLexer();
extern NBlock *programBlock;

int main() {

   if(yyparse() == 0 ){
    CodeGenContext context;
    context.emitIR(*programBlock);
    context.setTarget();
    context.runCode();
   } else {

     std::cout << "Parsing failed" << std::endl;
   }

  return 0;
}

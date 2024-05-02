#include "../include/Lexer.h"
#include "../include/AST.h"
#include "../include/IR.h"

extern int yyparse();
extern void initializeLexer();
extern NBlock *programBlock;

int main() {

  // std::string src{"@var : int = 23;"};

  // initializeLexer();
  // yyparse();


    // TheContext = std::make_unique<llvm::LLVMContext>();
    // Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);

    CodeGenContext context;
    //std::stack<int>test;

    const auto nint = std::make_unique<NInteger>(NInteger(10));
    const llvm::Value* value = nint->codeGen(context);

    //std::cout << "Generated value: " << value << std::endl;


  return 0;
}

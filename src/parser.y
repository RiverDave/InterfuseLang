%{ 

    #include "../include/Lexer.h"


//To be accessed in the lexer 


    NBlock *programBlock;

    //Lexer lex (std::fstream("../examples/test.pour"));
    //Lexer* lexerInstance = &lex;
    


    extern int yylex();
    void yyerror(const char* err){printf("ERROR: %s", err);}

    void initializeLexer(){
      printf("Initializing lexer\n");
      if(!lexerInstance){

        std::fstream file{"../examples/new.pour"};
        if (!file.is_open()) {
            perror("Failed to open file\n");
            // handle error
        } else {
            lexerInstance = new Lexer(file);
        }
        
      }

    }

%}



%union {

  Node *node;
  NBlock *block;
  NExpression *expr;
  NStatement *stmt;
  NIdentifier *id;
  NVariableDeclaration *var_decl;
  std::vector<NVariableDeclaration*> *var_decls;
  std::vector<NExpression*> *exprs;
  std::string *str;
  Token* token;

}

// These are specified in Token.h as well

//%token <token> KEYWORD KEYWORD_IF KEYWORD_ELSE KEYWORD_ELSE_IF KEYWORD_TRUE KEYWORD_FALSE
//%token <token> KEYWORD_LOOP_FOR KEYWORD_LOOP_DO KEYWORD_LOOP_WHILE 
//%token <token>  KEYWORD_PROCEDURE KEYWORD_PROVIDE

%token<token> TKNUMBER 
//%type<stmt> stmt
%type<expr> expr

     //| stmts stmt
     //;
//%start program

%%

// TKNUMBER represents the token obj in this case
expr: TKNUMBER 
    {
        auto val = new NInteger(std::stoi($1->getValue()));
        std::cout << val->getValue() << std::endl;
        delete val;

    }
    ;

%%


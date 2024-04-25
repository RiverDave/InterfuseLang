%{ 

    #include "../include/Lexer.h"


//To be accessed in the lexer 


    NBlock *programBlock;

    //Lexer lex (std::fstream("../examples/test.pour"));
    //Lexer* lexerInstance = &lex;
    


    extern int yylex();
    void yyerror(const char* err){printf("ERROR: %s \n", err);}

//TODO: pass fstream ot initialize lexer
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
  //std::string *str;
  NBlock *stmts;
  Token* token;


}

// These are specified in Token.h as well

//%token <token> KEYWORD KEYWORD_IF KEYWORD_ELSE KEYWORD_ELSE_IF KEYWORD_TRUE KEYWORD_FALSE
//%token <token> KEYWORD_LOOP_FOR KEYWORD_LOOP_DO KEYWORD_LOOP_WHILE 
//%token <token>  KEYWORD_PROCEDURE KEYWORD_PROVIDE
%token<token> TKNUMBER
%token<token> TKPLUS TKMINUS TKMULT TKDIV
%token<token> TKRETURN 
%token<token> TKLINEBREAK 
%token<token> TKASSIGNMENT 
//%token<token> TKSINGLECOMMENT TKMULTICOMMENT

//Involved in variables & functions
%token<token> TKIDENTIFIER 
//denote scopes/blocks
%token<token> TKCURLYOPEN TKCURLYCLOSE

%type<stmt> stmt 
%type<block> program stmts block
%type<var_decl> var_decl
//%type<var_decls> var_decls //not quite sure bout this
%type<expr> expr


%left TKPLUS TKMINUS
%right TKMULT TKDIV

%start program

%%

//Parser entry point
program:
    stmts 
    {
        programBlock = $1;
    } 
    ;

stmts: 
     stmt TKLINEBREAK
     {
        //Initialize new block
        //&& since block contains a statement list
        //push the statement into the block !!
        $$ = new NBlock();
        $$->statements.push_back($<stmt>1);

     } | 
     stmts stmt
     {
        //TODO: Elaborate further this solution(specifically)
        $$ = $<stmts>1;
        $1->statements.push_back($<stmt>2);
     }|

    stmts stmts
    {
    }|

    block
    {
    }
     ;

stmt:
    expr
    {
        $$ = new NExpressionStatement(*$1);
        std::cout << "Parsed exp" << std::endl;
    } |
    var_decl
    {
        //$$ = $1;
        std::cout << "Parsed var_decl" << std::endl;
    } |
    TKRETURN expr 
    {
        $$ = new NReturnStatement(*$2);
    }  
    ;

var_decl:

    TKIDENTIFIER 
    {
    
        //$$ = new NVariableDeclaration(*$1);
    } | 
    TKIDENTIFIER TKASSIGNMENT expr
    {
        //NOTE: This ast assumption is correct
        //$$ = new NVariableDeclaration(*$1, *$3);
    } 
    ;

block :
    TKCURLYOPEN stmt TKCURLYCLOSE
    {
        std::cout << "Parsed block " <<std::endl;
        //$$ = new NBlock();
        //$$->statements.push_back($2);
    } |
    TKCURLYOPEN stmts TKCURLYCLOSE
    {
        std::cout << "Parsed block " <<std::endl;
        //$$ = $2;
    } |
    TKCURLYOPEN expr TKCURLYCLOSE
    {
        std::cout << "Parsed block " <<std::endl;
        //$$ = new NBlock();
        //$$->statements.push_back(new NExpressionStatement(*$2));
    } |
    TKCURLYOPEN TKCURLYCLOSE
    {
        std::cout << "Parsed block " <<std::endl;
    //    $$ = new NBlock();
    }
    ;
// terminal obj (TKNUMBER) represents the token OBJECT preceded by the non-terminal obj (expr)
expr: TKNUMBER 
    {
    } |

    expr TKPLUS expr
    {

    } |
  
    expr TKMINUS expr
    {

    } |

    expr TKMULT expr
    {

    } |

    expr TKDIV expr
    {

    } 
    ;

%%


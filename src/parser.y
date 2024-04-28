%{ 

    #include "../include/Lexer.h"


//To be accessed in the lexer 


    NBlock *programBlock;

    //Lexer lex (std::fstream("../examples/test.pour"));
    //Lexer* lexerInstance = &lex;
    


    extern int yylex();
    void yyerror(const char* err){printf("ERROR: %s \n", err);}


%}


%union {

  Node *node;
  NBlock *block;
  NExpression *expr;
  NStatement *stmt;
  NIdentifier *id;
  NVariableDeclaration *var_decl;
	std::vector<NVariableDeclaration*> *varvec;
  std::vector<NExpression*> *exprs;

//an if stmts can contain multiple expressions and blocks
//This type will be changed later(its already defined in the ast)
  NExpression *if_stmt;
  NExpression *else_stmt;

  //for loop stmt
  NExpression *for_stmt;

  //std::string *str;
  NBlock *stmts;
  Token* token;


}

// These are specified in Token.h as well

//%token <token> KEYWORD KEYWORD_IF KEYWORD_ELSE KEYWORD_ELSE_IF KEYWORD_TRUE KEYWORD_FALSE
//%token <token> KEYWORD_LOOP_FOR KEYWORD_LOOP_DO KEYWORD_LOOP_WHILE 
//%token <token>  KEYWORD_PROCEDURE KEYWORD_PROVIDE
%token<token> TKNUMBER TKDOUBLE
%token<token> TKPLUS TKMINUS TKMULT TKDIV
%token<token> TKRETURN 
%token<token> TKCOMMA TKDOT TKARROW TKCOLON TKRANGE_INCLUSIVE 
%token<token> TKLINEBREAK  //statement delimiter
%token<token> TKASSIGNMENT 
//comparison operators
%token<token> TKLESS TKGREATER TKLESS_EQUAL TKGREATER_EQUAL TKEQUAL TKNOT_EQUAL
%token<token> TKAND TKOR
%token<token> TKNEGATION

%token<token> TKIDENTIFIER 
%token<token> TKDATATYPE 
%token<token> TKCURLYOPEN TKCURLYCLOSE
%token<token> TKPAROPEN TKPARCLOSE
%token<token> TKFUNCTION_KEY
%token<token> TKIF TKELSE TKELSEIF 
//loop stuff
%token<token> TKFOR TKIN

//data types


%type<id> id
//fn params are generally identifiers
%type<varvec> fn_args
//function call can be expressions
%type<exprs> fn_call_args
%type<stmt> stmt var_decl fn_decl 
%type<if_stmt> if_stmt else_stmt
%type<for_stmt> for_stmt
%type<block> program stmts block
//%type<var_decls> var_decls //not quite sure bout this
%type<expr> expr numeric comparison


%left TKPLUS TKMINUS
%right TKMULT TKDIV
%left TKLESS TKGREATER TKLESS_EQUAL TKGREATER_EQUAL TKEQUAL TKNOT_EQUAL

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
     //NOTE: This defines our statement delimiter (through ;)
     stmt TKLINEBREAK
     {
        //Initialize new block
        //&& since block contains a statement list
        //push the statement into the block !!
        $$ = new NBlock();
        $$->statements.push_back($<stmt>1);
     } | 
     stmts stmt TKLINEBREAK
     {
        //TODO: Elaborate further this solution(specifically)
        $$ = $<stmts>1;
        $1->statements.push_back($<stmt>2);
     } |

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
        std::cout << "Parsed return exp" << std::endl;
    } |

    fn_decl
    {
        //$$ = $1;
        std::cout << "Parsed fn_decl" << std::endl;
    } | 
    if_stmt
    {
        //$$ = $1;
        std::cout << "Parsed if_stmt" << std::endl;
    } |

    for_stmt
    {
        //$$ = $1;
        std::cout << "Parsed for_stmt" << std::endl;
    }
    ;

id : TKIDENTIFIER
    {
        //$$ = new NIdentifier(*$1);
//        std::cout << "Parsed id" << std::endl;
    }
    ;

var_decl:

    TKIDENTIFIER TKCOLON TKDATATYPE TKASSIGNMENT expr
    {
        //NOTE: This ast assumption is correct
        //handle data type as well inside ast
        //$$ = new NVariableDeclaration(*$1, *$3);
    } 

    |

//empty var_decl
    TKIDENTIFIER TKCOLON TKDATATYPE
    {
        //$$ = new NVariableDeclaration(*$1);
    }
    ;

fn_decl:
    TKFUNCTION_KEY id TKPAROPEN fn_args TKPARCLOSE TKARROW TKDATATYPE block
    {
        //$$ = new NFunctionDeclaration(*$1, *$3, $5);
    }
    ;

fn_args:
       //empty*
       {

       }|
    var_decl
    {
        //$$ = new std::vector<NVariableDeclaration*>();
        //$$->push_back(new NVariableDeclaration(*$1));
    } |
    fn_args TKCOMMA var_decl
    {

        //$$ = $1;
        //$$->push_back(new NVariableDeclaration(*$3));
    }
    ;

block :
    TKCURLYOPEN stmt TKCURLYCLOSE
    {
        //$$ = new NBlock();
        //$$->statements.push_back($2);
    } |
    TKCURLYOPEN stmts TKCURLYCLOSE
    {
        //$$ = $2;
    } |
    TKCURLYOPEN expr TKCURLYCLOSE
    {
        //$$ = new NBlock();
        //$$->statements.push_back(new NExpressionStatement(*$2));
    } |
    TKCURLYOPEN TKCURLYCLOSE
    {
    //    $$ = new NBlock();
    }
    ;

for_stmt:

//TODO: id wont be a var decl in this case but rather a local defined within the loop, in the ast 
//we should check if the id is defined within the current scope
//inclusive range: for @i in 1 := 10 {...
    TKFOR id TKIN expr TKRANGE_INCLUSIVE expr block
    {

        //$$ = new NForStatement(*$2, $4, $5);
    } | 

//Non inclusive range: for @i in 1 : 10 {...
    TKFOR id TKIN expr TKCOLON expr block
    {

        //$$ = new NForStatement(*$2, $4, $5);
    }
    ;

if_stmt:
    TKIF expr block 
    {
        //std::cout << "Parsed if_stmt" << std::endl;
        //$$ = new NIfStatement(*$2, $3, $4);
    } |

    TKIF expr block else_stmt
    {
        //$$ = new NIfStatement(*$2, $3, $4);
    }
    ;

else_stmt:
    TKELSE if_stmt
    {
       // std::cout << "Parsed else_if_stmt" << std::endl;
        //$$ = new NElseIfStatement(*$2);
    }
    |
    TKELSE block
    {
        //$$ = new NElseStatement($2);
    }
    |
    /* empty */
    {
        //$$ = nullptr;
    }
    ;

expr:

    id TKASSIGNMENT expr
    {

    } |  

    id TKPAROPEN fn_call_args TKPARCLOSE
    {
      std::cout << "Called fn" << std::endl;
    } |  

    id
    {
    std::cout << "Parsed id" << std::endl;
    //NOTE: Since this is being parsed as an exp and as well as a variable declaration
    //There should be a function that checks if the id is defined within current scope(locals)

    } |  

numeric 
    {

    } |

    expr TKMULT expr
    {

    } |

    expr TKDIV expr
    {

    } | 

    expr TKPLUS expr
    {

    } |
  
    expr TKMINUS expr
    {

    } |

    TKPAROPEN expr TKPARCLOSE
    {

    } |

    expr comparison expr
    {
      std::cout << "Parsed comparison" << std::endl;

    } |

    negation expr
    {
      std::cout << "Parsed negation" << std::endl;
    }
    ;

numeric:
    TKNUMBER
    {
    } | 
    TKDOUBLE
    {

    }
    ;

fn_call_args:

    {

    } | 
    expr
    {

    } |
    
    fn_call_args TKCOMMA expr
    {

      std::cout << "Parsed fn_call_args" << std::endl;
    }
    ;


negation : TKNEGATION
    {
    }
    ;

comparison: 
    TKLESS {  } 
    | TKGREATER {  } 
    | TKLESS_EQUAL {  } 
    | TKGREATER_EQUAL {  } 
    | TKEQUAL {  } 
    | TKNOT_EQUAL {  } 
    | TKAND {  } 
    | TKOR {  }
    ;

%%


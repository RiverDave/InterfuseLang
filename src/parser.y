%{ 

    #include "../include/Lexer.h"


//To be accessed in the lexer 


//Entry point for the parser(to be called in main by context)
    NBlock *programBlock;

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
  VariableList* varvec;
  std::vector<NExpression*> *exprs;

//an if stmts can contain multiple expressions and blocks
//This type will be changed later(its already defined in the ast)
  //NIfStatement *if_stmt;
  NElseStatement *else_stmt; // Change this line
  //for loop stmt
  //NStatement *for_stmt;

  NBlock *stmts;
  Token* token;


}

// These are specified in Token.h as well

//%token <token> KEYWORD KEYWORD_IF KEYWORD_ELSE KEYWORD_ELSE_IF KEYWORD_TRUE KEYWORD_FALSE
//%token <token> KEYWORD_LOOP_FOR KEYWORD_LOOP_DO KEYWORD_LOOP_WHILE 
//%token <token>  KEYWORD_PROCEDURE KEYWORD_PROVIDE
%token<token> TKNUMBER TKDOUBLE
%token<token> TKSTRING
%token<token> TKPLUS TKMINUS TKMULT TKDIV TKMOD
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
%token<token> TKFOR TKIN TKBREAK TKCONT

%type<id> id
%type<varvec> fn_args
%type<exprs> fn_call_args
%type<stmt> stmt var_decl fn_decl if_stmt for_stmt break_stmt continue_stmt
%type<else_stmt> else_stmt

//%type<for_stmt> for_stmt
%type<block> program stmts block
//%type<var_decls> var_decls //not quite sure bout this
%type<expr> expr numeric string 
%type<token> comparison negation


%left TKPLUS TKMINUS
%right TKMULT TKDIV TKMOD
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

      stmts error TKLINEBREAK
      {
        yyerror("Invalid Statement 1");
        yyerror;
      } |

      stmts block
      {
          $$ = $<stmts>1;
          $$ = $<block>2;
      } | 

      block
      {

      /* empty */

      }
       ;

stmt:

    expr
    {
        $$ = new NExpressionStatement(*$1);
    } |

    var_decl
    {
        //$$ = $<var_decl>1;
    } |

    TKRETURN expr 
    {
        $$ = new NReturnStatement($<expr>2);
    } |

    TKRETURN error 
    {
    yyerror("Expected expression after return");
    yyerror;

    } |

    TKRETURN
    {
        $$ = new NReturnStatement();
    } |

    fn_decl
    {
        //$$ = $1;
        //$$ = $<fn_decl>1;
    } | 
    if_stmt
    {
        //$$ = $<if_stmt>1;
    } |

    for_stmt
    {
        //$$ = $<for_stmt>1;
    } | 
    break_stmt
    {
        $$ = new NBreakStatement();


    } |
    continue_stmt
    {
        $$ = new NContinueStatement();
    }
//    |
//
 //   error
   // {
     //   yyerror("Invalid Statement 2");
       // yyerror;
   // }
    ;

id : TKIDENTIFIER
    {
        $$ = new NIdentifier($1->getValue());
        delete $1;
    }
    ;

var_decl:

    TKIDENTIFIER TKCOLON TKDATATYPE TKASSIGNMENT expr
    {
        NIdentifier* type = new NIdentifier($3->getValue().c_str());
        NIdentifier* id = new NIdentifier($1->getValue().c_str());

        $$ = new NVariableDeclaration(*type, id, $5);
    } 

    |

//empty var_decl
    TKIDENTIFIER TKCOLON TKDATATYPE
    {
        NIdentifier* type = new NIdentifier($3->getValue().c_str());
        NIdentifier* id = new NIdentifier($1->getValue().c_str());

        $$ = new NVariableDeclaration(*type, id);
    }
    ;

fn_decl:
    TKFUNCTION_KEY id TKPAROPEN fn_args TKPARCLOSE TKARROW TKDATATYPE block
    {
        NIdentifier* type = new NIdentifier($7->getValue().c_str());
        $$ = new NFnDeclaration(*$2, *$4, *type, $8);
    } | 

//Prototype declaration
    TKFUNCTION_KEY id TKPAROPEN fn_args TKPARCLOSE TKARROW TKDATATYPE 
    {
        NIdentifier* type = new NIdentifier($7->getValue().c_str());
        $$ = new NFnDeclaration(*$2, *$4, *type);
    }
    ;

fn_args:
       //empty*
       {
          $$ = new VariableList();

       }|
    var_decl
    {

//NOTE: Need to pass variable details before being able to push it to the vector
          $$ = new VariableList();
          $$->push_back(new NVariableDeclaration(*$<var_decl>1));
    } |
    fn_args TKCOMMA var_decl
    {
          $1->push_back(new NVariableDeclaration(*$<var_decl>3));
          //$1->push_back($3);
    }
    ;

block :
    TKCURLYOPEN stmt TKCURLYCLOSE
    {
        //$$ = $2;
        $$ = new NBlock();
        $$->statements.push_back($<stmt>2);
    } |
    TKCURLYOPEN stmts TKCURLYCLOSE
    {
        $$ = new NBlock();
        $$->statements.push_back($<stmt>2);
    } |
    TKCURLYOPEN expr TKCURLYCLOSE
    {
        $$ = new NBlock();
        $$->statements.push_back(new NExpressionStatement(*$<expr>2));
    } |
    TKCURLYOPEN TKCURLYCLOSE
    {
        $$ = new NBlock();
    }
    ;

for_stmt:

//TODO: id wont be a var decl in this case but rather a local defined within the loop, in the ast 
//we should check if the id is defined within the current scope
//inclusive range: for @i in @i < 10 := @i++ {...

//Non inclusive range: for @i in @i < 10 : @i = @i + 1 {...
    TKFOR id TKIN expr TKCOLON expr block
    {

        $$ = new NForStatement($2, $4, $6, $7);
    }
    ;

if_stmt:
    TKIF expr block 
    {

        $$ = new NIfStatement($<expr>2, $3);
    } |

    TKIF expr block else_stmt
    {
        //$$ = new NIfStatement($<expr>2, $3, $4);
        $$ = new NIfStatement($<expr>2, $3, $<else_stmt>4); 
    }
    ;

else_stmt:
    TKELSE if_stmt
    {
       $$ = $<else_stmt>2; 
       //$$ = $2;
    }
    |
    TKELSE block
    {

      $$ = new NElseStatement($2);
     //$$ = new NElseStatement($2);
    }
    |
    /* empty */
    {
        $$ = nullptr;
    }
    ;

break_stmt:
    TKBREAK
    ;

continue_stmt:
    TKCONT
    ;

expr:

    id TKASSIGNMENT expr
    {
    //Should call asignment from ast
      $$ = new NAssignment(*$1, *$3);

    } |  

    id TKPAROPEN fn_call_args TKPARCLOSE
    {
    // fn call basically
    $$ =  new NFnCall(*$<id>1, *$3);

    } |  

    id
    {
      $$ = $<id>1;

    //NOTE: Since this is being parsed as an exp and as well as a variable declaration
    //There should be a function that checks if the id is defined within current scope(locals)

    } |  

    numeric | 

    string |

    expr TKMULT expr
    {
      $$ = new NBinaryOperator(*$1, *$2, *$3);
      //delete $2;

    } |

    expr TKDIV expr
    {

      $$ = new NBinaryOperator(*$1, *$2, *$3);
      //delete $2;

    } | 

    expr TKPLUS expr
    {

      $$ = new NBinaryOperator(*$1, *$2, *$3);
      //delete $2;

    } |
  
    expr TKMINUS expr
    {

      $$ = new NBinaryOperator(*$1, *$2, *$3);
      //delete $2;

    } |

    expr TKMOD expr
    {
      $$ = new NBinaryOperator(*$1, *$2, *$3);
      //delete $2;

    } |

    TKPAROPEN expr TKPARCLOSE
    {
      $$ = $2;

    } |

    expr comparison expr
    {
      $$ = new NBinaryOperator(*$1, *$2, *$3);
      //delete $2;

    } |

    negation expr
    {
      $$ = new NUnaryOperator(*$1, *$2);
    }
    ;

numeric:
    TKNUMBER
    {

      $$ = new NInteger(std::atof($1->getValue().c_str()));
      delete $1;

    } | 
    TKDOUBLE
    {
      $$ = new NDouble(std::atof($1->getValue().c_str()));
      delete $1;


    }
    ;

string:
    TKSTRING
    {
      $$ = new NString($1->getValue());
      delete $1;
    }
    ;

fn_call_args:
    /* empty */
    {
        $$ = new ExpressionList();
    } |
    expr
    {
        $$ = new ExpressionList();
        $$->push_back($1);
    } |
    fn_call_args TKCOMMA expr
    {
        $1->push_back($3);
    }
    ;


negation : TKNEGATION
    ;

comparison: 
    TKLESS  
    | TKGREATER  
    | TKLESS_EQUAL  
    | TKGREATER_EQUAL  
    | TKEQUAL  
    | TKNOT_EQUAL  
    | TKAND  
    | TKOR 
    ;

%%


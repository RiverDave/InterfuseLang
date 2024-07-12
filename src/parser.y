%language "C++"
%require "3.7"
%define parser_class_name {fuse_parser}
%define api.value.type variant
%define parse.error verbose

%code requires {

    #include "Lexer.h"
    #include "AST.h"
    #include "FuseHandler.h"
    #include <iostream>
    #include <vector>
    #include <memory>
    #include <variant>

    using namespace std;
}

%code{

/*
void yy::fuse_parser::error (const string& m,

std::vector<std::unique_ptr<Token>> err_tokens = std::vector<std::unique_ptr<Token>>()) {

    if (err_tokens.size() >= 1){

      std::cerr << "INTERFUSE ERROR: " <<  err;
      TokenLocation err_range = fusehandler.getErrorLocation(std::move(err_tokens));
      std::cout << " At line: " << err_range << std::endl;
     }else{
      std::cerr << "INTERFUSE Untracked ERROR found -> " <<  err << std::endl;;
      getErrorCnt();

    }

  fusehandler.err_cnt++;
}
*/

    std::unique_ptr<NBlock> programBlock;
    FuseHandler fusehandler;
    int yylex(yy::fuse_parser::semantic_type* yylval);

    void getErrorCnt(){
      std::cout << "Aborting... Known Errors: " << fusehandler.err_cnt << std::endl;
    }


}

// These are specified in Token.h as well

%token<Token*> TKNUMBER TKDOUBLE
%token<Token*> TKSTRING
%token<Token*> TKPLUS TKMINUS TKMULT TKDIV TKMOD
%token<Token*> TKRETURN 
%token<Token*> TKCOMMA TKDOT TKARROW TKCOLON TKRANGE_INCLUSIVE 
%token<Token*> TKLINEBREAK  //statement delimiter
%token<Token*> TKASSIGNMENT 
//comparison operators
%token<Token*> TKLESS TKGREATER TKLESS_EQUAL TKGREATER_EQUAL TKEQUAL TKNOT_EQUAL
// Loop stuff
%token<Token*> TKAND TKOR

%token<Token*> TKNEGATION

%token<Token*> TKIDENTIFIER 
%token<Token*> TKDATATYPE 
%token<Token*> TKCURLYOPEN TKCURLYCLOSE
%token<Token*> TKPAROPEN TKPARCLOSE
%token<Token*> TKFUNCTION_KEY
%token<Token*> TKIF TKELSE TKELSEIF 
%token<Token*> TKFOR TKIN TKBREAK TKCONT
%token<Token*> TKINVALID


%type<Token*> comparison negation binary_op
%type<std::unique_ptr<NIdentifier>> id
%type<std::unique_ptr<VariableList>> fn_args
%type<unique_ptr<vector<unique_ptr<NExpression>>>> fn_call_args
%type<std::unique_ptr<NStatement>> stmt  fn_decl if_stmt for_stmt break_stmt continue_stmt
%type <std::unique_ptr<NVariableDeclaration>> var_decl
%type<std::unique_ptr<NElseStatement>> else_stmt

%type<std::unique_ptr<NBlock>> program stmts block
//%type<var_decls> var_decls //not quite sure bout this
%type<std::unique_ptr<NExpression>> expr numeric string




//Operator precedence
%left TKPLUS TKMINUS
%right TKMULT TKDIV TKMOD
%left TKLESS TKGREATER TKLESS_EQUAL TKGREATER_EQUAL TKEQUAL TKNOT_EQUAL

%start program

%%

//Parser entry point
program:
    stmts
    {
        programBlock = std::move($1);
    }
    ;

stmts:
     stmt 
     {
        //Initialize new block
        //&& since block contains a statement list
        //push the statement into the block !!
        $$ = std::make_unique<NBlock>();
        if($1 != nullptr)
        {
            $$->statements.push_back(std::move($1));
        } 

     } |

     stmt error
     {
        std::vector<std::unique_ptr<Token>> local_error;
        error("Error found in statement"/*, std::move(local_error)*/);
        $$ = nullptr;
        YYABORT;

     } |
     stmts stmt 
     {
        //TODO: Elaborate further this solution(specifically)
        $$ = std::move($1);

        if($2 != nullptr)
        {
          $1->statements.push_back(std::move($2));

        } else if($2 == nullptr){
          /*
          std::vector<std::unique_ptr<Token>> local_error;
          error("Missing ; Delimiter", std::move(local_error));// FIXME: Wrong error
          $$ = nullptr;
          getErrorCnt();
          YYABORT;
          */
        }


     } |

      stmts block
      {
          $$ = std::move($1);
          $$ = std::move($2);
      } | 

      block
      {

      /* empty */

      }
       ;


stmt:

    expr
    {
        if($1 != nullptr)
        {
            $$ = std::make_unique<NExpressionStatement>(std::move($1));
        }

    } |

    var_decl
    {
        //$$ = $<var_decl>1;
    } |

    TKRETURN expr
    {
        $$ = std::make_unique<NReturnStatement>(std::move($2));
    } |

    TKRETURN
    {
        $$ = std::make_unique<NReturnStatement>();
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
        $$ = std::make_unique<NBreakStatement>();


    } |
    continue_stmt
    {
        $$ = std::make_unique<NContinueStatement>();
    }
    ;

id : TKIDENTIFIER
    {
        $$ = std::make_unique<NIdentifier>($1->getValue());
        delete $1;
    }
    ;

var_decl:

    TKIDENTIFIER TKCOLON TKDATATYPE TKASSIGNMENT expr
    {

        if(!$5){
          std::vector<std::unique_ptr<Token>> local_error;
          local_error.push_back(std::make_unique<Token>(*$4));
          error("Invalid assignment expression to variable declaration"/*, std::move(local_error)*/);
          $$ = nullptr;
          getErrorCnt();
          YYABORT;

        }else{
          std::unique_ptr<NIdentifier> type = std::make_unique<NIdentifier>($3->getValue().c_str());
          std::unique_ptr<NIdentifier> id = std::make_unique<NIdentifier>($1->getValue().c_str());
          $$ = std::make_unique<NVariableDeclaration>(std::move(type), std::move(id), std::move($5));

        }


    } 

    |

//empty var_decl
    TKIDENTIFIER TKCOLON TKDATATYPE
    {
        std::unique_ptr<NIdentifier> type = std::make_unique<NIdentifier>($3->getValue().c_str());
        std::unique_ptr<NIdentifier> id = std::make_unique<NIdentifier>($1->getValue().c_str());

        $$ = std::make_unique<NVariableDeclaration>(std::move(type), std::move(id));
    }
    ;

fn_decl:
    TKFUNCTION_KEY id TKPAROPEN fn_args TKPARCLOSE TKARROW TKDATATYPE block
    {

        std::unique_ptr<NIdentifier>type = std::make_unique<NIdentifier>($7->getValue().c_str());
        $$ = std::make_unique<NFnDeclaration>(std::move($2), std::move($4), std::move(type), std::move($8));
    } | 

//Prototype declaration
    TKFUNCTION_KEY id TKPAROPEN fn_args TKPARCLOSE TKARROW TKDATATYPE 
    {
        std::unique_ptr<NIdentifier>type = std::make_unique<NIdentifier>($7->getValue().c_str());
        $$ = std::make_unique<NFnDeclaration>(std::move($2), std::move($4), std::move(type));
    }
    ;

fn_args:
       //empty*
   {
      $$ = std::make_unique<std::vector<std::unique_ptr<NVariableDeclaration>>>();//aka var list

   } |

    var_decl
    {
          $$->push_back(std::move($1));
    } |
    fn_args TKCOMMA var_decl
    {
          $1->push_back(std::move($3));
    }
    ;

block :
    TKCURLYOPEN stmt TKCURLYCLOSE
    {
        //$$ = $2;
        $$ = std::make_unique<NBlock>();
        $$->statements.push_back(std::move($2));
    } |
    TKCURLYOPEN stmts TKCURLYCLOSE
    {
        //risky
        //NOTE: NOT SURE BOUT THIS
        $$ = std::move($2);
        //$$ = std::make_unique<NBlock>();
        //auto stmt = std::move(std::get<std::unique_ptr<NStatement>>($2));
        //$$->statements.push_back(std::move(stmt));
    } |
    TKCURLYOPEN expr TKCURLYCLOSE
    {

        $$ = std::make_unique<NBlock>();
        $$->statements.push_back(std::make_unique<NExpressionStatement>(std::move($2)));
    } |
    TKCURLYOPEN TKCURLYCLOSE
    {
        $$ = std::make_unique<NBlock>();
    }
    ;

for_stmt:

//TODO: id wont be a var decl in this case but rather a local defined within the loop, in the ast 
//we should check if the id is defined within the current scope
//inclusive range: for @i in @i < 10 := @i++ {...

//Non inclusive range: for @i in @i < 10 : @i = @i + 1 {...
    TKFOR id TKIN expr TKCOLON expr block
    {

        $$ = std::make_unique<NForStatement>(std::move($2), std::move($4), std::move($6), std::move($7));
    } | 
    TKFOR  error
    {
        std::vector<std::unique_ptr<Token>> local_error;
        local_error.push_back(std::make_unique<Token>(*$1));
        error("Incorrect 'for' statement in for loop"/*, std::move(local_error)*/);
        $$ = nullptr;
        getErrorCnt();
        YYABORT;
    }
    ;

if_stmt:

    TKIF expr block 
    {

        if(!$2){
          std::vector<std::unique_ptr<Token>> local_error;
          local_error.push_back(std::make_unique<Token>(*$1));
          error("Invalid expression in if statement"/*, std::move(local_error)*/);
          $$ = nullptr;
          getErrorCnt();
          YYABORT;

        }
        
        $$ = std::make_unique<NIfStatement>(std::move($2), std::move($3));
    } |

    TKIF expr block else_stmt
    {
        $$ = std::make_unique<NIfStatement>(std::move($2), std::move($3), std::move($4)); 
    } |

    ;

else_stmt:
    TKELSE if_stmt
    {
    //UNSUPORTED FOR NOW
       //$$ = $<else_stmt>2;
       //$$ = $2;
    }
    |
    TKELSE block
    {

      $$ = std::make_unique<NElseStatement>(std::move($2));
     //$$ = new NElseStatement($2);
    }
    |
    /* empty */
    {
        //$$ = nullptr;
    }
    ;

break_stmt:
    TKBREAK
    ;

continue_stmt:
    TKCONT
    ;

fn_call_args:
    /* empty */
    {
        $$ = std::make_unique<ExpressionList>();
    } |
    expr
    {
        $$ = std::make_unique<ExpressionList>();
        $$->push_back(std::move($1));
    } |
    fn_call_args TKCOMMA expr
    {
        if(!$3){
          std::vector<std::unique_ptr<Token>> local_error;
          local_error.push_back(std::make_unique<Token>(*$2));
          error("Invalid expression in function call"/*, std::move(local_error)*/);
          $$ = nullptr;
          getErrorCnt();
          YYABORT;
        }

        $1->push_back(std::move($3));
    } |

    TKINVALID
    {
      std::vector<std::unique_ptr<Token>> local_error;
      local_error.push_back(std::make_unique<Token>(*$1));
      error("Invalid Token in function call"/*, std::move(local_error)*/);
      $$ = nullptr;
      getErrorCnt();
      YYABORT;
    }
    ;

expr:

    TKINVALID
    {
      std::vector<std::unique_ptr<Token>> local_error;
      local_error.push_back(std::make_unique<Token>(*$1));
      error("Invalid Token"/*, std::move(local_error)*/);
      $$ = nullptr;
    } |

    error 
    {
      std::vector<std::unique_ptr<Token>> local_error;
      error("Invalid expression"/*, std::move(local_error)*/);
      $$ = nullptr;
      getErrorCnt();
      YYABORT;

    } |

    id TKASSIGNMENT expr
    {
    if(!$3){
      std::vector<std::unique_ptr<Token>> local_error;
      local_error.push_back(std::make_unique<Token>(*$2));
      error("Invalid assignment expression"/*, std::move(local_error)*/);
      $$ = nullptr;
      getErrorCnt();
      YYABORT;

    }
      //Should call asignment from ast
        $$ = std::make_unique<NAssignment>(std::move($1), std::move($3));

    } |

    id TKPAROPEN fn_call_args TKPARCLOSE
    {
    $$ = std::make_unique<NFnCall>(std::move($1), std::move($3));

    } |

    id
    {
      $$ = std::move($1);

    } |

    numeric
    {

      $$ = std::move($1);
    }|
    string
    {

      $$ = std::move($1);

    }|

    expr binary_op expr
    {
      if(!$1 || !$3)
      {
        std::vector<std::unique_ptr<Token>> local_error;
        local_error.push_back(std::make_unique<Token>(*$2));
        error("Invalid operand expression"/*, std::move(local_error)*/);
        $$ = nullptr;
        getErrorCnt();
        YYABORT;

      } else {
        $$ = std::make_unique<NBinaryOperator>(std::move($1), $2, std::move($3));
      }

      //delete $2;

    } |

    TKPAROPEN expr TKPARCLOSE
    {
      $$ = std::move($2);

    } |

    expr comparison expr
    {

      if(!$1 || !$3)
      {
        std::vector<std::unique_ptr<Token>> local_error;
        local_error.push_back(std::make_unique<Token>(*$2));
        error("Invalid operand comparison expression"/*, std::move(local_error)*/);
        $$ = nullptr;
        getErrorCnt();
        YYABORT;

      } else {
        $$ = std::make_unique<NBinaryOperator>(std::move($1), std::move($2), std::move($3));
      }

    } |

    negation expr
    {
      if(!$2)
      {
        std::vector<std::unique_ptr<Token>> local_error;
        local_error.push_back(std::make_unique<Token>(*$1));
        error("Invalid negation expression"/*, std::move(local_error)*/);
        $$ = nullptr;
        getErrorCnt();
        YYABORT;

      } else {
        $$ = std::make_unique<NUnaryOperator>($1, std::move($2));
      }
    }
    ;

numeric:
    TKNUMBER
    {

      $$ = std::make_unique<NInteger>(std::atof($1->getValue().c_str()));
      //delete $1;

    } | 
    TKDOUBLE
    {
      $$ = std::make_unique<NDouble>(std::atof($1->getValue().c_str()));
     // delete $1;


    }
    ;

string:
    TKSTRING
    {
      $$ = std::make_unique<NString>($1->getValue());
      //delete $1;
    }
    ;




binary_op:
    TKPLUS
    | TKMINUS
    | TKMULT
    | TKDIV
    | TKMOD
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

void yy::fuse_parser::error(const string& m) {
  std::cerr << "INTERFUSE ERROR: " <<  m << std::endl;
}




/* This file defines the construction of fuse's AST, as the parser recognizes sequence of tokens, it creates the nodes specified in AST. */

%language "C++"
%require "3.2"
%define parser_class_name {fuse_parser}
%define api.value.type variant
%define parse.error detailed
//%define parse.assert true //FIXME: These assertions fail miserably
%define parse.trace true

%code requires {

    #include "Lexer.h"
    #include "AST.h"
    //#include "FuseHandler.h"
    #include <iostream>
    #include <vector>
    #include <memory>
    #include <variant>

    using namespace std;
}

%locations
%define api.location.type {TokenLocation}

%code{

//To be integrated with the parser(BISON)
    Lexer *lexerInstance;
    std::shared_ptr<NBlock> programBlock;
    int yylex(yy::fuse_parser::semantic_type* yylval, TokenLocation* yylloc);



}

// These are specified in Token.h as well

%token<std::shared_ptr<Token>> TKNUMBER TKDOUBLE
%token<std::shared_ptr<Token>> TKSTRING
%token<std::shared_ptr<Token>> TKPLUS TKMINUS TKMULT TKDIV TKMOD
%token<std::shared_ptr<Token>> TKRETURN 
%token<std::shared_ptr<Token>> TKCOMMA TKDOT TKARROW TKCOLON TKRANGE_INCLUSIVE 
%token<std::shared_ptr<Token>> TKLINEBREAK  //statement delimiter
%token<std::shared_ptr<Token>> TKASSIGNMENT 
//comparison operators
%token<std::shared_ptr<Token>> TKLESS TKGREATER TKLESS_EQUAL TKGREATER_EQUAL TKEQUAL TKNOT_EQUAL
// Loop stuff
%token<std::shared_ptr<Token>> TKAND TKOR
%token<std::shared_ptr<Token>> TKNEGATION
%token<std::shared_ptr<Token>> TKIDENTIFIER 
%token<std::shared_ptr<Token>> TKDATATYPE 
%token<std::shared_ptr<Token>> TKCURLYOPEN TKCURLYCLOSE
%token<std::shared_ptr<Token>> TKPAROPEN TKPARCLOSE
%token<std::shared_ptr<Token>> TKFUNCTION_KEY
%token<std::shared_ptr<Token>> TKIF TKELSE TKELSEIF 
%token<std::shared_ptr<Token>> TKFOR TKIN TKBREAK TKCONT
%token<std::shared_ptr<Token>> TKINVALID
%type<std::shared_ptr<Token>> comparison negation binary_op

%type<std::shared_ptr<NIdentifier>> id
%type<std::shared_ptr<VariableList>> fn_args
%type<shared_ptr<vector<shared_ptr<NExpression>>>> fn_call_args
%type<std::shared_ptr<NStatement>> stmt  fn_decl if_stmt for_stmt break_stmt continue_stmt
%type <std::shared_ptr<NVariableDeclaration>> var_decl
%type<std::shared_ptr<NElseStatement>> else_stmt

%type<std::shared_ptr<NBlock>> program stmts block
//%type<var_decls> var_decls //not quite sure bout this
%type<std::shared_ptr<NExpression>> expr numeric string




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
        assert($1);
        programBlock = $1;
    }
    ;

stmts:
     stmt 
     {
        //Initialize new block
        //&& since block contains a statement list
        //push the statement into the block !!
        $$ = std::make_shared<NBlock>();
        if($1)
        {
            $$->statements.push_back($1);
        } 

     } |

     stmt error
     {
        std::vector<std::unique_ptr<Token>> local_error;
        $$ = nullptr;
        YYABORT;

     } |
     stmts stmt
     {
        //TODO: Elaborate further this solution(specifically)
        $$ = $1;
        assert($1);

        if($2 != nullptr)
        {
          $1->statements.push_back($2);

        } else {
          error(@2, "Invalid statement");
          //error("Missing ; Delimiter", (local_error));// FIXME: Wrong error
          $$ = nullptr;
          //getErrorCnt();
          YYABORT;
        }


     } |

      stmts block
      {
          $$ = $1;
          $$ = $2;
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
            $$ = std::make_shared<NExpressionStatement>($1);
        }

    } |

    var_decl
    {
        $$ = $1;
    } |

    TKRETURN expr
    {
        $$ = std::make_shared<NReturnStatement>($2);
    } |

    TKRETURN
    {
        $$ = std::make_shared<NReturnStatement>();
    } |

    fn_decl
    {
        $$ = $1;
    } | 
    if_stmt
    {
        $$ = $1;
    } |

    for_stmt
    {
        $$ = $1;
    } | 
    break_stmt
    {
        $$ = std::make_shared<NBreakStatement>();


    } |
    continue_stmt
    {
        $$ = std::make_shared<NContinueStatement>();
    }
    ;

id : TKIDENTIFIER
    {
        $$ = std::make_shared<NIdentifier>($1->getValue());
        //delete $1;
    }
    ;

var_decl:

    TKIDENTIFIER TKCOLON TKDATATYPE TKASSIGNMENT expr
    {

        if(!$5){
          std::vector<std::unique_ptr<Token>> local_error;
          error(@5, "Invalid assignment expression to variable declaration"/*, (local_error)*/);
          $$ = nullptr;
          //getErrorCnt();
          YYABORT;

        }else{
          std::shared_ptr<NIdentifier> type = std::make_shared<NIdentifier>($3->getValue().c_str());
          std::shared_ptr<NIdentifier> id = std::make_shared<NIdentifier>($1->getValue().c_str());
          $$ = std::make_shared<NVariableDeclaration>(type, id, $5);

        }


    } 

    |

//empty var_decl
    TKIDENTIFIER TKCOLON TKDATATYPE
    {
        std::shared_ptr<NIdentifier> type = std::make_shared<NIdentifier>($3->getValue().c_str());
        std::shared_ptr<NIdentifier> id = std::make_shared<NIdentifier>($1->getValue().c_str());

        $$ = std::make_shared<NVariableDeclaration>(type, id);
    }
    ;

fn_decl:
    TKFUNCTION_KEY id TKPAROPEN fn_args TKPARCLOSE TKARROW TKDATATYPE block
    {

        std::shared_ptr<NIdentifier>type = std::make_shared<NIdentifier>($7->getValue().c_str());
        $$ = std::make_shared<NFnDeclaration>($2, $4, type, $8);
    } |

//Prototype declaration
    TKFUNCTION_KEY id TKPAROPEN fn_args TKPARCLOSE TKARROW TKDATATYPE 
    {
        std::shared_ptr<NIdentifier>type = std::make_shared<NIdentifier>($7->getValue().c_str());
        $$ = std::make_shared<NFnDeclaration>($2, $4, type);
    }
    ;

fn_args:
       //empty*
   {
      $$ = std::make_shared<std::vector<std::shared_ptr<NVariableDeclaration>>>();//aka var list

   } |

    var_decl
    {

          $$ = std::make_shared<std::vector<std::shared_ptr<NVariableDeclaration>>>();//aka var list

          //std::cout << "Adding var decl to fn args #1" << std::endl;
          $$->push_back($1);
    } |
    fn_args TKCOMMA var_decl
    {
          //std::cout << "Adding var decl to fn args #2" << std::endl;
          $1->push_back($3);
          $$ = $1;
    }
    ;

block :
    TKCURLYOPEN stmt TKCURLYCLOSE
    {
        //$$ = $2;
        $$ = std::make_shared<NBlock>();
          assert($2);
        $$->statements.push_back(($2));
    } |
    TKCURLYOPEN stmts TKCURLYCLOSE
    {
        //risky
        //NOTE: NOT SURE BOUT THIS
        $$ = ($2);
        //$$ = std::make_shared<NBlock>();
        //auto stmt = (std::get<std::unique_ptr<NStatement>>($2));
        //$$->statements.push_back((stmt));
    } |
    TKCURLYOPEN expr TKCURLYCLOSE
    {

        $$ = std::make_shared<NBlock>();
        $$->statements.push_back(std::make_shared<NExpressionStatement>(($2)));
    } |
    TKCURLYOPEN TKCURLYCLOSE
    {
        $$ = std::make_shared<NBlock>();
    }
    ;

for_stmt:

//TODO: id wont be a var decl in this case but rather a local defined within the loop block, in the ast 
//we should check if the id is defined within the current scope
//inclusive range: for @i in @i < 10 := @i++ {...

//Non inclusive range: for @i in @i < 10 : @i = @i + 1 {...
    TKFOR id TKIN expr TKCOLON expr block
    {

        $$ = std::make_shared<NForStatement>($2, $4, $6, $7);
    } | 
    TKFOR  error
    {
        std::vector<std::unique_ptr<Token>> local_error;
        error(@2, "Incorrect 'for' statement in for loop"/*, (local_error)*/);
        $$ = nullptr;
        //getErrorCnt();
        YYABORT;
    }
    ;

if_stmt:

    TKIF expr block 
    {

        if(!$2){
          std::vector<std::unique_ptr<Token>> local_error;
          error(@2,"Invalid expression in if statement"/*, (local_error)*/);
          $$ = nullptr;
          //getErrorCnt();
          YYABORT;

        }
        
        $$ = std::make_shared<NIfStatement>($2, $3);
    } |

    TKIF expr block else_stmt
    {
        $$ = std::make_shared<NIfStatement>($2, $3, $4); 
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

      $$ = std::make_shared<NElseStatement>(($2));
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
        $$ = std::make_shared<ExpressionList>();
    } |
    expr
    {
        $$ = std::make_shared<ExpressionList>();
        $$->push_back(($1));
    } |
    fn_call_args TKCOMMA expr
    {
        if(!$3){
          error(@3, "Invalid expression in function call"/*, (local_error)*/);
          YYABORT;
        }

        $1->push_back(($3));
        $$ = $1;
    } |

    TKINVALID
    {
      std::vector<std::unique_ptr<Token>> local_error;
      error(@1, "Invalid Token in function call"/*, (local_error)*/);
      $$ = nullptr;
      //getErrorCnt();
      YYABORT;
    }
    ;

expr:

    TKINVALID
    {
      error(@1,"Invalid Token"/*, (local_error)*/);
    } |

    error 
    {
      error(@1,"Invalid expression"/*, (local_error)*/);
      YYABORT;

    } |

    id TKASSIGNMENT expr
    {
    if(!$3){
      error(@2,"Invalid assignment expression"/*, (local_error)*/);
      YYABORT;

    }
      //Should call asignment from ast
        $$ = std::make_shared<NAssignment>($1, $3);

    } |

    id TKPAROPEN fn_call_args TKPARCLOSE
    {
    $$ = std::make_shared<NFnCall>($1, $3);

    } |

    id
    {
      $$ = ($1);

    } |

    numeric
    {

      $$ = ($1);
    }|
    string
    {

      $$ = ($1);

    }|

    expr binary_op expr
    {
      if(!$1 || !$3)
      {
        error(@2,"Invalid operand expression"/*, (local_error)*/);
        YYABORT;

      } else {
        $$ = std::make_shared<NBinaryOperator>($1, $2, $3);
      }

    } |

    TKPAROPEN expr TKPARCLOSE
    {
      $$ = ($2);

    } |

    expr comparison expr
    {

      if(!$1 || !$3)
      {
        error(@2, "Invalid operand comparison expression"/*, (local_error)*/);
        YYABORT;

      } else {
        $$ = std::make_shared<NBinaryOperator>($1, $2, $3);
      }

    } |

    negation expr
    {
      if(!$2)
      {
        error(@1,"Invalid negation expression"/*, (local_error)*/);
        YYABORT;

      } else {
        $$ = std::make_shared<NUnaryOperator>($1, $2);
      }
    }
    ;

numeric:
    TKNUMBER
    {

      $$ = std::make_shared<NInteger>(std::atof($1->getValue().c_str()));
      //delete $1;

    } | 
    TKDOUBLE
    {
      $$ = std::make_shared<NDouble>(std::atof($1->getValue().c_str()));
     // delete $1;


    }
    ;

string:
    TKSTRING
    {
      $$ = std::make_shared<NString>($1->getValue());
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

void yy::fuse_parser::error(const TokenLocation& loc, const string& m) {

  if(lexerInstance->_position.getLocation().line != loc.line && loc.line == 1){
    std::cerr << "INTERFUSE ERROR " << m << std::endl;
  }else{
    std::cerr << "INTERFUSE ERROR at " << loc << ": " << m << std::endl;
  }


}

%{ 

    #include "../include/Lexer.h"


//To be accessed in the lexer 


    NBlock *programBlock;

    //Lexer lex (std::fstream("../examples/test.pour"));
    //Lexer* lexerInstance = &lex;
    


    extern int yylex();
    void yyerror(const char* err){printf("ERROR: %s \n", err);}

//TODO: pass fstream to initialize lexer directly from here
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
	std::vector<NVariableDeclaration*> *varvec;
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
%token<token> TKCOMMA TKDOT TKARROW TKCOLON
%token<token> TKLINEBREAK  //statement delimiter
%token<token> TKASSIGNMENT 
//%token<token> TKSINGLECOMMENT TKMULTICOMMENT

%token<token> TKIDENTIFIER 
//denote scopes/blocks
%token<token> TKDATATYPE 
%token<token> TKCURLYOPEN TKCURLYCLOSE
%token<token> TKPAROPEN TKPARCLOSE
%token<token> TKFUNCTION_KEY

//data types


%type<id> id
//fn params are generally identifiers
%type<varvec> fn_args
//function call can be expressions
%type<exprs> fn_call_args
%type<stmt> stmt var_decl fn_decl
%type<block> program stmts block
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

//    stmts stmts
    //{
//    }|

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
        std::cout << "Parsed fn_args" << std::endl;
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

TKNUMBER 
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

%%


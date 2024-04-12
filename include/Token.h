#include <iostream>
#include <string>
#ifndef TOKEN_H
#define TOKEN_H

// To see how tokens are usually separated:
// https://neovim.io/doc/user/syntax.html

enum TOKEN_TYPE {

  INVALID, // None of the above, invalid syntax
  NUMBER,
  FLOAT,
  BOOLEAN,  // true | false
  ACCESSOR, // dots(.)  -> not sure about this
  TYPE_SPECIFIER,
  STRING,
  IDENTIFIER, // variables, syntactically begin with a '$'.
              //
  KEYWORD,    // restricted keywords?, i need to research about this

  // boolean keywords
  KEYWORD_IF,
  KEYWORD_ELSE,
  KEYWORD_ELSE_IF,
  KEYWORD_TRUE,
  KEYWORD_FALSE,

  //Function keywords
  KEYWORD_PROCEDURE,
  KEYWORD_PROVIDE,

  // Comment types
  COMMENT_SINGLE_LINE,
  COMMENT_MULTI_LINE,

  // iteration keywords
  KEYWORD_LOOP_FOR,
  KEYWORD_LOOP_WHILE,
  KEYWORD_LOOP_DO,

  //Bracket pairs tokens

  PARENTHESIS_OPEN,
  PARENTHESIS_CLOSE,
  BRACKET_OPEN,
  BRACKET_CLOSE,
  CURLY_BRACKET_OPEN,
  CURLY_BRACKET_CLOSE,

  //basic arithmetic operators
  OPERATOR_PLUS, 
  OPERATOR_MINUS,
  OPERATOR_MULTIPLY,
  OPERATOR_DIVIDE,

  ASSIGNMENT,
  LINEBREAK, // -> (;) |  need to thing about this, do i consider forcing the
             // user to use semis?
  SPACE,

  // data types(to be used much later)
  DATA_TYPE,
};

class Token {

public:
  Token() = default;

  Token(TOKEN_TYPE TokenType = INVALID, const std::string &v = "Unknown")
      : type(TokenType), value(v) {}

  // Used for debugging reasons
  friend std::ostream &operator<<(std::ostream &os, const Token &obj);
  
  TOKEN_TYPE getType() { return type; };
  const std::string getValue() { return value; };

private:
  // vals
  TOKEN_TYPE type;
  std::string value;

  std::ostream &display(std::ostream &os) const {

    // macro that stringifies enum name with the purpose
    // of logging
#define XTOKEN_CASE(TOKEN_TYPE)                                                \
  case TOKEN_TYPE:                                                             \
    os << "Type: " << #TOKEN_TYPE;                                             \
    break;

    switch (type) {
      XTOKEN_CASE(INVALID)
      XTOKEN_CASE(NUMBER)
      XTOKEN_CASE(FLOAT)
      XTOKEN_CASE(BOOLEAN)
      XTOKEN_CASE(ACCESSOR)
      XTOKEN_CASE(TYPE_SPECIFIER)
      XTOKEN_CASE(STRING)
      XTOKEN_CASE(IDENTIFIER)
      XTOKEN_CASE(ASSIGNMENT)
      XTOKEN_CASE(KEYWORD)
      XTOKEN_CASE(KEYWORD_IF)
      XTOKEN_CASE(KEYWORD_ELSE)
      XTOKEN_CASE(KEYWORD_ELSE_IF)
      XTOKEN_CASE(KEYWORD_TRUE)
      XTOKEN_CASE(KEYWORD_FALSE)
      XTOKEN_CASE(KEYWORD_LOOP_FOR)
      XTOKEN_CASE(KEYWORD_LOOP_DO)
      XTOKEN_CASE(KEYWORD_LOOP_WHILE)
      XTOKEN_CASE(KEYWORD_PROCEDURE)
      XTOKEN_CASE(KEYWORD_PROVIDE)
      XTOKEN_CASE(PARENTHESIS_OPEN)
      XTOKEN_CASE(PARENTHESIS_CLOSE)
      XTOKEN_CASE(BRACKET_OPEN)
      XTOKEN_CASE(BRACKET_CLOSE)
      XTOKEN_CASE(CURLY_BRACKET_OPEN)
      XTOKEN_CASE(CURLY_BRACKET_CLOSE)
      XTOKEN_CASE(OPERATOR_PLUS)
      XTOKEN_CASE(OPERATOR_MINUS)
      XTOKEN_CASE(OPERATOR_MULTIPLY)
      XTOKEN_CASE(OPERATOR_DIVIDE)
      XTOKEN_CASE(COMMENT_SINGLE_LINE)
      XTOKEN_CASE(COMMENT_MULTI_LINE)
      XTOKEN_CASE(LINEBREAK)
      XTOKEN_CASE(SPACE)
    default:
      os << "Type: Unknown";
    }

    // Print the value as before
    os << " Value: " << value << std::endl;
    return os;
  }
};

#endif

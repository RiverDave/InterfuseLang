#include <iostream>
#include <string>
#ifndef TOKEN_H
#define TOKEN_H

// To see how tokens are usually separated: https://neovim.io/doc/user/syntax.html

  enum TOKEN_TYPE {

    INVALID, // None of the above, invalid syntax
    NUMBER,
    FLOAT,
    BOOLEAN, // true | false
    ACCESSOR, // dots(.)  -> not sure about this
    TYPE_SPECIFIER,
    STRING,
    IDENTIFIER,  // variables, syntactically begin with a '$'.
                 //
    KEYWORD,     // restricted keywords?, i need to research about this

    //boolean keywords
    KEYWORD_IF,
    KEYWORD_ELSE,
    KEYWORD_ELSE_IF,

    //iteration keywords
    KEYWORD_LOOP_FOR,
    KEYWORD_LOOP_WHILE,
    KEYWORD_LOOP_DO,


    OPERATOR,    // arithmethic expressions
    ASSIGNMENT,
    LINEBREAK, // -> (;) |  need to thing about this, do i consider forcing the user to use semis?
    SPACE,

    //data types(to be used much later)
    DATA_TYPE,
  };

class Token {

public:



  Token() = default;

  Token(TOKEN_TYPE TokenType = INVALID, const std::string &v = "Unknown")
      : type(TokenType), value(v) {}

  // Used for debugging reasons
  friend std::ostream &operator<<(std::ostream &os, const Token &obj);

private:
  //vals
  TOKEN_TYPE type;
  std::string value;

std::ostream &display(std::ostream &os) const {
  //macro that stringifies enum name with the purpose
  //of logging 

    #define XTOKEN_CASE(TOKEN_TYPE) \
        case TOKEN_TYPE: os << "Type: " << #TOKEN_TYPE; break;

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
        XTOKEN_CASE(KEYWORD_LOOP_FOR)
        XTOKEN_CASE(KEYWORD_LOOP_DO)
        XTOKEN_CASE(KEYWORD_LOOP_WHILE)
        XTOKEN_CASE(LINEBREAK)
        XTOKEN_CASE(SPACE)
        default: os << "Type: Unknown";
    }

    // Print the value as before
    os << " Value: " << value << std::endl;
    return os;
}

};

#endif

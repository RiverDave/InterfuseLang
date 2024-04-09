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
    CONDITIONAL, // if, endif, else elif
    KEYWORD,     // restricted keywords?, i need to research about this
    OPERATOR,    // arithmethic expressions
    ASSIGNMENT,
    LABEL,
    LINEBREAK, // -> (;) |  need to thing about this, do i consider forcing the user to use semis?
    SPACE,
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
        XTOKEN_CASE(CONDITIONAL)
        XTOKEN_CASE(ASSIGNMENT)
        XTOKEN_CASE(KEYWORD)
        XTOKEN_CASE(OPERATOR)
        XTOKEN_CASE(LABEL)
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

#include <iostream>
#include <string>
#ifndef TOKEN_H
#define TOKEN_H

// A good reference is https://neovim.io/doc/user/syntax.html
enum TOKEN_TYPE {

  NUMBER,
  FLOAT,
  BOOLEAN,
  STRING,
  IDENTIFIER,
  CONDITIONAL,
  KEYWORD,
  OPERATOR,
  LABEL,
  UNKNOWN,
};


class Token {

  // type of string
  TOKEN_TYPE type;
  // actual token value
  std::string value;



  public:

  std::ostream& display(std::ostream &os) const{
    os << "Type: " << type << " Value: " << value << std::endl;  
    return os;
  }

  Token()=default;
  Token(TOKEN_TYPE TokenType = UNKNOWN, const std::string& v = "Unknown") : type(TokenType), value(v) {}

  //Used for debugging reasons


  friend std::ostream& operator <<(std::ostream& os, const Token& obj);
};




#endif

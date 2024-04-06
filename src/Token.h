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
};


class Token {

  TOKEN_TYPE type;
  std::string value;

  Token()=default;
};

#endif

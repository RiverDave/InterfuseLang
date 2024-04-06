#include "Token.h"
#include <unordered_map>
#include <string>
#include <iostream>
#include <vector>

#ifndef LEXER_H
#define LEXER_H

//At some point the result of this whole process should be
//to return a token which would be something like
// A Char/Num/Symbol(Token?) -> Its meaning
 enum CHAR_TYPE {
  ALPHABETIC_CHAR,
  DIGIT,
  SPECIAL, // <, ? , / 
  PUNCTUATOR, // comma, dot ?
  WHITESPACE,
  LINEBREAK
 };

class Lexer {
  // Should assign each type with its respective type
  // for example ['.'] = PUNCTUATOR
  // void setup_char_types(std::unordered_map<char, SUBTOKEN_TYPE>&);

  public:
  Lexer() = default;

  // Should text into tokens
  std::vector<Token> tokenize(const std::string& str);

};
#endif

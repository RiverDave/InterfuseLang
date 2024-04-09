#ifndef LEXER_CPP
#define LEXER_CPP
#include "../include/Lexer.h" //FIXME: These shouldn't be included with relative path
#include "../include/Token.h"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <vector>

// Since this is static it requires to be initialized
// outside for some reason
// std::unordered_map<char, CHAR_TYPE> Lexer::subTokenClassifier;

Lexer::Lexer(const std::string_view input)
    : input(input), _position(input.begin()) {}

inline bool isWhiteSpace(char ch) {
  return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
}

inline bool isIdentifier(char ch) {
  return isalpha(ch) || ch == '@'; // In theory identifiers should be made only by alphabetic characters
                      // in case of variables they'll start with a '$' sign
}

//NOTE: If no further condition is needed here
//consider unwrapping this condition
inline bool isNumeric(char ch) {
  return isdigit(ch);
}

Token Lexer::get_next_token() {

  // Should be referenced to input
  auto curr_char = *_position;
  switch (curr_char) {

  case ';':
    _position++;
    return Token(LINEBREAK, ";");
    break;

    
  case '=':
    _position++;
    return Token(ASSIGNMENT, "=");
    break;

  case ' ':
  case '\t':
  case '\n':
  case '\v':
  case '\f':
  case '\r':
    _position++;
    return Token(SPACE, " ");
    break;

    // TODO: Add checking if its a string {"" || ' '}
    //  evaluate any alphabetic word or digit

  default:

    //In case is variable or any other Pourer keyword
    if (isIdentifier(curr_char)) {
      
      //since we know we got an alphabetic character
      //We'll keep moving our iterator until we find something that 
      //is not alphabetic: @number=32;


      auto buffer = std::find_if_not(_position, input.end(), isIdentifier);
      

      //range initialization

      std::string word = {_position, buffer};
      //update current position
      _position = buffer; 
      return Token(IDENTIFIER, word);

    }

    
    // Same as above
    if (isNumeric(curr_char)) {
      auto buffer = std::find_if_not(_position, input.end(), isNumeric);
      std::string number = {_position, buffer};
      _position = buffer; 
      return Token(NUMBER, number);
    }

    _position++;
    return Token(INVALID, "\0");
    break;
  }
}

std::vector<Token> Lexer::tokenize() {

  std::vector<Token> parsed_tokens;

  while (_position != input.end()) {
    //This function should be in charge of updating
    //the iterator's position
    const Token curr_token = this->get_next_token();
    parsed_tokens.push_back(curr_token);
  }

  return parsed_tokens;
}


#endif

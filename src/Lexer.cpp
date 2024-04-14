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

// utils
inline bool isWhiteSpace(char ch) {
  return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
}

inline bool isIdentifier(char ch) {
  return isalpha(ch) || ch == '@'; // In theory identifiers should be made only
                                   // by alphabetic characters in case of
                                   // variables they'll start with a '$' sign
}
inline bool isString(char ch) {
  if (ch == '"') {
    return ch == '"';
  }

  if (ch == '\'') {
    return ch == '\'';
  }

  return false;
}

// Will ensure there are 3 consecutive backticks to demark it as multiblock
// comment

auto check_consecutive_backticks = [](const std::string_view::iterator &_position,
                                      const std::string_view &input) -> bool {
  auto _pos = _position;

  int count{};
  for (auto it = _pos; it != input.end(); ++it) {

    if (*it == '`') {
      count++;
      if (count == 3) {
        return true;
      }
    } else {
      count = 0;
    }
  }
  return false;
};

auto move_itr_bounds = [](const std::string_view &input, auto &_itr) -> void {
  // When working with the C++ container library, the proper type for
  // the difference between iterators is the member typedef
  // difference_type, which is often synonymous with std::ptrdiff_t.
  // -cppref
  if (_itr != input.end()) {
    _itr++;
  } else {
    return;
  }
};

// Should be able to find closing pair like -> "" , ''

// TODO: Create error handling function that terminates program
// if lexing process failed(invalid token)

// determines type of keyword
Token getKeywordType(const std::string &keyword) {

  // FIXME: This syntax looks terrible
  // Also, should I store string literals
  // somewhere else?
  if (keyword == "if") {
    return Token(KEYWORD_IF, keyword);

  } else if (keyword == "else") {
    return Token(KEYWORD_ELSE, keyword);

  } else if (keyword == "elif") {
    return Token(KEYWORD_ELSE_IF, keyword);

  } else if (keyword == "for") {
    return Token(KEYWORD_LOOP_FOR, keyword);

  } else if (keyword == "while") {
    return Token(KEYWORD_LOOP_WHILE, keyword);

  } else if (keyword == "do") {
    return Token(KEYWORD_LOOP_DO, keyword);

  } else if (keyword == "true") {
    return Token(KEYWORD_TRUE, keyword);

  } else if (keyword == "false") {
    return Token(KEYWORD_FALSE, keyword);

  } else if (keyword == "provide") { // aka return
    return Token(KEYWORD_PROVIDE, keyword);

  } else if (keyword == "procedure") {
    return Token(KEYWORD_PROCEDURE, keyword);
  }

  return Token(INVALID, keyword);
}

Lexer::Lexer(const std::string_view input)
    : input(input), _position(input.begin()) {}

// NOTE: If no further condition is needed here
// consider unwrapping this condition
inline bool isNumeric(char ch) { return isdigit(ch); }

Token Lexer::get_next_token() {

  // Should be referenced to input
  auto curr_char = *_position;
  switch (curr_char) {

  case ';':
    ++_position;
    return Token(LINEBREAK, ";");
    break;

  case '=':
    ++_position;
    return Token(ASSIGNMENT, "=");
    break;

    // paired tokens (will be checked in parser)
  case '(':
    ++_position;
    return Token(PARENTHESIS_OPEN, "(");
    break;

  case ')':
    ++_position;
    return Token(PARENTHESIS_CLOSE, ")");
    break;

  case '{':
    ++_position;
    return Token(CURLY_BRACKET_OPEN, "{");
    break;

  case '}':
    ++_position;
    return Token(CURLY_BRACKET_CLOSE, "}");
    break;

  case '[':
    ++_position;
    return Token(BRACKET_OPEN, "[");
    break;

  case ']':
    ++_position;
    return Token(BRACKET_CLOSE, "]");
    break;

    // arithmetic operators
  case '+':
    ++_position;
    return Token(OPERATOR_PLUS, "+");
    break;

  case '-':
    ++_position;
    return Token(OPERATOR_MINUS, "-");
    break;

  case '*':
    ++_position;
    return Token(OPERATOR_MULTIPLY, "*");
    break;

  case '/':
    ++_position;
    return Token(OPERATOR_DIVIDE, "/");
    break;

  case '#': {

    // Look for an endline '\n' and ignore all chars in between
    auto endline = std::find(_position, input.end(), '\n');

    // NOTE: Im not sure how safe it is do this
    // specially the second condition
    if (*endline == '\n' && endline != input.end()) {
      endline++;
    }
    _position = endline;
    return Token(COMMENT_SINGLE_LINE, "#");
  } break;

  case ' ':
  case '\t':
  case '\n':
  case '\v':
  case '\f':
  case '\r':
    ++_position;
    return Token(SPACE, " ");
    break;

  case '`': {
    // Handle multi line comments which are used with ``` like in markdown
    // Im aware this type of logic could be greatly simplified with std::regex

    if (check_consecutive_backticks(_position, input)) {
      _position += 3;

      // auto literal = *_position + *_position + *_position;
      std::string multi_comment{1, curr_char};
      multi_comment += curr_char;
      multi_comment += curr_char;

      auto closing_comment = std::find(_position, input.end(), curr_char);

      if (closing_comment == input.end() ||
          !check_consecutive_backticks(closing_comment, input)) {
        // Not a valid multi-line comment
        ++_position;
        return {INVALID, "\0"};
      }

      // will check for comment block
      if (check_consecutive_backticks(closing_comment, input)) {
        _position = closing_comment + 3;
        return Token(COMMENT_MULTI_LINE, multi_comment);
      }
    }

    ++_position;
    return {INVALID, "\0"};

  } break;
  case '\'':
  case '"': {
    auto old_pos = _position;

    // Moves iterator to avoid finding the current char

    move_itr_bounds(input, _position);

    // find closing pair
    auto closing_match = std::find(_position, input.end(), *old_pos);

    if (*old_pos == *closing_match) {
      // I need to move closing_match since it will set the char again in a
      // quote, causing this function to be called more than once!

      move_itr_bounds(input, closing_match);
      _position = closing_match;

      return Token(STRING, std::string{old_pos, _position});
    }

    ++_position;
    return Token(INVALID, "\0");
  } break;

  default:

    // In case is variable or any other Pourer keyword
    if (curr_char == '@') {

      // since we know we got an alphabetic character
      // We'll keep moving our iterator until we find something that
      // is not alphabetic: @number=32;

      auto old_pos = _position;

      move_itr_bounds(input, _position);

      if (old_pos == _position || _position == input.end() ||
          *_position == '@') {
        // out of bounds, prolly end of string
        return Token(INVALID, "\0");
      }

      std::string_view::iterator buffer = std::find_if_not(
          _position, input.end(), [](char ch) { return isalnum(ch); });

      // range initialization
      std::string identifier = {_position, buffer};
      std::cout << *buffer << "\n";

      // word found could be a Pourer keyword
      _position = buffer;
      return Token(IDENTIFIER, identifier);
      // update current position
    } else if (isNumeric(curr_char)) {

      auto buffer = std::find_if_not(_position, input.end(), isNumeric);
      std::string number = {_position, buffer};
      _position = buffer;
      return Token(NUMBER, number);
    } else { // token could potentially be a keyword

      auto buffer = std::find_if_not(_position, input.end(), isalnum);
      std::string word = {_position, buffer};
      _position = buffer;

      // Check  keyword is valid
      return getKeywordType(word);
    }
    break;
    // token could be a keyword

    ++_position;
    return Token(INVALID, "\0");
  }
}

std::vector<Token> Lexer::tokenize() {

  std::vector<Token> parsed_tokens;

  while (_position != input.end()) {
    // This function is in charge of updating
    // the iterator's position
    const Token curr_token = this->get_next_token();
    parsed_tokens.push_back(curr_token);
  }

  return parsed_tokens;
}

#endif

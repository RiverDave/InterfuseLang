#include "../include/Lexer.h" //FIXME: These shouldn't be included with relative path
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "../build/parser.hpp"

//BISON Interface
extern "C" int yylex() {
  if (!lexerInstance) {
    std::fstream file{"../examples/new.pour"};
    lexerInstance = new Lexer(file);
  }

  Token token = lexerInstance->get_next_token();
  // std::cout << token << std::endl;

  // Skip whitespaces, They're not relevant in the parsing process
  while (token.getType() == SPACE || token.getType() == COMMENT_SINGLE_LINE ||
         token.getType() == COMMENT_MULTI_LINE) {

    if(lexerInstance->_position == lexerInstance->input.end()){
      return 0;
    }

    token = lexerInstance->get_next_token();
  }

  switch (token.getType()) {
  case NUMBER:
    yylval.token = new Token(token);
    return TKNUMBER;

  case OPERATOR_PLUS:
    yylval.token = new Token(token);
    return TKPLUS;

  case OPERATOR_MINUS:
    yylval.token = new Token(token);
    return TKMINUS;

  case OPERATOR_MULTIPLY:
    yylval.token = new Token(token);
    return TKMULT;

  case OPERATOR_DIVIDE:
    yylval.token = new Token(token);
    return TKDIV;

  case KEYWORD_RET:
    yylval.token = new Token(token);
    return TKRETURN;

  case CURLY_BRACKET_OPEN:
    yylval.token = new Token(token);
    return TKCURLYOPEN;

  case CURLY_BRACKET_CLOSE:
    yylval.token = new Token(token);
    return TKCURLYCLOSE;

  case LINEBREAK:
    yylval.token = new Token(token);
    return TKLINEBREAK;

  case ASSIGNMENT:
    yylval.token = new Token(token);
    return TKASSIGNMENT;

  case IDENTIFIER:
    yylval.token = new Token(token);
    return TKIDENTIFIER;

  // case COMMENT_SINGLE_LINE:
  //   yylval.token = new Token(token);
  //   return TKSINGLECOMMENT;
  //
  // case COMMENT_MULTI_LINE:
  //   yylval.token = new Token(token);
  //   return TKMULTICOMMENT;
  default:
    return 0; // Return 0 if no more tokens
  }
}
// std::cout << "Tokenized: " << token << std::endl;
// return token.getType();

// Since this is static it requires to be initialized
// outside for some reason
// std::unordered_map<char, CHAR_TYPE> Lexer::subTokenClassifier;

// utils
inline bool isWhiteSpace(const char ch) {
  return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
}

inline bool isIdentifier(const char ch) {
  return isalpha(ch) || ch == '@'; // In theory identifiers should be made only
                                   // by alphabetic characters in case of
                                   // variables they'll start with a '$' sign
}
inline bool isString(const char ch) { return ch == '"' || ch == '\''; }
// Will ensure there are 3 consecutive backticks to demark it as multiblock
// comment

auto check_consecutive_backticks = [](const std::string::iterator &_position,
                                      const std::string &input) -> bool {
  auto _pos = _position;

  int count{};
  for (auto it = _pos; it != input.end(); ++it) {

    //NOTE: This might be redundant, considered we've already checked for the backticks
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

auto move_itr_bounds = [](const std::string &input, auto &_itr) -> void {
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

std::unordered_map<std::string, TOKEN_TYPE> keywordMap = {

    {"if", KEYWORD_IF},
    {"else", KEYWORD_ELSE},
    {"elif", KEYWORD_ELSE_IF},
    {"for", KEYWORD_LOOP_FOR},
    {"while", KEYWORD_LOOP_WHILE},
    {"do", KEYWORD_LOOP_DO},
    {"true", KEYWORD_TRUE},
    {"false", KEYWORD_FALSE},
    {"ret", KEYWORD_RET},
    {"procedure", KEYWORD_PROCEDURE}};

Token getKeywordType(const std::string &keyword) {
  auto it = keywordMap.find(keyword);
  if (it != keywordMap.end()) {
    return Token(it->second, keyword);
  }
  return Token(INVALID, keyword);
}

Lexer::Lexer(const std::string &input)
    : input(input), _position(this->input.begin()) {
    }

//FIXME: This is getting called twice for some reason
Lexer::Lexer(const std::fstream &src) : input() {

  // Read file and store it in a string_view

  std::ostringstream ss;
  ss << src.rdbuf();
  if (!src.good()) {
    if (src.eof()) {
      std::cout << "EOF reached" << std::endl;
    } else if (src.fail()) {
      std::cout << "Logical error on i/o operation" << std::endl;
    } else if (src.bad()) {
      std::cout << "Read/write error on i/o operation" << std::endl;
    }
  }
  input = ss.str();
  _position = input.begin();
}

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

  case '=':
    ++_position;
    return Token(ASSIGNMENT, "=");

    // paired tokens (will be checked in parser)
  case '(':
    ++_position;
    return Token(PARENTHESIS_OPEN, "(");

  case ')':
    ++_position;
    return Token(PARENTHESIS_CLOSE, ")");

  case '{':
    ++_position;
    return Token(CURLY_BRACKET_OPEN, "{");

  case '}':
    ++_position;
    return Token(CURLY_BRACKET_CLOSE, "}");

  case '[':
    ++_position;
    return Token(BRACKET_OPEN, "[");

  case ']':
    ++_position;
    return Token(BRACKET_CLOSE, "]");

    // arithmetic operators
  case '+':
    ++_position;
    return Token(OPERATOR_PLUS, "+");

  case '-':
    ++_position;
    return Token(OPERATOR_MINUS, "-");

  case '*':
    ++_position;
    return Token(OPERATOR_MULTIPLY, "*");

  case '/':
    ++_position;
    return Token(OPERATOR_DIVIDE, "/");

  case '#': {

    // Look for an endline '\n' and ignore all chars in between
    auto endline = std::find(_position, input.end(), '\n');

    if (endline != input.end()) {
      ++endline;
    }
    _position = endline;
    return Token(COMMENT_SINGLE_LINE, "#");
  }

  case ' ':
  case '\t':
  case '\n':
  case '\v':
  case '\f':
  case '\r':
    ++_position;
    return Token(SPACE, " ");

  case EOF:
    return Token(T_EOF, "EOF");

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
        return Token{INVALID, "\0"};
      }

      // will check for comment block
      if (check_consecutive_backticks(closing_comment, input)) {
        _position = closing_comment + 3;
        return Token(COMMENT_MULTI_LINE, multi_comment);
      }
    }

    ++_position;
    return Token{INVALID, "\0"};
  }
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

    // In case is variable 
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

      std::string::iterator buffer = std::find_if_not(
          _position, input.end(), [](char ch) { return isalnum(ch) || ch == '_'; });

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

      const auto buffer = std::find_if_not(_position, input.end(), isalnum);
      const std::string word = {_position, buffer};
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

#include "Lexer.h"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

#include "../build/parser.hpp"

extern FuseHandler fusehandler;


inline void logError(Token tok, const std::string &msg) {
    std::cerr << "INTERFUSE ERROR: " << msg << " at " << tok.getLocation() << std::endl;
    fusehandler.err_cnt++;
}

// BISON Interface

int yylex(yy::fuse_parser::semantic_type *yylval) {
    if (!lexerInstance) {
        if (!_global_file_path.empty()) {
            lexerInstance = new Lexer(std::fstream(_global_file_path));
        }
    }

    assert(lexerInstance);

    Token token = lexerInstance->get_next_token();

    //Aside from 1 char tokens, multiple char token locations were set in
    //the get_next_token function

    if (token.getValue().size() == 1 && token.getType() != IDENTIFIER && token.getType() != SPACE) {
        size_t nval = lexerInstance->_position.getLocation().range.first - 1;
        TokenLocation loc = lexerInstance->_position.getLocation();
        loc.range.first = nval;
        token.setLocation(loc);
    }


    // Skip whitespaces, They're not relevant in the parsing process
    while (token.getType() == SPACE || token.getType() == COMMENT_SINGLE_LINE ||
           token.getType() == COMMENT_MULTI_LINE) {

        //Exit lexing process
        if (*lexerInstance->_position == lexerInstance->input.end()) {

            // std::cout << "Lexing finished: " << std::endl;
            // std::cout << " Line: " << lexerInstance->_position.getLocation().line << " Col: " << lexerInstance->_position.getLocation().range.first << std::endl;
            return 0;
        }

        token = lexerInstance->get_next_token();
        //TODO: fix this mess of code
        if (token.getValue().size() == 1 && token.getType() != IDENTIFIER && token.getType() != SPACE) {
            size_t nval = lexerInstance->_position.getLocation().range.first - 1;
            TokenLocation loc = lexerInstance->_position.getLocation();
            loc.range.first = nval;
            token.setLocation(loc);

            //Hardcoded af, Need to FIX
            if (loc.range.first != 1)
                lexerInstance->_position.getLocation().range.first++;
        }
    }

    // std::cout << token << " ";
    // std::cout << token.getLocation() << std::endl;


    //Make sure that token order is correct, last token coordinates
    //Should be greater than the previous token passed
    if (!lexerInstance->assert_stk.empty()) {
        assert(token.getLocation() > lexerInstance->assert_stk.top().getLocation());
        lexerInstance->assert_stk.pop();
    }


    lexerInstance->assert_stk.push(token);

    // Token types defined in bison file
    switch (token.getType()) {

        case NUMBER:
            yylval->as<Token *>() = new Token(token);
            return yy::fuse_parser::token::TKNUMBER;

        case DOUBLE:
            yylval->as<Token *>() = new Token(token);
            return yy::fuse_parser::token::TKDOUBLE;

        case OPERATOR_PLUS:
            yylval->as<Token *>() = new Token(token);
            return yy::fuse_parser::token::TKPLUS;

        case OPERATOR_MINUS:
            yylval->as<Token *>() = new Token(token);
            return yy::fuse_parser::token::TKMINUS;

        case OPERATOR_MULTIPLY:
            yylval->as<Token *>() = new Token(token);
            return yy::fuse_parser::token::TKMULT;

        case OPERATOR_DIVIDE:
            yylval->as<Token *>() = new Token(token);
            return yy::fuse_parser::token::TKDIV;

        case KEYWORD_RET:
            yylval->as<Token *>() = new Token(token);
            return yy::fuse_parser::token::TKRETURN;

        case CURLY_BRACKET_OPEN:
            yylval->as<Token *>() = new Token(token);
            return yy::fuse_parser::token::TKCURLYOPEN;

        case CURLY_BRACKET_CLOSE:
            yylval->as<Token *>() = new Token(token);
            return yy::fuse_parser::token::TKCURLYCLOSE;

        case PARENTHESIS_OPEN:
            yylval->as<Token *>() = new Token(token);
            return yy::fuse_parser::token::TKPAROPEN;

        case PARENTHESIS_CLOSE:
            yylval->as<Token *>() = new Token(token);
            return yy::fuse_parser::token::TKPARCLOSE;

        case COMMA:
            yylval->as<Token *>() = new Token(token);
            return yy::fuse_parser::token::TKCOMMA;

        case DOT:
            yylval->as<Token *>() = new Token(token);
            return yy::fuse_parser::token::TKDOT;

        case COLON:
            yylval->as<Token *>() = new Token(token);
            return yy::fuse_parser::token::TKCOLON;

        case RANGE_INCLUSIVE:
            yylval->as<Token *>() = new Token(token);
            return yy::fuse_parser::token::TKRANGE_INCLUSIVE;

        case ARROW:
            yylval->as<Token *>() = new Token(token);
            return yy::fuse_parser::token::TKARROW;

        case LINEBREAK:
            yylval->as<Token *>() = new Token(token);
            return yy::fuse_parser::token::TKLINEBREAK;

        case ASSIGNMENT:
            yylval->as<Token *>() = new Token(token);
            return yy::fuse_parser::token::TKASSIGNMENT;

        case IDENTIFIER:
            yylval->as<Token *>() = new Token(token);
            return yy::fuse_parser::token::TKIDENTIFIER;

        case KEYWORD_PROCEDURE:
            yylval->as<Token *>() = new Token(token);
            return yy::fuse_parser::token::TKFUNCTION_KEY;

        case DATA_TYPE:
            yylval->as<Token *>() = new Token(token);
            return yy::fuse_parser::token::TKDATATYPE;

        case KEYWORD_IF:
            yylval->as<Token *>() = new Token(token);
            return yy::fuse_parser::token::TKIF;

        case KEYWORD_ELSE:
            yylval->as<Token *>() = new Token(token);
            return yy::fuse_parser::token::TKELSE;

        case KEYWORD_ELSE_IF:
            yylval->as<Token *>() = new Token(token);
            return yy::fuse_parser::token::TKELSEIF;

        case KEYWORD_LOOP_FOR:
            yylval->as<Token *>() = new Token(token);
            return yy::fuse_parser::token::TKFOR;

        case KEYWORD_LOOP_IN:
            yylval->as<Token *>() = new Token(token);
            return yy::fuse_parser::token::TKIN;

        case KEYWORD_LOOP_BREAK:
            yylval->as<Token *>() = new Token(token);
            return yy::fuse_parser::token::TKBREAK;

        // operator stuff
        case OPERATOR_EQUALS:
            yylval->as<Token *>() = new Token(token);
            return yy::fuse_parser::token::TKEQUAL;

        case OPERATOR_LESS_THAN:
            yylval->as<Token *>() = new Token(token);
            return yy::fuse_parser::token::TKLESS;

        case OPERATOR_LESS_THAN_EQUALS:
            yylval->as<Token *>() = new Token(token);
            return yy::fuse_parser::token::TKLESS_EQUAL;

        case OPERATOR_GREATER_THAN:
            yylval->as<Token *>() = new Token(token);
            return yy::fuse_parser::token::TKGREATER;

        case OPERATOR_GREATER_THAN_EQUALS:
            yylval->as<Token *>() = new Token(token);
            return yy::fuse_parser::token::TKGREATER_EQUAL;

        case OPERATOR_NOT_EQUALS:
            yylval->as<Token *>() = new Token(token);
            return yy::fuse_parser::token::TKNOT_EQUAL;

        case OPERATOR_AND:
            yylval->as<Token *>() = new Token(token);
            return yy::fuse_parser::token::TKAND;

        case OPERATOR_OR:
            yylval->as<Token *>() = new Token(token);
            return yy::fuse_parser::token::TKOR;

        case OPERATOR_NEGATION:
            yylval->as<Token *>() = new Token(token);
            return yy::fuse_parser::token::TKNEGATION;

        case KEYWORD_LOOP_CONTINUE:
            yylval->as<Token *>() = new Token(token);
            return yy::fuse_parser::token::TKCONT;

        case OPERATOR_MODULO:
            yylval->as<Token *>() = new Token(token);
            return yy::fuse_parser::token::TKMOD;

        case STRING:
            yylval->as<Token *>() = new Token(token);
            return yy::fuse_parser::token::TKSTRING;

        case INVALID:
            logError(token, "Invalid token");
            yylval->as<Token *>() = new Token(token);
            return yy::fuse_parser::token::TKINVALID;

        default:
            return 0;
    }


    return 0;
}

// utils
inline bool isWhiteSpace(const char ch) {
    return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
}

inline bool isIdentifier(const char ch) {
    return isalpha(ch) || ch == '@';// In theory identifiers should be made only
                                    // by alphabetic characters in case of
                                    // variables they'll start with a '@' sign
}
inline bool isString(const char ch) { return ch == '"' || ch == '\''; }
// Will ensure there are 3 consecutive backticks to demark it as multiblock
// comment

inline bool check_consecutive_backticks(const std::string::iterator &_position,
                                        const std::string &input) {
    auto _pos = _position;

    int count{};
    for (auto it = _pos; it != input.end(); ++it) {

        // NOTE: This might be redundant, considered we've already checked for the
        // backticks
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

inline void Lexer::move_itr_bounds() {
    if (_position.getIterator() != input.end()) {
        ++_position;
    } else {
        return;
    }
};
// Should be able to find closing pair like -> "" , ''


// determines type of keyword

static const std::unordered_map<std::string, TOKEN_TYPE> keywordMap = {

        {"if", KEYWORD_IF},
        {"else", KEYWORD_ELSE},
        {"elif", KEYWORD_ELSE_IF},
        {"for", KEYWORD_LOOP_FOR},
        {"in", KEYWORD_LOOP_IN},
        {"while", KEYWORD_LOOP_WHILE},
        {"do", KEYWORD_LOOP_DO},
        {"true", KEYWORD_TRUE},
        {"false", KEYWORD_FALSE},
        {"ret", KEYWORD_RET},
        {"fn", KEYWORD_PROCEDURE},
        {"break", KEYWORD_LOOP_BREAK},
        {"continue", KEYWORD_LOOP_CONTINUE},
};

static const std::unordered_map<std::string, TOKEN_TYPE> dataTypeMap = {

        {"double", DATA_TYPE},
        {"int", DATA_TYPE},
        {"bool", DATA_TYPE},
        {"str", DATA_TYPE},
        {"float", DATA_TYPE},
        {"char", DATA_TYPE},
        {"void", DATA_TYPE}};

Token Lexer::checkKeywordFromMap(
        const std::string &keyword,
        const std::unordered_map<std::string, TOKEN_TYPE> &mp) {
    auto it = mp.find(keyword);
    if (it != mp.end()) {
        return Token(it->second, keyword);
    }
    return invalidToken(keyword);
}

Lexer::Lexer(const std::fstream &src) : input(), _position() {

    // Read file and store its data it in a string_view
    std::ostringstream ss;
    ss << src.rdbuf();
    if (!src.good()) {
        if (src.eof()) {
            std::cerr << "EOF reached" << std::endl;
        } else if (src.fail() || src.bad()) {
            std::cerr << "Logical error on i/o operation, Unable to read from file" << std::endl;
            exit(1);
        }
    }

    input = ss.str();
    _position.setInput(input);
}

// NOTE: If no further condition is needed here
// consider unwrapping this condition
inline bool isNumeric(char ch) { return isdigit(ch); }

std::optional<char> Lexer::peek_next_char() {

    const std::optional<char> next = *(++(*_position));// Dereference character beneath incremented iterator(Nice syntax btw)
    ;

    if (next.has_value()) {
        return next.value();
    }
    return std::nullopt;
}


Token Lexer::handle_operator(char current, char next, TOKEN_TYPE current_op, TOKEN_TYPE next_op) {
    TokenLocation npos = _position.getLocation();
    auto next_char = peek_next_char();

    Token tok = Token(current_op, std::string(1, current));

    if (next_char.has_value()) {
        if (next_char.value() == next) {
            tok = Token(next_op, std::string(1, current) + next);
            npos.range.second = npos.range.first + 1;
            tok.setLocation(npos);
            _position += 2;//Set iterator to next space
            return tok;
        }
    }

    ++_position;
    return tok;
}


//Search closing token forward
Token Lexer::handle_matched_token(char open_char, char close_char, TOKEN_TYPE open_type, TOKEN_TYPE close_type) {

    auto find = std::find(*_position, input.end(), close_char);

    if (find == input.end()) {
        logError(Token(INVALID, std::string(1, open_char), _position.getLocation()), std::string{"Unterminated token found"});
        return Token{CURLY_BRACKET_OPEN, std::string{open_char}};
    }

    return Token{open_type, std::string{open_char}};
}

//Search closing token backwards
Token Lexer::handle_reversed_matched_token(char closing_char, char open_char, TOKEN_TYPE close_type, TOKEN_TYPE open_type) {

    std::string::reverse_iterator rbegin(*_position);
    std::string::reverse_iterator rend(input.begin());
    auto find = std::find(rbegin, rend, open_char);

    if (find == rend) {
        logError(Token(INVALID, std::string(1, closing_char), _position.getLocation()), std::string{"Extraneous token found"});
        return Token{CURLY_BRACKET_CLOSE, std::string{closing_char}};
    }

    return Token{close_type, std::string{closing_char}};
}

Token Lexer::get_next_token() {

    // Should be referenced to input
    char curr_char = **_position;
    switch (curr_char) {

        case '&': {

            //            // Im not including bitwise stuff for now
            //            Token ntoken = handle_operator(curr_char, '&', INVALID, OPERATOR_AND);
            //            return ntoken;

            return handle_operator(curr_char, '&', INVALID, OPERATOR_AND);
        }

        case '|':
            // Im not including bitwise stuff for now
            return handle_operator(curr_char, '|', INVALID, OPERATOR_OR);

        case '!': {

            return handle_operator(curr_char, '=', OPERATOR_NEGATION, OPERATOR_NOT_EQUALS);
        }

        case ';':
            ++_position;
            return Token(LINEBREAK, ";");

        case '=': {

            return handle_operator(curr_char, '=', ASSIGNMENT, OPERATOR_EQUALS);
        }
            return Token(ASSIGNMENT, "=");

            // paired tokens (will be checked in parser)
        case '(':
            ++_position;
            return handle_matched_token('(', ')', PARENTHESIS_OPEN, PARENTHESIS_CLOSE);

        case ')':
            ++_position;
            return handle_reversed_matched_token(')', '(', PARENTHESIS_CLOSE, PARENTHESIS_OPEN);

        case '{':
            ++_position;
            return handle_matched_token('{', '}', CURLY_BRACKET_OPEN, CURLY_BRACKET_CLOSE);

        case '}':
            ++_position;
            return handle_reversed_matched_token('}', '{', CURLY_BRACKET_CLOSE, CURLY_BRACKET_OPEN);

        case '[':
            ++_position;
            return handle_matched_token('[', ']', BRACKET_OPEN, BRACKET_CLOSE);

        case ']':
            ++_position;
            return handle_reversed_matched_token(']', '[', BRACKET_CLOSE, BRACKET_OPEN);

            // arithmetic operators
        case '+':
            ++_position;
            return Token(OPERATOR_PLUS, "+");

        case '-': {
            return handle_operator(curr_char, '>', OPERATOR_MINUS, ARROW);
        }

        case ':': {

            return handle_operator(curr_char, '=', COLON, RANGE_INCLUSIVE);
        }

        case '.':
            ++_position;
            return Token(DOT, ".");

        case ',':
            ++_position;
            return Token(COMMA, ",");

        case '*':
            ++_position;
            return Token(OPERATOR_MULTIPLY, "*");

        case '/':
            ++_position;
            return Token(OPERATOR_DIVIDE, "/");

        case '%':
            ++_position;
            return Token(OPERATOR_MODULO, "%");

        case '#': {

            // Look for an endline '\n' and ignore all chars in between
            auto endline = std::find(*_position, input.end(), '\n');

            if (endline != input.end()) {
                ++endline;
            }
            _position = endline;
            return Token(COMMENT_SINGLE_LINE, "#");
        }

            // comparision stuff

        case '<': {
            return handle_operator(curr_char, '=', OPERATOR_LESS_THAN, OPERATOR_LESS_THAN_EQUALS);
        }

        case '>': {
            return handle_operator(curr_char, '=', OPERATOR_GREATER_THAN, OPERATOR_GREATER_THAN_EQUALS);
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

            if (check_consecutive_backticks(*_position, input)) {
                _position += 3;

                // auto literal = *_position + *_position + *_position;
                std::string multi_comment{1, curr_char};
                multi_comment += curr_char;
                multi_comment += curr_char;

                auto closing_comment = std::find(*_position, input.end(), curr_char);

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


            // Move iterator to avoid finding the current char

            std::string::iterator old_pos = *_position;
            TokenLocation npos = _position.getLocation();
            move_itr_bounds();


            // find closing pair
            auto closing_match = std::find(*_position, input.end(), '"');

            if (*closing_match == '"') {
                // I need to move closing_match since it will set the char again in a
                // quote, causing this function to be called more than once!

                move_itr_bounds();
                _position = closing_match;
                npos.range.second = npos.range.first + std::string(old_pos, closing_match).size();
                //Despite the coordinates tracking the whole string including quotes
                //The value we care about is the Inner chars only
                Token tok = Token(STRING, std::string{old_pos + 1, *_position});
                tok.setLocation(npos);

                //Current pos is the closing quote
                //Move iterator to avoid finding the closing quote
                move_itr_bounds();

                return tok;
            }

            logError(Token(INVALID, std::string(1, curr_char), npos), std::string{"Unterminated string found"});
            ++_position;
            return Token(INVALID, std::string(1, curr_char));
        }

            //TODO: Heavily refactor this part of the code,
            //      it's a mess and it's hard to read
        default:

            // In case is variable
            if (curr_char == '@') {

                // since we know we got an alphabetic character
                // We'll keep moving our iterator until we find something that
                // is not alphabetic: @number=32;


                std::string::iterator old_pos = *_position;
                TokenLocation npos = _position.getLocation();

                // Move itr to avoid lexing '@'
                move_itr_bounds();


                if (old_pos == *_position || *_position == input.end() ||
                    **_position == '@') {
                    // out of bounds, prolly end of string
                    return Token(INVALID, "\0");
                }

                std::string::iterator buffer =
                        std::find_if_not(*_position, input.end(),
                                         [&](char ch) { return isalnum(ch) || ch == '_'; });

                std::string identifier = {*_position, buffer};

                // word found could be a Pourer keyword
                _position = buffer;
                npos.range.second = npos.range.first + identifier.size();

                Token tok = Token(IDENTIFIER, identifier);
                tok.setLocation(npos);

                return tok;
                // update current position
            } else if (isNumeric(curr_char)) {

                std::string::iterator old_pos = *_position;
                TokenLocation npos = _position.getLocation();

                auto buffer = std::find_if_not(*_position, input.end(), [&](char ch) {
                    return isNumeric(ch) || ch == '.';
                });

                // get dot count
                auto dotcnt = std::count(*_position, buffer, '.');

                if (dotcnt > 1) {
                    return Token(INVALID, "\0");
                } else if (dotcnt == 1) {

                    Token tok = Token(DOUBLE, std::string{*_position, buffer});

                    _position = buffer;
                    npos.range.second = npos.range.first + tok.getValue().size() - 1;
                    tok.setLocation(npos);
                    return tok;
                }

                std::string number = {*_position, buffer};
                Token tok = Token(NUMBER, number);

                _position = buffer;
                npos.range.second = npos.range.first + number.size() - 1;
                tok.setLocation(npos);
                return tok;

            } else if (isalnum(curr_char)) {// token could potentially be a keyword

                auto buffer = std::find_if_not(*_position, input.end(),
                                               static_cast<int (*)(int)>(std::isalnum)); //Weird cast due to ambiguous overload
                std::string::iterator old_pos = *_position;
                TokenLocation npos = _position.getLocation();

                const std::string word = {*_position, buffer};

                // Check  keyword is valid
                Token tok = checkKeywordFromMap(word, keywordMap);

                if (tok.getType() != INVALID) {

                    //FIXME: Returning tok  directly does not return the item as is!
                    Token tk = tok;
                    _position = buffer;
                    npos.range.second = npos.range.first + word.size() - 1;
                    tk.setLocation(npos);

                    return tk;
                } else {
                    //Check if word is a data type
                    Token tok = checkKeywordFromMap(word, dataTypeMap);
                    _position = buffer;

                    if (tok.getType() != INVALID) {
                        npos.range.second = npos.range.first + word.size() - 1;
                        tok.setLocation(npos);
                        return tok;
                    }

                    npos.range.second = npos.range.first + word.size() - 1;
                    tok.setLocation(npos);
                    return tok;
                }
            }
            //            break;
            // token could be a keyword

            ++_position;
            return Token(INVALID, "\0");
    }
}

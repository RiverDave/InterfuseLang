#include <iostream>
#include <string>
#include <optional>
#ifndef TOKEN_H
#define TOKEN_H

// To see how tokens are usually separated:
// https://neovim.io/doc/user/syntax.html

enum TOKEN_TYPE {

    INVALID,// None of the below, invalid syntax
    NUMBER,
    DOUBLE,
    BOOLEAN, // true | false
    ACCESSOR,// dots(.)  -> not sure about this
    TYPE_SPECIFIER,
    STRING,
    IDENTIFIER,// variables, syntactically begin with a '@'.

    KEYWORD,// restricted keywords?, i need to research about this

    // boolean keywords
    KEYWORD_IF,
    KEYWORD_ELSE,
    KEYWORD_ELSE_IF,
    KEYWORD_TRUE,
    KEYWORD_FALSE,

    // Function keywords
    KEYWORD_PROCEDURE,
    KEYWORD_RET,// aka return

    // Comment types
    COMMENT_SINGLE_LINE,
    COMMENT_MULTI_LINE,

    // iteration keywords
    KEYWORD_LOOP_FOR,
    KEYWORD_LOOP_IN,
    KEYWORD_LOOP_WHILE,
    KEYWORD_LOOP_DO,
    KEYWORD_LOOP_BREAK,
    KEYWORD_LOOP_CONTINUE,

    // Bracket pairs tokens
    PARENTHESIS_OPEN,
    PARENTHESIS_CLOSE,
    BRACKET_OPEN,
    BRACKET_CLOSE,
    CURLY_BRACKET_OPEN,
    CURLY_BRACKET_CLOSE,

    // basic arithmetic operators
    OPERATOR_PLUS,
    OPERATOR_MINUS,
    OPERATOR_MULTIPLY,
    OPERATOR_DIVIDE,
    OPERATOR_MODULO,


    RANGE_INCLUSIVE,
    RANGE_NON_INCLUSIVE,
    DOT,
    COMMA,
    ARROW,
    COLON,

    ASSIGNMENT,
    LINEBREAK,// -> (;) |  need to thing about this, do i consider forcing the
              // user to use semis?
    SPACE,

    //boolean operators
    OPERATOR_AND,
    OPERATOR_OR,
    OPERATOR_NOT,
    OPERATOR_EQUALS,
    OPERATOR_NOT_EQUALS,
    OPERATOR_GREATER_THAN,
    OPERATOR_LESS_THAN,
    OPERATOR_GREATER_THAN_EQUALS,
    OPERATOR_LESS_THAN_EQUALS,
    OPERATOR_NEGATION,

    DATA_TYPE,
    DATA_TYPE_CHAR,
    DATA_TYPE_INT,
    DATA_TYPE_BOOL,
    DATA_TYPE_DOUBLE,
    DATA_TYPE_STRING,
    DATA_TYPE_FLOAT,

    T_EOF,// Denotes end of file
};

struct TokenLocation {

    int line = 1;
    //first -> column number, second -> denotes the end 
    std::pair<int, std::optional<int>> range = {1, std::nullopt};

    std::ostream &display(std::ostream &os) const {
        os << "" << line << ":" << range.first ;

        if (range.second.has_value() && range.second != 0 && range.second != range.first) {
            os << "-" << range.second.value() << std::endl;
        }

        return os;
    }

    friend std::ostream &operator<<(std::ostream &os, const TokenLocation &obj) {
        return obj.display(os);
    }
};

class Token {

public:
    explicit Token(TOKEN_TYPE TokenType = INVALID,
                   const std::string &val = "Unknown")
        : type(TokenType), value(val) {}

    explicit Token(TOKEN_TYPE TokenType, const std::string &val,
                   TokenLocation loc)
        : type(TokenType), value(val), location(loc) {}

    Token(const Token &other) : type(other.type), value(other.value) {}

    Token &operator=(const Token &other) {
        if (this != &other) {
            type = other.type;
            value = other.value;
        }
        return *this;
    }

    // Used for debugging reasons
    friend std::ostream &operator<<(std::ostream &os, const Token &obj);

    [[nodiscard]] TOKEN_TYPE getType() { return type; };
    [[nodiscard]] const std::string getValue() { return value; };
    [[nodiscard]] TokenLocation getLocation() { return location; };
    void setLocation(TokenLocation loc) { location = loc; };


private:
    //TODO: Add line number and column number to track the position of the token
    //Which would exponentially improve error handling

    TOKEN_TYPE type;
    std::string value;
    TokenLocation location;

    std::ostream &display(std::ostream &os) const {

        // macro that stringifies enum name with the purpose
        // of logging
#define XTOKEN_CASE(TOKEN_TYPE)        \
    case TOKEN_TYPE:                   \
        os << "Type: " << #TOKEN_TYPE; \
        break;

        switch (type) {
            XTOKEN_CASE(INVALID)
            XTOKEN_CASE(NUMBER)
            XTOKEN_CASE(DOUBLE)
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
            XTOKEN_CASE(KEYWORD_LOOP_BREAK)
            XTOKEN_CASE(KEYWORD_LOOP_CONTINUE)
            XTOKEN_CASE(KEYWORD_PROCEDURE)
            XTOKEN_CASE(KEYWORD_RET)
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
            XTOKEN_CASE(OPERATOR_MODULO)
            XTOKEN_CASE(COMMENT_SINGLE_LINE)
            XTOKEN_CASE(COMMENT_MULTI_LINE)
            XTOKEN_CASE(DOT)
            XTOKEN_CASE(COMMA)
            XTOKEN_CASE(OPERATOR_AND)
            XTOKEN_CASE(OPERATOR_OR)
            XTOKEN_CASE(OPERATOR_NOT)
            XTOKEN_CASE(OPERATOR_EQUALS)
            XTOKEN_CASE(OPERATOR_NOT_EQUALS)
            XTOKEN_CASE(OPERATOR_GREATER_THAN)
            XTOKEN_CASE(OPERATOR_LESS_THAN)
            XTOKEN_CASE(OPERATOR_GREATER_THAN_EQUALS)
            XTOKEN_CASE(OPERATOR_LESS_THAN_EQUALS)
            XTOKEN_CASE(OPERATOR_NEGATION)
            XTOKEN_CASE(RANGE_INCLUSIVE)
            XTOKEN_CASE(ARROW)
            XTOKEN_CASE(COLON)
            XTOKEN_CASE(DATA_TYPE)
            XTOKEN_CASE(LINEBREAK)
            XTOKEN_CASE(SPACE)
            XTOKEN_CASE(T_EOF)
            default:
                os << "Type: Unknown";
        }

        // Print the value as before
        os << " Value: " << value << std::endl;
        return os;
    }
};

#endif

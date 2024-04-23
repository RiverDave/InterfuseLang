#include "../include/Token.h"
#include <optional>
#include <vector>

class Parser {

public:
  explicit Parser(std::vector<Token> &tokens)
      : _tokens(tokens), _current(_tokens.begin()) {}
  void parse() {
    //TODO: Determine parsing strategy
  }

private:
  std::vector<Token> _tokens;
  std::vector<Token>::iterator _current;


  inline std::optional<Token> get_current_token() {
    if (_current != _tokens.end()) {
      return *_current;
    }
    return {};
  }

  inline std::optional<Token> get_previous() {

    if (_current != _tokens.begin()) {
      auto back = _current - 1;
      return *back;
    }
    return {};
  }

  inline std::optional<Token> peek() {
    auto next = _current + 1;
    if (next != _tokens.end()) {
      return *next;
    }
    return {};
  }

  inline bool check(TOKEN_TYPE type) {
    if (get_current_token().has_value() &&
        get_current_token().value().getType() == type) {
      ++_current;
      return true;
    }
    return false;
  }

  // Message is used in error handling
  inline void consume(TOKEN_TYPE type, const std::string &message) {

    if (_current != _tokens.end() && _current->getType() == type) {
      ++_current;
    }
    throw std::runtime_error(message);
  }

  bool match(std::initializer_list<TOKEN_TYPE> types) {

    for (const auto &type : types) {
      if (check(type)) {
        consume(type, "Expect number or '('.");
        return true;
      }
    }

    return false;
  }
};

/*


#NOTE: This is a great reference:
# Copilot ---

To build your parser, you need to implement the `parse` method in your `Parser`
class. This method will process the tokens received from the lexer and construct
an Abstract Syntax Tree (AST) or some other internal representation of the
program.

Here's a simple example of how you might implement a recursive descent parser
for a simple arithmetic expression language. This is just a starting point and
you'll need to expand and modify this to suit your language's grammar.

```cpp
class Parser {
public:
    explicit Parser(std::vector<Token>& tokens) : _tokens(tokens), _current(0)
{}

    void parse() {
        while (!isAtEnd()) {
            try {
                expression();
            } catch (const std::runtime_error& error) {
                std::cerr << error.what() << std::endl;
                break;
            }
        }
    }

private:
    std::vector<Token> _tokens;
    size_t _current;

    void expression() {
        term();
        while (match({PLUS, MINUS})) {
            Token operatorToken = previous();
            term();
            // Here you would create an AST node with the operator and the two
operands
        }
    }

    void term() {
        factor();
        while (match({STAR, SLASH})) {
            Token operatorToken = previous();
            factor();
            // Here you would create an AST node with the operator and the two
operands
        }
    }

    void factor() {
        if (match({NUMBER})) {
            // Here you would create an AST node with the number
        } else if (match({LEFT_PAREN})) {
            expression();
            consume(RIGHT_PAREN, "Expect ')' after expression.");
        } else {
            throw std::runtime_error("Expect number or '('.");
        }
    }

    bool match(std::initializer_list<TokenType> types) {
        for (const auto& type : types) {
            if (check(type)) {
                advance();
                return true;
            }
        }
        return false;
    }

    bool check(TokenType type) {
        if (isAtEnd()) return false;
        return peek().type == type;
    }

    Token advance() {
        if (!isAtEnd()) _current++;
        return previous();
    }

    bool isAtEnd() {
        return _current >= _tokens.size();
    }

    Token peek() {
        return _tokens[_current];
    }

    Token previous() {
        return _tokens[_current - 1];
    }

    void consume(TokenType type, const std::string& message) {
        if (check(type)) advance();
        else throw std::runtime_error(message);
    }
};
```

This parser handles expressions with any number of binary operators, and
respects the usual precedence and associativity rules: multiplication and
division before addition and subtraction, and left associativity for all
operators. It also handles parenthesized expressions. It expects the input to be
a sequence of `NUMBER`, `PLUS`, `MINUS`, `STAR`, `SLASH`, `LEFT_PAREN`, and
`RIGHT_PAREN` tokens.

## User ---








 */

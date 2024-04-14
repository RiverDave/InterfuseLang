#include <vector>
#include "../include/Token.h"

class Parser {

public:

  explicit Parser(std::vector<Token> &tokens) : _tokens(tokens) {}

  void parse();
private:
  std::vector<Token> _tokens;

};

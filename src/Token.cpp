#include "Token.h"
#ifndef TOKEN_CPP
#define TOKEN_CPP


  std::ostream& operator <<(std::ostream& os, const Token& obj){
   obj.display(os) ;
   return os;
  }
#endif

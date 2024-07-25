//This is not being used as of now, It was replaced by bison's built-in error handler
#ifndef FUSEHANDLER_H
#define FUSEHANDLER_H

#include <Token.h>
#include <memory>
#include <vector>

class FuseHandler final {


public:
    FuseHandler() = default;
    //Takes a vector of tokens and outputs ranges, location of these errors
    TokenLocation getErrorLocation(std::vector<std::unique_ptr<Token>>);
    int err_cnt;


    //Should visually display in a fancy way (see rustc) the line where the error token
    //is located
    void prettyFmtError();
};


#endif

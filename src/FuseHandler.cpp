#include <FuseHandler.h>
#include <Token.h>


//We need the values to first and last Token
//and return the line member of the first
// & the ranges.second from last, which is the location
// of the error sequence.
// TokenLocation FuseHandler::getErrorLocation(std::vector<std::unique_ptr<Token>> tokens) {
//
//     //Assert that all tokens are on the same line
//     int line = tokens.front()->getLocation().line;
//
//     for (const auto &token: tokens) {
//         assert(token->getLocation().line == line);
//     }
//
//     TokenLocation coordinates;
//     coordinates.line = line;
//
//     if (tokens.size() >= 2) {
//
//         coordinates.range.first = tokens.front()->getLocation().range.first;
//
//         if(tokens.back()->getLocation().range.second.has_value())
//             coordinates.range.second = tokens.back()->getLocation().range.second.value();
//         else if(tokens.front()->getLocation().range.second.has_value())
//             coordinates.range.second = tokens.front()->getLocation().range.second.has_value();
//
//
//
//     } else {
//
//         coordinates.range.first = tokens.front()->getLocation().range.first;
//         if(tokens.front()->getLocation().range.second.has_value())
//           coordinates.range.second = tokens.front()->getLocation().range.second.value();
//     }
//
//
//     return coordinates;
// }

#include "Token.h"
#include <string>

#ifndef ITERATOR_INTERFACE
#define ITERATOR_INTERFACE

class IteratorWrapper {

    std::string::iterator pos;
    TokenLocation location;

public:
    void setInput(std::string &);
    explicit IteratorWrapper() = default;
    explicit IteratorWrapper(const IteratorWrapper &other) : pos(other.pos), location(other.location) {
    }

    IteratorWrapper &operator+=(const int &);
    IteratorWrapper &operator++();
    IteratorWrapper &operator--();//TODO
    IteratorWrapper &operator=(const std::string::iterator &);
    std::string::iterator &operator*();
    [[nodiscard]] TokenLocation &getLocation() { return location; }
    [[nodiscard]] std::pair<size_t, std::optional<size_t>> &getColumnCoordinates() { return location.range; };
    [[nodiscard]] std::string::iterator &getIterator() { return pos; }// alternative to dereferencing(cleaner)
};


#endif

#include "Token.h"
#include <string>

#ifndef ITERATOR_INTERFACE
#define ITERATOR_INTERFACE

class IteratorInterface {

    std::string::iterator pos;
    TokenLocation location;

public:
    void setInput(std::string&);
    explicit IteratorInterface() = default;
    explicit IteratorInterface(const IteratorInterface &other) : pos(other.pos), location(other.location) {
    }

    IteratorInterface &operator+=(const int &other);
    IteratorInterface &operator++();
    IteratorInterface &operator--();
    IteratorInterface &operator=(const std::string::iterator &other);
    std::string::iterator &operator*();
    [[nodiscard]] TokenLocation& getLocation() { return location; }
    [[nodiscard]] std::string::iterator& getIterator() { return pos; }// alternative to dereferencing(cleaner)
};


#endif

#include "IteratorInterface.h"
#include <cassert>

//TODO: Implement exceptions on failures

void IteratorInterface::setInput(std::string& input) {
    pos = input.begin();
}

IteratorInterface &IteratorInterface::operator++() {

    if (*pos == '\n') {
        // std::cout << "New line" << std::endl;
        location.range.first = 1;
        location.line++;
    } else {
        // std::cout << "Not new line" << std::endl;
        location.range.first++;
    }

    pos++;
    return *this;
}

IteratorInterface &IteratorInterface::operator+=(const int &other) {
    int val = other;

    while (val--)
        ++(*this);

    return *this;
}

IteratorInterface &IteratorInterface::operator=(const std::string::iterator &other) {
    assert(other >= pos);

    while (pos != other)
        ++(*this);

    return *this;
}

std::string::iterator &IteratorInterface::operator*() {
//    assert(*pos != '\0' && "Invalid char in iterator");
    return pos;
}

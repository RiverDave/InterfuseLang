#include "IteratorInterface.h"
#include <cassert>

//TODO: Implement exceptions on failures

void IteratorWrapper::setInput(std::string& input) {
    pos = input.begin();
}

IteratorWrapper &IteratorWrapper::operator++() {

    if (*pos == '\n') {
        // std::cout << "New line" << std::endl;
        location.begin = 1;
        location.line++;
    } else {
        // std::cout << "Not new line" << std::endl;
        location.begin++;
    }

    pos++;
    return *this;
}

IteratorWrapper &IteratorWrapper::operator+=(const int &other) {
    int val = other;

    while (val--)
        ++(*this);

    return *this;
}

IteratorWrapper &IteratorWrapper::operator=(const std::string::iterator &other) {
    assert(other >= pos);

    while (pos != other)
        ++(*this);

    return *this;
}

std::string::iterator &IteratorWrapper::operator*() {
//    assert(*pos != '\0' && "Invalid char in iterator");
    return pos;
}

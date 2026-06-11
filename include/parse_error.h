#ifndef PARSE_ERROR_H
#define PARSE_ERROR_H

#include <stdexcept>
#include <sstream>
#include <string>

class ParseError : public std::runtime_error {
public:
    ParseError(const std::string& message)
        : std::runtime_error(format(message)) {}

private:
    static std::string format(const std::string& message) {
        std::ostringstream oss;
        oss << "\n[ Error ]\n"
            << "reason: " << message;
        return oss.str();
    }
};

#endif

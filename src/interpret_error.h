#ifndef INTERPRET_ERROR_H
#define INTERPRET_ERROR_H

#include <stdexcept>
#include <sstream>
#include <string>

class InterpretError : public std::runtime_error {
public:
    InterpretError(const std::string& message)
        : std::runtime_error(format(message)) {}

private:
    static std::string format(const std::string& message) {
        std::ostringstream oss;
        oss << "[ Interpret Error ]\n"
            << "reason: " << message;
        return oss.str();
    }
};

#endif

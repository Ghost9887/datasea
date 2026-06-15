#ifndef VALUE_H
#define VALUE_H

#include <common.h>
#include <variant>
#include <vector>

struct Value {
    std::variant<std::monostate, int, double, bool, std::string, std::vector<Value>> m_data;
};

std::string value_to_string(const Value &value);

#endif

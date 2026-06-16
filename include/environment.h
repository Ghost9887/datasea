#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <common.h>
#include <value.h>
#include <vector>
#include <unordered_map>
#include <optional>

class Environment {
public:
    Environment(Environment *parent);
    bool insert_var(std::string name, Value value);
    bool assign_var(const std::string &name, Value value);
    std::optional<Value> get_var(const std::string &name);   
    bool has_parent();
    ~Environment() = default;
private:
    std::unordered_map<std::string, Value> m_variables {};
    Environment *m_parent;
};

#endif

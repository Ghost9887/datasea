#include <environment.h>

Environment::Environment(Environment *parent) :
    m_parent(std::move(parent)) {}

bool Environment::insert_var(std::string name, Value value) {
    if (m_variables.find(name) == m_variables.end()) {
        m_variables.insert({name, value});
        return true;
    }
    return false;
}

bool Environment::assign_var(const std::string &name, Value value) {
    if (m_variables.find(name) != m_variables.end()) {
        m_variables.at(name) = value;
        return true;
    }else if (m_parent) {
        return m_parent->assign_var(name, value);
    }
    return false;
}

std::optional<Value> Environment::get_var(const std::string &name) {
    if (m_variables.find(name) != m_variables.end()) {
        return m_variables.at(name);
    }else if (m_parent) {
        return m_parent->get_var(name);
    }
    return std::nullopt;
}

bool Environment::has_parent() {
    return m_parent;
}

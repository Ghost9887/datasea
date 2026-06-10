#include <token.h>

Token::Token(TokenType type, Value value, size_t line, size_t column) :
	m_type(type), m_value(value), m_line(line), m_column(column) {}

std::string Token::to_string() const {
	return std::format("Token[{}, {}, {}, {}]", tokens_map.at(m_type), Token::value_to_string(m_value), std::to_string(m_line), std::to_string(m_column));
}

std::string Token::value_to_string(const Value &value) {
    return std::visit([](auto &&arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        }else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        }else if constexpr (std::is_arithmetic_v<T>) {
            return std::to_string(arg);
        }else {
            return "empty";
        }
    }, value);
}

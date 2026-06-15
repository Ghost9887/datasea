#include <token.h>

Token::Token(TokenType type, Value value, size_t line, size_t column) :
	m_type(type), m_value(std::move(value)), m_line(line), m_column(column) {}

std::string Token::to_string() const {
	return std::format("Token[{}, {}, {}, {}]", tokens_map.at(m_type), value_to_string(m_value), std::to_string(m_line), std::to_string(m_column));
}

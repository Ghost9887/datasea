#ifndef TOKEN_H
#define TOKEN_H

#include <common.h>
#include <variant>
#include <unordered_map>

enum class TokenType {
	LPAREN, RPAREN,
	LBRACE, RBRACE,
	DOT, DOUBLE_DOT, COMMA,

	STRING, DIGIT, BOOL,

	VARCHAR, INT, BOOLEAN,

	TABLE, COLUMN, COUNT,

	UKNOWN
};

inline static const std::unordered_map<TokenType, std::string> tokens_map = {
	{TokenType::LPAREN, "LParen"}, {TokenType::RPAREN, "RParen"},
	{TokenType::LBRACE, "LBrace"}, {TokenType::RBRACE, "RBrace"},
	{TokenType::DOT, "Dot"}, {TokenType::DOUBLE_DOT, "Double_Dot"},
	{TokenType::COMMA, "Comma"}, 

	{TokenType::STRING, "String"}, {TokenType::DIGIT, "Digit"},
	{TokenType::BOOL, "Bool"},

	{TokenType::VARCHAR, "Varchar"}, {TokenType::INT, "Int"},
	{TokenType::BOOLEAN, "Boolean"}, 

	{TokenType::TABLE, "Table"}, {TokenType::COLUMN, "Column"},
	{TokenType::COUNT, "Count"},

	{TokenType::UKNOWN, "Uknown"},
};

inline static const std::unordered_map<std::string, TokenType> keywords_map = {
	{"varchar", TokenType::VARCHAR}, {"int", TokenType::INT},
	{"boolean", TokenType::BOOLEAN}, {"table", TokenType::TABLE},
	{"column", TokenType::COLUMN}, {"count", TokenType::COUNT}
};

using Value = std::variant<std::monostate, int, std::string, bool>;

class Token {
public:
	Token(TokenType type, Value value, size_t line, size_t column);
	std::string to_string() const;
	static std::string value_to_string(const Value &value);
	~Token() = default;
public:
	TokenType m_type;
	Value m_value;
	size_t m_line;
	size_t m_column;
};

inline std::ostream& operator<<(std::ostream& os, const Token& t) {
    os << t.to_string();
    return os;
}

#endif

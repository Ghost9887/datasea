#ifndef PARSER_H
#define PARSER_H

#include <common.h>
#include <token.h>
#include <stmnt.h>
#include <parse_error.h>
#include <optional>

class Parser {
public:
	Parser(std::vector<Token> tokens);
	std::vector<std::unique_ptr<Stmnt>> parse();
	~Parser() = default;
private:
	std::unique_ptr<Stmnt> statement();
	std::unique_ptr<Stmnt> parse_table();
	std::unique_ptr<Stmnt> parse_block();
    std::unique_ptr<Stmnt> parse_locale();
	std::unique_ptr<Expr> expression();
	std::unique_ptr<Expr> parse_column();
	std::unique_ptr<Expr> parse_count();
	ColumnType parse_column_type();
	ColumnType parse_varchar_type();
	ColumnType parse_int_type();
	ColumnType parse_boolean_type();
	template<typename... TokenTypes>
	bool match(TokenTypes... types);
	std::optional<std::reference_wrapper<Token>> peek();
	void advance();
	Token &previous();
	void consume(TokenType type, const std::string &message);
	[[noreturn]]
	void error(const std::string &message);
private:
	std::vector<Token> m_tokens;
	size_t m_ip {0};
};

#endif

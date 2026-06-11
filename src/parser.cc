#include <parser.h>

Parser::Parser(std::vector<Token> tokens) :
	m_tokens(std::move(tokens)) {}

std::vector<std::unique_ptr<Stmnt>> Parser::parse() {
	std::vector<std::unique_ptr<Stmnt>> statements {};
	
	try {
		while (m_ip < m_tokens.size()) {
			statements.push_back(statement());
		}
		return statements;
	} catch(const ParseError &e) {
		std::cerr << e.what() << std::endl;
		return statements;
	}
}

std::unique_ptr<Stmnt> Parser::statement() {
	if (match(TokenType::TABLE)) return parse_table();
	else if (match(TokenType::LBRACE)) return parse_block();

	error("Expected 'table' || 'block'.");
}

std::unique_ptr<Stmnt> Parser::parse_table() {
	consume(TokenType::LPAREN, "Expected '('.");
	consume(TokenType::STRING, "Expected 'table_name'.");
	std::string table_name = std::get<std::string>(previous().m_value);
	consume(TokenType::RPAREN, "Expected ')'.");
	std::unique_ptr<Stmnt> table_body { statement() };
	return std::make_unique<TableStmnt>(table_name, std::move(table_body));
}

std::unique_ptr<Stmnt> Parser::parse_block() {
	constexpr int MAX_BLOCK_SIZE = 256;
	std::vector<std::unique_ptr<Expr>> expressions {};
	size_t count {0};
	while (true) {
		if (count >= MAX_BLOCK_SIZE) { 
			error("Block limit reached");
		}
		expressions.push_back(expression());
		if (!match(TokenType::RBRACE)) {
			consume(TokenType::COMMA, "Expected ','.");
		}else break;
	}
	return std::make_unique<BlockStmnt>(std::move(expressions));
}

std::unique_ptr<Expr> Parser::expression() {
	if (match(TokenType::COLUMN)) return parse_column();
	else if (match(TokenType::COUNT)) return parse_count();

	error("Expected 'column' || 'count'.");
}

std::unique_ptr<Expr> Parser::parse_count() {
	consume(TokenType::LPAREN, "Expected '('.");
	consume(TokenType::DIGIT, "Expected 'count'.");
	int count { std::get<int>(previous().m_value) };
	consume(TokenType::RPAREN, "Expected ')'.");
	return std::make_unique<CountExpr>(count);
}

std::unique_ptr<Expr> Parser::parse_column() {
	consume(TokenType::LPAREN, "Expected '('.");
	consume(TokenType::STRING, "Expected 'column_name'.");
	std::string column_name { std::get<std::string>(previous().m_value) };
	consume(TokenType::COMMA, "Expected ','.");
	ColumnType column_type { parse_column_type() };
	consume(TokenType::RPAREN, "Expected ')'.");
	return std::make_unique<ColumnExpr>(column_name, column_type);
}

ColumnType Parser::parse_column_type() {
	if (match(TokenType::VARCHAR)) return parse_varchar_type();
	else if (match(TokenType::INT)) return parse_int_type();
	else if (match(TokenType::BOOLEAN)) return parse_boolean_type();

	error("Expcected 'varchar' || 'int' || 'boolean'.");
}

ColumnType Parser::parse_varchar_type() {
	consume(TokenType::LPAREN, "Expected '('.");
	consume(TokenType::DIGIT, "Expected 'varchar amount'.");
	int amount { std::get<int>(previous().m_value) };
	consume(TokenType::RPAREN, "Expected ')'.");
	consume(TokenType::COMMA, "Expected ','.");
	consume(TokenType::STRING, "Expected 'pattern'.");
	std::string pattern { std::get<std::string>(previous().m_value) };
	return VarcharType(amount, pattern);
}

ColumnType Parser::parse_int_type() {
	consume(TokenType::LPAREN, "Expected '('.");
	consume(TokenType::DIGIT, "Expected 'start'.");
	int start { std::get<int>(previous().m_value) };
	consume(TokenType::DOUBLE_DOT, "Expected '..'.");
	std::optional<int> end { std::nullopt };
	if (match(TokenType::DIGIT)) {
		end = std::get<int>(previous().m_value);
	}
	consume(TokenType::RPAREN, "Expected ')'.");
	return IntType(start, end);
}

ColumnType Parser::parse_boolean_type() {
	return BooleanType();
}

template<typename... TokenTypes>
bool Parser::match(TokenTypes... types) {
    for (TokenType type : {types...}) {
        std::optional<std::reference_wrapper<Token>> token { peek() };
        if (token.has_value() && token.value().get().m_type == type) { 
            advance();
            return true;
        }
    }
    return false;
}

std::optional<std::reference_wrapper<Token>> Parser::peek() {
    if (m_ip < m_tokens.size()) {
        return m_tokens.at(m_ip);
    }
    return std::nullopt;
}

Token &Parser::previous() {
    return m_tokens.at(m_ip - 1);
}

void Parser::advance() {
    m_ip++;
}

void Parser::consume(TokenType type, const std::string &message) {
    std::optional<std::reference_wrapper<Token>> token { peek() };
    if (token.has_value() && token.value().get().m_type == type) {
        advance();
        return;
    }
 	
	error(message);
}

[[noreturn]]
void Parser::error(const std::string &message) {
	Token &token = previous();
	throw ParseError(std::format("{}  [line: {}, column: {}]",
		message,
		token.m_line,
		token.m_column
	));
}

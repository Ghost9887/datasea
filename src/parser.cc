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
    else if (match(TokenType::LOCALE)) return parse_locale();
    else if (match(TokenType::ROWS)) return parse_rows();
    else if (match(TokenType::COLUMN)) return parse_column();

	error("Expected 'table' || 'locale' || 'rows' || 'column'.");
}

std::unique_ptr<Stmnt> Parser::parse_rows() {
	consume(TokenType::LPAREN, "Expected '('.");
	consume(TokenType::DIGIT, "Expected 'count'.");
	int count { std::get<int>(previous().m_value) };
	consume(TokenType::RPAREN, "Expected ')'.");
    std::unique_ptr<Stmnt> body { statement() };
	return std::make_unique<RowStmnt>(count, std::move(body));
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
	std::vector<std::unique_ptr<Stmnt>> expressions {};
	size_t count {0};
	while (true) {
		if (count >= MAX_BLOCK_SIZE) { 
			error("Block limit reached");
            break;
		}
        if (match(TokenType::RBRACE)) break;
		expressions.push_back(statement());
	}
	return std::make_unique<BlockStmnt>(std::move(expressions));
}

std::unique_ptr<Stmnt> Parser::parse_locale() {
    consume(TokenType::LPAREN, "Expected '('.");
    consume(TokenType::STRING, "Expected 'locale_value'.");
    std::string locale { std::get<std::string>(previous().m_value) };
    consume(TokenType::RPAREN, "Expected ')'.");
    consume(TokenType::SEMICOLON, "Expected ';'.");
    return std::make_unique<LocaleStmnt>(std::move(locale));
}

std::unique_ptr<Stmnt> Parser::parse_column() {
	consume(TokenType::LPAREN, "Expected '('.");
	consume(TokenType::STRING, "Expected 'column_name'.");
	std::string column_name { std::get<std::string>(previous().m_value) };
	consume(TokenType::COMMA, "Expected ','.");
	ColumnType column_type { parse_column_type() };
    std::optional<std::unique_ptr<Expr>> parameter { std::nullopt };  
    if (match(TokenType::COMMA)) {
        parameter = expression();
    }
	consume(TokenType::RPAREN, "Expected ')'.");
    consume(TokenType::SEMICOLON, "Expected ';'.");
	return std::make_unique<ColumnStmnt>(column_name, column_type, std::move(parameter));
}

ColumnType Parser::parse_column_type() {
    if (match(TokenType::VARCHAR)) return ColumnType::VARCHAR;   
    else if (match(TokenType::INT)) return ColumnType::INT;
    else if (match(TokenType::BOOLEAN)) return ColumnType::BOOLEAN;
	error("Expcected 'varchar' || 'int' || 'boolean'.");
}

std::unique_ptr<Expr> Parser::expression() {
    if (match(TokenType::INCREMENT)) return parse_increment();
    else if (match(TokenType::RANDOM)) return parse_random();
    else if (match(TokenType::GEN)) return parse_gen();
    else if (match(TokenType::FORMAT)) return parse_format();

    error("Expected 'increment' || 'random' || 'format'.");
}

std::unique_ptr<Expr> Parser::parse_increment() {
    consume(TokenType::LPAREN, "Expected '('.");
    consume(TokenType::DIGIT, "Expected 'start'.");
    int start { std::get<int>(previous().m_value) };
    consume(TokenType::DOUBLE_DOT, "Expected '..'.");
    std::optional<int> end { std::nullopt };
    if (match(TokenType::DIGIT)) {
        end = std::get<int>(previous().m_value);
    }
    consume(TokenType::RPAREN, "Expected ')'.");
    return std::make_unique<IncrementExpr>(start, end);
}

std::unique_ptr<Expr> Parser::parse_random() {
    consume(TokenType::LPAREN, "Expected '('.");
    consume(TokenType::DIGIT, "Expected 'start'.");
    int start { std::get<int>(previous().m_value) };
    consume(TokenType::DOUBLE_DOT, "Expected '..'.");
    std::optional<int> end { std::nullopt };
    if (match(TokenType::DIGIT)) {
        end = std::get<int>(previous().m_value);
    }
    consume(TokenType::RPAREN, "Expected ')'.");
    return std::make_unique<RandomExpr>(start, end);
}

std::unique_ptr<Expr> Parser::parse_gen() {
    consume(TokenType::LPAREN, "Expected '('.");
    if (match(TokenType::BOOLEAN, TokenType::FIRST_NAME, TokenType::LAST_NAME, TokenType::SEX)) {
        TokenType type { previous().m_type };
        consume(TokenType::RPAREN, "Expected ')'.");
        return std::make_unique<GenExpr>(type);
    }else {
        error("Expected boolean || firstname || lastname || sex");
    }
}

std::unique_ptr<Expr> Parser::parse_format() {
    constexpr size_t MAX_VARIABLES { 5 };
    size_t counter { 0 };
    consume(TokenType::LPAREN, "Expected '('.");
    consume(TokenType::STRING, "Expected pattern'.");
    std::string pattern { std::get<std::string>(previous().m_value) };
    consume(TokenType::COMMA, "Expected ','.");
    std::vector<std::unique_ptr<Expr>> variables {};
    while (true) {
        if (counter >= MAX_VARIABLES) {
            error("Too many format variables [max is 5].");
        }
        variables.push_back(expression());
        if (match(TokenType::RPAREN)) break;
        else consume(TokenType::COMMA, "Expected ','.");
        counter++;
    }
    return std::make_unique<FormatExpr>(std::move(pattern), std::move(variables)); 
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

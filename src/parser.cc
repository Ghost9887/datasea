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
        std::exit(1);
	}
}

std::unique_ptr<Stmnt> Parser::statement() {
	if (match(TokenType::TABLE)) return parse_table();
	else if (match(TokenType::LBRACE)) return parse_block();
    else if (match(TokenType::LOCALE)) return parse_locale();
    else if (match(TokenType::COLUMN)) return parse_column();
    else if (match(TokenType::LET)) return parse_decl();
    else if (match(TokenType::PRINT)) return parse_print();
    else if (match(TokenType::IDENTIFIER)) return parse_assign();

	error("Expected statement");
}


std::unique_ptr<Stmnt> Parser::parse_table() {
	consume(TokenType::LPAREN, "Expected '('.");
	consume(TokenType::STRING, "Expected 'table_name'.");
	std::string table_name = std::get<std::string>(previous().m_value.m_data);
    consume(TokenType::COMMA, "Expected ','.");
    consume(TokenType::INT, "Expected 'row_count'.");
    int row_count = std::get<int>(previous().m_value.m_data);
	consume(TokenType::RPAREN, "Expected ')'.");
	std::unique_ptr<Stmnt> table_body { statement() };
	return std::make_unique<TableStmnt>(table_name, row_count, std::move(table_body));
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
    std::string locale { std::get<std::string>(previous().m_value.m_data) };
    consume(TokenType::RPAREN, "Expected ')'.");
    consume(TokenType::SEMICOLON, "Expected ';'.");
    return std::make_unique<LocaleStmnt>(std::move(locale));
}

std::unique_ptr<Stmnt> Parser::parse_column() {
	consume(TokenType::LPAREN, "Expected '('.");
	consume(TokenType::STRING, "Expected 'column_name'.");
	std::string column_name { std::get<std::string>(previous().m_value.m_data) };
    consume(TokenType::COMMA, "Expected ','.");
    std::unique_ptr<Expr> parameter { expression() };  
	consume(TokenType::RPAREN, "Expected ')'.");
    consume(TokenType::SEMICOLON, "Expected ';'.");
	return std::make_unique<ColumnStmnt>(column_name, std::move(parameter));
}

std::unique_ptr<Stmnt> Parser::parse_decl() {
    consume(TokenType::IDENTIFIER, "Expected 'variable_name'");
    std::string name { std::get<std::string>(previous().m_value.m_data) };
    if (match(TokenType::EQUAL)) {
        std::unique_ptr<Expr> expr { expression() };
        consume(TokenType::SEMICOLON, "Expected ';'.");
        return std::make_unique<DeclStmnt>(name, std::move(expr));
    }
    consume(TokenType::SEMICOLON, "Exptected ';'.");
    Value value { std::monostate() };
    return std::make_unique<DeclStmnt>(name, std::make_unique<ValueExpr>(std::move(value)));
}

std::unique_ptr<Stmnt> Parser::parse_print() {
    consume(TokenType::LPAREN, "Expected '('.");
    std::unique_ptr<Expr> expr { expression() };
    consume(TokenType::RPAREN, "Expected ')'.");
    consume(TokenType::SEMICOLON, "Expected ';'.");
    return std::make_unique<PrintStmnt>(std::move(expr));
}

std::unique_ptr<Stmnt> Parser::parse_assign() {
    std::string name { std::get<std::string>(previous().m_value.m_data) };
    consume(TokenType::EQUAL, "Expected '='.");
    std::unique_ptr<Expr> expr { expression() };
    consume(TokenType::SEMICOLON, "Expected ';'.");
    return std::make_unique<AssignStmnt>(std::move(name), std::move(expr));
}

std::unique_ptr<Expr> Parser::expression() {
    if (match(TokenType::INCREMENT)) return parse_increment();
    else if (match(TokenType::RANDINT)) return parse_randint();
    else if (match(TokenType::RANDBOOL)) return parse_randbool();
    else if (match(TokenType::RANDDOUBLE)) return parse_randdouble();
    else if (match(TokenType::GEN)) return parse_gen();
    else if (match(TokenType::FORMAT)) return parse_format();
    else if (match(TokenType::IDENTIFIER)) return parse_variable();
    else if (match(TokenType::LBRACKET)) return parse_list();
    else return parse_value();
}

std::unique_ptr<Expr> Parser::parse_increment() {
    consume(TokenType::LPAREN, "Expected '('.");
    consume(TokenType::INT, "Expected 'increment_start'.");
    int start { std::get<int>(previous().m_value.m_data) };
    consume(TokenType::DOUBLE_DOT, "Expected '..'.");
    std::optional<int> end { std::nullopt };
    if (match(TokenType::INT)) {
        end = std::get<int>(previous().m_value.m_data);
    }
    consume(TokenType::RPAREN, "Expected ')'.");
    return std::make_unique<IncrementExpr>(start, end);
}

std::unique_ptr<Expr> Parser::parse_randint() {
    consume(TokenType::LPAREN, "Expected '('.");
    consume(TokenType::INT, "Expected 'start'.");
    int start { std::get<int>(previous().m_value.m_data) };
    consume(TokenType::DOUBLE_DOT, "Expected '..'.");
    std::optional<int> end { std::nullopt };
    if (match(TokenType::INT)) {
        end = std::get<int>(previous().m_value.m_data);
    }
    consume(TokenType::RPAREN, "Expected ')'.");
    return std::make_unique<RandintExpr>(start, end);
}

std::unique_ptr<Expr> Parser::parse_randbool() {
    consume(TokenType::LPAREN, "Expected '('.");
    double weight { 0.5 };
    if (match(TokenType::DOUBLE)) {
        weight = std::get<double>(previous().m_value.m_data);
    }
    consume(TokenType::RPAREN, "Expected ')'.");
    return std::make_unique<RandboolExpr>(weight);
}

std::unique_ptr<Expr> Parser::parse_randdouble() {
    consume(TokenType::LPAREN, "Expected '('.");
    consume(TokenType::DOUBLE, "Expected 'start'.");
    double start { std::get<double>(previous().m_value.m_data) };
    consume(TokenType::DOUBLE_DOT, "Expected '..'.");
    std::optional<double> end { std::nullopt };
    if (match(TokenType::DOUBLE)) {
        end = std::get<double>(previous().m_value.m_data);
    }
    consume(TokenType::RPAREN, "Expected ')'.");
    return std::make_unique<RanddoubleExpr>(start, end);
}

std::unique_ptr<Expr> Parser::parse_gen() {
    consume(TokenType::LPAREN, "Expected '('.");
    advance();
    TokenType type { previous().m_type };
    consume(TokenType::RPAREN, "Expected ')'.");
    return std::make_unique<GenExpr>(type);
}

std::unique_ptr<Expr> Parser::parse_format() {
    consume(TokenType::LPAREN, "Expected '('.");
    consume(TokenType::STRING, "Expected pattern'.");
    std::string pattern { std::get<std::string>(previous().m_value.m_data) };
    std::vector<std::unique_ptr<Expr>> arguments {};
    if(match(TokenType::COMMA)) {
        while (true) {
            arguments.push_back(expression());
            if (match(TokenType::RPAREN)) break;
            else consume(TokenType::COMMA, "Expected ','.");
        }
        return std::make_unique<FormatExpr>(std::move(pattern), std::move(arguments));
    }
    consume(TokenType::RPAREN, "Expected ')'.");
    return std::make_unique<FormatExpr>(std::move(pattern), std::move(arguments)); 
}

std::unique_ptr<Expr> Parser::parse_value() {
    if (match(TokenType::STRING, TokenType::INT, TokenType::DOUBLE, TokenType::BOOL, TokenType::_NULL)) {
        Value value { previous().m_value.m_data };
        return std::make_unique<ValueExpr>(std::move(value));
    }

    error("Expected value.");
}

std::unique_ptr<Expr> Parser::parse_variable() {
    std::string variable_name { std::get<std::string>(previous().m_value.m_data) };
    std::vector<std::unique_ptr<Expr>> functions {};
    while (match(TokenType::DOT)) {
        functions.push_back(func_expression());
    }
    return std::make_unique<VariableExpr>(std::move(variable_name), std::move(functions));
}

std::unique_ptr<Expr> Parser::parse_list() {
    constexpr size_t MAX_ARRAY_SIZE { 256 };
    size_t counter {0};
    std::vector<std::unique_ptr<Expr>> expressions {};
    while (true) {
        if (match(TokenType::RBRACKET)) break;
        if (counter > 0) consume(TokenType::COMMA, "Expected ','.");
        if (counter >= MAX_ARRAY_SIZE) error("Too many values in array: [max is 256].");
        expressions.push_back(expression());
        counter++;
    }
    return std::make_unique<ListExpr>(std::move(expressions));
}

std::unique_ptr<Expr> Parser::func_expression() {
    if (match(TokenType::AT)) return parse_at_func();
    else if (match(TokenType::SUBSTR)) return parse_substr_func();
    else if (match(TokenType::LOWER)) return parse_lower_func();
    else if (match(TokenType::UPPER)) return parse_upper_func();

    error("Expected function expression");
}

std::unique_ptr<Expr> Parser::parse_at_func() {
    consume(TokenType::LPAREN, "Expected '('.");
    std::unique_ptr<Expr> index { expression() };
    consume(TokenType::RPAREN, "Expected ')'.");
    return std::make_unique<AtFuncExpr>(std::move(index));
}

std::unique_ptr<Expr> Parser::parse_substr_func() {
    consume(TokenType::LPAREN, "Expected '('.");
    std::unique_ptr<Expr> start { expression() };
    consume(TokenType::DOUBLE_DOT, "Expected '..'.");
    std::unique_ptr<Expr> end { expression() };
    consume(TokenType::RPAREN, "Expected ')'.");
    return std::make_unique<SubstrFuncExpr>(std::move(start), std::move(end));
}

std::unique_ptr<Expr> Parser::parse_lower_func() {
    consume(TokenType::LPAREN, "Expected '('.");
    consume(TokenType::RPAREN, "Expected ')'.");
    return std::make_unique<LowerFuncExpr>();
}

std::unique_ptr<Expr> Parser::parse_upper_func() {
    consume(TokenType::LPAREN, "Expected '('.");
    consume(TokenType::RPAREN, "Expected ')'.");
    return std::make_unique<UpperFuncExpr>();
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

#include <scanner.h>

Scanner::Scanner(std::string source) :
	m_source(source) {}

std::vector<Token> Scanner::tokenize() {
	while(in_bounds()) {
		m_start = m_current;
		char c = advance();

		switch (c) {
			case '(': make_token(TokenType::LPAREN); break;
			case ')': make_token(TokenType::RPAREN); break;
			case '{': make_token(TokenType::LBRACE); break;
			case '}': make_token(TokenType::RBRACE); break;
			case '.': {
				if (peek() == '.') {
					make_token(TokenType::DOUBLE_DOT);
					advance();
				}else make_token(TokenType::DOT);
			};break;
			case ',': make_token(TokenType::COMMA); break;
			case '\n': m_line++; m_column = 0; break;
			case '"': parse_string(); break;
			default: {
				 if (std::isdigit(c)) {
					 parse_digit();
				 }else if (std::isalpha(c)) {
					 parse_keyword();
				 }
			}; break;
		}
	}

	return m_tokens;
}

bool Scanner::in_bounds() {
	return m_current < m_source.size();
}

char Scanner::advance() {
	m_column++;
	return m_source.at(m_current++);
}

char Scanner::peek() {
	return m_source.at(m_current);
}

void Scanner::make_token(TokenType type) {
	m_tokens.emplace_back(Token(type, std::monostate(), m_line, m_column));
}

void Scanner::make_token(TokenType type, Value value) {
	m_tokens.emplace_back(Token(type, value, m_line, m_column));
}

void Scanner::parse_string() {
	m_start = m_current;
	while (in_bounds() && peek() != '"') advance();
	make_token(TokenType::STRING, m_source.substr(m_start, m_current - m_start));
	advance();
}

void Scanner::parse_digit() {
	while(std::isdigit(peek())) {
		advance();
	}
	std::string digit { m_source.substr(m_start, m_current - m_start) };
	make_token(TokenType::DIGIT, atoi(digit.c_str()));
}

void Scanner::parse_keyword() {
	while (std::isalpha(peek())) advance();
	
	std::string keyword { m_source.substr(m_start, m_current - m_start) };

	if (keywords_map.find(keyword) != keywords_map.end()) {
		make_token(keywords_map.at(keyword));
	} else {
		make_token(TokenType::UKNOWN);
	}
}

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
            case '[': make_token(TokenType::LBRACKET); break;
            case ']': make_token(TokenType::RBRACKET); break;
            case '$': make_token(TokenType::DOLLAR); break;
            case '=': make_token(TokenType::EQUAL); break;
            case '-': parse_digit(); break;
			case '.': {
				if (peek() == '.') {
					make_token(TokenType::DOUBLE_DOT);
					advance();
				}else make_token(TokenType::DOT);
			};break;
			case ',': make_token(TokenType::COMMA); break;
            case ';': make_token(TokenType::SEMICOLON); break;
			case '\n': m_line++; m_column = 0; break;
			case '"': parse_string(); break;
			default: {
				 if (std::isdigit(c)) {
					 parse_digit();
				 }else if (is_alpha(c)) {
					 parse_identifier();
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
	m_tokens.emplace_back(Token(type, Value{ std::monostate() }, m_line, m_column));
}

void Scanner::make_token(TokenType type, Value value) {
	m_tokens.emplace_back(Token(type, std::move(value), m_line, m_column));
}

void Scanner::parse_string() {
	m_start = m_current;
	while (in_bounds() && peek() != '"') advance();
	make_token(TokenType::STRING, Value{ m_source.substr(m_start, m_current - m_start) });
	advance();
}

void Scanner::parse_digit() {
    bool is_double { false };
	while(std::isdigit(peek())) {
		advance();
	}
    
    /*
    if (peek() == '.') {
        is_double = true;
        advance();
        while(std::isdigit(peek())) {
		   advance();
	    }
    }
    */

	std::string digit { m_source.substr(m_start, m_current - m_start) };

    if (is_double) {
	    make_token(TokenType::DOUBLE, Value{ stod(digit) });
        return;
    }
    make_token(TokenType::INT, Value{ stoi(digit) });
}

bool Scanner::is_alpha_numeric(char c) {
    return ('0' <= c && c <= '9')
                    ||
           ('a' <= c && c <= 'z')
                    ||
                (c == '_');
}

bool Scanner::is_alpha(char c) {
    return ('a' <= c && c <= 'z')
                    ||
                (c == '_');
}

void Scanner::parse_identifier() {
	while (is_alpha_numeric(peek())) advance();
	
	std::string identifier { m_source.substr(m_start, m_current - m_start) };

    if (identifier == "null") make_token(TokenType::_NULL, Value{ std::monostate() });
    else if (identifier == "true") make_token(TokenType::BOOL, Value{ true });
    else if (identifier == "false") make_token(TokenType::BOOL, Value {false});
    else if (keywords_map.find(identifier) != keywords_map.end()) {
		make_token(keywords_map.at(identifier));
	} else {
		make_token(TokenType::IDENTIFIER, Value{ identifier });
	}
}

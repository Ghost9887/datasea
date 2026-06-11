#ifndef SCANNER_h
#define SCANNER_h

#include <common.h>
#include <token.h>
#include <vector>

class Scanner {
public:
	Scanner(std::string source);
	std::vector<Token> tokenize();
	~Scanner() = default;
private:
	std::string m_source;
	size_t m_start { 0 };
	size_t m_current { 0 };
	size_t m_line { 0 };
	size_t m_column { 0 };
	std::vector<Token> m_tokens {};
private:
	bool in_bounds();
	char advance(); 
	char peek();
	void make_token(TokenType type);
	void make_token(TokenType type, Value value);
	void parse_string();
	void parse_digit();
	void parse_keyword();
};

#endif

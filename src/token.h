#ifndef TOKEN_H
#define TOKEN_H

#include <common.h>
#include <variant>
#include <unordered_map>
#include <value.h>

enum class TokenType {
	LPAREN, RPAREN,
	LBRACE, RBRACE,
	DOT, DOUBLE_DOT, COMMA,
    SEMICOLON, DOLLAR,
    EQUAL, RBRACKET, LBRACKET,
    MINUS,

	STRING, INT, DOUBLE, BOOL, IDENTIFIER,
    _NULL,

	TABLE, COLUMN,     
    LOCALE, RANDINT, 
    RANDBOOL, RANDDOUBLE, 
    INCREMENT, 
    FORMAT, GEN, LET,
    PRINT, AT, SUBSTR, 
    LOWER, UPPER,

    //GEN TYPE IDENTIFIERS
    FIRST_NAME, LAST_NAME,
    SEX, CITY, COMPANY, DEPARTMENT, 
    JOB_TITLE, STATE, STREET_NAME,

	UKNOWN
};

inline static const std::unordered_map<TokenType, std::string> tokens_map = {
	{TokenType::LPAREN, "LParen"}, {TokenType::RPAREN, "RParen"},
	{TokenType::LBRACE, "LBrace"}, {TokenType::RBRACE, "RBrace"},
	{TokenType::DOT, "Dot"}, {TokenType::DOUBLE_DOT, "Double_Dot"},
	{TokenType::COMMA, "Comma"}, {TokenType::SEMICOLON, "Semicolon"}, 
    {TokenType::DOLLAR, "Dollar"}, {TokenType::EQUAL, "Equal"},
    {TokenType::LBRACKET, "LBracket"}, {TokenType::RBRACKET, "RBracket"},
    {TokenType::MINUS, "Minus"},

	{TokenType::STRING, "String"}, {TokenType::INT, "Int"},
    {TokenType::DOUBLE, "Double"}, {TokenType::BOOL, "Bool"}, 
    {TokenType::IDENTIFIER, "Identifier"}, {TokenType::_NULL, "Null"},
    {TokenType::AT, "At"}, {TokenType::SUBSTR, "Substr"},
    {TokenType::LOWER, "Lower"}, {TokenType::UPPER, "Upper"},

	{TokenType::TABLE, "Table"}, {TokenType::COLUMN, "Column"},
	{TokenType::INCREMENT, "Increment"}, {TokenType::RANDINT, "Randint"}, 
    {TokenType::RANDBOOL, "Randbool"}, {TokenType::RANDDOUBLE, "Randdouble"}, 
    {TokenType::FORMAT, "Format"},
    {TokenType::GEN, "Gen"}, {TokenType::LET, "Let"}, 

    {TokenType::FIRST_NAME, "First_Name"}, {TokenType::LAST_NAME, "Last_Name"},
    {TokenType::SEX, "Sex"}, {TokenType::PRINT, "Print"}, {TokenType::CITY, "City"},
    {TokenType::COMPANY, "Company"}, {TokenType::DEPARTMENT, "Department"},
    {TokenType::JOB_TITLE, "Job_Title"}, {TokenType::STATE, "State"}, 
    {TokenType::STREET_NAME, "Street_Name"},

    {TokenType::LOCALE, "Locale"},

	{TokenType::UKNOWN, "Uknown"},
};

inline static const std::unordered_map<std::string, TokenType> keywords_map = {
    //keywords
    {"table", TokenType::TABLE}, {"column", TokenType::COLUMN}, 
    {"increment", TokenType::INCREMENT}, {"randint", TokenType::RANDINT},
    {"randbool", TokenType::RANDBOOL},  {"randdouble", TokenType::RANDDOUBLE},
    {"locale", TokenType::LOCALE}, {"format", TokenType::FORMAT},
    {"gen", TokenType::GEN}, {"let", TokenType::LET}, 

    //gen keywords
    {"_firstname", TokenType::FIRST_NAME}, {"_boolean", TokenType::BOOL},
    {"_lastname", TokenType::LAST_NAME}, {"_sex", TokenType::SEX}, 
    {"_city", TokenType::CITY}, {"_company", TokenType::COMPANY},
    {"_department", TokenType::DEPARTMENT}, {"_jobtitle", TokenType::JOB_TITLE},
    {"_state", TokenType::STATE}, {"_streetname", TokenType::STREET_NAME},

    //functions
    {"print", TokenType::PRINT}, {"at", TokenType::AT}, 
    {"substr", TokenType::SUBSTR}, {"lower", TokenType::LOWER},
    {"upper", TokenType::UPPER}, 
};

class Token {
public:
	Token(TokenType type, Value value, size_t line, size_t column);
	std::string to_string() const;
	~Token() = default;
public:
	TokenType m_type;
	Value m_value;
	size_t m_line;
	size_t m_column;
};

#endif

#ifndef EXPR_H
#define EXPR_H

#include <common.h>
#include <optional>
#include <variant>

class ColumnExpr;
class CountExpr;

class ExprVisitor {
public:
	virtual void visitColumnExpr(ColumnExpr &expr) = 0;
	virtual void visitCountExpr(CountExpr &expr) = 0;
	virtual ~ExprVisitor() = default;
};

class Expr {
public:
	virtual void accept(ExprVisitor &visitor) = 0;
	virtual std::string to_string() const = 0;
	virtual ~Expr() = default;
};

class VarcharType {
public:
	VarcharType(int count, std::string pattern);
	std::string to_string() const;
	~VarcharType() = default;
public:
	int m_count;
	std::string m_pattern;
};

class IntType {
public:
	IntType(int start, std::optional<int> end, bool increment);
	std::string to_string() const;
	~IntType() = default;
public:
	int m_start;
	std::optional<int> m_end;
    bool m_increment;
};

class BooleanType {
public:
	BooleanType() = default;
	std::string to_string() const;
	~BooleanType() = default;
};

using ColumnType = std::variant<VarcharType, IntType, BooleanType>;

class ColumnExpr : public Expr {
public:
	ColumnExpr(std::string name, ColumnType column_type);
	void accept(ExprVisitor &visitor) override;
	std::string to_string() const override;
public:
	std::string m_name;
	ColumnType m_column_type;
};

class CountExpr : public Expr {
public: 
	CountExpr(int count);
	void accept(ExprVisitor &visitor) override;
	std::string to_string() const override;
public:
	int m_count;
};

#endif

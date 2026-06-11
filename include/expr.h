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
};

class Expr {
public:
	virtual void accept(ExprVisitor &visitor) = 0;
	virtual std::string to_string() const;
	virtual ~Expr() = default;
};

class VarcharType {
public:
	VarcharType(int count, std::string patter);
	std::string to_string() const;
	~VarcharType() = default;
public:
	int m_count;
	std::string m_pattern;
};

class IntType {
public:
	IntType(int start, std::optional<int> end);
	std::string to_string() const;
	~IntType() = default;
public:
	int m_start;
	std::optional<int> m_end;
};

class BooleanType {
public:
	BooleanType(bool value);
	std::string to_string() const;
	~BooleanType() = default;
public:
	bool m_value;
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

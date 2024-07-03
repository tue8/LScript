#pragma once

#include <memory>
#include <any>
#include <string>
#include <vector>
#include "Token.h"

class Call;
class Logical;
class Binary;
class Grouping;
class Literal;
class Unary;
class Variable;
class Assign;

template <typename T>
class ExprVisitor
{
public:
	virtual T visitCallExpr(Call& expr) = 0;
	virtual T visitLogicalExpr(Logical& expr) = 0;
	virtual T visitBinaryExpr(Binary &expr) = 0;
	virtual T visitGroupingExpr(Grouping &expr) = 0;
	virtual T visitLiteralExpr(Literal &expr) = 0;
	virtual T visitUnaryExpr(Unary& expr) = 0;
	virtual T visitVariableExpr(Variable& expr) = 0;
	virtual T visitAssignExpr(Assign &expr) = 0;
};

class Expr
{
public:
	virtual std::any accept(ExprVisitor<std::any> &visitor) = 0;
};

class Call : public Expr
{
public:
	Call(std::unique_ptr<Expr> callee, const Token& paren, std::vector<std::unique_ptr<Expr>> args)
		: callee(std::move(callee)), paren(paren), args(std::move(args))
	{}

	std::any accept(ExprVisitor<std::any>& visitor) override
	{
		return visitor.visitCallExpr(*this);
	}

	Expr& getCallee()
	{
		return *callee;
	}

	const Token& getParen()
	{
		return paren;
	}

	const std::vector<std::unique_ptr<Expr>>& getArgs()
	{
		return args;
	}

private:
	std::unique_ptr<Expr> callee;
	Token paren;
  std::vector<std::unique_ptr<Expr>> args;
};

class Logical : public Expr
{
public:
	Logical(std::unique_ptr<Expr> left, const Token& op, std::unique_ptr<Expr> right)
		: left(std::move(left)), op(op), right(std::move(right))
	{}

	std::any accept(ExprVisitor<std::any>& visitor) override
	{
		return visitor.visitLogicalExpr(*this);
	}

	Expr& getLeft()
	{
		return *left;
	}

	const Token& getOp()
	{
		return op;
	}

	Expr& getRight()
	{
		return *right;
	}

private:
	std::unique_ptr<Expr> left;
	Token op;
	std::unique_ptr<Expr> right;
};


class Binary : public Expr
{
public:
	Binary(std::unique_ptr<Expr> left, const Token &op, std::unique_ptr<Expr> right)
		: left(std::move(left)), op(op), right(std::move(right))
	{}

	std::any accept(ExprVisitor<std::any>& visitor) override
	{
		return visitor.visitBinaryExpr(*this);
	}

	Expr& getLeft()
	{
		return *left;
	}

	const Token& getOp()
	{
		return op;
	}

	Expr& getRight()
	{
		return *right;
	}

private:
	std::unique_ptr<Expr> left;
	Token op;
	std::unique_ptr<Expr> right;
};


class Grouping : public Expr
{
public:
	Grouping(std::unique_ptr<Expr> expression)
		: expression(std::move(expression))
	{}

	std::any accept(ExprVisitor<std::any>& visitor) override
	{
		return visitor.visitGroupingExpr(*this);
	}

	Expr& getExpr()
	{
		return *expression;
	}

private:
	std::unique_ptr<Expr> expression;
};

class Literal : public Expr
{
public:
	Literal(std::any lit)
		: lit(lit)
	{}

	std::any accept(ExprVisitor<std::any>& visitor) override
	{
		return visitor.visitLiteralExpr(*this);
	}

	std::any getLit()
	{
		return lit;
	}
private:
	std::any lit;
};


class Unary : public Expr
{
public:
	Unary(const Token& op, std::unique_ptr<Expr> right)
		: op(op), right(std::move(right))
	{}

	std::any accept(ExprVisitor<std::any>& visitor) override
	{
		return visitor.visitUnaryExpr(*this);
	}

	Expr& getRight()
	{
		return *right;
	}

	const Token& getOp()
	{
		return op;
	}

private:
	Token op;
	std::unique_ptr<Expr> right;
};

class Variable : public Expr
{
public:
	Variable(const Token& name)
		: name(name)
	{}

	std::any accept(ExprVisitor<std::any>& visitor) override
	{
		return visitor.visitVariableExpr(*this);
	}

	const Token& getName()
	{
		return name;
	}

private:
	Token name;
};

class Assign : public Expr
{
public:
	Assign(const Token& name, std::unique_ptr<Expr> value)
		: name(name), value(std::move(value))
	{}

	std::any accept(ExprVisitor<std::any>& visitor) override
	{
		return visitor.visitAssignExpr(*this);
	}

	const Token& getName()
	{
		return name;
	}

	Expr& getValue()
	{
		return *value;
	}
private:
	Token name;
	std::unique_ptr<Expr> value;
};

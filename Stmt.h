#pragma once

#include <memory>
#include <any>
#include <string>
#include "Expr.h"
#include "Environment.h"
#include <vector>

class If;
class Block;
class Expression;
class Print;
class Var;
class While;

template <typename T>
class StmtVisitor
{
public:
	virtual T visitIfStmt(If& stmt) = 0;
	virtual T visitBlockStmt(Block& stmt) = 0;
	virtual T visitExpressionStmt(Expression& stmt) = 0;
	virtual T visitPrintStmt(Print& stmt) = 0;
	virtual T visitVarStmt(Var& stmt) = 0;
	virtual T visitWhileStmt(While& stmt) = 0;
};

class Stmt
{
public:
	virtual std::any accept(StmtVisitor<std::any>& visitor) = 0;
};

class If : public Stmt
{
public:
	If(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> thenBranch, std::unique_ptr<Stmt> elseBranch)
		: condition(std::move(condition)),
			thenBranch(std::move(thenBranch)),
			elseBranch(std::move(elseBranch))
	{
    elseExist = (elseBranch == nullptr);
  }

	std::any accept(StmtVisitor<std::any>& visitor) override
	{
		return visitor.visitIfStmt(*this);
	}

	Expr& getCondition()
	{
		return *condition;
	}

	Stmt& getThen()
	{
		return *thenBranch;
	}

	Stmt& getElse()
	{
		return *elseBranch;
	}

  bool hasElse()
  {
    return elseExist;
  }
private:
	std::unique_ptr<Expr> condition;
	std::unique_ptr<Stmt> thenBranch;
	std::unique_ptr<Stmt> elseBranch;
  bool elseExist;
};


class Block : public Stmt
{
public:
	Block(std::vector<std::unique_ptr<Stmt>> statements)
		: statements(std::move(statements))
	{}

	std::any accept(StmtVisitor<std::any>& visitor) override
	{
		return visitor.visitBlockStmt(*this);
	}

	const std::vector<std::unique_ptr<Stmt>>& getStatements()
	{
		return statements;
	}
private:
	std::vector<std::unique_ptr<Stmt>> statements;
};


class Expression : public Stmt
{
public:
	Expression(std::unique_ptr<Expr> expression)
		: expression(std::move(expression))
	{}

	std::any accept(StmtVisitor<std::any>& visitor) override
	{
		return visitor.visitExpressionStmt(*this);
	}

	Expr& getExpr()
	{
		return *expression;
	}

private:
	std::unique_ptr<Expr> expression;
};

class Print : public Stmt
{
public:
	Print(std::unique_ptr<Expr> expression)
		: expression(std::move(expression))
	{}

	std::any accept(StmtVisitor<std::any>& visitor) override
	{
		return visitor.visitPrintStmt(*this);
	}

	Expr& getExpr()
	{
		return *expression;
	}

private:
	std::unique_ptr<Expr> expression;
};

class Var : public Stmt
{
public:
	Var(const Token& name, std::unique_ptr<Expr> initializer)
		: name(name), initializer(std::move(initializer))
	{}

	std::any accept(StmtVisitor<std::any>& visitor) override
	{
		return visitor.visitVarStmt(*this);
	}

	Token getName()
	{
		return name;
	}

	Expr& getInitializer()
	{
		return *initializer;
	}

private:
	Token name;
	std::unique_ptr<Expr> initializer;
};

class While : public Stmt
{
public:
	While(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body)
		: condition(std::move(condition)),
			body(std::move(body))
	{}

	std::any accept(StmtVisitor<std::any>& visitor) override
	{
		return visitor.visitWhileStmt(*this);
	}

	Expr& getCondition()
	{
		return *condition;
	}

	Stmt& getBody()
	{
		return *body;
	}
private:
	std::unique_ptr<Expr> condition;
	std::unique_ptr<Stmt> body;
};

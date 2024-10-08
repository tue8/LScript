#pragma once

#include "Stmt.h"
#include "Environment.h"
#include <list>

class Interpreter : public ExprVisitor<std::any>, public StmtVisitor<std::any>
{
public:
	void interpret(std::list<std::unique_ptr<Stmt>> statements);
	void setEnv(const Environment &env);
	Environment getEnv();
  	void executeBlock(const std::vector<std::unique_ptr<Stmt>>& statements, Environment env);
private:
	std::any execute(Stmt& stmt);
	std::any evaluate(Expr& expr);
	std::any visitReturnStmt(Return& stmt) override;
	std::any visitFunctionStmt(Function& stmt) override;
	std::any visitBreakStmt(Break& stmt) override;
	std::any visitContinueStmt(Continue& stmt) override;
	std::any visitWhileStmt(While& stmt) override;
	std::any visitIfStmt(If& stmt) override;
	std::any visitExpressionStmt(Expression& stmt) override;
	std::any visitPrintStmt(Print& stmt) override;
	std::any visitVarStmt(Var& stmt) override;
	std::any visitBlockStmt(Block& stmt) override;
	std::any visitCallExpr(Call& expr) override;
	std::any visitLogicalExpr(Logical& expr) override;
	std::any visitBinaryExpr(Binary& expr) override;
	std::any visitGroupingExpr(Grouping& expr) override;
	std::any visitLiteralExpr(Literal& expr) override;
	std::any visitUnaryExpr(Unary& expr) override;
	std::any visitVariableExpr(Variable& expr) override;
	std::any visitAssignExpr(Assign& expr) override;
	std::any visitLambdaExpr(Lambda& expr) override;
private:
	Environment environment;
};

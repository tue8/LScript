#pragma once
#include <vector>
#include <list>
#include "Stmt.h"

class Parser
{
public:
  Parser(const std::vector<Token>& tokens) : tokens(tokens) {}
  std::list<std::unique_ptr<Stmt>> parse();
private:
  std::unique_ptr<Stmt> declaration();
  std::unique_ptr<Stmt> function(std::string functionType);
  std::unique_ptr<Stmt> varDeclaration();
  std::unique_ptr<Stmt> statement();
  std::vector<std::unique_ptr<Stmt>> block();
  std::unique_ptr<Stmt> returnStatement();
  std::unique_ptr<Stmt> breakStatement();
  std::unique_ptr<Stmt> continueStatement();
  std::unique_ptr<Stmt> expressionStatement();
  std::unique_ptr<Stmt> forStatement();
  std::unique_ptr<Stmt> ifStatement();
  std::unique_ptr<Stmt> whileStatement();
  std::unique_ptr<Stmt> printStatement();
  std::unique_ptr<Expr> primary();
  std::unique_ptr<Expr> finishCall(std::unique_ptr<Expr> callee);
  std::unique_ptr<Expr> noParenCall(std::unique_ptr<Expr> callee);
  std::unique_ptr<Expr> call();
  std::unique_ptr<Expr> unary();
  std::unique_ptr<Expr> factor();
  std::unique_ptr<Expr> term();
  std::unique_ptr<Expr> comparision();
  std::unique_ptr<Expr> equality();
  std::unique_ptr<Expr> andExpr();
  std::unique_ptr<Expr> orExpr();
  std::unique_ptr<Expr> assignment();
  std::unique_ptr<Expr> lambda();
  std::unique_ptr<Expr> expression();

  const Token& consume(TokenType type, std::string msg);

  bool match(TokenType type);
  bool check(TokenType type);
  const Token& advance();
  bool isAtEnd();
  const Token& peek();
  const Token& previous();
  void synchronize();
private:
  int current = 0;
  std::vector<Token> tokens;
};

#pragma once
#include <vector>
#include <list>
#include "Stmt.h"

class Parser
{
public:
  Parser(const std::vector<Token>& tokens) : tokens(tokens) {}
  std::list<Stmt*> parse();
private:
  std::unique_ptr<Stmt> declaration();
  std::unique_ptr<Stmt> varDeclaration();
  std::unique_ptr<Stmt> statement();
  std::vector<std::unique_ptr<Stmt>> block();
  std::unique_ptr<Stmt> expressionStatement();
  std::unique_ptr<Stmt> ifStatement();
  std::unique_ptr<Stmt> whileStatement();
  std::unique_ptr<Stmt> printStatement();
  std::unique_ptr<Expr> primary();
  std::unique_ptr<Expr> unary();
  std::unique_ptr<Expr> factor();
  std::unique_ptr<Expr> term();
  std::unique_ptr<Expr> comparision();
  std::unique_ptr<Expr> equality();
  std::unique_ptr<Expr> andExpr();
  std::unique_ptr<Expr> orExpr();
  std::unique_ptr<Expr> assignment();
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

#include "Parser.h"
#include "Token.h"
#include <any>
#include <iostream>
#include <utility>

/*
* 
* Recursive descent
* 
* "starts from the top or outermost grammar rule
* and works its way down into the nested subexpressions before
* finally reaching the leaves of the syntax tree."
* 
* expression -> equality -> comparision -> term -> factor -> unary -> primary
* 
*/

const Token& Parser::peek()
{
  return tokens.at(current);
}

const Token& Parser::previous()
{
  return tokens.at(current - 1);
}

/*
* Advance until we reach the next statement
*/
void Parser::synchronize()
{
  advance();

  while (!isAtEnd())
  {
    if (previous().type == SEMICOLON)
      break;

    switch (peek().type)
    {
    case CLASS:
    case FUNC:
    case VAR:
    case FOR:
    case IF:
    case WHILE:
    case PRINT:
    case RETURN:
      break;
    }

    advance();
  }
}

bool Parser::isAtEnd()
{
  return peek().type == _EOF_;
}

const Token& Parser::advance()
{
  if (!isAtEnd())
    current++;
  return previous();
}

bool Parser::match(TokenType type)
{
  if (check(type))
  {
    advance();
    return true;
  }
  return false;
}

bool Parser::check(TokenType type)
{
  if (isAtEnd())
    return false;
  return peek().type == type;
}

static void error(const Token& token, std::string msg)
{
  if (token.type == EOF)
    std::cerr << "[" << token.line << "] at end: " << msg << std::endl;
  else
    std::cerr << "[" << token.line << "] at '" << token.lexeme << "': " << msg << std::endl;
}

std::list<Stmt*> Parser::parse()
{
  std::list<Stmt*> statements;
  while (!isAtEnd())
    statements.push_back(declaration().get());
  return statements;
}

std::unique_ptr<Stmt> Parser::declaration()
{
  try {
    if (match(VAR))
      return varDeclaration();
    return statement();
  }
  catch (std::pair<const Token&, std::string>& tokStr)
  {
    synchronize();
    error(tokStr.first, tokStr.second);
    return nullptr;
  }
}

std::unique_ptr<Stmt> Parser::varDeclaration()
{
  Token name = consume(IDENTIFIER, "Except variable name.");
  std::unique_ptr<Expr> initializer = nullptr;
  if (match(EQUAL))
    initializer = expression();
  consume(SEMICOLON, "Expect ';' after variable declaration");
  return std::make_unique<Var>(name, std::move(initializer));
}

std::unique_ptr<Stmt> Parser::statement()
{
  if (match(WHILE))
    return std::move(whileStatement());
  if (match(IF))
    return std::move(ifStatement());
  if (match(PRINT))
    return std::move(printStatement());
  if (match(LEFT_BRACE))
    return std::make_unique<Block>(block());
  return std::move(expressionStatement());
}

std::vector<std::unique_ptr<Stmt>> Parser::block()
{
  std::vector<std::unique_ptr<Stmt>> statements;
  while (!check(RIGHT_BRACE) && !isAtEnd())
    statements.push_back(declaration());
  consume(RIGHT_BRACE, "Expected '}' after block.");
  return statements;
}

std::unique_ptr<Stmt> Parser::expressionStatement()
{
  auto value = expression();
  consume(SEMICOLON, "Expected ; after statement");
  return std::make_unique<Expression>(std::move(value));
}

std::unique_ptr<Stmt> Parser::whileStatement()
{
  consume(LEFT_PAREN, "Expected '(' after 'while'");
  auto condition = expression();
  consume(RIGHT_PAREN, "Expected ')' after 'while'");
  auto body = statement();
  return std::make_unique<While>(std::move(condition), std::move(body));
}

std::unique_ptr<Stmt> Parser::ifStatement()
{
  consume(LEFT_PAREN, "Expected '(' after 'if'");
  auto condition = expression();
  consume(RIGHT_PAREN, "Expected ')' after condition");
  auto thenBranch = statement();
  auto elseBranch = (match(ELSE)) ? statement() : nullptr;
  return std::make_unique<If>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<Stmt> Parser::printStatement()
{
  auto value = expression();
  consume(SEMICOLON, "Expected ; after statement");
  return std::make_unique<Print>(std::move(value));
}

std::unique_ptr<Expr> Parser::primary()
{
  if (match(FALSE))
    return std::make_unique<Literal>(false);

  if (match(TRUE))
    return std::make_unique<Literal>(true);

  if (match(NIL))
    return std::make_unique<Literal>(std::any());

  if (match(NUMBER) || match(STRING))
    return std::make_unique<Literal>(previous().lit);

  if (match(IDENTIFIER))
    return std::make_unique<Variable>(previous());

  if (match(LEFT_PAREN))
  {
    auto grExpr = expression();
    consume(RIGHT_PAREN, "Expect ')' after expression.");
    return std::make_unique<Grouping>(std::move(grExpr));
  }

  throw (std::make_pair(std::ref(peek()), std::string("I FUCKING expected expression.")));
}

std::unique_ptr<Expr> Parser::unary()
{
  if (match(BANG) || match(MINUS))
  {
    Token op = previous();
    auto right = unary();
    std::unique_ptr<Expr> expr(new Unary(op, std::move(right)));
    return std::move(expr);
  }

  return std::move(primary());
}

std::unique_ptr<Expr> Parser::factor()
{ 
  std::unique_ptr<Expr> expr = unary();

  while (match(SLASH) || match(STAR))
  {
    Token op = previous();
    auto right = unary();
    /* move current unique_ptr expr and create a new one with make_unique */
    expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
  }

  return std::move(expr);
}

std::unique_ptr<Expr> Parser::term()
{
  std::unique_ptr<Expr> expr = factor();

  while (match(MINUS) || match(PLUS))
  {
    Token op = previous();
    auto right = factor();
    expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
  }

  return std::move(expr);
}

std::unique_ptr<Expr> Parser::comparision()
{
  std::unique_ptr<Expr> expr = term();

  while (match(GREATER) || match(GREATER_EQUAL) ||
         match(LESS)    || match(LESS_EQUAL))
  {
    Token op = previous();
    auto right = term();
    expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
  }
  return std::move(expr);
}

std::unique_ptr<Expr> Parser::equality()
{
  std::unique_ptr<Expr> expr = comparision();

  while (match(BANG_EQUAL) || match(EQUAL_EQUAL))
  {
    Token op = previous();
    auto right = comparision();
    expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
  }

  return std::move(expr);
}

std::unique_ptr<Expr> Parser::andExpr()
{
  std::unique_ptr<Expr> expr = equality();

  while (match(AND))
  {
    Token op = previous();
    auto right = andExpr();
    expr = std::make_unique<Logical>(std::move(expr), op, std::move(right));
  }

  return std::move(expr);
}

std::unique_ptr<Expr> Parser::orExpr()
{
  std::unique_ptr<Expr> expr = andExpr();

  while (match(OR))
  {
    Token op = previous();
    auto right = andExpr();
    expr = std::make_unique<Logical>(std::move(expr), op, std::move(right));
  }

  return std::move(expr);
}

std::unique_ptr<Expr> Parser::assignment()
{
  std::unique_ptr<Expr> expr = orExpr();

  if (match(EQUAL))
  {
    const Token equals = previous();
    auto value = assignment();

    if (dynamic_cast<Variable*>(expr.get()))
    {
      Token name = static_cast<Variable*>(expr.get())->getName();
      return std::make_unique<Assign>(name, std::move(value));
    }
    
    throw (std::make_pair(std::ref(equals), std::string("Invalid assignment target.")));
  }
  return std::move(expr);
}

std::unique_ptr<Expr> Parser::expression()
{
 return std::move(assignment());
}

const Token& Parser::consume(TokenType type, std::string msg)
{
  if (check(type))
    return advance();
  throw (std::make_pair(std::ref(peek()), msg));
}

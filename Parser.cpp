#include "Parser.h"
#include "Stmt.h"
#include "Token.h"
#include <any>
#include <iostream>
#include <memory>
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
  if (token.type == _EOF_)
    std::cerr << "PARSER ERROR: [" << token.line << "] at end: " << msg << std::endl;
  else
    std::cerr << "PARSER ERROR: [" << token.line << "] at '" << token.lexeme << "': " << msg << std::endl;
}

std::list<std::unique_ptr<Stmt>> Parser::parse()
{
  std::list<std::unique_ptr<Stmt>> statements;
  while (!isAtEnd())
  {
    statements.push_back(std::move(declaration()));
  }
  return statements;
}

std::unique_ptr<Stmt> Parser::declaration()
{
  try {
    if (match(FUNC) && (peek().type == IDENTIFIER)) return function("function");
    if (match(VAR)) return varDeclaration();
    return statement();
  }
  catch (std::pair<const Token&, std::string>& tokStr)
  {
    synchronize();
    error(tokStr.first, tokStr.second);
    return nullptr;
  }
}

std::unique_ptr<Stmt> Parser::function(std::string functionType)
{
  Token name = consume(IDENTIFIER, "Expected " + functionType + " name type shii");
  consume(LEFT_PAREN, "Expected '(' after " + functionType + " name");
  std::vector<Token> parameters;
  if (!check(RIGHT_PAREN))
  {
    do
    {
      if (parameters.size() > 255) 
        error(peek(), "Hey! Fuck you! You cannot have more than 255 parameters.");
      parameters.push_back(consume(IDENTIFIER, "Expected parameter name after ','"));
    } while (match(COMMA));
  }
  consume(RIGHT_PAREN, "Expected ')' after parameters.");
  consume(LEFT_BRACE, "Expected '{' after ')'");
  auto body = block();
  return std::make_unique<Function>(name, parameters, std::move(body)); 
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
  if (match(RETURN))      return std::move(returnStatement());
  if (match(BREAK))      return std::move(breakStatement());
  if (match(CONTINUE))   return std::move(continueStatement());
  if (match(FOR))        return std::move(forStatement());
  if (match(WHILE))      return std::move(whileStatement());
  if (match(IF))         return std::move(ifStatement());
  if (match(PRINT))      return std::move(printStatement());
  if (match(LEFT_BRACE)) return std::make_unique<Block>(block());
  return std::move(expressionStatement());
}

std::unique_ptr<Stmt> Parser::returnStatement()
{
  Token token = previous();
  if (match(SEMICOLON))
    return std::make_unique<Return>(token, nullptr);
  auto value = expression();
  consume(SEMICOLON, "Expected ';' after return statement");
  return std::make_unique<Return>(token, std::move(value));
}

std::unique_ptr<Stmt> Parser::breakStatement()
{
  consume(SEMICOLON, "You did not place ';' after break... *sigh* Give me a break... Let's break up.");
  return std::make_unique<Break>();  
}

std::unique_ptr<Stmt> Parser::continueStatement()
{
  consume(SEMICOLON, "Expected ';' after continue.");
  return std::make_unique<Continue>();
}

std::vector<std::unique_ptr<Stmt>> Parser::block()
{
  std::vector<std::unique_ptr<Stmt>> statements;
  while (!check(RIGHT_BRACE) && !isAtEnd())
    statements.push_back(declaration());
  consume(RIGHT_BRACE, "Expected '}' after block.");
  return statements;
}

std::unique_ptr<Stmt> Parser::forStatement()
{
  consume(LEFT_PAREN, "Expected '(' after 'for'");
  std::unique_ptr<Stmt> initializer;

  if (match(SEMICOLON)) initializer = nullptr;
  else if (match(VAR))  initializer = varDeclaration();
  else                  initializer = expressionStatement();

  auto condition = (!check(SEMICOLON)) ? expression() : nullptr;
  consume(SEMICOLON, "Expected ';' after for loop condition");
  auto increment = (!check(RIGHT_PAREN)) ? expression() : nullptr;
  consume(RIGHT_PAREN, "Expected ')' after to after for loop");
  std::unique_ptr<Stmt> body = statement();
  std::vector<std::unique_ptr<Stmt>> bodyVec;
  if (increment != nullptr)
  {
    bodyVec.push_back(std::move(body));
    bodyVec.push_back(std::make_unique<Expression>(std::move(increment)));
    body = std::make_unique<Block>(std::move(bodyVec));
  }
  if (condition == nullptr)
    condition = std::make_unique<Literal>(true);
  body = std::make_unique<While>(std::move(condition), std::move(body));
  if (initializer != nullptr)
  {
    bodyVec.clear();
    bodyVec.push_back(std::move(initializer));
    bodyVec.push_back(std::move(body));
    body = std::make_unique<Block>(std::move(bodyVec));
  }
  return body;
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
  if (match(FALSE))      return std::make_unique<Literal>(false);
  if (match(TRUE))       return std::make_unique<Literal>(true);
  if (match(NIL))        return std::make_unique<Literal>(std::any());
  if (match(IDENTIFIER)) return std::make_unique<Variable>(previous());
  if (match(NUMBER) || match(STRING))
    return std::make_unique<Literal>(previous().lit);
  if (match(LEFT_PAREN))
  {
    auto grExpr = expression();
    consume(RIGHT_PAREN, "Expect ')' after expression.");
    return std::make_unique<Grouping>(std::move(grExpr));
  }

  throw (std::make_pair(std::ref(peek()), std::string("I FUCKING expected expression.")));
}

std::unique_ptr<Expr> Parser::finishCall(std::unique_ptr<Expr> callee)
{
  std::vector<std::unique_ptr<Expr>> args;

  if (!check(RIGHT_PAREN))
  {
    do
    {
      if (args.size() >= 255)
        error(peek(), "Can't have more than 255 arguments");
      args.push_back(std::move(expression()));
    } while(match(COMMA));
  }

  Token paren = consume(RIGHT_PAREN, "Expected '(' after arguments.");
  return std::make_unique<Call>(std::move(callee), paren, std::move(args));
}

std::unique_ptr<Expr> Parser::call()
{
  std::unique_ptr<Expr> expr = primary();

  while (true)
  {
    if (match(LEFT_PAREN))
    {
      expr = finishCall(std::move(expr));
    }
    else
    { 
      break; 
    }
  }

  return std::move(expr);
}

std::unique_ptr<Expr> Parser::unary()
{
  if (match(BANG) || match(MINUS))
  {
    Token op = previous();
    auto right = unary();
    return std::make_unique<Unary>(op, std::move(right));
  }

  return std::move(call());
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

std::unique_ptr<Expr> Parser::lambda()
{
  consume(LEFT_PAREN, "Expected '(' for lambda parameters.");
  std::vector<Token> parameters;
  if (!check(RIGHT_PAREN))
  {
    do
    {
      if (parameters.size() > 255)
        error(peek(), "Hey! Fuck you! You cannot have more than 255 parameters.");
      parameters.push_back(consume(IDENTIFIER, "Expected parameter name after ','"));
    } while (match(COMMA));
  }
  consume(RIGHT_PAREN, "Expected ')' after parameters.");
  consume(LEFT_BRACE, "Expected '{' after arrow");
  auto body = block();
  return std::make_unique<Lambda>(parameters, std::move(body));
}


std::unique_ptr<Expr> Parser::expression()
{
  if (match(FUNC))
    return std::move(lambda());
 return std::move(assignment());
}

const Token& Parser::consume(TokenType type, std::string msg)
{
  if (check(type))
    return advance();
  throw (std::make_pair(std::ref(peek()), msg));
}
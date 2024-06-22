#pragma once

#include "Token.h"
#include <vector>
#include <unordered_map>

class Lexer
{
private:
  std::string src;
  std::vector<Token> tokens;
  std::unordered_map<std::string, TokenType> keywords;
  int start = 0;
  int curr = 0;
  int line = 0;
public:
  Lexer(std::string src);
  std::vector<Token>& lexAll();
private:
  void lex();
  bool isAtEnd();
  char advance();
  bool match(char c);
  char peek();
  char peekNext();
  void addToken(TokenType type, std::any lit);
  void addToken(TokenType type);
  void ctypeComment();
  bool isDigit(char c);
  bool isAlpha(char c);
  bool isAlphaNum(char c);
  void string();
  void number();
  void identifier();
};
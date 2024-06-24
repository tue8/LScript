#pragma once
#include <string>
#include <ostream>
#include <any>

enum TokenType {
  LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
  COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

  BANG, BANG_EQUAL,
  EQUAL, EQUAL_EQUAL,
  GREATER, GREATER_EQUAL,
  LESS, LESS_EQUAL,

  IDENTIFIER, STRING, NUMBER,

  AND, CLASS, ELSE, FALSE, FUNC, FOR, IF, NIL, OR,
  PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,
  BREAK, CONTINUE,

  _EOF_
};

class Token
{
public:
  TokenType type;
  std::string lexeme;
  std::any lit;
  int line;
public:
  Token(TokenType type, std::string lexeme, std::any lit, int line)
    : type(type), lexeme(lexeme), lit(lit), line(line)
  {}

  friend std::ostream& operator << (std::ostream& os, const Token& token) {
    os << "Line: " << token.line << " [" << token.type << "]" << " " << token.lexeme;
    if (token.type == NUMBER)
      os << " num: " << std::any_cast<double>(token.lit);
    else if (token.type == STRING)
      os << " str: " << std::any_cast<std::string>(token.lit);
    return os;
  }
};

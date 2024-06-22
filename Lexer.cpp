#include "Lexer.h"
#include <iostream>

Lexer::Lexer(std::string src)
  : src(src)
{
  keywords["and"] = AND;
  keywords["class"] = CLASS;
  keywords["else"] = ELSE;
  keywords["false"] = FALSE;
  keywords["for"] = FOR;
  keywords["function"] = FUNC;
  keywords["if"] = IF;
  keywords["nil"] = NIL;
  keywords["or"] = OR;
  keywords["print"] = PRINT;
  keywords["return"] = RETURN;
  keywords["super"] = SUPER;
  keywords["this"] = THIS;
  keywords["true"] = TRUE;
  keywords["var"] = VAR;
  keywords["while"] = WHILE;
}

bool Lexer::isAtEnd()
{
  return curr >= src.length();
}

char Lexer::advance()
{
  return src.at(curr++);
}

bool Lexer::match(char c)
{
  if (isAtEnd())
    return false;
  if (src.at(curr) != c)
    return false;
  curr++;
  return true;
}

char Lexer::peek()
{
  if (isAtEnd())
    return '\0';
  return src.at(curr);
}

char Lexer::peekNext()
{
  if (curr + 1 >= src.length())
    return '\0';
  return src.at(curr + 1);
}

void Lexer::addToken(TokenType type, std::any lit)
{
  tokens.push_back(Token(type, src.substr(start, curr - start), lit, line));
}

void Lexer::addToken(TokenType type)
{
  addToken(type, nullptr);
}

bool Lexer::isDigit(char c)
{
  return c >= '0' && c <= '9';
}

bool Lexer::isAlpha(char c)
{
  return (c >= 'a' && c <= 'z') ||
         (c >= 'A' && c <= 'Z') ||
          c == '_';
}

bool Lexer::isAlphaNum(char c)
{
  return isAlpha(c) || isDigit(c);
}

void Lexer::number()
{
  while (isDigit(peek()))
    advance();

  if (peek() == '.' && isDigit(peekNext()))
  {
    /* consume '.' */
    advance();
    while (isDigit(peek()))
      advance();
  }

  addToken(NUMBER, std::stod(src.substr(start, curr - start)));
}

void Lexer::string()
{
  while (peek() != '"' && !isAtEnd())
  {
    if (peek() == '\n')
      line++;
    advance();
  }

  if (isAtEnd())
  {
    std::cerr << "Unterminated string" << std::endl;
    return;
  }

  /* current is at the quote so consume quote */
  advance();
  /* trim surrounding quotes */
  addToken(STRING, src.substr(start + 1, (curr - 1) - (start + 1)));
}

void Lexer::ctypeComment()
{
  while (!(peek() == '*' && peekNext() == '/') && !isAtEnd())
  {
    if (peek() == '\n')
      line++;
    advance();
  }

  if (isAtEnd())
  {
    std::cerr << "You didn't close your comment.. You fucked up BIG ONE" << std::endl;
    return;
  }

  /* close comments */
  advance();
  advance();
}

void Lexer::identifier()
{
  while (isAlphaNum(peek()))
    advance();
  std::string str = src.substr(start, curr - start);
  if (keywords.find(str) == keywords.end())
    addToken(IDENTIFIER);
  else
    addToken(keywords.find(str)->second);
}

void Lexer::lex()
{
  char c = advance();
  switch (c) {
    case '(': addToken(LEFT_PAREN); break;
    case ')': addToken(RIGHT_PAREN); break;
    case '{': addToken(LEFT_BRACE); break;
    case '}': addToken(RIGHT_BRACE); break;
    case ',': addToken(COMMA); break;
    case '.': addToken(DOT); break;
    case '-': addToken(MINUS); break;
    case '+': addToken(PLUS); break;
    case ';': addToken(SEMICOLON); break;
    case '*': addToken(STAR); break;
    case '!':
      addToken(match('=') ? BANG_EQUAL : BANG);
      break;
    case '=':
      addToken(match('=') ? EQUAL_EQUAL : EQUAL);
      break;
    case '<':
      addToken(match('=') ? LESS_EQUAL : LESS);
      break;
    case '>':
      addToken(match('=') ? GREATER_EQUAL : GREATER);
      break;
    case '/':
      if (match('/'))
      {
        while (peek() != '\n' && !isAtEnd())
          advance();
      }
      else if (match('*'))
      {
        ctypeComment();
      }
      else
      {
        addToken(SLASH);
      }
      break;
    case ' ':
    case '\r':
    case '\t':
      // Ignore whitespace.
      break;
    case '\n':
      line++;
      break;
    case '"':
      string();
      break;
    default:
      if (isDigit(c))
        number();
      else if (isAlpha(c))
        identifier();
      else
        std::cerr << "ERRRRRRR!!! LEXER ERROR!!!" << std::endl;
      break;
  }
}

std::vector<Token>& Lexer::lexAll()
{
  while (!isAtEnd())
  {
    start = curr;
    lex();
  }
  tokens.push_back(Token(_EOF_, "", 0, line));
  return tokens;
}
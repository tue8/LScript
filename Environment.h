#pragma once

#include <map>
#include "Token.h"

class Environment
{
public:
  Environment() : enclosing(nullptr) {}
  Environment(Environment *enclosing) : enclosing(enclosing) {}
  std::any get(const Token& name);
  void define(std::string name, std::any value);
  void assign(const Token& name, std::any value);
private:
  Environment *enclosing;
  std::map<std::string, std::any> values;
};
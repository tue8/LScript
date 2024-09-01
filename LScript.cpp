#include <iostream>
#include <fstream>
#include <sstream>

#ifdef __EMSCRIPTEN__
  #include <emscripten.h>
#endif

#include "Parser.h"
#include "Lexer.h"
#include "Interpreter.h"

Interpreter interpreter;

static void run(std::string str)
{
  Lexer lexer = Lexer(str);
  std::vector<Token> tokens = lexer.lexAll();
#ifdef LDEBUG
  for (const auto& token : tokens)
  {
       std::cout << token << std::endl;
  }
#endif
  Parser parser = Parser(tokens);
  std::list<std::unique_ptr<Stmt>> stmt_list = parser.parse();
  if (!stmt_list.empty())
    interpreter.interpret(std::move(stmt_list));
}

#ifdef __EMSCRIPTEN__
extern "C"
{
  EMSCRIPTEN_KEEPALIVE
  void crun(const char *c_str)
  {
    run(c_str);
  }
}
#endif

static int runFile(char *script_name)
{
	std::ifstream fileStream(script_name);
	std::ostringstream oss;
	oss << fileStream.rdbuf();
	std::string fileStr = oss.str();
	if (fileStr.empty())
		return 1;
	run(fileStr);
	return 0;
}

static int runPrompt()
{
	for (;;)
	{
		std::string line;
		std::cout << "> ";
		std::getline(std::cin, line);
		if (!line.empty())
			run(line);
	}
	return 0;
}

int main(int argc, char **argv)
{
	if (argc > 2)
	{
		std::cerr << "Usage: LScript [script]" << std::endl;
		return 1;
	}

#ifdef LDEBUG
	std::string script;
	std::cout << "Run script: ";
	std::cin >> script;
	return runFile((char*)std::string("../scripts/" + script).c_str());
#else
	return (argc == 2) ? runFile(argv[1]) : runPrompt();
#endif
}

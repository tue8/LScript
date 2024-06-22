# CMakeList.txt : CMake project for LScript, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("LScript")

# Add source to this project's executable.
add_executable (LScript "LScript.cpp" "LScript.h" "Lexer.cpp" "Lexer.h"  "Token.h" "Parser.h" "Parser.cpp" "Interpreter.h" "Interpreter.cpp" "Stmt.h" "Environment.h" "Environment.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET LScript PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.

# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -pedantic -O2")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DLDEBUG")

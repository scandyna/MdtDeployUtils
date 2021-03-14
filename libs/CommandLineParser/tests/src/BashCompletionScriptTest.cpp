/****************************************************************************
 **
 ** Copyright (C) 2011-2021 Philippe Steinmann.
 **
 ** This file is part of MdtApplication library.
 **
 ** MdtApplication is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** MdtApplication is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with MdtApplication.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "catch2/catch.hpp"
#include "Catch2QString.h"
#include "Mdt/CommandLineParser/BashCompletion/ScriptCompletionsFunction.h"
#include "Mdt/CommandLineParser/BashCompletion/Script.h"
#include <QLatin1String>
#include <QByteArray>
#include <iostream>

using namespace Mdt::CommandLineParser::BashCompletion;

void printScriptToConsole(const Script & script)
{
  std::cout << "***** Script *****\n";
  std::cout << script.toString().toLocal8Bit().toStdString() << '\n';
  std::cout << "***** END script *****" << std::endl;
}

void printScriptCompletionsFunctionToConsole(const ScriptCompletionsFunction & f)
{
  std::cout << "***** Script completions function *****\n";
  std::cout << f.toString().toLocal8Bit().toStdString() << '\n';
  std::cout << "***** END script completions function *****" << std::endl;
}

TEST_CASE("ScriptCompletionsFunctionVariable")
{
  ScriptCompletionsFunctionVariable executable( QLatin1String("executable"), QLatin1String("$1") );
  REQUIRE( executable.name() == QLatin1String("executable") );
  REQUIRE( executable.toString() == QLatin1String("  local executable=$1") );
}

TEST_CASE("ScriptCompletionsFunction_Construct_name")
{
  ScriptCompletionsFunction f( QLatin1String("app"), ScriptCaseStatementExpression( QLatin1String("$arg") ) );
  REQUIRE( f.name() == QLatin1String("_app_completions") );
}

TEST_CASE("ScriptCompletionsFunction_variables")
{
  ScriptCompletionsFunction f( QLatin1String("app"), ScriptCaseStatementExpression( QLatin1String("$arg") ) );

  SECTION("No variable")
  {
    REQUIRE( f.variableCount() == 0 );
  }

  SECTION("executable=$1")
  {
    f.addVariable( QLatin1String("executable"), QLatin1String("$1") );
    REQUIRE( f.variableCount() == 1 );
    REQUIRE( f.variableAt(0).name() == QLatin1String("executable") );
  }
}

TEST_CASE("ScriptCompletionsFunction_getVariablesBlockString")
{
  ScriptCompletionsFunction f( QLatin1String("app"), ScriptCaseStatementExpression( QLatin1String("$arg") ) );

  SECTION("No variable")
  {
    REQUIRE( f.getVariablesBlockString().isEmpty() );
  }

  SECTION("1 variable")
  {
    f.addVariable( QLatin1String("A"), QLatin1String("a") );
    REQUIRE( f.getVariablesBlockString() == QLatin1String("  local A=a\n\n") );
  }

  SECTION("2 variables")
  {
    f.addVariable( QLatin1String("A"), QLatin1String("a") );
    f.addVariable( QLatin1String("B"), QLatin1String("b") );
    REQUIRE( f.getVariablesBlockString() == QLatin1String("  local A=a\n  local B=b\n\n") );
  }
}

TEST_CASE("ScriptCaseClause_attributes")
{
  SECTION("Default clause")
  {
    ScriptCaseClause clause;
    REQUIRE( clause.pattern() == QLatin1String("*") );
    REQUIRE( clause.command().isEmpty() );
    REQUIRE( !clause.hasCommand() );
  }

  SECTION("Default clause with command")
  {
    ScriptCaseClause clause( QLatin1String("command x") );
    REQUIRE( clause.pattern() == QLatin1String("*") );
    REQUIRE( clause.command() == QLatin1String("command x") );
    REQUIRE( clause.hasCommand() );
  }

  SECTION("Clause with pattern and command")
  {
    ScriptCaseClause clause( QLatin1String("options"), QLatin1String("list-options") );
    REQUIRE( clause.pattern() == QLatin1String("options") );
    REQUIRE( clause.command() == QLatin1String("list-options") );
    REQUIRE( clause.hasCommand() );
  }
}

TEST_CASE("ScriptCaseClause_toString")
{
  QString expectedResult;

  SECTION("Default clause")
  {
    ScriptCaseClause clause;
    expectedResult = QLatin1String(
      "    *)\n"
      "      ;;"
    );
    REQUIRE( clause.toString() == expectedResult );
  }

  SECTION("Default clause with command")
  {
    ScriptCaseClause clause( QLatin1String("command x") );
    expectedResult = QLatin1String(
      "    *)\n"
      "      command x\n"
      "      ;;"
    );
    REQUIRE( clause.toString() == expectedResult );
  }

  SECTION("Clause with pattern and command")
  {
    ScriptCaseClause clause( QLatin1String("options"), QLatin1String("list-options") );
    expectedResult = QLatin1String(
      "    options)\n"
      "      list-options\n"
      "      ;;"
    );
    REQUIRE( clause.toString() == expectedResult );
  }
}

TEST_CASE("ScriptCaseStatementExpression")
{
  ScriptCaseStatementExpression expression( QLatin1String("$arg") );
  REQUIRE( expression.toString() == QLatin1String("$arg") );
}

TEST_CASE("ScriptCaseStatement_attributes")
{
  ScriptCaseStatement statement( ScriptCaseStatementExpression( QLatin1String("$arg") ) );

  SECTION("No clause")
  {
    REQUIRE( statement.clauseCount() == 0 );
    REQUIRE( statement.isEmpty() );
  }

  SECTION("1 clause")
  {
    statement.addClause( QLatin1String("options"), QLatin1String("compgen -W") );
    REQUIRE( statement.clauseCount() == 1 );
    REQUIRE( !statement.isEmpty() );
    REQUIRE( statement.clauseAt(0).pattern() == QLatin1String("options") );
    REQUIRE( statement.clauseAt(0).command() == QLatin1String("compgen -W") );
  }

  SECTION("default clause")
  {
    statement.addDefaultClause();
    REQUIRE( statement.clauseCount() == 1 );
    REQUIRE( !statement.isEmpty() );
  }

  SECTION("default clause with a command")
  {
    statement.addDefaultClause( QLatin1String("echo command") );
    REQUIRE( statement.clauseCount() == 1 );
    REQUIRE( !statement.isEmpty() );
    REQUIRE( statement.clauseAt(0).command() == QLatin1String("echo command") );
  }
}

TEST_CASE("ScriptCaseStatement_toString")
{
  ScriptCaseStatement statement( ScriptCaseStatementExpression( QLatin1String("$currentArgument") ) );
  QString expectedResult;

  SECTION("1 clause")
  {
    statement.addClause( QLatin1String("options"), QLatin1String("compgen -W") );
    expectedResult = QLatin1String(
      "  case $currentArgument in\n\n"
      "    options)\n"
      "      compgen -W\n"
      "      ;;\n\n"
      "  esac"
    );
    REQUIRE( statement.toString() == expectedResult );
  }

  SECTION("2 clauses")
  {
    statement.addClause( QLatin1String("options"), QLatin1String("compgen -W") );
    statement.addDefaultClause();
    expectedResult = QLatin1String(
      "  case $currentArgument in\n\n"
      "    options)\n"
      "      compgen -W\n"
      "      ;;\n\n"
      "    *)\n"
      "      ;;\n\n"
      "  esac"
    );
    REQUIRE( statement.toString() == expectedResult );
  }
}

TEST_CASE("Script")
{
  Script script( QLatin1String("app") );

  SECTION("Check completion function name")
  {
    REQUIRE( script.completionsFunction().name() == QLatin1String("_app_completions") );
  }

  SECTION("Check complete command")
  {
    REQUIRE( script.getCompleteCommandString() == QLatin1String("complete -F _app_completions app") );
  }
}

TEST_CASE("ScriptCompletionsFunction_sandbox")
{
  ScriptCompletionsFunction f( QLatin1String("app"), ScriptCaseStatementExpression( QLatin1String("$arg") ) );

  SECTION("app name only")
  {
    printScriptCompletionsFunctionToConsole(f);
  }

  SECTION("variables")
  {
    f.addVariable( QLatin1String("cur"), QLatin1String("\"${COMP_WORDS[COMP_CWORD]}\"") );
    f.addVariable( QLatin1String("executable"), QLatin1String("\"$1\"") );
    printScriptCompletionsFunctionToConsole(f);
  }
}

TEST_CASE("Script sandbox")
{
  Script script( QLatin1String("app") );
  script.addClause( QLatin1String("options"), QLatin1String("COMPREPLY=($(compgen -W \"-h --help\" -- \"$cur\"))") );
  script.addDefaultClause();

  printScriptToConsole(script);
}

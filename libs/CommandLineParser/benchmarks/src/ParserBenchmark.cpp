/****************************************************************************
 **
 ** Copyright (C) 2020-2021 Philippe Steinmann.
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
#include "ParserBenchmark.h"
#include "Mdt/CommandLineParser/ParserDefinition.h"
#include "Mdt/CommandLineParser/Parser.h"
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QCoreApplication>
#include <QLatin1String>
#include <QStringList>
#include <vector>
#include <string>

using namespace Mdt::CommandLineParser;

QStringList qStringListFromUtf8Strings(const std::vector<std::string> & args)
{
  QStringList arguments;

  for(const auto & arg : args){
    arguments.append( QString::fromStdString(arg) );
  }

  return arguments;
}

void ParserBenchmark::qCommandLineParser_parse()
{
  QCommandLineParser parser;
  parser.addHelpOption();

  QCommandLineOption forceOption( QStringList{QLatin1String("f"),QLatin1String("force")}, QLatin1String("Force option") );
  parser.addOption(forceOption);

  QCommandLineOption overwriteBehaviorOption( QStringList{QLatin1String("overwrite-behavior")}, QLatin1String("Overwrite behavior option") );
  overwriteBehaviorOption.setValueName( QLatin1String("behavior") );
  parser.addOption(overwriteBehaviorOption);

  const QStringList arguments = qStringListFromUtf8Strings({"app","--force","--overwrite-behavior","keep","file.txt","/tmp"});

  QBENCHMARK{
    QVERIFY( parser.parse(arguments) );
  }

  QVERIFY( parser.isSet(forceOption) );
  QCOMPARE( parser.value(overwriteBehaviorOption), QLatin1String("keep") );
}

void ParserBenchmark::mdtCommandLineParser_parse()
{
  ParserDefinition parserDefinition;
  parserDefinition.addHelpOption();

  ParserDefinitionOption forceOption( 'f', QLatin1String("force"), QLatin1String("Force option") );
  parserDefinition.addOption(forceOption);

  ParserDefinitionOption overwriteBehaviorOption( QLatin1String("overwrite-behavior"), QLatin1String("Overwrite behavior") );
  overwriteBehaviorOption.setValueName( QLatin1String("behavior") );
  parserDefinition.addOption(overwriteBehaviorOption);

  const QStringList arguments = qStringListFromUtf8Strings({"app","--force","--overwrite-behavior","keep","file.txt","/tmp"});

  Parser parser(parserDefinition);

  QBENCHMARK{
    QVERIFY( parser.parse(arguments) );
  }

  const auto result = parser.toParserResult();
  QVERIFY( result.isSet(forceOption) );
  QCOMPARE( result.getValues(overwriteBehaviorOption), QStringList{QLatin1String("keep")} );
}

void ParserBenchmark::mdtCommandLineParserWithSubCommnds_parse()
{
  ParserDefinition parserDefinition;
  parserDefinition.addHelpOption();

  ParserDefinitionOption forceOption( 'f', QLatin1String("force"), QLatin1String("Force option") );
  parserDefinition.addOption(forceOption);

  ParserDefinitionOption overwriteBehaviorOption( QLatin1String("overwrite-behavior"), QLatin1String("Overwrite behavior") );
  overwriteBehaviorOption.setValueName( QLatin1String("behavior") );

  ParserDefinitionCommand copyCommand( QLatin1String("copy") );
  copyCommand.addOption(overwriteBehaviorOption);

  parserDefinition.addSubCommand(copyCommand);

  const QStringList arguments = qStringListFromUtf8Strings({"app","copy","--overwrite-behavior","keep","file.txt","/tmp"});

  Parser parser(parserDefinition);

  QBENCHMARK{
    QVERIFY( parser.parse(arguments) );
  }

  const auto result = parser.toParserResult();
  QVERIFY( result.hasSubCommand() );
  QCOMPARE( result.subCommand().getValues(overwriteBehaviorOption), QStringList{QLatin1String("keep")} );
}

void ParserBenchmark::qCommandLineParser()
{
  const QStringList arguments = qStringListFromUtf8Strings({"app","--force","--overwrite-behavior","keep","file.txt","/tmp"});

  QBENCHMARK{
    QCommandLineParser parser;
    parser.addHelpOption();

    QCommandLineOption forceOption( QStringList{QLatin1String("f"),QLatin1String("force")}, QLatin1String("Force option") );
    parser.addOption(forceOption);

    QCommandLineOption overwriteBehaviorOption( QStringList{QLatin1String("overwrite-behavior")}, QLatin1String("Overwrite behavior option") );
    overwriteBehaviorOption.setValueName( QLatin1String("behavior") );
    parser.addOption(overwriteBehaviorOption);

    QVERIFY( parser.parse(arguments) );

    QVERIFY( parser.isSet(forceOption) );
    QCOMPARE( parser.value(overwriteBehaviorOption), QLatin1String("keep") );
  }
}

void ParserBenchmark::mdtCommandLineParser()
{
  const QStringList arguments = qStringListFromUtf8Strings({"app","--force","--overwrite-behavior","keep","file.txt","/tmp"});

  QBENCHMARK{
    ParserDefinition parserDefinition;
    parserDefinition.addHelpOption();

    ParserDefinitionOption forceOption( 'f', QLatin1String("force"), QLatin1String("Force option") );
    parserDefinition.addOption(forceOption);

    ParserDefinitionOption overwriteBehaviorOption( QLatin1String("overwrite-behavior"), QLatin1String("Overwrite behavior") );
    overwriteBehaviorOption.setValueName( QLatin1String("behavior") );
    parserDefinition.addOption(overwriteBehaviorOption);

    Parser parser(parserDefinition);

    QVERIFY( parser.parse(arguments) );

    const auto result = parser.toParserResult();
    QVERIFY( result.isSet(forceOption) );
    QCOMPARE( result.getValues(overwriteBehaviorOption), QStringList{QLatin1String("keep")} );
  }
}

void ParserBenchmark::mdtCommandLineParser_buildResultAndCheck()
{
  const QStringList arguments = qStringListFromUtf8Strings({"app","--force","--overwrite-behavior","keep","file.txt","/tmp"});

  ParserDefinition parserDefinition;
  parserDefinition.addHelpOption();

  ParserDefinitionOption forceOption( 'f', QLatin1String("force"), QLatin1String("Force option") );
  parserDefinition.addOption(forceOption);

  ParserDefinitionOption overwriteBehaviorOption( QLatin1String("overwrite-behavior"), QLatin1String("Overwrite behavior") );
  overwriteBehaviorOption.setValueName( QLatin1String("behavior") );
  parserDefinition.addOption(overwriteBehaviorOption);

  Parser parser(parserDefinition);
  QVERIFY( parser.parse(arguments) );

  QBENCHMARK{
    const auto result = parser.toParserResult();
    QVERIFY( result.isSet(forceOption) );
    QCOMPARE( result.getValues(overwriteBehaviorOption), QStringList{QLatin1String("keep")} );
  }
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);
  ParserBenchmark test;

  return QTest::qExec(&test, argc, argv);
}

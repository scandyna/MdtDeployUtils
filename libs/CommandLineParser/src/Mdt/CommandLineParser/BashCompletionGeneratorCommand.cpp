/****************************************************************************
 **
 ** Copyright (C) 2011-2020 Philippe Steinmann.
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
#include "BashCompletionGeneratorCommand.h"
// #include <QStringList>
// #include <QByteArray>
// 
// #include <QDebug>

// BashCompletionGeneratorCommand BashCompletionGeneratorCommand::mainCommandFromParser(const QCommandLineParser& parser)
// {
//   BashCompletionGeneratorCommand command;
// 
//   addArgumentsAndOptionsFromParserToCommand(parser, command);
// 
//   return command;
// }
// 
// BashCompletionGeneratorCommand BashCompletionGeneratorCommand::subCommandFromParser(const QCommandLineParser& parser, const std::string& commandName)
// {
//   assert( !commandName.empty() );
// 
//   BashCompletionGeneratorCommand command(commandName);
// 
//   return command;
// }
// 
// void BashCompletionGeneratorCommand::addArgumentToCommand(const QString& argument, BashCompletionGeneratorCommand& command)
// {
//   command.addArgument( argument.toStdString() );
// }
// 
// void BashCompletionGeneratorCommand::addArgumentsAndOptionsFromParserToCommand(const QCommandLineParser& parser, BashCompletionGeneratorCommand& command)
// {
//   const QStringList arguments = parser.positionalArguments();
//   
//   qDebug() << "args: " << arguments;
//   
//   for(const QString & argument : arguments){
//     addArgumentToCommand(argument, command);
//   }
// }

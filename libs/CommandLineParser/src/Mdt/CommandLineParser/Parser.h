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
#ifndef MDT_COMMAND_LINE_PARSER_PARSER_H
#define MDT_COMMAND_LINE_PARSER_PARSER_H

#include "ParserDefinition.h"
#include "ParserResult.h"
#include "CommandLine/CommandLine.h"
#include "mdt_commandlineparser_export.h"
#include <QStringList>
#include <QCoreApplication>

namespace Mdt{ namespace CommandLineParser{

  /*! \brief Parse the command line from a definition and returns a result
   *
   * \code
   * using namespace Mdt::CommandLineParser;
   *
   * int main(int argc, char **argv)
   * {
   *   QCoreApplication app(argc, argv);
   *
   *   ParserDefinition parserDefinition;
   *   // Setup parser definition
   *   ...
   *
   *   Parser parser(parserDefinition);
   *   if( !parser.parse( app.arguments() ) ){
   *     // Error handling
   *     return 1;
   *   }
   *
   *   const ParserResult parserResult = parser.toParserResult();
   *
   * }
   * \endcode
   *
   * \sa ParserDefinition
   * \sa ParserResult
   *
   * \todo Should also limit to option possible values if defined.
   * \note see also Bash completion
   *
   * \todo Parser Impl: Should the command-line reflect what the user typed on error ?
   * If yes, how to define the undefined ??
   *
   */
  class MDT_COMMANDLINEPARSER_EXPORT Parser
  {
    Q_DECLARE_TR_FUNCTIONS(Parser)

   public:

    /*! \brief Construct a parser for \a parserDefinition
     */
    explicit Parser(const ParserDefinition & parserDefinition) noexcept
     : mParserDefinition(parserDefinition)
    {
    }

    /*! \brief Parse \a arguments regarding \a parserDefinition
     *
     * As QCommandLineParser, it is checked if a unknown option exists in \a arguments,
     * but no check is made to enforce the count of positional arguments.
     *
     * For example, if the application expects 2 positional arguments:
     * \code
     * myapp [options] source destination
     * \endcode
     * this function will not fail, either the count of positional arguments does not match:
     * \code
     * // Ok
     * myapp
     *
     * // Ok
     * myapp file1.txt
     *
     * // Ok
     * myapp file1.txt /tmp other-positional-argument
     * \endcode
     *
     * Sub-commands are also supported,
     * for example:
     * \code
     * myapp [options] command [command-options] command-argument-1 command-argument-2
     * \endcode
     * possible calls:
     * \code
     * // Ok
     * myapp
     *
     * // Ok
     * myapp copy file1.txt /tmp
     *
     * // Ok
     * myapp arg1 copy file1.txt /tmp arg2
     * \endcode
     */
    bool parse(const QStringList & arguments);

    /*! \brief Check if this parser has error
     */
    bool hasError() const noexcept
    {
      return !mErrorText.isEmpty();
    }

    /*! \brief Get the error text of this parser
     *
     * This should only be called when parse() returns false.
     */
    const QString & errorText() const noexcept
    {
      return mErrorText;
    }

    /*! \brief Returns a list of unknown option names
     *
     * The list will include the name of unknown options without any prefix dash
     * (i.e. --unknown-option on the command line will become unknown-option name).
     */
    QStringList getUnknownOptionNames() const noexcept;

    /*! \brief Get the command line of this parser
     *
     * \sa toParserResult()
     */
    const CommandLine::CommandLine & commandLine() const noexcept
    {
      return mCommandLine;
    }

    /*! \brief Get a parser result out from this parser
     */
    ParserResult toParserResult() const noexcept;

   private:

    CommandLine::CommandLine mCommandLine;
    ParserDefinition mParserDefinition;
    QString mErrorText;
  };

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_PARSER_H

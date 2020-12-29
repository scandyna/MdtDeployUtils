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
#ifndef MDT_COMMAND_LINE_PARSER_PARSER_H
#define MDT_COMMAND_LINE_PARSER_PARSER_H

#include "ParserDefinition.h"
#include "ParserResult.h"
#include "mdt_commandlineparser_export.h"
#include <QStringList>
#include <QCoreApplication>

namespace Mdt{ namespace CommandLineParser{

  /*! \brief Parse the command line from a definition and returns a result
   *
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
   *   Parser parser;
   *   ParserResult parserResult = parser.parse( parserDefinition, app.arguments() );
   *   if( parserResult.hasError() ){
   *     // Error handling
   *     return 1;
   *   }
   * }
   * \endcode
   *
   * \sa ParserDefinition
   * \sa ParserResult
   */
  class MDT_COMMANDLINEPARSER_EXPORT Parser
  {
    Q_DECLARE_TR_FUNCTIONS(Parser)

   public:

    /*! \brief Parse \a arguments regarding \a parserDefinition
     */
    ParserResult parse(const ParserDefinition & parserDefinition, const QStringList & arguments);
  };

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_PARSER_H

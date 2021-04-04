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
#ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_COMPGEN_ARGUMENT_ALGORITHM_H
#define MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_COMPGEN_ARGUMENT_ALGORITHM_H

#include "CompgenAction.h"
#include "CompgenWordList.h"
#include "CompgenArgument.h"
#include "mdt_commandlineparser_export.h"
#include <QString>
#include <QLatin1String>
#include <QLatin1Char>
#include <QStringBuilder>
#include <boost/variant.hpp>

namespace Mdt{ namespace CommandLineParser{ namespace BashCompletion{

  /*! \brief Get the string representation for \a action
   */
  static
  QString compgenActionToString(CompgenAction action) noexcept
  {
    switch(action){
      case CompgenAction::ListFiles:
        return QLatin1String("-A file");
      case CompgenAction::ListDirectories:
        return QLatin1String("-A directory");
    }

    return QString();
  }

  /*! \brief Get the string representation for \a wordList
   */
  inline
  QString compgenWordListToString(const CompgenWordList & wordList) noexcept
  {
    return QLatin1String("-W \"") % wordList.wordList.join( QLatin1Char(' ') ) % QLatin1Char('"');
  }

  /*! \internal Visitor for compgenArgumentToString()
   */
  class MDT_COMMANDLINEPARSER_EXPORT CompgenArgumentToString : public boost::static_visitor<QString>
  {
   public:

    QString operator()(const CompgenAction & action) const noexcept
    {
      return compgenActionToString(action);
    }

    QString operator()(const CompgenWordList & wordList) const noexcept
    {
      return compgenWordListToString(wordList);
    }

    template<typename T>
    QString operator()(const T &) const noexcept
    {
      return QString();
    }
  };

  /*! \brief Get the string representation for \a argument
   */
  static
  QString compgenArgumentToString(const CompgenArgument & argument) noexcept
  {
    return boost::apply_visitor(CompgenArgumentToString(), argument);
  }

  /*! \brief Get the string representation for \a argumentList
   */
  inline
  QString compgenArgumentListToString(const CompgenArgumentList & argumentList) noexcept
  {
    QString str;

    for(const auto & argument : argumentList){
      str += compgenArgumentToString(argument) % QLatin1Char(' ');
    }

    return str;
  }

}}} // namespace Mdt{ namespace CommandLineParser{ namespace BashCompletion{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_COMPGEN_ARGUMENT_ALGORITHM_H

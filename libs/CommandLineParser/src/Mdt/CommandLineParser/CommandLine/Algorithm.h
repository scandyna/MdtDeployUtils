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
#ifndef MDT_COMMAND_LINE_PARSER_COMMAND_LINE_ALGORITHM_H
#define MDT_COMMAND_LINE_PARSER_COMMAND_LINE_ALGORITHM_H

#include "Argument.h"
#include "mdt_commandlineparser_export.h"
#include <QString>
#include <boost/variant.hpp>
#include <algorithm>
#include <iterator>
#include <cassert>

namespace Mdt{ namespace CommandLineParser{ namespace CommandLine{

  /*! \internal Visitor to check if a argument is the sub-command name
   */
  class MDT_COMMANDLINEPARSER_EXPORT IsSubCommandNameArgument : public boost::static_visitor<bool>
  {
   public:

    bool operator()(const SubCommandName &) const noexcept
    {
      return true;
    }

    template<typename T>
    bool operator()(const T &) const noexcept
    {
      return false;
    }
  };

  /*! \brief Check if \a argument is the sub-command name
   */
  inline
  bool isSubCommandNameArgument(const Argument & argument) noexcept
  {
    return boost::apply_visitor(IsSubCommandNameArgument(), argument);
  }

  /*! \brief Find the sub-command argument in \a arguments
   */
  static
  ArgumentList::const_iterator findSubCommandNameArgument(const ArgumentList & arguments) noexcept
  {
    return std::find_if( arguments.cbegin(), arguments.cend(), isSubCommandNameArgument );
  }

  /*! \brief Find the sub-command argument index in \a arguments
   *
   * Returns the index where the sub-command is located,
   * or a value < 0 if no sub-command name exists.
   */
  inline
  int findSubCommandNameArgumentIndex(const ArgumentList & arguments) noexcept
  {
    const auto it = findSubCommandNameArgument(arguments);
    if( it == arguments.cend() ){
      return -1;
    }
    return static_cast<int>( std::distance( arguments.cbegin(), it ) );
  }

  /*! \internal Visitor to get the sub-command name
   */
  class MDT_COMMANDLINEPARSER_EXPORT GetSubCommandName : public boost::static_visitor<QString>
  {
   public:

    QString operator()(const SubCommandName & argument) const
    {
      return argument.name;
    }

    template<typename T>
    QString operator()(const T &) const noexcept
    {
      return QString();
    }
  };

  /*! \brief Get the sub-command name for given \a argument
   *
   * \pre \a argument must be a SubCommandName argument
   * \sa isSubCommandNameArgument()
   */
  inline
  QString getSubCommandName(const Argument & argument)
  {
    assert( isSubCommandNameArgument(argument) );

    return boost::apply_visitor(GetSubCommandName(), argument);
  }

  /*! \internal Visitor to check if a argument is the sub-command name
   */
  class MDT_COMMANDLINEPARSER_EXPORT IsOptionArgument : public boost::static_visitor<bool>
  {
   public:

    bool operator()(const Option &) const noexcept
    {
      return true;
    }

    template<typename T>
    bool operator()(const T &) const noexcept
    {
      return false;
    }
  };

  /*! \brief Check if \a argument is a option
   *
   * \note if \a argument is a option value,
   *  or a option with value, this function returns false
   */
  inline
  bool isOption(const Argument & argument) noexcept
  {
    return boost::apply_visitor(IsOptionArgument(), argument);
  }

}}} // namespace Mdt{ namespace CommandLineParser{ namespace CommandLine{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_COMMAND_LINE_ALGORITHM_H

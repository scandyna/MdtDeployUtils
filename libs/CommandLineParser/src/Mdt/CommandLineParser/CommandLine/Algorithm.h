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

  /*! \brief Find the sub-command argument name in the argument list in range \a first \a last
   */
  static
  ArgumentList::const_iterator findSubCommandNameArgument(ArgumentList::const_iterator first, ArgumentList::const_iterator last) noexcept
  {
    return std::find_if( first, last, isSubCommandNameArgument );
  }

  /*! \brief Find the sub-command argument in \a arguments
   */
  inline
  ArgumentList::const_iterator findSubCommandNameArgument(const ArgumentList & arguments) noexcept
  {
    return findSubCommandNameArgument( arguments.cbegin(), arguments.cend() );
  }

  /*! \brief Find the sub-command argument name index in the argument list in range \a first \a last
   *
   * Returns the index where the sub-command is located,
   * or a value < 0 if no sub-command name exists.
   */
  static
  int findSubCommandNameArgumentIndex(ArgumentList::const_iterator first, ArgumentList::const_iterator last) noexcept
  {
    const auto it = findSubCommandNameArgument(first, last);
    if( it == last ){
      return -1;
    }
    return static_cast<int>( std::distance( first, it ) );
  }

  /*! \brief Find the sub-command argument index in \a arguments
   *
   * Returns the index where the sub-command is located,
   * or a value < 0 if no sub-command name exists.
   */
  inline
  int findSubCommandNameArgumentIndex(const ArgumentList & arguments) noexcept
  {
    return findSubCommandNameArgumentIndex( arguments.cbegin(), arguments.cend() );
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

  /*! \brief Find the sub-command name in \a arguments
   *
   * Returns the name of the sub-command if it exists in \a arguments ,
   * otherwise a empty string
   */
  inline
  QString findSubCommandName(const ArgumentList & arguments) noexcept
  {
    const auto it = findSubCommandNameArgument(arguments);
    if( it == arguments.cend() ){
      return QString();
    }

    return getSubCommandName(*it);
  }

  /*! \internal Visitor to check if a argument is a option
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

  /*! \internal Visitor to get the option name of a argument
   */
  class MDT_COMMANDLINEPARSER_EXPORT OptionName : public boost::static_visitor<QString>
  {
   public:

    QString operator()(const Option & option) const noexcept
    {
      return option.name;
    }

    QString operator()(const OptionWithValue & option) const noexcept
    {
      return option.name;
    }

    template<typename T>
    QString operator()(const T &) const noexcept
    {
      return QString();
    }
  };

  /*! \brief Get the option name for \a argument
   *
   * Returns the name of the option if \a argument it is one
   * (i.e. a option or a option with value),
   * otherwise a empty string
   */
  inline
  QString optionName(const Argument & argument)
  {
    return boost::apply_visitor(OptionName(), argument);
  }

  /*! \internal Visitor to check if a argument is a option
   */
  class MDT_COMMANDLINEPARSER_EXPORT IsOptionOrOptionWithValueOrAnyDash : public boost::static_visitor<bool>
  {
   public:

    bool operator()(const Option &) const noexcept
    {
      return true;
    }

    bool operator()(const OptionWithValue &) const noexcept
    {
      return true;
    }

    bool operator()(const SingleDash &) const noexcept
    {
      return true;
    }

    bool operator()(const DoubleDash &) const noexcept
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
   * Returns true if \a argument is a option,
   * a option with a value, a single dash or a double dash,
   * otherwise false.
   *
   * \note will also return false if \a argument is a option value
   */
  inline
  bool isOptionOrOptionWithValueOrAnyDash(const Argument & argument) noexcept
  {
    return boost::apply_visitor(IsOptionOrOptionWithValueOrAnyDash(), argument);
  }

  /*! \internal Visitor to check if a argument is a option
   */
  class MDT_COMMANDLINEPARSER_EXPORT IsOptionExpectingValueArgument : public boost::static_visitor<bool>
  {
   public:

    bool operator()(const Option & option) const noexcept
    {
      return option.expectsValue;
    }

    template<typename T>
    bool operator()(const T &) const noexcept
    {
      return false;
    }
  };

  /*! \brief Check if \a argument is a option that expects a value
   */
  inline
  bool isOptionExpectingValue(const Argument & argument) noexcept
  {
    return boost::apply_visitor(IsOptionExpectingValueArgument(), argument);
  }

  /*! \internal Visitor to check if a argument is a positional argument
   */
  class MDT_COMMANDLINEPARSER_EXPORT IsPositionalArgumentArgument : public boost::static_visitor<bool>
  {
   public:

    bool operator()(const PositionalArgument &) const noexcept
    {
      return true;
    }

    template<typename T>
    bool operator()(const T &) const noexcept
    {
      return false;
    }
  };

  /*! \brief Check if \a argument is a positional argument
   */
  inline
  bool isPositionalArgument(const Argument & argument) noexcept
  {
    return boost::apply_visitor(IsPositionalArgumentArgument(), argument);
  }

  /*! \internal Visitor to get a positional argument value
   */
  class MDT_COMMANDLINEPARSER_EXPORT GetPositionalArgumentValue : public boost::static_visitor<QString>
  {
   public:

    QString operator()(const PositionalArgument & argument) const
    {
      return argument.value;
    }

    template<typename T>
    QString operator()(const T &) const noexcept
    {
      return QString();
    }
  };

  /*! \brief Get the positional argument value for given \a argument
   *
   * \pre \a argument must be a positional argument
   * \sa isPositionalArgument()
   */
  inline
  QString getPositionalArgumentValue(const Argument & argument)
  {
    assert( isPositionalArgument(argument) );

    return boost::apply_visitor(GetPositionalArgumentValue(), argument);
  }

}}} // namespace Mdt{ namespace CommandLineParser{ namespace CommandLine{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_COMMAND_LINE_ALGORITHM_H

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
#ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_ACTION_VARIANT_ALGORITHM_H
#define MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_ACTION_VARIANT_ALGORITHM_H

#include "ActionVariant.h"
#include "mdt_commandlineparser_export.h"
#include <QString>
#include <boost/variant.hpp>

namespace Mdt{ namespace CommandLineParser{ namespace BashCompletion{

  /*! \internal Visitor for isActionVariantDefined()
   */
  class MDT_COMMANDLINEPARSER_EXPORT IsActionVariantDefined : public boost::static_visitor<bool>
  {
   public:

    bool operator()(boost::blank) const noexcept
    {
      return false;
    }

    template<typename T>
    bool operator()(const T &) const noexcept
    {
      return true;
    }
  };

  /*! \internal
   */
  inline
  bool isActionVariantDefined(const ActionVariant & action) noexcept
  {
    return boost::apply_visitor(IsActionVariantDefined(), action);
  }

  /*! \internal Visitor for isActionVariantCompgenCommand()
   */
  class MDT_COMMANDLINEPARSER_EXPORT IsActionVariantCompgenCommand : public boost::static_visitor<bool>
  {
   public:

    bool operator()(const CompgenCommand &) const noexcept
    {
      return true;
    }

    template<typename T>
    bool operator()(const T &) const noexcept
    {
      return false;
    }
  };

  /*! \internal
   */
  inline
  bool isActionVariantCompgenCommand(const ActionVariant & action) noexcept
  {
    return boost::apply_visitor(IsActionVariantCompgenCommand(), action);
  }

  /*! \internal Visitor for isActionVariantCustomAction()
   */
  class MDT_COMMANDLINEPARSER_EXPORT IsActionVariantCustomAction : public boost::static_visitor<bool>
  {
   public:

    bool operator()(const CustomAction &) const noexcept
    {
      return true;
    }

    template<typename T>
    bool operator()(const T &) const noexcept
    {
      return false;
    }
  };

  /*! \internal
   */
  inline
  bool isActionVariantCustomAction(const ActionVariant & action) noexcept
  {
    return boost::apply_visitor(IsActionVariantCustomAction(), action);
  }

  /*! \internal
   */
  MDT_COMMANDLINEPARSER_EXPORT
  QString actionVariantToCompreplyArrayItemString(const ActionVariant & action) noexcept;

}}} // namespace Mdt{ namespace CommandLineParser{ namespace BashCompletion{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_ACTION_VARIANT_ALGORITHM_H

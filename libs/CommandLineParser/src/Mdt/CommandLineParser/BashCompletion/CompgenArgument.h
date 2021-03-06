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
#ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_COMPGEN_ARGUMENT_H
#define MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_COMPGEN_ARGUMENT_H

#include "CompgenAction.h"
#include "CompgenWordList.h"
#include "mdt_commandlineparser_export.h"
#include <boost/variant.hpp>
#include <vector>

namespace Mdt{ namespace CommandLineParser{ namespace BashCompletion{

  /*! \internal
   */
  using CompgenArgument = boost::variant<
    CompgenAction, CompgenWordList>;

  /*! \internal
   */
  using CompgenArgumentList = std::vector<CompgenArgument>;

}}} // namespace Mdt{ namespace CommandLineParser{ namespace BashCompletion{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_COMPGEN_ARGUMENT_H

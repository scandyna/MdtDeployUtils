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
#ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_ACTION_LIST_H
#define MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_ACTION_LIST_H

#include "Action.h"
#include "mdt_commandlineparser_export.h"
#include <QString>
#include <vector>
#include <cstddef>
#include <cassert>

namespace Mdt{ namespace CommandLineParser{ namespace BashCompletion{

  /*! \brief List of actions to perform in a Bash completion script
   */
  class MDT_COMMANDLINEPARSER_EXPORT ActionList
  {
   public:

    /*! \brief Add a action to this list
     */
    void addAction(const Action & action) noexcept
    {
      mActions.push_back(action);
    }

    /*! \brief Check if this list is empty
     */
    bool isEmpty() const noexcept
    {
      return mActions.empty();
    }

    /*! \brief Get the count of actions this list holds
     */
    int actionCount() const noexcept
    {
      return static_cast<int>( mActions.size() );
    }

    /*! \brief Get the action at \a index
     *
     * \pre \a index must be in valid range ( 0 <= \a index < actionCount() )
     */
    const Action & actionAt(int index) const noexcept
    {
      assert( index >= 0 );
      assert( index < actionCount() );

      return mActions[static_cast<size_t>(index)];
    }

    /*! \brief Get the COMPREPLY string for this action list
     */
    QString toCompreplyString() const noexcept;

   private:

    std::vector<Action> mActions;
  };

}}} // namespace Mdt{ namespace CommandLineParser{ namespace BashCompletion{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_ACTION_LIST_H

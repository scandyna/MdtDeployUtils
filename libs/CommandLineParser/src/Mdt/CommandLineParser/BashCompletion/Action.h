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
#ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_ACTION_H
#define MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_ACTION_H

#include "CompgenAction.h"
#include "CustomAction.h"
#include "ActionVariant.h"
#include "ActionVariantAlgorithm.h"
#include "mdt_commandlineparser_export.h"
#include <QString>

namespace Mdt{ namespace CommandLineParser{ namespace BashCompletion{

  /*! \brief Action to perform in a Bash completion script
   *
   * \todo Should review custom actions.
   * 1) should provide a way the application can give a list of words,
   *    and let compgen do the completion:
   * \code
   * COMPREPLY=($(compgen -W "$("$executable" completion-list-packages)" -- "$cur"))
   * \endcode
   * 2) the user can handle the completion itself, which seems not so interesting:
   * \code
   * COMPREPLY=($("$executable" completion-list-packages -- "$cur"))
   * \endcode
   */
  class MDT_COMMANDLINEPARSER_EXPORT Action
  {
   public:

    /*! \brief Set a compgen command
     */
    void setCompgenCommand(const CompgenCommand & command) noexcept
    {
      mAction = command;
    }

    /*! \brief Set a custom action
     *
     * Set a custom action to perform in the completion script.
     *
     * The result for completion has to be out to the standard output (std::cout),
     * and will used in the \a COMPREPLY variable.
     *
     * To call your application, the \a '$executable' name can be used.
     *
     * \note Because the executable could be a path containing spaces,
     * it should be protected with double quotes.
     *
     * As example, say you have to list the packages available on the system (like apt-cache).
     * For this, the application will be called with a dedicated query:
     * \code
     * action.setCustomAction("\"$executable\" completion-list-packages");
     * \endcode
     *
     * In the generated script, \a '$executable' will be defined
     * in the completion function:
     * \code
     * executable="$1"
     * \endcode
     * then the executable will be called to fill the \a COMPREPLY variable:
     * \code
     * COMPREPLY=($("$executable" completion-list-packages))
     * \endcode
     */
    void setCustomAction(const QString & action) noexcept
    {
      mAction = CustomAction{action};
    }

    /*! \brief Check if this action is defined
     */
    bool isDefined() const noexcept
    {
      return isActionVariantDefined(mAction);
    }

    /*! \brief Check if this action is a compgen command
     */
    bool isCompgenCommand() const noexcept
    {
      return isActionVariantCompgenCommand(mAction);
    }

    /*! \brief Check if this action is a custom action
     */
    bool isCustomAction() const noexcept
    {
      return isActionVariantCustomAction(mAction);
    }

    /*! \brief Get the COMPREPLY item string for this action
     *
     * Returns the string of the form:
     * \code
     * $(compgen -A file -- \"$cur\")
     * \endcode
     */
    QString toCompreplyArrayItemString() const noexcept
    {
      return actionVariantToCompreplyArrayItemString(mAction);
    }

    /*! \brief Get the COMPREPLY string for this action
     *
     * Returns the string of the form:
     * \code
     * COMPREPLY=($(compgen -A file -- \"$cur\"))
     * \endcode
     */
    QString toCompreplyString() const noexcept;

   private:

    ActionVariant mAction;
  };

}}} // namespace Mdt{ namespace CommandLineParser{ namespace BashCompletion{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_ACTION_H

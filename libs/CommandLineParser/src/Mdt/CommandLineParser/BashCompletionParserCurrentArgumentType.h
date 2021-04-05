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
#ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_PARSER_CURRENT_ARGUMENT_TYPE_H
#define MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_PARSER_CURRENT_ARGUMENT_TYPE_H

namespace Mdt{ namespace CommandLineParser{

  /*! \brief Represents the type of the current argument in a Bash completion
   */
  enum class BashCompletionParserCurrentArgumentType
  {
    Undefined,                        /*!< Undefined type */
    OptionNameValue,                  /*!< In the completion script, a case for a specific option will handle completion.
                                       * For example, a option named \a input-file would be handled as:
                                       * \code
                                       * option-input-file-value)
                                       *   COMPREPLY=($(compgen -A file -- "$cur"))
                                       * \endcode
                                       *
                                       * As other example, a option named \a overwrite-behavior
                                       * that accepts \a keep \a fail or \a overwrite as value:
                                       * \code
                                       * option-overwrite-behavior-value)
                                       *   COMPREPLY=($(compgen -W "keep fail overwrite" -- "$cur"))
                                       * \endcode
                                       *
                                       * A custom action could also have been defined for a option named \a package :
                                       * \code
                                       * option-package-value)
                                       *   COMPREPLY=($(compgen -W "$("$executable" completion-list-packages)" -- "$cur"))
                                       * \endcode
                                       */
    PositionalArgumentName,           /*!< In the completion script, a case for a specific positional argument will handle completion.
                                       * For example, a positional argument that accepts a file would be handled as:
                                       * \code
                                       * source)
                                       *   COMPREPLY=($(compgen -A file -- "$cur"))
                                       * \endcode
                                       *
                                       * A custom action could also have been defined for a positional argument:
                                       * \code
                                       * package)
                                       *   COMPREPLY=($(compgen -W "$("$executable" completion-list-packages)" -- "$cur"))
                                       * \endcode
                                       */
    Options,                          /*!< In the completion script, a case to list options will handle completion.
                                       * Example:
                                       * \code
                                       * options)
                                       *   COMPREPLY=($(compgen -W "-h --help" -- "$cur"))
                                       * \endcode
                                       */
    OptionsOrPositionalArgumentName,  /*!< For the situation the completion can list options or the value of a positional argument,
                                       * for example:
                                       * \code
                                       *     app
                                       * cursor: ^
                                       * \endcode
                                       *
                                       * In the completion script,
                                       * a case to list options and the value of a specific positional argument
                                       * will handle completion.
                                       * For example:
                                       * \code
                                       * options-or-source)
                                       *   COMPREPLY=($(compgen -A file -W "-h --help" -- "$cur"))
                                       * \endcode
                                       */
    Commands,                         /*!< For the situation the completion can list sub-commands, for example:
                                       * \code
                                       *     app c
                                       * cursor: ^
                                       * \endcode
                                       *
                                       * In the completion script, a case to list available commands
                                       * will handle completion.
                                       * For example:
                                       * \code
                                       * commands)
                                       *   COMPREPLY=($(compgen -W "copy cut" -- "$cur"))
                                       * \endcode
                                       */
    OptionsOrCommands                 /*!< For the situation the completion can list sub-commands or options,
                                       * for example:
                                       * \code
                                       *     app
                                       * cursor: ^
                                       * \endcode
                                       *
                                       * In the completion script, a case to list available commands and options
                                       * will handle completion.
                                       * For example:
                                       * \code
                                       * options-or-commands)
                                       *   COMPREPLY=($(compgen -W "-h --help copy cut" -- "$cur"))
                                       * \endcode
                                       */
  };

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_PARSER_CURRENT_ARGUMENT_TYPE_H

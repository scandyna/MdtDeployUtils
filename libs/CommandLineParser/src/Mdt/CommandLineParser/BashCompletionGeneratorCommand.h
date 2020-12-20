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
#ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_GENERATOR_COMMAND_H
#define MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_GENERATOR_COMMAND_H

#include "BashCompletionGeneratorOption.h"
#include "ParserDefinitionCommand.h"
#include "mdt_commandlineparser_export.h"
#include <QString>
#include <QStringList>
#include <cassert>
#include <vector>

namespace Mdt{ namespace CommandLineParser{

  /*! \brief Command for BashCompletionGenerator
   *
   * A command holds a list of arguments and options
   */
  class MDT_COMMANDLINEPARSER_EXPORT BashCompletionGeneratorCommand
  {
   public:

    /*! \brief Construct a command with no name
     *
     * This is useful for the main command
     */
    BashCompletionGeneratorCommand() = default;

    /*! \brief Construct a command
     *
     * \pre \a name must not be empty
     */
    BashCompletionGeneratorCommand(const QString & name)
     : mName(name)
    {
      assert( !name.trimmed().isEmpty() );
    }

    /*! \brief Copy construct a command from \a other
     */
    BashCompletionGeneratorCommand(const BashCompletionGeneratorCommand & other) = default;

    /*! \brief Copy assign \a other to this command
     */
    BashCompletionGeneratorCommand & operator=(BashCompletionGeneratorCommand && other) = default;

    /*! \brief Move construct a command from \a other
     */
    BashCompletionGeneratorCommand(BashCompletionGeneratorCommand && other) = default;

    /*! \brief Move assign \a other to this command
     */
    BashCompletionGeneratorCommand & operator=(const BashCompletionGeneratorCommand & other) = default;

    /*! \brief Check if this command has a name
     */
    bool hasName() const noexcept
    {
      return !mName.isEmpty();
    }

    /*! \brief Get the name of this command
     */
    const QString & name() const noexcept
    {
      return mName;
    }

    /*! \brief Add a argument to this command
     *
     * \pre \a arg must not be empty
     * 
     * \todo rename to addPositionalArgument()
     * \todo add ArgumentType at first (must match ParserDefinition API)
     *       ArgumentType should not be optional
     */
    void addArgument(const QString & arg)
    {
      assert( !arg.trimmed().isEmpty() );

      mArguments.push_back(arg);
    }

    /*! \brief Get the list of arguments of this command
     */
    const QStringList & arguments() const noexcept
    {
      return mArguments;
    }

    /*! \brief Check if this command is empty
     *
     * Return true if this command has no arguments and no options
     */
    bool isEmpty() const noexcept
    {
      return mArguments.isEmpty() && mOptions.empty();
    }

    /*! \brief Add a command line option
     */
    void addOption(const BashCompletionGeneratorOption & option)
    {
      mOptions.push_back(option);
    }

    /*! \brief Add a command line option
     *
     * \pre \a shortName must be a character [a-z]
     * \pre \a name must not be empty
     * \pre \a name must not begin with a dash or a slash
     * \pre \a name must not contain any equal
     *
     * \todo enforce those precontidions
     */
    void addOption(char shortName, const QString & name)
    {
      assert( !name.trimmed().isEmpty() );

      addOption( BashCompletionGeneratorOption(shortName, name) );
    }

    /*! \brief Add a command line option
     *
     * \pre \a name must not be empty
     * \pre \a name must not begin with a dash or a slash
     * \pre \a name must not contain any equal
     *
     * \todo enforce those precontidions
     */
    void addOption(const QString & name)
    {
      assert( !name.trimmed().isEmpty() );

      addOption( BashCompletionGeneratorOption(name) );
    }

    /*! \brief Get the list of options of this command
     */
    const std::vector<BashCompletionGeneratorOption> & options() const noexcept
    {
      return mOptions;
    }

    /*! \brief Set the directory completion to \a enable for this command
     */
    void setDirectoryCompletionEnabled(bool enable) noexcept
    {
      mDirectoryCompletionEnabled = enable;
    }

    /*! \brief Check if directory completion is enabled for this command
     */
    bool directoryCompletionEnabled() const noexcept
    {
      return mDirectoryCompletionEnabled;
    }

    /*! \brief Get a generator command from \a command
     */
    static
    BashCompletionGeneratorCommand fromParserDefinitionCommand(const ParserDefinitionCommand & command);

  //   /*! \brief Get a command from \a parser
  //    */
  //   static
  //   BashCompletionGeneratorCommand mainCommandFromParser(const QCommandLineParser & parser);
  // 
  //   /*! \brief Get a command from \a parser
  //    *
  //    * \pre \a commandName must not be empty
  //    */
  //   static
  //   BashCompletionGeneratorCommand subCommandFromParser(const QCommandLineParser & parser, const std::string & commandName);

  private:

  //   static
  //   void addArgumentToCommand(const QString & argument, BashCompletionGeneratorCommand & command);
  // 
  //   static
  //   void addArgumentsAndOptionsFromParserToCommand(const QCommandLineParser & parser, BashCompletionGeneratorCommand & command);

    bool mDirectoryCompletionEnabled = false;
    QString mName;
    QStringList mArguments;
    std::vector<BashCompletionGeneratorOption> mOptions;
  };

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_GENERATOR_COMMAND_H

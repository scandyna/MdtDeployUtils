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
#ifndef MDT_COMMAND_LINE_PARSER_PARSER_DEFINITION_IMPL_H
#define MDT_COMMAND_LINE_PARSER_PARSER_DEFINITION_IMPL_H

#include "ParserDefinition.h"
#include "ParserDefinitionOption.h"
#include "ParserDefinitionPositionalArgument.h"
#include "ParserDefinitionCommand.h"
#include "Algorithm.h"
#include <QString>
#include <QLatin1String>
#include <QChar>
#include <QLatin1Char>
#include <QStringBuilder>
#include <QCoreApplication>
#include <cassert>
#include <vector>
#include <iterator>

// #include <QDebug>

namespace Mdt{ namespace CommandLineParser{

  namespace Impl{

    /*! \brief Check if \a c is a breackable character
     */
    static
    bool isBreackableChar(QChar c) noexcept
    {
      return c.isSpace();
    }

    /*! \brief Find the point where to break a text so that its length wil be <= \a maxLength
     *
     * \pre \a maxLength must be > 1
     */
    static
    QString::const_iterator findBreakPoint(QString::const_iterator first, QString::const_iterator last, int maxLength)
    {
      assert( maxLength > 1 );

      auto lastBreakable = last;
      int index = 0;

      while(first != last){
        const QChar c = *first;
        if( isBreackableChar(c) ){
          lastBreakable = first;
        }

        // Maybe we are at max length with a breakable char
        if( (index == maxLength) && (lastBreakable != last) ){
          return lastBreakable;
        }

        /*
         * Index is at the last position allowed by maxLength
         *
         * Possible cases:
         *  1) we also are reaching the en of the string: 'AB'
         *                                                  ^
         *  2) the next char is a beakable: 'AB '
         *                                    ^
         *  3) the next char is not a breakable: 'ABC'
         *                                         ^
         */
        if( (index == maxLength-1) && (lastBreakable == last) ){
          auto next = first+1;
          if(next == last){
            return last;
          }
          if( isBreackableChar(*next) ){
            return next;
          }

          return first;
        }

        ++index;
        ++first;
      }

      return first;
    }

    /*! \brief Returns a string from \a text breaked so that the length of each line is no longer than \a maxLength
     *
     * \pre \a leftPadLength must be >= 0
     * \pre \a maxLength must be > 1
     */
    static
    QString breakText(const QString & text, int leftPadLength, int maxLength)
    {
      assert( leftPadLength >= 0 );
      assert( maxLength > 1 );

      /*
       * First attempt was to modify text inplace.
       * This is not easy to get right while iterating over the text.
       * With QString, we also have to care about the iterator problem,
       * see https://doc.qt.io/qt-5/containers.html#implicit-sharing-iterator-problem .
       * Also, while modifying text, memory allocations and copies will occur anyway.
       * (and also: accessing a non const iterator of QString can also produce a deep copy).
       */

      QString result;
      const QString padding = QString( leftPadLength, QLatin1Char(' ') );

      auto first = text.cbegin();
      const auto last = text.cend();

      while(first != last){
        const auto breakPoint = findBreakPoint(first, last, maxLength);
        const int posInText = std::distance(text.cbegin(), first);
        const int charsToCopy = std::distance(first, breakPoint);

        first += charsToCopy;

        if(breakPoint == last){
          result += text.mid(posInText, charsToCopy);
        }else{
          if( isBreackableChar(*breakPoint) ){
            /*
             * Note: we don't have to handle the case of the breakPoint being a EOL,
             * because we skip the break point.
             */
            result += text.mid(posInText, charsToCopy) + QLatin1Char('\n') + padding;
            ++first;
          }else{
            result += text.mid(posInText, charsToCopy) + QLatin1String("-\n") + padding;
          }
        }
      }

      return result;
    }

  } // namespace Impl{

  /*! \internal
   */
  class ParserDefinition_Impl
  {
    Q_DECLARE_TR_FUNCTIONS(ParserDefinition_Impl)

   public:

    static
    QString argumentToUsageString(const ParserDefinitionPositionalArgument & argument)
    {
      if( argument.hasSyntax() ){
        return argument.syntax();
      }
      return argument.name();
    }

    static
    QString argumentListToUsageString(const std::vector<ParserDefinitionPositionalArgument> & arguments)
    {
      return joinToQString(arguments, ' ', argumentToUsageString);
    }

    static
    QString getUsageText(const QString & applicationName, const std::vector<ParserDefinitionOption> & options,
                         const std::vector<ParserDefinitionPositionalArgument> & arguments,
                         const std::vector<ParserDefinitionCommand> & subCommands)
    {
      QString usageText = tr("Usage: %1").arg(applicationName);

      if( !options.empty() ){
        usageText += QLatin1String(" [") % tr("options") % QLatin1Char(']');
      }

      if( !arguments.empty() ){
        usageText += QLatin1Char(' ') % argumentListToUsageString(arguments);
      }

      if( !subCommands.empty() ){
        usageText += QLatin1Char(' ') % tr("command");
      }

      return usageText;
    }


    static
    QString wrapText(const QString & names, int longestName, const QString & description)
    {
      const QLatin1String indent("  ");

      return indent % names.leftJustified(longestName) % indent % description;
    }

    static
    QString wrapText(const ParserDefinitionOption & option, int longestName)
    {
    }

    static
    QString wrapText(const ParserDefinitionPositionalArgument & argument, int longestName)
    {
    }

    static
    QString getOptionsHelpText(const std::vector<ParserDefinitionOption> & options)
    {
      QString text = tr("Options:\n");
      
      
      return text;
    }

    static
    QString getPositionalArgumentsHelpText(const std::vector<ParserDefinitionPositionalArgument> & arguments)
    {
      QString text = tr("Arguments:\n");
      
      
      return text;
    }

    static
    QString getAvailableSubCommandsHelpText(const std::vector<ParserDefinitionCommand> & subCommands)
    {
      QString text = tr("Commands:\n");
      
      
      return text;
    }
  };

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_PARSER_DEFINITION_IMPL_H

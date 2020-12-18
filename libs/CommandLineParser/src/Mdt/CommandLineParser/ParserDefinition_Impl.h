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

#include <QDebug>

namespace Mdt{ namespace CommandLineParser{

  /*! \brief Check if \a c is a breackable character
   */
  static
  bool isBreackableChar(QChar c) noexcept
  {
    return c.isSpace();
  }

  /*! \brief Find the last point where to breack \a text so that its length will be <= \a maxLength
   *
   * \pre \a maxLength must be > 1
   */
  static
  int findIndexOfBreakPoint(const QString & text, int maxLength)
  {
    assert( maxLength > 1 );

    const int textLength = text.length();
    int lastBreakable = -1;

    for(int i = 0; i < textLength; ++i){
      const QChar c = text.at(i);
      if( isBreackableChar(c) ){
        lastBreakable = i;
      }
    }

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

//       const bool currentPositionIsBreakable = isBreackableChar(*first);
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
  QString breakText_2(const QString & text, int leftPadLength, int maxLength)
  {
    /*
     * First attempt was to modify text inplace.
     * This is not easy to get right while iterating over the text.
     * With QString, we also have to care about the iterator problem,
     * see https://doc.qt.io/qt-5/containers.html#implicit-sharing-iterator-problem .
     * Also, while modifying text, memory allocations and copies will occur anyway.
     * (and also: accessing a non const iterator of QString can also produce a deep copy).
     */

    assert( leftPadLength >= 0 );
    assert( maxLength > 1 );

    qDebug() << "breakText(): " << text;
    
    QString result;
    const QString padding = QString( leftPadLength, QLatin1Char(' ') );

    auto first = text.cbegin();
    const auto last = text.cend();

    while(first != last){
      const auto breakPoint = findBreakPoint(first, last, maxLength);
      const int posInText = std::distance(text.cbegin(), first);
      const int charsToCopy = std::distance(first, breakPoint);
      
      qDebug() << " - At end: " << (breakPoint == last);
      
      qDebug() << " - posInText: " << posInText;
      qDebug() << " - charsToCopy: " << charsToCopy;
      
      first += charsToCopy;

      if(breakPoint == last){
        result += text.mid(posInText, charsToCopy);
      }else{
        qDebug() << " - found break point " << *breakPoint;
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

  /*! \brief \todo document
   *
   * \pre \a leftPadLength must be >= 0
   * \pre \a maxLength must be > 1
   */
  static
  void breakText(QString & text, int leftPadLength, int maxLength)
  {
    assert( leftPadLength >= 0 );
    assert( maxLength > 1 );

    text = breakText_2(text, leftPadLength, maxLength);
    return;

    /** \todo Modifying QString is risky + will probably produce a deep copy
     *
     * Mybe pass text as const QString and build a resulting string to return ?
     *
     * NOTE: forget vector<QChar>, this is a copy anyway
     *
     * Or, again: index access
     *
     * To avoid API change:
     * make a function that takes a const QString and returns a QString,
     * then call it from here (avoids changing all the tests for sandbox)
     *
     * text = breakText_2(text);
     * 
     */
    

    qDebug() << "breakText(): " << text;

    /** \todo Use non const iterator ( text.begin() ),
     *   then we could use iterators to replace some part
     *   (f.ex. at breack-point)
     *
     * NOTE: remeber: iterators invalidation !
     *
     * OR: keep as is..
     *
     * Should work with index
     */
    auto first = text.cbegin();
    const auto last = text.cend();

      /// \todo After insert/modify, get the iterators again, all ! (COW!)
      /// Review this: file:///usr/share/qt5/doc/qtcore/containers.html#
      ///
      /// NOTE: once detached, but not copied, is there any problem ? Think not !
      ///   --> Also, if passd text is allready detached, what happens ?!!!?

    /*
    while(first != last){
      const auto breakPoint = findBreakPoint(first, last, maxLength);
      if( breakPoint == text.cend() ){
        return;
      }
      
      qDebug() << " - found break point " << *breakPoint;
      
      first = breakPoint;

    }
    */
    
    const auto breakPoint = findBreakPoint(text.cbegin(), text.cend(), maxLength);
    if( breakPoint == text.cend() ){
      return;
    }

    /// \todo below code should become a function
    const int index = std::distance( text.cbegin(), breakPoint );
    
    qDebug() << " - found break point " << *breakPoint << " at index " << index;
    
    const QString padding = QString( leftPadLength, QLatin1Char(' ') );
    
    /// \todo index checking etc..
    if( isBreackableChar(*breakPoint) ){
      text.replace( index, 1, QLatin1Char('\n') );  /// And is not EOL !
      text.insert( index+1, padding );
    }else{
      text.insert( index, QLatin1String("-\n") + padding );
//       text.insert( index+2, padding );
    }
    
  }

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

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
#ifndef MDT_COMMAND_LINE_PARSER_ALGORITHM_H
#define MDT_COMMAND_LINE_PARSER_ALGORITHM_H

#include <QString>
#include <QChar>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include <cassert>

namespace Mdt{ namespace CommandLineParser{

  /*! \brief Apply a function until a predicate is true
   *
   * Applies the given function object \a f
   * to the result of dereferencing every iterator,
   * until the given function object \a p returns true,
   * in the range [first, last), in order.
   *
   * \note  The function \a f is also applied for the element
   *  for wich \a p returns true.
   *  The predicate \a p is checked after \a f was applied.
   *
   * \param f function object to be applied.
   * The signature of the function should be equivalent to the following:
   * \code
   * void fun(const Type &a);
   * \endcode
   *
   * \param p unary predicate which returns ​true for the required element.
   * The signature of the function should be equivalent to the following:
   * \code
   * bool pred(const Type &a);
   * \endcode
   *
   * \tparam InputIt iterator that meets the requirements of LegacyInputIterator.
   * \tparam UnaryFunction function that meets the requirements of MoveConstructible.
   *  Does not have to be CopyConstructible
   * \tparam UnaryPredicate function that meets the requirements of Predicate.
   *
   * \return Iterator to the first element for wich \a p returns true or \a last if no such element is found.
   */
  template<typename InputIt, typename UnaryFunction, typename UnaryPredicate>
  InputIt forEachUntil(InputIt first, InputIt last, UnaryFunction f, UnaryPredicate p)
  {
    while(first != last){
      f(*first);
      if( p(*first) ){
        return first;
      }
      ++first;
    }

    return first;
  }

  /*! \brief Get a QString from latin1 \a c
   */
  inline
  QString qStringFromLatin1Char(char c) noexcept
  {
    return QString( QChar::fromLatin1(c) );
  }

  /*! \brief Get a QString out from chars \a a and \a b
   */
  inline
  QString qStringFromChars(char a, char b)
  {
    char buffer[2] = {a,b};

    return QString::fromLatin1(buffer, 2);
  }

  /*! \brief Get a QString with the content of a sub-string
   */
  inline
  QString qStringFromSubString(QString::const_iterator first, QString::const_iterator last) noexcept
  {
    QString subString;

    std::copy( first, last, std::back_inserter(subString) );

    return subString;
  }

  /*! \brief Joint each element in \a list to a single string with each element seperated by given \a separator
   *
   * Extract is a functor to extract the string from a element in \a list ,
   * and is of the form:
   * \code
   * QString extract(const T & element);
   * \endcode
   */
  template<typename T, typename Extract>
  QString joinToQString(const std::vector<T> & list, char separator, const Extract & extract)
  {
    using sizeType = typename std::vector<T>::size_type;

    QString result;

    if( list.empty() ){
      return result;
    }

    sizeType lastElement = list.size() - 1;
    assert( lastElement < list.size() );
    for(sizeType i = 0; i < lastElement; ++i){
      result += extract(list[i]) + QChar::fromLatin1(separator);
    }
    result += extract(list[lastElement]);

    return result;
  }

  /*! \brief Joint each strings in \a list to a single string with each element seperated by given \a separator
   */
  inline
  std::string joinToStdString(const std::vector<std::string> & list, char separator)
  {
    using sizeType = std::vector<std::string>::size_type;

    std::string result;

    if( list.empty() ){
      return result;
    }

    sizeType lastElement = list.size() - 1;
    assert( lastElement < list.size() );
    for(sizeType i = 0; i < lastElement; ++i){
      result += list[i] + separator;
    }
    result += list[lastElement];

    return result;
  }

  /*! \brief Add elements from \a source vector to the end of \a destinaton vector
   */
  template<typename T>
  void appendToStdVector(const std::vector<T> & source, std::vector<T> & destinaton)
  {
    std::copy( source.cbegin(), source.cend(), std::back_inserter(destinaton) );
  }

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_ALGORITHM_H

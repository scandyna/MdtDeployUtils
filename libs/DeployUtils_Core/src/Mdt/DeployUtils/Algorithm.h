/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2020-2023 Philippe Steinmann.
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with this program.  If not, see <https://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_DEPLOY_UTILS_ALGORITHM_H
#define MDT_DEPLOY_UTILS_ALGORITHM_H

#include "mdt_deployutilscore_export.h"
#include <QString>
#include <QChar>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include <cassert>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Check if \a str starts with \a s
   *
   * \note current implementation is case sensitive
   * \pre \a s must not be empty
   */
  inline
  bool stringStartsWith(const std::string & str, const std::string & s) noexcept
  {
    assert( !s.empty() );

    if( s.size() > str.size() ){
      return false;
    }
    const auto last = str.cbegin() + static_cast<std::string::difference_type>( s.size() );

    return std::equal( str.cbegin(), last, s.cbegin(), s.cend() );
  }

  /*! \brief Join each strings in \a list to a single string with each element seperated by given \a separator
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

  /*! \brief Join each element in \a container to a single string with each element seperated by given \a separator
   *
   * \a toQString is a function object that will be called
   * to get each element in \a container as a QString.
   * It signature should be equivalent to:
   * \code
   * QString f(const Type & a);
   * \endcode
   */
  template<typename Container, typename ToQString>
  QString joinToQString(const Container & container, const ToQString & toQString, const QString & separator) noexcept
  {
    using sizeType = typename Container::size_type;

    QString result;
    const sizeType size = container.size();
    if(size == 0){
      return result;
    }
    assert(size >= 1);

    result = toQString( container[0] );
    for(sizeType i=1; i < size; ++i){
      result += separator + toQString( container[i] );
    }

    return result;
  }

  /*! \brief Add elements from \a source vector to the end of \a destinaton vector
   */
  template<typename T>
  void appendToStdVector(const std::vector<T> & source, std::vector<T> & destinaton)
  {
    std::copy( source.cbegin(), source.cend(), std::back_inserter(destinaton) );
  }

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_ALGORITHM_H

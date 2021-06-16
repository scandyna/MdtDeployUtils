/****************************************************************************
 **
 ** Copyright (C) 2020-2021 Philippe Steinmann.
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
#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include "Mdt/DeployUtils/LibraryName.h"
#include <QString>
#include <QStringList>
#include <vector>
#include <string>
#include <cassert>

QStringList qStringListFromUtf8Strings(const std::vector<std::string> & args)
{
  QStringList arguments;

  for(const auto & arg : args){
    arguments.append( QString::fromStdString(arg) );
  }

  return arguments;
}

QString generateStringWithNChars(int n)
{
  assert( n > 0 );

  QString str;

  for(int i=0; i<n; ++i){
    str += QString::number(i%10);
  }
  assert( str.length() == n );

  return str;
}

bool containsQt5Core(const QStringList & libraries)
{
  for(const QString & library : libraries){
    const Mdt::DeployUtils::LibraryName libraryName(library);
    if( libraryName.name() == QLatin1String("Qt5Core") ){
      return true;
    }
  }

  return false;
}

#endif // #ifndef TEST_UTILS_H

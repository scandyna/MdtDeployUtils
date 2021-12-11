/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2021-2021 Philippe Steinmann.
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
#include "RPathElf.h"
#include <QStringList>
#include <QLatin1Char>
#include <QLatin1String>
#include <cassert>

namespace Mdt{ namespace DeployUtils{

RPathEntry RPathElf::rPathEntryFromString(const QString & path)
{
  const QString entryString = path.trimmed();
  assert( !entryString.isEmpty() );

  QString rpathEntryString;

  if( entryString.startsWith( QLatin1String("$ORIGIN") ) ){
    if(entryString.length() > 7){
      if( entryString.at(7) != QLatin1Char('/') ){
        const QString msg = tr("expected a '/' after $ORIGIN, got '%1'")
                            .arg( entryString.at(7) );
        throw RPathFormatError(msg);
      }
      rpathEntryString = entryString.right(entryString.length() - 8);
    }else{
      rpathEntryString = QLatin1String(".");
    }
  }else if( entryString.startsWith( QLatin1String("${ORIGIN}") ) ){
    if(entryString.length() > 9){
      if( entryString.at(9) != QLatin1Char('/') ){
        const QString msg = tr("expected a '/' after ${ORIGIN}, got '%1'")
                            .arg( entryString.at(7) );
        throw RPathFormatError(msg);
      }
      rpathEntryString = entryString.right(entryString.length() - 10);
    }else{
      rpathEntryString = QLatin1String(".");
    }
  }else{
    rpathEntryString = entryString;
  }

  if( rpathEntryString.isEmpty() ){
    rpathEntryString = QLatin1String(".");
  }

  return RPathEntry(rpathEntryString);
}

RPath RPathElf::rPathFromString(const QString & rpathString)
{
  RPath rpath;
  const QStringList rpathStringList = rpathString.split(QLatin1Char(':'), QString::SkipEmptyParts);

  for(const QString & path : rpathStringList){
    rpath.appendEntry( rPathEntryFromString(path) );
  }

  return rpath;
}

}} // namespace Mdt{ namespace DeployUtils{

/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2015-2021 Philippe Steinmann.
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
#include "LibraryVersion.h"
#include <QStringBuilder>
#include <QLatin1String>
#include <QChar>
#include <QStringRef>
#include <QVector>
#include <cassert>

namespace Mdt{ namespace DeployUtils{

LibraryVersion::LibraryVersion(int versionMajor, int versionMinor, int versionPatch)
 : mVersionMajor(versionMajor),
   mVersionMinor(versionMinor),
   mVersionPatch(versionPatch)
{
  assert(mVersionMajor >= 0);
  assert( (mVersionPatch < 0) || (mVersionMinor >= 0) );
}

LibraryVersion::LibraryVersion(const QStringRef & version)
{
  const auto stringList = version.split( QChar::fromLatin1('.'), QString::SkipEmptyParts );
  bool vMajorOk = true;
  bool vMinorOk = true;
  bool vPatchOk = true;

  switch(stringList.size()){
    case 1:
      mVersionMajor = stringList.at(0).toInt(&vMajorOk);
      break;
    case 2:
      mVersionMajor = stringList.at(0).toInt(&vMajorOk);
      mVersionMinor = stringList.at(1).toInt(&vMinorOk);
      break;
    case 3:
      mVersionMajor = stringList.at(0).toInt(&vMajorOk);
      mVersionMinor = stringList.at(1).toInt(&vMinorOk);
      mVersionPatch = stringList.at(2).toInt(&vPatchOk);
      break;
    default:
      break;
  }
  if(!vMajorOk || !vMinorOk || !vPatchOk){
    mVersionMajor = -1;
    mVersionMinor = -1;
    mVersionPatch = -1;
  }
}

QString LibraryVersion::toString() const
{
  QString version;

  if(isNull()){
    return version;
  }
  if(mVersionPatch >= 0){
    version = QString::number(mVersionMajor) % QLatin1String(".") % QString::number(mVersionMinor) % QLatin1String(".") % QString::number(mVersionPatch);
  }else if(mVersionMinor >= 0){
    version = QString::number(mVersionMajor) % QLatin1String(".") % QString::number(mVersionMinor);
  }else{
    version = QString::number(mVersionMajor);
  }

  return version;
}

}} // namespace Mdt{ namespace DeployUtils{

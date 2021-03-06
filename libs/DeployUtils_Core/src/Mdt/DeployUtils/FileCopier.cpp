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
#include "FileCopier.h"
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QLatin1String>
#include <QLatin1Char>
#include <QDateTime>
#include <cassert>

// #include <QDebug>

namespace Mdt{ namespace DeployUtils{

FileCopier::FileCopier(QObject* parent)
 : QObject(parent)
{
}

void FileCopier::createDirectory(const QString& directoryPath)
{
  auto dir = QDir::current();
  QFileInfo fi(directoryPath);

  if( fi.isFile() ){
    const QString msg = tr("Could not create directory '%1' because it allready exists and is a file.")
                        .arg(directoryPath);
    throw FileCopyError(msg);
  }

  if( !dir.mkpath(directoryPath) ){
    const QString msg = tr("Could not create directory '%1'")
                        .arg(directoryPath);
    throw FileCopyError(msg);
  }
}

void FileCopier::setOverwriteBehavior(OverwriteBehavior behavior) noexcept
{
  mOverwriteBehavior = behavior;
}

void FileCopier::copyFile(const QString & sourceFilePath, const QString & destinationDirectoryPath)
{
  assert( isExistingDirectory(destinationDirectoryPath) );

  QFileInfo sourceFileInfo(sourceFilePath);
  assert( sourceFileInfo.exists() );
  assert( sourceFileInfo.isFile() );

  const auto fileName = sourceFileInfo.fileName();
  const auto destinationFilePath = getDestinationFilePath(sourceFileInfo, destinationDirectoryPath);
  const QFileInfo destinationFileInfo(destinationFilePath);

  if( destinationFileInfo.exists() ){
    if( mOverwriteBehavior == OverwriteBehavior::Keep ){
      return;
    }
    if(destinationFileInfo == sourceFileInfo){
      return;
    }
    if( mOverwriteBehavior == OverwriteBehavior::Fail ){
      const QString msg = tr("copy file '%1' to '%2' failed because the destination file extsis (overwrite behavior is Fail)")
                          .arg( sourceFileInfo.absoluteFilePath(), destinationFileInfo.absoluteFilePath() );
      throw FileCopyError(msg);
    }
    assert( mOverwriteBehavior == OverwriteBehavior::Overwrite );
//     if( destinationFileInfo.created() >= sourceFileInfo.created() ){
// //       Console::info(3) << " allready up to date: " << sourceFileInfo.fileName();
//       return;
//     }
    QFile destinationFile( destinationFileInfo.absoluteFilePath() );
    if( !destinationFile.remove() ){
      const QString msg = tr("Could not remove destination file '%1'")
                          .arg( destinationFileInfo.absoluteFilePath() );
      throw FileCopyError(msg);
    }
  }

  const QString copyFileMsg = tr("copy %1 to %2").arg( sourceFileInfo.fileName(), destinationDirectoryPath );
  emit verboseMessage(copyFileMsg);

  QFile sourceFile( sourceFileInfo.absoluteFilePath() );
  if( !sourceFile.copy(destinationFilePath) ){
    const QString msg = tr("Could not copy file '%1' to '%2': %3")
                        .arg( sourceFileInfo.absoluteFilePath(), destinationDirectoryPath, sourceFile.errorString() );
    throw FileCopyError(msg);
  }
}

void FileCopier::copyFiles(const QStringList & sourceFilePathList, const QString & destinationDirectoryPath)
{
  createDirectory(destinationDirectoryPath);

  for(const QString & sourceFilePath : sourceFilePathList){
    copyFile(sourceFilePath, destinationDirectoryPath);
  }
}

bool FileCopier::isExistingDirectory(const QString & directoryPath) noexcept
{
  QFileInfo fi(directoryPath);

  if( fi.exists() ){
    return !fi.isFile();
  }

  return false;
}

QString FileCopier::getDestinationFilePath(const QString & sourceFilePath, const QString & destinationDirectoryPath) noexcept
{
  return getDestinationFilePath( QFileInfo(sourceFilePath), destinationDirectoryPath );
}

QString FileCopier::getDestinationFilePath(const QFileInfo & sourceFile, const QString & destinationDirectoryPath) noexcept
{
  return QDir::cleanPath( destinationDirectoryPath + QLatin1Char('/') + sourceFile.fileName() );
}

}} // namespace Mdt{ namespace DeployUtils{

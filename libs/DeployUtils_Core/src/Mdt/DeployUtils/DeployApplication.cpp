/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2022 Philippe Steinmann.
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
#include "DeployApplication.h"
#include "FileCopier.h"
#include "RPath.h"
#include "ExecutableFileReader.h"
#include "ExecutableFileWriter.h"
#include "CopySharedLibrariesTargetDependsOn.h"
#include "CopySharedLibrariesTargetDependsOnRequest.h"
#include <QLatin1String>
#include <QLatin1Char>
#include <QStringBuilder>
#include <QStringList>
#include <QDir>
#include <cassert>

namespace Mdt{ namespace DeployUtils{

void DeployApplication::execute(const DeployApplicationRequest & request)
{
  assert( !request.targetFilePath.trimmed().isEmpty() );
  assert( !request.destinationDirectoryPath.trimmed().isEmpty() );

  emit statusMessage(
    tr("Deploy application for executable %1")
    .arg(request.targetFilePath)
  );

  emit statusMessage(
    tr("Destination directory is %1")
    .arg(request.destinationDirectoryPath)
  );

  ExecutableFileReader reader;
  reader.openFile(request.targetFilePath);
  mPlatform = reader.getFilePlatform();
  reader.close();

  emit verboseMessage(
    tr("Executable targets %1")
    .arg( osName( mPlatform.operatingSystem() ) )
  );

  makeDirectoryStructure(request);
  installExecutable(request);
  copySharedLibrariesTargetDependsOn(request);
}

QString DeployApplication::sharedLibrariesDirectoryName(OperatingSystem os) noexcept
{
  assert(os != OperatingSystem::Unknown);

  switch(os){
    case OperatingSystem::Linux:
      return QLatin1String("lib");
    case OperatingSystem::Windows:
      return QLatin1String("bin");
    case OperatingSystem::Unknown:
      break;
  }

  return QString();
}

void DeployApplication::makeDirectoryStructure(const DeployApplicationRequest & request)
{
  assert( !mPlatform.isNull() );

  emit verboseMessage(
    tr("Make directory structure")
  );

  mBinDirDestinationPath = QDir::cleanPath( request.destinationDirectoryPath % QLatin1String("/bin") );
  emit debugMessage(
    tr("Create directory %1")
    .arg(mBinDirDestinationPath)
  );
  FileCopier::createDirectory(mBinDirDestinationPath);

  mLibDirDestinationPath = QDir::cleanPath(
    request.destinationDirectoryPath % QLatin1Char('/') % sharedLibrariesDirectoryName( mPlatform.operatingSystem() )
  );
  if(mLibDirDestinationPath != mBinDirDestinationPath){
    emit debugMessage(
      tr("Create directory %1")
      .arg(mLibDirDestinationPath)
    );
    FileCopier::createDirectory(mLibDirDestinationPath);
  }
}

void DeployApplication::installExecutable(const DeployApplicationRequest & request)
{
  assert( !mBinDirDestinationPath.isEmpty() );
  assert( !request.targetFilePath.isEmpty() );

  FileCopier fileCopier;
  fileCopier.setOverwriteBehavior(OverwriteBehavior::Overwrite);
  connect(&fileCopier, &FileCopier::verboseMessage, this, &DeployApplication::verboseMessage);

  fileCopier.copyFile(request.targetFilePath, mBinDirDestinationPath);
  assert( fileCopier.copiedFilesDestinationPathList().size() == 1 );

  if( mPlatform.supportsRPath() ){
    setRPathToInstalledExecutable(fileCopier.copiedFilesDestinationPathList().at(0), request);
  }
}

void DeployApplication::setRPathToInstalledExecutable(const QString & executableFilePath, const DeployApplicationRequest & request)
{
  assert( mPlatform.supportsRPath() );

  RPath rpath;
  if(!request.removeRpath){
    rpath.appendPath( QLatin1String("../lib") );
  }

  ExecutableFileWriter writer;
  connect(&writer, &ExecutableFileWriter::message, this, &DeployApplication::verboseMessage);
  connect(&writer, &ExecutableFileWriter::verboseMessage, this, &DeployApplication::verboseMessage);

  writer.openFile(executableFilePath, mPlatform);
  if(writer.getRunPath() != rpath){
    const QString msg = tr("update rpath for %1").arg(executableFilePath);
    emit verboseMessage(msg);
    writer.setRunPath(rpath);
  }
  writer.close();
}

    OverwriteBehavior overwriteBehavior = OverwriteBehavior::Fail;
    bool removeRpath = false;
    QStringList searchPrefixPathList;
    CompilerLocationType compilerLocationType = CompilerLocationType::Undefined;
    QString compilerLocationValue;
    QString targetFilePath;
    QString destinationDirectoryPath;

void DeployApplication::copySharedLibrariesTargetDependsOn(const DeployApplicationRequest & request)
{
  assert( !mLibDirDestinationPath.isEmpty() );

  CopySharedLibrariesTargetDependsOnRequest csltdoRequest;
  csltdoRequest.overwriteBehavior = request.shLibOverwriteBehavior;
  csltdoRequest.removeRpath = request.removeRpath;
  csltdoRequest.searchPrefixPathList = request.searchPrefixPathList;
  csltdoRequest.compilerLocation = request.compilerLocation;
  // Take the given target, it has the RPath informations
  csltdoRequest.targetFilePath = request.targetFilePath;
  csltdoRequest.destinationDirectoryPath = mLibDirDestinationPath;

  CopySharedLibrariesTargetDependsOn csltdo;
  connect(&csltdo, &CopySharedLibrariesTargetDependsOn::statusMessage, this, &DeployApplication::statusMessage);
  connect(&csltdo, &CopySharedLibrariesTargetDependsOn::verboseMessage, this, &DeployApplication::verboseMessage);
  connect(&csltdo, &CopySharedLibrariesTargetDependsOn::debugMessage, this, &DeployApplication::debugMessage);

  csltdo.execute(csltdoRequest);
}

QString DeployApplication::osName(OperatingSystem os) noexcept
{
  assert(os != OperatingSystem::Unknown);

  switch(os){
    case OperatingSystem::Linux:
      return QLatin1String("Linux");
    case OperatingSystem::Windows:
      return QLatin1String("Windows");
    case OperatingSystem::Unknown:
      break;
  }

  return QString();
}

}} // namespace Mdt{ namespace DeployUtils{

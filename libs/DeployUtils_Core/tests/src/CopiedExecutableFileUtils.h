// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2023-2023 Philippe Steinmann.
 **
 ****************************************************************************/
#ifndef COPIED_EXECUTABLE_FILE_UTILS_H
#define COPIED_EXECUTABLE_FILE_UTILS_H

#include "RPathUtils.h"
#include "Mdt/DeployUtils/CopiedExecutableFile.h"
#include "Mdt/DeployUtils/ExecutableFileToInstall.h"
#include "Mdt/DeployUtils/FileCopierFile.h"
#include "Mdt/DeployUtils/RPath.h"
#include <QFileInfo>
#include <QLatin1String>
#include <QString>
#include <string>
#include <vector>

Mdt::DeployUtils::CopiedExecutableFile
makeCopiedExecutableFileFromSourceAndDestinationPathAndRPath(const std::string & sourcePath,
                                                             const std::string & destinationPath,
                                                             const Mdt::DeployUtils::RPath & rpath) noexcept
{
  using Mdt::DeployUtils::FileCopierFile;
  using Mdt::DeployUtils::ExecutableFileToInstall;
  using Mdt::DeployUtils::CopiedExecutableFile;

  const QFileInfo soureFi( QString::fromStdString(sourcePath) );

  FileCopierFile copierFile;
  copierFile.setSourceFileInfo(soureFi);
  copierFile.setDestinationFileInfo( QString::fromStdString(destinationPath) );
  copierFile.setAsBeenCopied();

  const auto fileToInstall = ExecutableFileToInstall::fromFilePathAndRPath(soureFi, rpath);

  return CopiedExecutableFile::fromCopierFileAndFileToInstall(copierFile, fileToInstall);
}

Mdt::DeployUtils::CopiedExecutableFile
makeCopiedExecutableFileFromSourceAndDestinationPathAndRPath(const std::string & sourcePath,
                                                             const std::string & destinationPath,
                                                             const std::vector<std::string> & rpathPathList) noexcept
{
  using Mdt::DeployUtils::RPath;

  const RPath rpath = makeRPathFromPathList(rpathPathList);

  return makeCopiedExecutableFileFromSourceAndDestinationPathAndRPath(sourcePath, destinationPath, rpath);
}

Mdt::DeployUtils::CopiedExecutableFile
makeCopiedExecutableFileFromDestinationPathAndRPath(const std::string & path, const Mdt::DeployUtils::RPath & rpath) noexcept
{
  using Mdt::DeployUtils::FileCopierFile;
  using Mdt::DeployUtils::ExecutableFileToInstall;
  using Mdt::DeployUtils::RPath;
  using Mdt::DeployUtils::CopiedExecutableFile;

  FileCopierFile copierFile;
  copierFile.setDestinationFileInfo( QString::fromStdString(path) );
  copierFile.setAsBeenCopied();

  const QFileInfo sourcePath( QLatin1String("/build/someFile") );
  const auto fileToInstall = ExecutableFileToInstall::fromFilePathAndRPath(sourcePath, rpath);

  return CopiedExecutableFile::fromCopierFileAndFileToInstall(copierFile, fileToInstall);
}

Mdt::DeployUtils::CopiedExecutableFile
makeCopiedExecutableFileFromDestinationPathAndRPath(const std::string & path, const std::vector<std::string> & rpathPathList) noexcept
{
  using Mdt::DeployUtils::RPath;

  const RPath rpath = makeRPathFromPathList(rpathPathList);

  return makeCopiedExecutableFileFromDestinationPathAndRPath(path, rpath);
}


#endif // #ifndef COPIED_EXECUTABLE_FILE_UTILS_H

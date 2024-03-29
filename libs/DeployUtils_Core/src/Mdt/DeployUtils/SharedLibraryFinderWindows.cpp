/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2021-2023 Philippe Steinmann.
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
#include "SharedLibraryFinderWindows.h"
#include "SearchPathList.h"
#include "Mdt/DeployUtils/Impl/LibraryExcludeListWindows.h"
#include <QLatin1String>
#include <QStringList>
#include <QDir>
#include <algorithm>

namespace Mdt{ namespace DeployUtils{

SharedLibraryFinderWindows::SharedLibraryFinderWindows(const std::shared_ptr<const AbstractIsExistingValidSharedLibrary> & isExistingValidShLibOp,
                                                       std::shared_ptr<QtDistributionDirectory> & qtDistributionDirectory,
                                                       QObject *parent)
 : SharedLibraryFinderCommon(isExistingValidShLibOp, qtDistributionDirectory, parent)
{
}

void SharedLibraryFinderWindows::setExcludeMsvcLibraries(bool exclude) noexcept
{
  mExcludeMsvcLibraries = exclude;
}

void SharedLibraryFinderWindows::setExcludeWindowsApiSets(bool exclude) noexcept
{
  mExcludeWindowsApiSets = exclude;
}

void SharedLibraryFinderWindows::buildSearchPathList(const QFileInfo & binaryFilePath,
                                                     const PathList & searchFirstPathPrefixList,
                                                     ProcessorISA processorISA,
                                                     const std::shared_ptr<CompilerFinder> & compilerFinder) noexcept
{
  assert( !binaryFilePath.filePath().isEmpty() ); // see doc of QFileInfo::absoluteFilePath()

  PathList searchPathList;

  if( hasCompilerInstallDir(compilerFinder) ){
    /*
     * The main use-case to find specific redist directories is MSVC,
     * which can be several different directories.
     * (To see more about it, see MsvcFinder.cpp).
     *
     * Try to deduce if a executable was some sort of Debug build is not possible.
     * For example, MSVC, by default (when used with CMake),
     * will add some debug symbols for Release builds anyway.
     *
     * See also: https://stackoverflow.com/questions/11115832/how-to-check-if-an-executable-or-dll-is-build-in-release-or-debug-mode-c
     *
     * Anyway, DLL's like Qt and also MSVC runtimes
     * are suffixed with a d (or D),
     * so we better add both build types directores here in the search paths.
     */
    searchPathList.appendPathList( compilerFinder->findRedistDirectories(processorISA, BuildType::Release) );
    searchPathList.appendPathList( compilerFinder->findRedistDirectories(processorISA, BuildType::Debug) );
  }

  SearchPathList searchFirstPathList;
  searchFirstPathList.setIncludePathPrefixes(true);
  searchFirstPathList.setPathSuffixList({QLatin1String("bin"),QLatin1String("qt5/bin")});
  searchFirstPathList.appendPath( binaryFilePath.absoluteDir().path() );
  /// \todo this could pick some dll's from a other package !
  searchFirstPathList.setPathPrefixList(searchFirstPathPrefixList);

  searchPathList.appendPathList( searchFirstPathList.pathList() );

  /*
   * One possible way to build on Windows is to prepare a shell
   * with a environment that has several dependencies in the PATH.
   *
   * For cross-compilation, for example
   * building a Windows application on a Linux machine,
   * getting the Linux PATH has no sense.
   * The user will probably have to give some prefixes anyway
   * (like for example CMAKE_PREFIX_PATH),
   * which will then be in searchFirstPathPrefixList
   */
  if(Platform::nativeOperatingSystem() == OperatingSystem::Windows){
    SearchPathList pathSearchPathList;
    pathSearchPathList.setIncludePathPrefixes(true);
    pathSearchPathList.setPathSuffixList({QLatin1String("bin"),QLatin1String("qt5/bin")});
    /// \todo is this correct ?
    pathSearchPathList.setPathPrefixList( PathList::getSystemExecutablePathList() );
    searchPathList.appendPathList( pathSearchPathList.pathList() );
  }

  searchPathList.removeNonExistingDirectories();

  setSearchPathList(searchPathList);
}

BinaryDependenciesFile SharedLibraryFinderWindows::findLibraryAbsolutePathByAlternateNames(const QFileInfo & libraryFile)
{
  assert( !libraryFile.filePath().isEmpty() ); // see doc of QFileInfo::absoluteFilePath()
  assert( libraryFile.isAbsolute() );

  emit debugMessage(
    tr("  try %1").arg( libraryFile.absoluteFilePath() )
  );
  if( validateIsExistingValidSharedLibrary(libraryFile) ){
    return BinaryDependenciesFile::fromQFileInfo(libraryFile);
  }

  QFileInfo alternativeFile = libraryFile;
  const QDir directory = libraryFile.absoluteDir();

  alternativeFile.setFile( directory, libraryFile.fileName().toLower() );
  emit debugMessage(
    tr("  try %1").arg( alternativeFile.absoluteFilePath() )
  );
  if( validateIsExistingValidSharedLibrary(alternativeFile) ){
    return BinaryDependenciesFile::fromQFileInfo(alternativeFile);
  }

  alternativeFile.setFile( directory, libraryFile.fileName().toUpper() );
  emit debugMessage(
    tr("  try %1").arg( alternativeFile.absoluteFilePath() )
  );
  if( validateIsExistingValidSharedLibrary(alternativeFile) ){
    return BinaryDependenciesFile::fromQFileInfo(alternativeFile);
  }

  return BinaryDependenciesFile();
}

bool SharedLibraryFinderWindows::libraryHasToBeExcluded(const QString & library) const noexcept
{
  if( hasToExcludeMsvcLibraries() && isMsvcLibrary(library) ){
    return true;
  }
  if( hasToExcludeWindowsApiSets() && isWindowsApiSet(library) ){
    return true;
  }
  if( isDxgiLibrary(library) ){
    return true;
  }
  if( isDirect3D_11_Library(library) ){
    return true;
  }

  return false;
}

bool SharedLibraryFinderWindows::isMsvcLibrary(const QString & library) noexcept
{
  assert( !library.trimmed().isEmpty() );

  static const QStringList msvcLibrariesBaseNames{
    QLatin1String("concrt"),
    QLatin1String("msvcp"),
    QLatin1String("vccorlib"),
    QLatin1String("vcruntime"),
    QLatin1String("vcamp"),
    QLatin1String("vcomp")
  };

  const auto pred = [&library](const QString & msvcLibraryBaseName){
    return library.startsWith(msvcLibraryBaseName, Qt::CaseInsensitive);
  };

  const auto it = std::find_if(msvcLibrariesBaseNames.cbegin(), msvcLibrariesBaseNames.cend(), pred);

  return it != msvcLibrariesBaseNames.cend();
}

bool SharedLibraryFinderWindows::isWindowsApiSet(const QString & library) noexcept
{
  assert( !library.trimmed().isEmpty() );

  if( library.startsWith(QLatin1String("api-"), Qt::CaseInsensitive) ){
    return true;
  }
  if( library.startsWith(QLatin1String("ext-"), Qt::CaseInsensitive) ){
    return true;
  }

  return false;
}

bool SharedLibraryFinderWindows::isDxgiLibrary(const QString & library) noexcept
{
  assert( !library.trimmed().isEmpty() );

  static const QStringList dxgiLibraries{
    QLatin1String("dxgi.dll")
  };

  return dxgiLibraries.contains(library, Qt::CaseInsensitive);
}

bool SharedLibraryFinderWindows::isDirect3D_11_Library(const QString & library) noexcept
{
  assert( !library.trimmed().isEmpty() );

  static const QStringList direct3dLibraries{
    QLatin1String("d3d11.dll"),
    QLatin1String("D3DSCache.dll")
  };

  return direct3dLibraries.contains(library, Qt::CaseInsensitive);
}

bool SharedLibraryFinderWindows::doLibraryShouldBeDistributed(const QString & libraryName) const noexcept
{
  if( libraryHasToBeExcluded(libraryName) ){
    return false;
  }
  if( libraryIsInExcludeList(libraryName) ){
    return false;
  }

  return true;
}

bool SharedLibraryFinderWindows::libraryIsInExcludeList(const QString & libraryName) noexcept
{
  return libraryExcludelistWindows.contains(libraryName, Qt::CaseInsensitive);
}

QFileInfo SharedLibraryFinderWindows::doFindLibraryAbsolutePath(const QString & libraryName, const BinaryDependenciesFile & /*dependentFile*/)
{
  assert( !libraryName.trimmed().isEmpty() );

  /** \todo See if we keep using BinaryDependenciesFile or not here
   *
   * Seems better than a naked QFileInfo
   * \sa https://gitlab.com/scandyna/mdtdeployutils/-/issues/5 (BinaryDependenciesFile has to much attributes and is confusing)
   */

  emit verboseMessage(
    tr(" searching %1").arg(libraryName)
  );

  for( const QString & directory : searchPathList() ){
    QFileInfo libraryFile(directory, libraryName);
    const BinaryDependenciesFile library = findLibraryAbsolutePathByAlternateNames(libraryFile);
    if( !library.isNull() ){
      return QFileInfo( library.fileInfo() );
    }
  }

  const QString message = tr("could not find the absolute path for %1")
                          .arg(libraryName);
  throw FindDependencyError(message);
}

bool SharedLibraryFinderWindows::hasCompilerInstallDir(const std::shared_ptr<CompilerFinder> & compilerFinder) noexcept
{
  if( compilerFinder.get() == nullptr ){
    return false;
  }
  return compilerFinder->hasInstallDir();
}

}} // namespace Mdt{ namespace DeployUtils{

/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2021-2022 Philippe Steinmann.
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
#include <QDir>

namespace Mdt{ namespace DeployUtils{

SharedLibraryFinderWindows::SharedLibraryFinderWindows(QObject *parent)
 : QObject(parent)
{
}

PathList SharedLibraryFinderWindows::buildSearchPathList(const QFileInfo & binaryFilePath,
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

  return searchPathList;
}

void SharedLibraryFinderWindows::removeLibrariesInExcludeList(BinaryDependenciesFile & file) noexcept
{
  const auto pred = [](const QString & libraryName){
    return libraryExcludelistWindows.contains(libraryName, Qt::CaseInsensitive);
  };

  file.removeDependenciesFileNames(pred);
}

bool SharedLibraryFinderWindows::hasCompilerInstallDir(const std::shared_ptr<CompilerFinder> & compilerFinder) noexcept
{
  if( compilerFinder.get() == nullptr ){
    return false;
  }
  return compilerFinder->hasInstallDir();
}

}} // namespace Mdt{ namespace DeployUtils{

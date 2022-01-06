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
#ifndef MDT_DEPLOY_UTILS_SHARED_LIBRARY_FINDER_WINDOWS_H
#define MDT_DEPLOY_UTILS_SHARED_LIBRARY_FINDER_WINDOWS_H

#include "BinaryDependenciesFile.h"
#include "FindDependencyError.h"
#include "PathList.h"
#include "ProcessorISA.h"
#include "CompilerFinder.h"
#include "mdt_deployutilscore_export.h"
#include <QObject>
#include <QFileInfo>
#include <QDir>
#include <memory>
#include <cassert>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Helper to find a shared library on Windows
   *
   * \todo see https://bugreports.qt.io/browse/QTBUG-56566?jql=project%20%3D%20QTBUG%20AND%20text%20~%20%22windeployqt%22
   * \todo see https://bugreports.qt.io/browse/QTBUG-87677?jql=project%20%3D%20QTBUG%20AND%20text%20~%20%22windeployqt%22
   */
  class MDT_DEPLOYUTILSCORE_EXPORT SharedLibraryFinderWindows : public QObject
  {
    Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit SharedLibraryFinderWindows(QObject *parent = nullptr);

    /*! \brief Build and returns a list of path to directories where to find shared libraries
     *
     * \note \a compilerFinder has not be a valid.
     *  If it is valid (valid pointer and its install dir is set),
     *  it will be used to locate the redist directories
     *  (mostly used for MSVC).
     *
     * Building C++ code can require having some tools in the PATH:
     * - Using CMake with the "MinGW Makefiles" generator requires MinGW in the PATH.
     * - Using CMake with a "Visual Studio 1x 20xy" generator seems not to require something special in the PATH.
     *
     * \sa https://cmake.org/cmake/help/latest/generator/MinGW%20Makefiles.html
     * \sa https://cmake.org/cmake/help/latest/generator/Visual%20Studio%2015%202017.html
     * \sa The \ref libs_DeployUtils page
     *
     * Above we can see that some build processes require having some additional entries in the PATH.
     *
     * When using CMake, dependencies are also searched in CMAKE_PREFIX_PATH.
     *
     * Searching dependencies in the PATH should be done at last,
     * because we can end-up picking the wrong dll's.
     * For example, if we search VCRUNTIME140.dll with this PATH:
     * \code
     * C:/Qt/5.14.2/msvc2017_64/bin
     * C:/tools/python3.8/
     * \endcode
     * we could find it in the python3.8 installation.
     *
     * \sa https://gitlab.com/scandyna/mdtdeployutils/-/jobs/1937042537
     *
     * In above case, we have to make shure that the MSVC VC redist is in the PATH,
     * before anything else.
     *
     * \todo automate using compiler finder when MSVC, only on CMAKE ?
     *
     * \sa https://docs.microsoft.com/en-us/windows/win32/dlls/dynamic-link-library-search-order
     * \sa PathList::getSystemLibraryKnownPathListWindows()
     */
    static
    PathList buildSearchPathList(const QFileInfo & binaryFilePath,
                                 const PathList & searchFirstPathPrefixList,
                                 ProcessorISA processorISA,
                                 const std::shared_ptr<CompilerFinder> & compilerFinder) noexcept;

    /*! \brief Find the absolute path for given \a libraryFile for various possible names
     *
     * On Windows, file names are not case sensitive.
     * The required libraries can be given upper-case in a Pe file,
     * but it will match the lower-case one.
     * For example, MSVCRT.DLL can be in C:/path/msvcrt.dll
     *
     * If the search is done on a Windows machine, this is not a issue.
     * But, if we search dependencies for a Windows binary
     * on a non Windows machine, this must be taken in to account.
     *
     * \pre \a libraryFile must have a absolute path
     */
    template<typename IsExistingSharedLibraryOp>
    static
    BinaryDependenciesFile findLibraryAbsolutePathByAlternateNames(const QFileInfo & libraryFile,
                                                                   IsExistingSharedLibraryOp & isExistingSharedLibraryOp)
    {
      assert( !libraryFile.filePath().isEmpty() ); // see doc of QFileInfo::absoluteFilePath()
      assert( libraryFile.isAbsolute() );

      if( isExistingSharedLibraryOp(libraryFile) ){
        return BinaryDependenciesFile::fromQFileInfo(libraryFile);
      }

      QFileInfo alternativeFile = libraryFile;
      const QDir directory = libraryFile.absoluteDir();

      alternativeFile.setFile( directory, libraryFile.fileName().toLower() );
      if( isExistingSharedLibraryOp(alternativeFile) ){
        return BinaryDependenciesFile::fromQFileInfo(alternativeFile);
      }

      alternativeFile.setFile( directory, libraryFile.fileName().toUpper() );
      if( isExistingSharedLibraryOp(alternativeFile) ){
        return BinaryDependenciesFile::fromQFileInfo(alternativeFile);
      }

      return BinaryDependenciesFile();
    }

    /*! \brief Find the absolute path for given \a libraryName
     *
     * \pre \a libraryName must not be empty
     * \exception FindDependencyError
     * \sa https://docs.microsoft.com/en-us/windows/win32/dlls/dynamic-link-library-search-order
     *
     * \todo case of \a libraryName containing a full path is not implemented
     */
    template<typename IsExistingSharedLibraryOp>
    static
    BinaryDependenciesFile findLibraryAbsolutePath(const QString & libraryName,
                                                   const PathList & searchPathList,
                                                   IsExistingSharedLibraryOp & isExistingSharedLibraryOp)
    {
//       assert( !originFile.isNull() );
      assert( !libraryName.trimmed().isEmpty() );

      for(const QString & directory : searchPathList){
        QFileInfo libraryFile(directory, libraryName);
        const BinaryDependenciesFile library = findLibraryAbsolutePathByAlternateNames(libraryFile, isExistingSharedLibraryOp);
        if( !library.isNull() ){
          return library;
        }
      }

      const QString message = tr("could not find the absolute path for %1")
                              .arg(libraryName);
      throw FindDependencyError(message);
    }

    /*! \brief Find the absolute path for each direct dependency of \a file
     */
    template<typename IsExistingSharedLibraryOp>
    static
    BinaryDependenciesFileList findLibrariesAbsolutePath(BinaryDependenciesFile & file,
                                                         const PathList & searchPathList,
                                                         IsExistingSharedLibraryOp & isExistingSharedLibraryOp)
    {
      BinaryDependenciesFileList libraries;

      removeLibrariesInExcludeList(file);

      for( const QString & libraryName : file.dependenciesFileNames() ){
        libraries.push_back( findLibraryAbsolutePath(libraryName, searchPathList, isExistingSharedLibraryOp) );
      }

      return libraries;
    }

   private:

    static
    void removeLibrariesInExcludeList(BinaryDependenciesFile & file) noexcept;

    static
    bool hasCompilerInstallDir(const std::shared_ptr<CompilerFinder> & compilerFinder) noexcept;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_SHARED_LIBRARY_FINDER_WINDOWS_H

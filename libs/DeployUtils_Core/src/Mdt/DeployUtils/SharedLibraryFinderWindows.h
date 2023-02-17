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
#ifndef MDT_DEPLOY_UTILS_SHARED_LIBRARY_FINDER_WINDOWS_H
#define MDT_DEPLOY_UTILS_SHARED_LIBRARY_FINDER_WINDOWS_H

#include "SharedLibraryFinderCommon.h"
#include "BinaryDependenciesFile.h"
#include "FindDependencyError.h"
#include "PathList.h"
#include "ProcessorISA.h"
#include "CompilerFinder.h"
#include "mdt_deployutilscore_export.h"
#include <QString>
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
  class MDT_DEPLOYUTILSCORE_EXPORT SharedLibraryFinderWindows : public SharedLibraryFinderCommon
  {
    Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit SharedLibraryFinderWindows(const std::shared_ptr<const AbstractIsExistingValidSharedLibrary> & isExistingValidShLibOp,
                                        std::shared_ptr<QtDistributionDirectory> & qtDistributionDirectory,
                                        QObject *parent = nullptr);

    /*! \brief Build a list of path to directories where to find shared libraries
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
    void buildSearchPathList(const QFileInfo & binaryFilePath,
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
    BinaryDependenciesFile findLibraryAbsolutePathByAlternateNames(const QFileInfo & libraryFile);

    /*! \brief If \a exclude is true, dependencies that are part of MSVC runtime are excluded
     *
     * By default, dependencies that are part of the MSVC runtime
     * will be part of the dependencies.
     *
     * Microsoft recommends using central deployment of MSVC runtime.
     * This means the user should install the redistributable package itself.
     * The advantage of this, is that the libraries will be part of Windows updates.
     *
     * \sa setExcludeWindowsApiSets()
     * \sa https://docs.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist
     * \sa https://docs.microsoft.com/en-us/cpp/windows/universal-crt-deployment
     * \sa https://docs.microsoft.com/en-us/cpp/windows/determining-which-dlls-to-redistribute
     */
    void setExcludeMsvcLibraries(bool exclude) noexcept;

    /*! \brief If \a exclude is true,dependencies that are part of Windows API sets are excluded
     *
     * Some (pseudo-) libraries, starting for example with api-xx ,
     * have not to be deployed on Windows 10 or later.
     *
     * Be default, Windows API sets are excluded.
     *
     * \note this property is ignored if MSVC libraries are excluded.
     *
     * \sa setExcludeMsvcLibraries()
     * \sa https://stackoverflow.com/questions/38125376/no-api-ms-win-crt-runtime-l1-1-0-dll-on-windows-10-after-visual-c-2015-redistr
     * \sa https://docs.microsoft.com/en-us/windows/win32/apiindex/windows-apisets
     */
    void setExcludeWindowsApiSets(bool exclude) noexcept;

    /*! \brief Check if MSVC libraries have to be excluded.
     *
     * \sa setExcludeMsvcLibraries()
     */
    bool hasToExcludeMsvcLibraries() const noexcept
    {
      return mExcludeMsvcLibraries;
    }

    /*! \brief Check if Windows API sets have to be excluded
     *
     * \sa setExcludeWindowsApiSets()
     */
    bool hasToExcludeWindowsApiSets() const noexcept
    {
      if( hasToExcludeMsvcLibraries() ){
        return true;
      }

      return mExcludeWindowsApiSets;
    }

    /*! \brief Check if \a library has to be excluded
     */
    bool libraryHasToBeExcluded(const QString & library) const noexcept;

    /*! \brief Check if \a library is a MSVC library
     *
     * \pre \a library must not be empty
     */
    static
    bool isMsvcLibrary(const QString & library) noexcept;

    /*! \brief Check if \a library is a Windows API set
     *
     * \pre \a library must not be empty
     * \sa https://docs.microsoft.com/en-us/windows/win32/apiindex/windows-apisets
     */
    static
    bool isWindowsApiSet(const QString & library) noexcept;

    /*! \brief Check if \a library is a DirectX Graphics Infrastructure (DXGI) library
     *
     * DXGI seems to be distributed with DirectX 11 .
     *
     * \pre \a library must not be empty
     * \sa isDirect3D_11_Library()
     * \sa https://docs.microsoft.com/en-us/windows/win32/direct3darticles/dxgi-best-practices
     * \sa https://docs.microsoft.com/en-us/windows/win32/direct3darticles/direct3d11-deployment
     */
    static
    bool isDxgiLibrary(const QString & library) noexcept;

    /*! \brief Check if \a library is a Direct3D 11 library
     *
     * Direct3D 11 API libraries should be built in the OS
     * starting from Windows 7 and Windows Server 2008 R2 .
     *
     * \pre \a library must not be empty
     * \sa isDxgiLibrary()
     * \sa see https://docs.microsoft.com/en-us/windows/win32/direct3darticles/direct3d11-deployment
     * \sa https://gitlab.com/scandyna/mdtdeployutils/-/jobs/2044253463
     */
    static
    bool isDirect3D_11_Library(const QString & library) noexcept;

   private:

    OperatingSystem doOperatingSystem() const noexcept override
    {
      return OperatingSystem::Windows;
    }

    /*! \brief Check if given \a libraryName should be distributed
     */
    bool doLibraryShouldBeDistributed(const QString & libraryName) const noexcept override;

    static
    bool libraryIsInExcludeList(const QString & libraryName) noexcept;

    /*! \brief Find the absolute path for given \a libraryName
     *
     * \pre \a libraryName must not be empty
     * \exception FindDependencyError
     * \sa https://docs.microsoft.com/en-us/windows/win32/dlls/dynamic-link-library-search-order
     *
     * \todo case of \a libraryName containing a full path is not implemented
     */
    QFileInfo doFindLibraryAbsolutePath(const QString & libraryName, const BinaryDependenciesFile & dependentFile) override;

    static
    bool hasCompilerInstallDir(const std::shared_ptr<CompilerFinder> & compilerFinder) noexcept;

    bool mExcludeMsvcLibraries = false;
    bool mExcludeWindowsApiSets = true;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_SHARED_LIBRARY_FINDER_WINDOWS_H

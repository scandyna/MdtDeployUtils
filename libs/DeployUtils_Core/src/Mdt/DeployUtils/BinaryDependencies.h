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
#ifndef MDT_DEPLOY_UTILS_BINARY_DEPENDENCIES_H
#define MDT_DEPLOY_UTILS_BINARY_DEPENDENCIES_H

#include "FindDependencyError.h"
#include "PathList.h"
#include "ProcessorISA.h"
#include "CompilerFinder.h"
#include "BuildType.h"
#include "mdt_deployutils_export.h"
#include <QObject>
#include <QFileInfo>
#include <QString>
#include <QStringList>
#include <memory>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Find dependencies for a executable or a library
   *
   * \todo See various todo in CPP file
   */
  class MDT_DEPLOYUTILS_EXPORT BinaryDependencies : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    BinaryDependencies(QObject* parent = nullptr);

    /*! \brief Set the compiler finder
     *
     * \pre \a compilerFinder must be a valid pointer
     * \pre \a compilerFinder must have its install dir set
     */
    void setCompilerFinder(const std::shared_ptr<CompilerFinder> & compilerFinder) noexcept;

    /*! \brief Find dependencies for a executable or a shared library
     *
     * At first, the target platform will be determined by \a binaryFilePath .
     *
     * For target platforms that support RPATH, they will be used for libraries that have them set.
     * Then, depending on target platform, dependencies will be searched in the directory of \a binaryFilePath .
     * Then, dependencies will be searched, for each path in \a searchFirstPathPrefixList ,
     *  in known subdirectories (like lib, bin, qt5/lib, qt5/bin).
     * Finally, depending on target platform, dependencies can also be serached in
     *  in system paths.
     *
     * Some libraries, known not have to be distributed,
     * will not be part of the result.
     *
     * Each library returned in the result will have its absolute file path.
     *
     * \pre \a binaryFilePath must have its path set
     * \exception FindDependencyError
     * \sa buildSearchPathListLinux()
     * \sa https://man7.org/linux/man-pages/man8/ld.so.8.html
     */
    QStringList findDependencies(const QFileInfo & binaryFilePath, const PathList & searchFirstPathPrefixList);

    /*! \brief Build and returns a list of path to directories where to find shared libraries
     *
     * \sa PathList::getSystemLibraryKnownPathListLinux()
     * \sa https://man7.org/linux/man-pages/man8/ld.so.8.html
     */
    PathList buildSearchPathListLinux(const PathList & searchFirstPathPrefixList, ProcessorISA processorISA) const noexcept;

    /*! \brief Build and returns a list of path to directories where to find shared libraries
     *
     * \sa PathList::getSystemLibraryKnownPathListWindows()
     */
    PathList buildSearchPathListWindows(const QFileInfo & binaryFilePath, const PathList & searchFirstPathPrefixList,
                                        ProcessorISA processorISA, BuildType buildType) const noexcept;

   signals:

    void message(const QString & message) const;
    void verboseMessage(const QString & message) const;

   private:

    static
    bool checkIfIsWindowsDebugBuild(bool targetHasDebugSymbols, const QStringList & directDependentDllNames) noexcept;

    void emitSearchPathListMessage(const PathList & pathList) const;
    void emitBuildTypeMessage(BuildType buildType) const;
    bool hasCompilerInstallDir() const noexcept;

    std::shared_ptr<CompilerFinder> mCompilerFinder;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_BINARY_DEPENDENCIES_H

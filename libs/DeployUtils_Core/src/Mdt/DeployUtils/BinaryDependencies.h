/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2015-2022 Philippe Steinmann.
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
#include "Platform.h"
#include "mdt_deployutilscore_export.h"
#include <QObject>
#include <QFileInfo>
#include <QFileInfoList>
#include <QString>
#include <QStringList>
#include <memory>

namespace Mdt{ namespace DeployUtils{

  namespace Impl{

    class FindDependenciesImpl;

  } // namespace Impl{

  class QtDistributionDirectory;

  /*! \brief Find dependencies for a executable or a library
   */
  class MDT_DEPLOYUTILSCORE_EXPORT BinaryDependencies : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    BinaryDependencies(QObject* parent = nullptr);

    /*! \brief Destructor
     */
    ~BinaryDependencies() noexcept;

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
     * \pre \a binaryFilePath must have its absolute path set
     * \exception FindDependencyError
     *
     * \sa SharedLibraryFinderLinux
     * \sa SharedLibraryFinderWindows
     */
    QStringList findDependencies(const QFileInfo & binaryFilePath,
                                 const PathList & searchFirstPathPrefixList,
                                 std::shared_ptr<QtDistributionDirectory> & qtDistributionDirectory);

    /*! \brief Find dependencies for a list executables or a shared libraries
     *
     * \pre \a binaryFilePathList must not be empty
     * \pre each element in \a binaryFilePathList must have its absolute path set
     *
     * \sa findDependencies(const QFileInfo &, const PathList &)
     */
    QStringList findDependencies(const QFileInfoList & binaryFilePathList,
                                 const PathList & searchFirstPathPrefixList,
                                 std::shared_ptr<QtDistributionDirectory> & qtDistributionDirectory);

   signals:

    void message(const QString & message) const;
    void verboseMessage(const QString & message) const;
    void debugMessage(const QString & message) const;

   private:

    void emitSearchPathListMessage(const PathList & pathList) const;

    std::unique_ptr<Impl::FindDependenciesImpl> mImpl;
    std::shared_ptr<CompilerFinder> mCompilerFinder;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_BINARY_DEPENDENCIES_H

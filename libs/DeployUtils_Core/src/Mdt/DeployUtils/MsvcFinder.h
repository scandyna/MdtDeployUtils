/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2021-2021 Philippe Steinmann.
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
#ifndef MDT_DEPLOY_UTILS_MSVC_FINDER_H
#define MDT_DEPLOY_UTILS_MSVC_FINDER_H

#include "AbstractCompilerFinderEngine.h"
#include "FindCompilerError.h"
#include "MsvcVersion.h"
#include "mdt_deployutils_export.h"
#include <QString>
#include <QDir>
#include <QFileInfo>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Helper to find MSVC installation
   *
   * \todo Microsoft recommends not redistribute dll's, but vc_redistxYY.exe
   * \sa https://docs.microsoft.com/en-us/cpp/windows/determining-which-dlls-to-redistribute?view=msvc-160
   *
   * \todo see various todo in cpp file
   */
  class MDT_DEPLOYUTILS_EXPORT MsvcFinder : public AbstractCompilerFinderEngine
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit MsvcFinder(QObject* parent = nullptr);

    /*! \brief Find the root of MSVC for given \a version
     *
     * For example, for Visual Studio 2017,
     * the result could be
     * \code
     * C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools
     * \endcode
     *
     * \pre \a version must be valid
     *
     * \todo remove ?
     */
    QString findMsvcRoot(const MsvcVersion & version);

   private:

    Compiler doCompiler() const noexcept override
    {
      return Compiler::Msvc;
    }

    bool doIsSupportedCompiler(const QFileInfo & executablePath) const noexcept override;
    void doFindFromCxxCompilerPath(const QFileInfo & executablePath) override;

    void doSetInstallDir(const QString & path) override
    {
      mVcInstallDir = path;
    }

    bool doHasInstallDir() const noexcept override
    {
      return !mVcInstallDir.isEmpty();
    }

    QString doInstallDir() const noexcept override
    {
      return mVcInstallDir;
    }

    QString doFindRedistDirectory(ProcessorISA cpu, BuildType buildType) const override;

    static
    bool isDirectoryContainingDebugNonRedist(const QFileInfo & fi) noexcept;

    /** \todo Think that sorting is lexicographic, does it work ?
     *   Also, should deploy the version matching MSVC ?
     * \sa https://docs.microsoft.com/en-us/cpp/windows/determining-which-dlls-to-redistribute?view=msvc-160
     */
    static
    QDir findLatestVersionDirContainingDebugNonRedist(const QDir & dir);

    /** \todo Think that sorting is lexicographic, does it work ?
     *   Also, should deploy the version matching MSVC ?
     * \sa https://docs.microsoft.com/en-us/cpp/windows/determining-which-dlls-to-redistribute?view=msvc-160
     */
    static
    QDir findLatestVcCrtDirectory(const QDir & dir, BuildType buildType);

    static
    QString processorISADirectoryName(ProcessorISA cpu) noexcept;

    static
    bool useDebugRedist(BuildType buildType) noexcept;

    QString mVcInstallDir;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_MSVC_FINDER_H

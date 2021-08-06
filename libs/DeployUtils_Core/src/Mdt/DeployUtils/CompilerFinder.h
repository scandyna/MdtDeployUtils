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
#ifndef MDT_DEPLOY_UTILS_COMPILER_FINDER_H
#define MDT_DEPLOY_UTILS_COMPILER_FINDER_H

#include "FindCompilerError.h"
#include "Compiler.h"
#include "ProcessorISA.h"
#include "BuildType.h"
#include "mdt_deployutilscore_export.h"
#include <QObject>
#include <QFileInfo>
#include <QString>
#include <memory>

namespace Mdt{ namespace DeployUtils{

  class AbstractCompilerFinderEngine;

  /*! \brief Helper to find a compiler installation
   *
   * \todo check which method throws exceptions
   */
  class MDT_DEPLOYUTILSCORE_EXPORT CompilerFinder : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit CompilerFinder(QObject* parent = nullptr);

    /*! \brief Destructor
     */
    ~CompilerFinder() noexcept;

    /*! \brief Find a compiler installation from its executable path
     *
     * \pre \a executablePath must be a absolute path
     * \exception FindCompilerError
     */
    void findFromCxxCompilerPath(const QFileInfo & executablePath);

    /*! \brief Set the installation directory
     *
     * \pre \a path must not be empty
     * \exception FindCompilerError
     */
    void setInstallDir(const QString & path, Compiler compiler);

    /*! \brief Returns true if a compiler install dir have been found
     */
    bool hasInstallDir() const noexcept;

    /*! \brief Get the install dir
     *
     * \pre the install dir must be defined
     * \sa hasInstallDir()
     */
    QString installDir() const noexcept;

    /*! \brief Find the path to the redistributable libraries
     *
     * \pre the install dir must be defined
     * \sa hasInstallDir()
     * \pre \a cpu must be defined
     * \exception FindCompilerError
     */
    QString findRedistDirectory(ProcessorISA cpu, BuildType buildType) const;

   private:

    void instanciateMsvcEngine();

    static
    QString compilerName(Compiler compiler);

    std::unique_ptr<AbstractCompilerFinderEngine> mEngine;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_COMPILER_FINDER_H

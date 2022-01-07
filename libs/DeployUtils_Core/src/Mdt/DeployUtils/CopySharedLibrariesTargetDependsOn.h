/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2020-2022 Philippe Steinmann.
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
#ifndef MDT_DEPLOY_UTILS_COPY_SHARED_LIBRARIES_TARGET_DEPENDS_ON_H
#define MDT_DEPLOY_UTILS_COPY_SHARED_LIBRARIES_TARGET_DEPENDS_ON_H

#include "CopySharedLibrariesTargetDependsOnRequest.h"
#include "OverwriteBehavior.h"
#include "Platform.h"
#include "mdt_deployutilscore_export.h"
#include <QObject>
#include <QString>
#include <QStringList>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Copy shared libraries a target depends on to a destination directory
   *
   * Some variables that figure here are taken from CopySharedLibrariesTargetDependsOnRequest .
   *
   * If the source and destination locations for a shared library are the same,
   * it will not be copied, and its rpath informations will not be changed at all,
   * and this regardless of \a removeRpath and \a overwriteBehavior.
   * (The only descent case this happens is a UNIX system wide installation,
   * for a target that depends on shared libraries allready installed system wide).
   *
   * If a shared library allready exists at the destination location,
   * but its not the same as the source (source and destination locations are different),
   * the behavior is defined by \a overwriteBehavior :
   * - If \a overwriteBehavior is OverwriteBehavior::Keep, the destination library will not be changed at all.
   * - If \a overwriteBehavior is OverwriteBehavior::Overwrite, the destination library will replaced.
   * - If \a overwriteBehavior is OverwriteBehavior::Fail, a fatal error is thrown.
   *
   * By default, the \a overwriteBehavior is OverwriteBehavior::Fail.
   *
   * By default, on platform that supports rpath,
   * the rpath informations is set to the relative location of the shared library
   * (i.e. $ORIGIN for ELF files on platforms like Linux) for each one that has been copied.
   * If \a removeRpath is true, the rpath informations are removed for each shared library that has been copied.
   *
   * To find dependencies, \a searchPrefixPathList will be used.
   *
   * \sa BinaryDependencies
   *
   * Some compiler specific shared libraries could be necessary to be copied.
   * Those libraries are probably provided in the installation of the compiler
   * used to build the project.
   *
   * To find those libraries, the locations of the used compiler should be provided.
   * This is done by setting \a compilerLocationType and \a compilerLocationValue :
   * - CompilerLocationType::Undefined: \a compilerLocationValue is not used.
   * - CompilerLocationType::FromEnv: \a compilerLocationValue is the name of a environment variable, like VcInstallDir .
   * - CompilerLocationType::CompilerPath: \a compilerLocationValue is the full path to the compiler. This can be used with CMAKE_CXX_COMPILER .
   *
   * \note This is mostly required for MSVC
   * \sa CompilerFinder
   *
   * \todo See various todo in CPP file
   *
   * \todo Should also require full path to tools, like ldd, objdump, etc..
   * (we should not do magic search here, which can be wrong when a user specify its own version of a specific tool, or cross-compilation)
   */
  class MDT_DEPLOYUTILSCORE_EXPORT CopySharedLibrariesTargetDependsOn : public QObject
  {
   Q_OBJECT

  public:

    /*! \brief Constructor
     */
    explicit CopySharedLibrariesTargetDependsOn(QObject *parent = nullptr)
     : QObject(parent)
    {
    }

    /*! \brief Copy shared libraries a target depends on to a destination directory
     *
     * \pre request's \a targetFilePath must be specified
     * \pre request's \a destinationDirectoryPath must be specified
     * \todo tools must also be specified
     *
     * \exception FindCompilerError
     * \exception FileOpenError
     * \exception ExecutableFileReadError
     * \exception FindDependencyError
     * \exception FileCopyError
     * \exception ExecutableFileWriteError
     */
    void execute(const CopySharedLibrariesTargetDependsOnRequest & request);

   signals:

    void message(const QString & message) const;
    void verboseMessage(const QString & message) const;

   private:

    void emitSearchPrefixPathListMessage(const QStringList & pathList) const noexcept;
    void emitFoundDependenciesMessage(const QStringList & dependencies) const noexcept;

    static
    QString overwriteBehaviorToString(OverwriteBehavior overwriteBehavior) noexcept;

    void setRPathToCopiedDependencies(const QStringList & destinationFilePathList,
                                      const CopySharedLibrariesTargetDependsOnRequest & request,
                                      const Platform & platform);
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_COPY_SHARED_LIBRARIES_TARGET_DEPENDS_ON_H

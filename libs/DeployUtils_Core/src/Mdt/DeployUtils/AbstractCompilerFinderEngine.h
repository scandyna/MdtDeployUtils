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
#ifndef MDT_DEPLOY_UTILS_ABSTRACT_COMPILER_FINDER_ENGINE_H
#define MDT_DEPLOY_UTILS_ABSTRACT_COMPILER_FINDER_ENGINE_H

#include "Compiler.h"
#include "mdt_deployutils_export.h"
#include <QObject>
#include <QFileInfo>
#include <QDir>
#include <cassert>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Interface to implement a engine for CompilerFinder
   */
  class MDT_DEPLOYUTILS_EXPORT AbstractCompilerFinderEngine : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit AbstractCompilerFinderEngine(QObject* parent = nullptr)
     : QObject(parent)
    {
    }

    /*! \brief Returns the compiler the implementation handles
     */
    Compiler compiler() const noexcept
    {
      return doCompiler();
    }

    /*! \brief Check if \a executablePath refers to a compiler this engine supports
     *
     * \pre \a executablePath must be a absolute path
     */
    bool isSupportedCompiler(const QFileInfo & executablePath) const noexcept
    {
      assert( executablePath.isAbsolute() );

      return doIsSupportedCompiler(executablePath);
    }

    /*! \brief Find a compiler installation from its executable path
     *
     * \pre \a executablePath must be a absolute path
     * \pre \a executablePath must be a supported compiler
     * \exception FindCompilerError
     */
    void findFromCxxCompilerPath(const QFileInfo & executablePath)
    {
      assert( executablePath.isAbsolute() );
      assert( isSupportedCompiler(executablePath) );

      doFindFromCxxCompilerPath(executablePath);
    }

    /*! \brief Set the installation directory
     *
     * \pre \a path must not be empty
     */
    void setInstallDir(const QString & path)
    {
      assert( !path.isEmpty() );

      doSetInstallDir(path);
    }

    /*! \brief Returns true if a compiler install dir have been found
     */
    bool hasInstallDir() const noexcept
    {
      return doHasInstallDir();
    }

    /*! \brief Get the install dir
     */
    QString installDir() const noexcept
    {
      return doInstallDir();
    }

   protected:

    /*! \brief Call cdUp() on \a dir for \a level count
     */
    static
    bool cdUp(QDir & dir, int level) noexcept
    {
      for(int i=1; i <= level; ++i){
        if( !dir.cdUp() ){
          return false;
        }
      }
      return true;
    }

   private:

    virtual Compiler doCompiler() const noexcept = 0;
    virtual bool doIsSupportedCompiler(const QFileInfo & executablePath) const noexcept = 0;
    virtual void doFindFromCxxCompilerPath(const QFileInfo & executablePath) = 0;
    virtual void doSetInstallDir(const QString & path) = 0;
    virtual bool doHasInstallDir() const noexcept = 0;
    virtual QString doInstallDir() const noexcept = 0;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_ABSTRACT_COMPILER_FINDER_ENGINE_H

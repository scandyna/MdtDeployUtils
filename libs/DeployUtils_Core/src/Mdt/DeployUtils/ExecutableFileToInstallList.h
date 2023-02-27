// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2023-2023 Philippe Steinmann.
 **
 ****************************************************************************/
#ifndef MDT_DEPLOY_UTILS_EXECUTABLE_FILE_TO_INSTALL_LIST_H
#define MDT_DEPLOY_UTILS_EXECUTABLE_FILE_TO_INSTALL_LIST_H

#include "mdt_deployutilscore_export.h"

namespace Mdt{ namespace DeployUtils{

  /*! \brief
   */
  class MDT_DEPLOYUTILSCORE_EXPORT ExecutableFileToInstallList
  {
   public:

    void addFile() noexcept;

   private:


  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_EXECUTABLE_FILE_TO_INSTALL_LIST_H

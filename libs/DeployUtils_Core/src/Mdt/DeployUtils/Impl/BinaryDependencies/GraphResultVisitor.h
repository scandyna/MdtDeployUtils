// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2022 Philippe Steinmann.
 **
 ****************************************************************************/
#ifndef MDT_DEPLOY_UTILS_IMPL_BINARY_DEPENDENCIES_GRAPH_RESULT_VISITOR_H
#define MDT_DEPLOY_UTILS_IMPL_BINARY_DEPENDENCIES_GRAPH_RESULT_VISITOR_H

#include "GraphDef.h"
#include "GraphFile.h"
#include "FileComparison.h"
#include "Mdt/DeployUtils/BinaryDependenciesResult.h"
#include "Mdt/DeployUtils/BinaryDependenciesResultLibrary.h"
#include "Mdt/DeployUtils/OperatingSystem.h"
#include <boost/graph/breadth_first_search.hpp>

#include <QDebug>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace BinaryDependencies{

  /*! \internal
   */
  inline
  bool graphFileIsResultTarget(const GraphFile & file, const BinaryDependenciesResult & result, OperatingSystem os) noexcept
  {
    assert( os != OperatingSystem::Unknown );

    return fileNamesAreEqual(file.fileName(), result.target().fileName(), os);
  }

  /*! \internal
   */
  inline
  void addGraphFileToResult(const GraphFile & file, BinaryDependenciesResult & result, OperatingSystem os) noexcept
  {
    assert( os != OperatingSystem::Unknown );

    if( graphFileIsResultTarget(file, result, os) ){
      return;
    }

    if( file.isNotFound() ){
      result.addNotFoundLibrary( file.fileInfo() );
      return;
    }

    if( file.shouldNotBeRedistributed() ){
      result.addLibraryToNotRedistribute( file.fileInfo() );
      return;
    }

    result.addFoundLibrary( file.fileInfo() );
  }

  /*! \internal
   */
  class GraphResultVisitor
  {
   public:

    using event_filter = boost::on_discover_vertex;

    explicit
    GraphResultVisitor(BinaryDependenciesResult & result, OperatingSystem os) noexcept
     : mResult(result),
       mOs(os)
    {
      assert( mOs != OperatingSystem::Unknown );
    }

    /*! \brief
     */
    void operator()(VertexDescriptor v, const GraphAL & graph) noexcept
    {
      const GraphFile & file = graph[v];
      
      qDebug() << "result, add " << file.fileName();
      
      addGraphFileToResult(file, mResult, mOs);
    }

   private:

    BinaryDependenciesResult & mResult;
    OperatingSystem mOs;
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace BinaryDependencies{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_BINARY_DEPENDENCIES_GRAPH_RESULT_VISITOR_H

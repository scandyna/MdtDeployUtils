// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2022 Philippe Steinmann.
 **
 ****************************************************************************/
#ifndef MDT_DEPLOY_UTILS_IMPL_BINARY_DEPENDENCIES_GRAPH_DEF_H
#define MDT_DEPLOY_UTILS_IMPL_BINARY_DEPENDENCIES_GRAPH_DEF_H

#include "GraphFile.h"
#include <boost/graph/adjacency_list.hpp>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace BinaryDependencies{

  using GraphAL = boost::adjacency_list<
    boost::hash_setS, // OutEdgeList: hash_setS (boost::unordered_set): choosed to avoid adding a edge more than once
    boost::vecS,      // VertexList: vecS (std::vector)
    boost::directedS, // Directed: directedS (Directed graph)
    GraphFile         // VertexProperties
  >;

  using VertexDescriptor = boost::graph_traits<GraphAL>::vertex_descriptor;
  using EdgeDescriptor = boost::graph_traits<GraphAL>::edge_descriptor;

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace BinaryDependencies{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_BINARY_DEPENDENCIES_GRAPH_DEF_H

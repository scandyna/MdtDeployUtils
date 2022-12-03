// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2022 Philippe Steinmann.
 **
 ****************************************************************************/
#include "catch2/catch.hpp"
#include "Catch2QString.h"
#include "Mdt/DeployUtils/BinaryDependenciesGraph.h"
#include <QLatin1String>
#include <QString>
#include <string>
#include <algorithm>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>

#include <QDebug>

using namespace Mdt::DeployUtils;

/** \todo
 *
 * - To find some file (Vertex), not BFS required, find by using vertex iterators
 * 
 * - Cound we solve all transitive deps in implementation ? Would be very nice !
 *   -> findTransitiveDependencies( targets, someVisitor )
 */

/** \internal How to build the graph
 *
 * We begin with some target,
 * that is a binary (executable or shared library).
 * That target is the full path to a binary on the file system.
 * \code
 * (app)
 * \endcode
 *
 * We have to read it to extract its direct dependencies,
 * which are library names.
 * We also have to read its rpath that helps finding those direct dependencies (for platforms that supports it)
 * \note Are rpath always only for direct dependencies ?
 * Now we have to add the direct dependencies of given target to the graph:
 * \code
 *     (app)
 *     /   \
 * (libA) (Qt5Core)
 * \endcode
 *
 * Now, for \a libA and \a Qt5Core , we have to:
 * - Find the full path, using rpath from \a app
 * - Read them to extract direct dependencies library names + rpath
 * - Add them to the graph
 *
 * We should not modify the graph during a BFS,
 * because that will invalidate internals, like iterators.
 * \sa https://boost-users.boost.narkive.com/Tt0rF8Ao/bgl-add-vertex-during-breadth-first-search
 *
 * So, process the graph (BFS) and return a list of new dependencies.
 *
 * \code
 * struct XY
 * {
 *   using event_filter = boost::on_discover_vertex;
 *
 *   // Called when a new vertex is discovered
 *   void operator()()
 *   {
 *     // Check state of u
 *   }
 *
 *   void discover_vertex(VertexDescriptor u, const Graph & g) const
 *   {
 *   }
 *
 *   X findLibraryAbsolutePath(fileName, rpath, os);
 * };
 * \endcode
 * 
 * \todo document that we have to handle things like colors
 * 
 * \todo See BFS alogorithm to help !
 * 
 * \code
 * BinaryDependenciesGraphBuildVisitor visitor(...,...);
 * graph.addFile(target);
 * queue.enqueue(target);
 *
 * while( !queue.isEmpty() ){
 *   u = queue.dequeue();
 *   // process() calls BFS - No graph modifications here
 *   directDepenencies = graph.processXY(u, visitor);
 *   // Now add the new direct dependencies to the graph
 *   graph.addDependencies(u, directDepenencies);
 *   queue.enqueue(directDepenencies);
 * }
 * \endcode
 *
 * \todo document that complexity is not good, but tradeoff..
 *
 * Processing multiple targets should also work:
 * \code
 * graph.addXY(targets);
 * queue.enqueue(targets);
 * \endcode
 *
 * 
 * \code
 * graph.addXY(target);
 * VertexList newDependencies;
 * do{
 *   // Could return a leaf an finish to early !!
 *   Vertex currentTarget = graph.findUnprocessedXY();
 *   // process() calls BFS
 *   VertexList newDependencies = graph.processXY(currentTarget, f);
 *   graph.addDependencies(currentTarget, newDependencies);
 * }while( !newDependencies.isEmpty() );
 * \endcode
 */

using Graph = boost::adjacency_list<
  boost::hash_setS, // OutEdgeList: hash_setS (boost::unordered_set): choosed to avoid adding a edge more than once
  boost::vecS,      // VertexList: vecS (std::vector)
  boost::directedS, // Directed: directedS (Directed graph)
  QString               // VertexProperties:  ?????
>;

using VertexDescriptor = boost::graph_traits<Graph>::vertex_descriptor;
using EdgeDescriptor = boost::graph_traits<Graph>::edge_descriptor;

/*! \internal Visitor used to build a binary dependencies graph
 */
template<typename Reader>
class BinaryDependenciesGraphBuildVisitor : public boost::default_bfs_visitor
{
 public:

//   using event_filter = boost::on_discover_vertex;
//   using event_filter = boost::on_examine_edge;

  /*! \brief Read the file associated with the new discovered vertex
   *
   * Will read the new discovered file to get its direct dependencies librarie names
   * and its rpath (if supported).
   *
   * If this was already done for the given file,
   * nothing is done (the graph can be processed multiple times during build).
   *
   * \pre The full path of given file must have been set
   */
  void discover_vertex(VertexDescriptor u, const Graph & g)
  {
    qDebug() << "read: " << g[u];
  }

  /*! \brief Find the full path for the file associated with given target vertex
   *
   * Will find the full path for given library name
   * (associated with the target vertex of given edge).
   *
   * If appropriate for the platform,
   * the rpath from the "parent" file will be used
   * ("parent" is the file associated with the source vertex of given edge).
   *
   * If the full path is already set,
   * nothing is done (the graph can be processed multiple times during build).
   *
   * \todo flag if find full path fails
   */
  void examine_edge(EdgeDescriptor e, const Graph & g)
  {
    const VertexDescriptor parent = boost::source(e, g);
    const VertexDescriptor current = boost::target(e, g);

    qDebug() << " find full path of " << g[current] << " using rpath from " << g[parent];
  }

//     template<typename Vertex, typename Graph>

//     void operator()(VertexDescriptor u, const Graph & g) /*const*/
//   void operator()(EdgeDescriptor e, const Graph & g) /*const*/
//   {
// //       qDebug() << "new vertex: " << u;
//     qDebug() << "new? edge, source: " << boost::source(e, g) << ", target: " << boost::target(e, g);
//     
//     const VertexDescriptor parent = boost::source(e, g);
//     const VertexDescriptor current = boost::target(e, g);
//     
//     // Find absolute path for current using rpath from parent
//     
//     // Read current to extract rpath + direct dependencies file names
//     
//   }

 private:

//   void findLibraryAbsolutePath()
//   {
//   }
};


TEST_CASE("sandbox")
{
  VertexDescriptor a, b, c;
  Graph graph;

  a = boost::add_vertex( QLatin1String("a"), graph);
  b = boost::add_vertex(graph);
  c = boost::add_vertex(graph);

  qDebug() << "a type     : " << typeid(a).name() << " , hash: " << typeid(a).hash_code();
  qDebug() << "size_t type: " << typeid(size_t).name() << " , hash: " << typeid(size_t).hash_code();

  
  qDebug() << "graph[0] type: " << typeid(graph[0]).name();
  qDebug() << "graph[a] type: " << typeid(graph[a]).name();
  
  qDebug() << "graph[a]: " << graph[a];
  
  graph[c] = QLatin1String("c");
  qDebug() << "graph[c]: " << graph[c];
  qDebug() << "graph[c] size: " << graph[c].size();
  
  EdgeDescriptor edge;
  bool inserted = false;

  boost::tie(edge, inserted) = boost::add_edge(a, b, graph);
  qDebug() << "inserted a->b: " << inserted;

  boost::tie(edge, inserted) = boost::add_edge(a, b, graph);
  qDebug() << "inserted a->b: " << inserted;

  boost::tie(edge, inserted) = boost::add_edge(a, c, graph);
  qDebug() << "inserted a->c: " << inserted;

  boost::tie(edge, inserted) = boost::add_edge(c, a, graph);
  qDebug() << "inserted c->a: " << inserted;
  
// //   boost::bfs_visitor<MyBFSVisitor> visitor;
  BinaryDependenciesGraphBuildVisitor<int> visitor;
  boost::breadth_first_search( graph, a, boost::visitor(visitor) );
}


BinaryDependenciesFile binaryDependenciesFileFromPath(const std::string & path)
{
  return BinaryDependenciesFile::fromQFileInfo( QString::fromStdString(path));
}

bool fileListContainsFileName(const BinaryDependenciesFileList & files, const std::string & name)
{
  const QString expectedName = QString::fromStdString(name);
  const auto pred = [&expectedName](const BinaryDependenciesFile & file){
    return file.fileName() == expectedName;
  };

  const auto it = std::find_if(files.cbegin(), files.cend(), pred);

  return it != files.cend();
}


TEST_CASE("addFile_findUnprocessedFiles")
{
  BinaryDependenciesGraph graph;
  BinaryDependenciesFileList unprocessedFiles;

  SECTION("empty graph")
  {
    unprocessedFiles = graph.findUnprocessedFiles();

    REQUIRE( unprocessedFiles.empty() );
  }

  SECTION("app")
  {
    const auto app = binaryDependenciesFileFromPath("/path/to/app");

    unprocessedFiles = graph.findUnprocessedFiles();

    REQUIRE( unprocessedFiles.size() == 1 );
    REQUIRE( fileListContainsFileName(unprocessedFiles, "app") );
  }
}

TEST_CASE("solvingExample")
{
  BinaryDependenciesGraph graph;
//   BinaryDependenciesResult result;

  SECTION("app (no dependencies)")
  {
    REQUIRE(false);
  }

  SECTION("test not implemented")
  {
    REQUIRE(false);
  }
}

TEST_CASE("solvingExample_multipleTargets")
{
  SECTION("test not implemented")
  {
    REQUIRE(false);
  }
}

// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2022 Philippe Steinmann.
 **
 ****************************************************************************/
#ifndef MDT_DEPLOY_UTILS_IMPL_BINARY_DEPENDENCIES_GRAPH_BUILD_VISITOR_H
#define MDT_DEPLOY_UTILS_IMPL_BINARY_DEPENDENCIES_GRAPH_BUILD_VISITOR_H

#include "GraphFile.h"
#include "GraphDef.h"
#include "DiscoveredDependenciesList.h"
#include "Mdt/DeployUtils/AbstractSharedLibraryFinder.h"
#include "Mdt/DeployUtils/Platform.h"
#include "Mdt/DeployUtils/FindDependencyError.h"
#include "mdt_deployutilscore_export.h"
#include <QObject>
#include <QFileInfo>
#include <QString>
#include <boost/graph/breadth_first_search.hpp>
#include <cassert>

#include <QDebug>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace BinaryDependencies{

  /*! \internal Worker for GraphBuildVisitor
   *
   * This worker provides translation and signal/slot support.
   */
  class MDT_DEPLOYUTILSCORE_EXPORT GraphBuildVisitorWorker : public QObject
  {
    Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit
    GraphBuildVisitorWorker(AbstractSharedLibraryFinder & sharedLibraryFinder,
                            const Platform & platform,
                            DiscoveredDependenciesList & discoveredDependenciesList,
                            QObject *parent = nullptr) noexcept
    : QObject(parent),
      mSharedLibraryFinder(sharedLibraryFinder),
      mPlatform(platform),
      mDiscoveredDependenciesList(discoveredDependenciesList)
    {
    }

    /*!\brief Read given file to extract dependencies and rpath if supported
     */
    template<typename Reader>
    void readFile(GraphFile & file, Reader & reader)
    {
      assert( !file.isReaden() );
      assert( file.hasAbsolutePath() );
      assert( !reader.isOpen() );

      qDebug() << "read: " << file.fileName() << " (path: " << file.fileInfo().absoluteFilePath() << ")";
      /// emitProcessingCurrentFileMessage(currentFile);

      reader.openFile(file.fileInfo(), mPlatform);
      if( !reader.isExecutableOrSharedLibrary() ){
        /**
        const QString message = tr("'%1' is not a executable or a shared library")
                                .arg( file.absoluteFilePath() );
        throw FindDependencyError(message);
        */
      }

      mDiscoveredDependenciesList.setDirectDependenciesFileNames( file, reader.getNeededSharedLibraries() );

      /// emitDirectDependenciesMessage(currentFile);

      file.setRPath( reader.getRunPath() );

      reader.close();
    }

    /*! \brief Find the absolute path for given library name
     */
    void findLibraryAbsolutePath(GraphFile & file, const GraphFile & parentFile)
    {
      assert( !file.hasBeenSearched() );
      assert( fileInfoIsFileNameWithoutPath( file.fileInfo() ) );

      const QString libraryName = file.fileName();

      if( !mSharedLibraryFinder.libraryShouldBeDistributed(libraryName) ){
        file.markAsNotToBeRedistributed();
        return;
      }
      qDebug() << " find full path of " << libraryName << " using rpath from " << parentFile.fileName();

      const auto dependentFile = BinaryDependenciesFile::fromQFileInfo( parentFile.fileInfo() );
      try{
        const QFileInfo path = mSharedLibraryFinder.findLibraryAbsolutePath( libraryName, dependentFile );
        assert( fileInfoIsAbsolutePath(path) );
        file.setAbsoluteFilePath(path);
      }catch(const FindDependencyError &){
        file.markAsNotFound();
      }
    }

   private:

    AbstractSharedLibraryFinder & mSharedLibraryFinder;
    const Platform & mPlatform;
    DiscoveredDependenciesList & mDiscoveredDependenciesList;
  };


  /*! \internal Visitor used to build a binary dependencies graph
   */
  template<typename Reader>
  class GraphBuildVisitor : public boost::default_bfs_visitor
  {
   public:

    /*! \brief Constructor
     *
     * The worker inherits QObject and requires the Q_OBJECT macro.
     * So, it can't be a class templated on reader.
     * As workaround, reference the reader here.
     *
     * We have to modify some vertex properties (set absolute file path, rpath, ...).
     * The BFS takes a const graph, so we can't use the graph passed as argument
     * to this visitor methods.
     * As workaround, we reference the graph as non const.
     */
    explicit
    GraphBuildVisitor(GraphBuildVisitorWorker & worker, Reader & reader, GraphAL & graph) noexcept
    : mWorker(worker),
      mReader(reader),
      mGraph(graph)
    {
    }

    /*! \brief Read the file associated with the new discovered vertex
     *
     * Will read the new discovered file to get its direct dependencies librarie names
     * and its rpath (if supported).
     *
     * If this was already done for the given file,
     * nothing is done (the graph can be processed multiple times during build).
     *
     * \pre The absolute path of given file must have been set
     */
    void discover_vertex(VertexDescriptor u, const GraphAL &)
    {
      GraphFile & file = mGraph[u];

  //     qDebug() << "discover vertex " << file.fileName();

      if( file.isReaden() ){
        return;
      }
      assert( file.hasAbsolutePath() );

      mWorker.readFile(file, mReader);

      file.markAsReaden();
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
    void examine_edge(EdgeDescriptor e, const GraphAL & g)
    {
      const VertexDescriptor parent = boost::source(e, g);
      const VertexDescriptor current = boost::target(e, g);

  //     const GraphFile & parentFile = mGraph[parent];
      GraphFile & currentFile = mGraph[current];

      if( currentFile.hasBeenSearched() ){
        return;
      }

      mWorker.findLibraryAbsolutePath(currentFile, mGraph[parent]);
    }

  private:

    GraphBuildVisitorWorker & mWorker;
    Reader & mReader;
    GraphAL & mGraph;
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace BinaryDependencies{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_BINARY_DEPENDENCIES_GRAPH_BUILD_VISITOR_H

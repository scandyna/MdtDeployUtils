/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2020-2021 Philippe Steinmann.
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
#ifndef MDT_DEPLOY_UTILS_MESSAGE_LOGGER_H
#define MDT_DEPLOY_UTILS_MESSAGE_LOGGER_H

#include "AbstractMessageLoggerBackend.h"
#include "mdt_deployutils_export.h"
#include <QString>
#include <exception>
#include <memory>
#include <type_traits>
#include <cassert>


namespace Mdt{ namespace DeployUtils{

  /*! \brief Simple message logger
   *
   * This documentation assumes:
   * \code
   * #include <Mdt/DeployUtils/MessageLogger.h>
   *
   * using namespace Mdt::DeployUtils;
   * \endcode
   *
   * One instance of the message logger must be created before using it:
   * \code
   * int main(int argc, char **argv)
   * {
   *   QCoreApplication app(argc, argv);
   *
   *   MessageLogger messageLogger;
   * }
   * \endcode
   *
   * The instance of the logger should be created after
   * the QCoreApplication (or QGuiApplication or QApplication) instance.
   *
   * The reason a instance is required is to manage the cleanup
   * of the internally used ressources.
   * This way the cleanup is done at %main() exit,
   * and not after, at some unspecified time.
   * This can become more important if a backend
   * uses QObject, because it then will also depend on QCoreApplication lifetime.
   *
   * Then messages can be shown using plain functions:
   * \code
   * showInfo("Some info");
   * showError("Some error");
   * \endcode
   *
   * The Qt signal/slot mechanism can also be used:
   * \code
   * MyUseCase useCase;
   * QObject::connect(&useCase, &MyUseCase::message, MessageLogger::info);
   *
   * try{
   *   useCase.execute();
   * }catch(const SomeError & error){
   *   showError(error);
   * }
   * \endcode
   *
   * By default, messages will be logged to the console,
   * but a other backend can be used:
   * \code
   * MessageLogger::setBackend<CMakeStyleMessageLogger>();
   * \endcode
   *
   * \sa showInfo()
   * \sa showError(const QString &)
   * \sa showError(const std::exception &)
   */
  class MDT_DEPLOYUTILS_EXPORT MessageLogger
  {
   public:

    /*! \brief Create a global instance of the message logger
     *
     * This will use a ConsoleMessageLogger as backend.
     *
     * \pre Only 1 instance of a message logger can be created
     */
    explicit MessageLogger();

    /*! \brief Cleanup the instance logger
     */
    ~MessageLogger() noexcept;

    MessageLogger(const MessageLogger &) = delete;
    MessageLogger & operator=(const MessageLogger &) = delete;
    MessageLogger(MessageLogger &&) = delete;
    MessageLogger & operator=(MessageLogger &&) = delete;

    /*! \brief Set the backend
     *
     * Set the backend of the logger.
     *
     * The logger will instanciate the backend
     * and manage its lifetime.
     *
     * If some setup hase to be done,
     * the returned pointer can be used:
     * \code
     * auto *backend = MessageLogger::setBackend<MyBackend>();
     * backend->setup(...);
     * \endcode
     *
     * \pre \a Backend must inherit AbstractMessageLoggerBackend
     */
    template<typename Backend>
    static
    Backend *setBackend()
    {
      static_assert( std::is_base_of<AbstractMessageLoggerBackend, Backend>::value,
                     "Backend must inherit AbstractMessageLoggerBackend" );
      assert( MessageLogger::isInitialized() );

      instance->mBackend = std::make_unique<Backend>();

      return static_cast<Backend*>( instance->mBackend.get() );
    }

    /*! \brief Log a information
     */
    static
    void info(const QString & message);

    /*! \brief Log a error
     */
    static
    void error(const QString & message);

    /*! \brief Log a error
     */
    static
    void error(const std::exception & err);

    /*! \brief Check if the logger is initialized
     */
    static
    bool isInitialized() noexcept
    {
      return instance != nullptr;
    }

   private:

    std::unique_ptr<AbstractMessageLoggerBackend> mBackend;
    static MessageLogger *instance;
  };

  /*! \brief Log a information
   *
   * \sa MessageLogger
   */
  inline
  void showInfo(const QString & message)
  {
    MessageLogger::info(message);
  }

  /*! \brief Log a error
   *
   * \sa MessageLogger
   */
  inline
  void showError(const QString & message)
  {
    MessageLogger::error(message);
  }

  /*! \brief Log a error
   *
   * \sa MessageLogger
   */
  inline
  void showError(const std::exception & error)
  {
    MessageLogger::error(error);
  }

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_MESSAGE_LOGGER_H

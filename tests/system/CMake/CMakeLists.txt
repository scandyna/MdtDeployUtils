# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

if(WIN32)
  string(REGEX REPLACE "^[a-zA-Z]:" ""
    MDT_SYSTEM_TESTS_INSTALL_PREFIX
    "${CMAKE_INSTALL_PREFIX}"
  )
else()
  set(MDT_SYSTEM_TESTS_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
endif()


add_subdirectory(QtCoreApp)
add_subdirectory(QtWidgetsApp)

# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

add_executable(qtcoreapp
  src/main.cpp
)

target_link_libraries(qtcoreapp
  PRIVATE
    Qt5::Core
)

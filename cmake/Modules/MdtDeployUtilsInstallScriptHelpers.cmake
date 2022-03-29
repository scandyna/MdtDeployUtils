# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

# Prevent having //path if DESTIR is not used
set(destDir "$ENV{DESTDIR}")
if(destDir)
  if(WIN32)
    # Remove C: from CMAKE_INSTALL_PREFIX
    string(REGEX REPLACE "^[a-zA-Z]:" ""
      installPrefix
      "${CMAKE_INSTALL_PREFIX}"
    )
    get_filename_component(MDT_INSTALL_PREFIX_WITH_DESTDIR "$ENV{DESTDIR}/${installPrefix}" ABSOLUTE)
    unset(installPrefix)
  else()
    get_filename_component(MDT_INSTALL_PREFIX_WITH_DESTDIR "$ENV{DESTDIR}/${CMAKE_INSTALL_PREFIX}" ABSOLUTE)
  endif()
else()
  get_filename_component(MDT_INSTALL_PREFIX_WITH_DESTDIR "${CMAKE_INSTALL_PREFIX}" ABSOLUTE)
endif()
unset(destDir)


function(execute_mdtdeployutils)

  set(options)
  set(oneValueArgs MDTDEPLOYUTILS_EXECUTABLE RUNTIME_ENV)
  set(multiValueArgs ARGUMENTS)
  cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(NOT ARG_MDTDEPLOYUTILS_EXECUTABLE)
    message(FATAL_ERROR "execute_mdtdeployutils(): mandatory argument MDTDEPLOYUTILS_EXECUTABLE missing")
  endif()
#   if(NOT ARG_RUNTIME_ENV)
#     message(FATAL_ERROR "execute_mdtdeployutils(): mandatory argument RUNTIME_ENV missing")
#   endif()
  if(NOT ARG_ARGUMENTS)
    message(FATAL_ERROR "execute_mdtdeployutils(): mandatory argument ARGUMENTS missing")
  endif()
  if(ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "execute_mdtdeployutils(): unknown arguments passed: ${ARG_UNPARSED_ARGUMENTS}")
  endif()


  set(deployUtilsExecutable "${ARG_MDTDEPLOYUTILS_EXECUTABLE}")
  message(DEBUG "deployUtilsExecutable: ${deployUtilsExecutable}")

  if(ARG_RUNTIME_ENV)
    set(deployUtilsRuntimeEnv "${ARG_RUNTIME_ENV}")
    if(WIN32)
      # ; have to be escaped, otherwise the environment will not be set properly
      # See: https://cmake.org/pipermail/cmake/2009-May/029425.html
      string(REPLACE ";" "\\;" deployUtilsRuntimeEnv "${deployUtilsRuntimeEnv}")
    endif()
    message(DEBUG "deployUtilsRuntimeEnv: ${deployUtilsRuntimeEnv}")
  endif()

  set(hasDeployUtilsRuntimeEnv FALSE)
  if( NOT ("${deployUtilsRuntimeEnv}" STREQUAL "") )
    set(hasDeployUtilsRuntimeEnv TRUE)
  endif()

  if(hasDeployUtilsRuntimeEnv)
    message(DEBUG "mdtdeployutils will be called with cmake -E env")
  else()
    message(DEBUG "mdtdeployutils will be called directly (no runtime env)")
  endif()

#   message(DEBUG "MDT_DEPLOY_APPLICATION_INSTALL_SCRIPT_DEPLOY_UTILS_ARGUMENTS: @MDT_DEPLOY_APPLICATION_INSTALL_SCRIPT_DEPLOY_UTILS_ARGUMENTS@")

  if(hasDeployUtilsRuntimeEnv)
    execute_process(
      COMMAND "${CMAKE_COMMAND}" -E env "${deployUtilsRuntimeEnv}"
              "${deployUtilsExecutable}" ${ARG_ARGUMENTS}
      RESULTS_VARIABLE mdtDeployUtilsResult
      ERROR_VARIABLE mdtDeployUtilsError
    )
  else()
    execute_process(
      COMMAND "${deployUtilsExecutable}" ${ARG_ARGUMENTS}
      RESULTS_VARIABLE mdtDeployUtilsResult
      ERROR_VARIABLE mdtDeployUtilsError
    )
  endif()

  if( NOT (${mdtDeployUtilsResult} EQUAL 0) )
    message(FATAL_ERROR "execute_mdtdeployutils() failed: ${mdtDeployUtilsError}")
  endif()

endfunction()

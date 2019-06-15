# Install script for directory: /home/punk4tt4ck/Desktop/VUT/4_semester/IZG/Projekt/IZG_PROJ/errorCodes

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/punk4tt4ck/Desktop/VUT/4_semester/IZG/Projekt/IZG_PROJ/build/errorCodes/liberrorCodes.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDevelx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/errorCodes" TYPE FILE FILES "/home/punk4tt4ck/Desktop/VUT/4_semester/IZG/Projekt/IZG_PROJ/build/errorCodes/errorCodes/errorcodes_export.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDevelx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/errorCodes" TYPE FILE FILES "/home/punk4tt4ck/Desktop/VUT/4_semester/IZG/Projekt/IZG_PROJ/errorCodes/src/errorCodes/errorCodes.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDevelx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/errorCodes" TYPE FILE FILES
    "/home/punk4tt4ck/Desktop/VUT/4_semester/IZG/Projekt/IZG_PROJ/build/errorCodes/errorCodes/errorCodesConfig.cmake"
    "/home/punk4tt4ck/Desktop/VUT/4_semester/IZG/Projekt/IZG_PROJ/build/errorCodes/errorCodes/errorCodesConfigVersion.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/errorCodes/errorCodesTargets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/errorCodes/errorCodesTargets.cmake"
         "/home/punk4tt4ck/Desktop/VUT/4_semester/IZG/Projekt/IZG_PROJ/build/errorCodes/CMakeFiles/Export/lib/cmake/errorCodes/errorCodesTargets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/errorCodes/errorCodesTargets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/errorCodes/errorCodesTargets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/errorCodes" TYPE FILE FILES "/home/punk4tt4ck/Desktop/VUT/4_semester/IZG/Projekt/IZG_PROJ/build/errorCodes/CMakeFiles/Export/lib/cmake/errorCodes/errorCodesTargets.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^()$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/errorCodes" TYPE FILE FILES "/home/punk4tt4ck/Desktop/VUT/4_semester/IZG/Projekt/IZG_PROJ/build/errorCodes/CMakeFiles/Export/lib/cmake/errorCodes/errorCodesTargets-noconfig.cmake")
  endif()
endif()


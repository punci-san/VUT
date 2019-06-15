cmake_minimum_required(VERSION 3.12.0)

PROJECT(izgProject)

SET(CMAKE_CXX_STANDARD 14)
SET(CMAKE_C_STANDARD 99)

set(SOURCES 
  student/main.c
  student/gpu.c
  student/gpu.h
  student/cpu.c
  student/cpu.h
  student/fwd.h
  student/linearAlgebra.c
  student/linearAlgebra.h
  student/drawPoints.cpp
  student/drawTriangles.cpp
  student/window.h
  student/window.c
  student/application.h
  student/application.c
  student/callbackAndData.h
  student/callbackAndData.c
  student/method.h
  student/method.c
  student/emptyMethod.h
  student/emptyMethod.c
  student/pointMethod.h
  student/pointMethod.c
  student/pointCircleMethod.h
  student/pointCircleMethod.c
  student/pointSquareMethod.h
  student/pointSquareMethod.c
  student/pointBoxMethod.h
  student/pointBoxMethod.c
  student/phongMethod.h
  student/phongMethod.c
  student/triangleMethod.h
  student/triangleMethod.c
  student/triangle3DMethod.h
  student/triangle3DMethod.c
  student/parseArguments.h
  student/parseArguments.c
  student/bunny.h
  student/bunny.c
  student/globals.h
  student/globals.c
  student/camera.h
  student/camera.c
  tests/conformanceTests.cpp
  tests/conformanceTests.h
  tests/performanceTest.h
  tests/performanceTest.cpp
  tests/takeScreenShot.h
  tests/takeScreenShot.cpp
  tests/groundTruth.c
  tests/groundTruth.h
  tests/tests.cpp
  )

find_package(SDL2 CONFIG REQUIRED)

add_subdirectory(errorCodes)
add_subdirectory(stack)
add_subdirectory(queue)
add_subdirectory(vector)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} SDL2::SDL2 SDL2::SDL2main stack::stack queue::queue vector::vector)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

option(${PROJECT_NAME}_BUILD_INTERNAL_TESTS "toggle building of internal tests")
if(${PROJECT_NAME}_BUILD_INTERNAL_TESTS)
  #enable_testing()
  add_subdirectory(tests/internalTests)
  #add_test(NAME baseTest COMMAND tests)
endif()

#set(GPU_INCLUDES
#  )
#
#set(STUDENT_SOURCES
#  student/student_cpu.c
#  student/student_pipeline.c
#  student/student_shader.c
#  student/linearAlgebra.c
#  student/main.c
#  student/camera.c
#  student/bunny.c
#  student/mouseCamera.c
#  student/swapBuffers.c
#  student/globals.c
#  )
#
#set(STUDENT_INCLUDES
#  student/student_cpu.h
#  student/student_pipeline.h
#  student/student_shader.h
#  student/gpu.h
#  student/uniforms.h
#  student/buffer.h
#  student/vertexPuller.h
#  student/program.h
#  student/camera.h
#  student/bunny.h
#  student/mouseCamera.h
#  student/swapBuffers.h
#  student/globals.h
#  )
#
#set(TESTS_SOURCES
#  tests/conformanceTests.cpp
#  tests/performanceTest.cpp
#  )
#
#set(TESTS_INCLUDES
#  tests/conformanceTests.h
#  tests/performanceTest.h
#  )
#
#set(EXAMPLE_SOURCES
#  examples/triangleExample.c
#  )
#
#set(EXAMPLE_HEADERS
#  examples/triangleExample.h
#  )
#
#set(3RDPARTY_SOURCES
#  )
#
#set(3RDPARTY_INCLUDES
#  3rdParty/catch.hpp
#  )
#
#SET(CMAKE_CXX_STANDARD 14)
#SET(CMAKE_C_STANDARD 99)
#
#IF(CMAKE_COMPILER_IS_GNUCXX)
#  add_compile_options(-W)
#  add_compile_options(-Wall)
#  add_compile_options(-pedantic)
#  add_compile_options(-Wconversion)
#ENDIF()
#
#IF(CMAKE_COMPILER_IS_GNUCC)
#  add_compile_options(-W)
#  add_compile_options(-Wall)
#  add_compile_options(-pedantic)
#  add_compile_options(-Wconversion)
#ENDIF()
#
#
#add_executable(${APPLICATION_NAME} ${GPU_SOURCES} ${GPU_INCLUDES} ${STUDENT_SOURCES} ${STUDENT_INCLUDES} ${TESTS_SOURCES} ${TESTS_INCLUDES} ${3RDPARTY_SOURCES} ${3RDPARTY_INCLUDES} ${EXAMPLE_SOURCES} ${EXAMPLE_HEADERS})
#target_link_libraries(${APPLICATION_NAME} SDL2::SDL2 SDL2::SDL2main)
#target_include_directories(${APPLICATION_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

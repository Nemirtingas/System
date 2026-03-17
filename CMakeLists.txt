cmake_minimum_required(VERSION 3.21)

# ======================
# Configurable variables
# ======================

set(SYSTEM_PROJECT_NAME "System" CACHE STRING "Project name")
string(TOUPPER "${SYSTEM_PROJECT_NAME}" SYSTEM_PROJECT_NAME_UPPER)
set(SYSTEM_NAMESPACE "Nemirtingas" CACHE STRING "CMake namespace")

project(${SYSTEM_PROJECT_NAME} LANGUAGES CXX)

set(SYSTEM_TARGET_NAME ${SYSTEM_PROJECT_NAME})
set(SYSTEM_TARGET_NAME_ALIAS "${SYSTEM_NAMESPACE}::${SYSTEM_PROJECT_NAME}")

# Root directory (safe for include() usage)
set(SYSTEM_PROJECT_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})

# ======================
# Options
# ======================

option(${SYSTEM_PROJECT_NAME_UPPER}_DYNAMIC_RUNTIME "Use dynamic runtime on Windows" ON)
option(${SYSTEM_PROJECT_NAME_UPPER}_BUILD_TESTS "Build tests" OFF)
option(${SYSTEM_PROJECT_NAME_UPPER}_BUILD_TESTS_DOTNET "Build .NET tests" OFF)

# ======================
# Library
# ======================

if(TARGET ${SYSTEM_TARGET_NAME})
  return()
endif()

add_library(${SYSTEM_TARGET_NAME})
add_library(${SYSTEM_TARGET_NAME_ALIAS} ALIAS ${SYSTEM_TARGET_NAME})

target_compile_features(${SYSTEM_TARGET_NAME} PUBLIC cxx_std_17)

# Sources
target_sources(${SYSTEM_TARGET_NAME}
  PRIVATE
    ${SYSTEM_PROJECT_ROOT_DIR}/src/CPUExtentions.cpp
    ${SYSTEM_PROJECT_ROOT_DIR}/src/Encoding.cpp
    ${SYSTEM_PROJECT_ROOT_DIR}/src/Filesystem.cpp
    ${SYSTEM_PROJECT_ROOT_DIR}/src/Date.cpp
    ${SYSTEM_PROJECT_ROOT_DIR}/src/Guid.cpp
    ${SYSTEM_PROJECT_ROOT_DIR}/src/Library.cpp
    ${SYSTEM_PROJECT_ROOT_DIR}/src/String.cpp
    ${SYSTEM_PROJECT_ROOT_DIR}/src/System.cpp
    ${SYSTEM_PROJECT_ROOT_DIR}/src/System_internals.cpp
    ${SYSTEM_PROJECT_ROOT_DIR}/src/DotNet.cpp

    $<$<PLATFORM_ID:Windows>:${SYSTEM_PROJECT_ROOT_DIR}/src/WinRegister.cpp>

  PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${SYSTEM_PROJECT_ROOT_DIR}/include
    FILES
      ${SYSTEM_PROJECT_ROOT_DIR}/include/System/System.h
      ${SYSTEM_PROJECT_ROOT_DIR}/include/System/SystemMacro.h
      ${SYSTEM_PROJECT_ROOT_DIR}/include/System/Filesystem.h
      ${SYSTEM_PROJECT_ROOT_DIR}/include/System/Date.h
      ${SYSTEM_PROJECT_ROOT_DIR}/include/System/Library.h
      ${SYSTEM_PROJECT_ROOT_DIR}/include/System/SystemCompiler.h
      ${SYSTEM_PROJECT_ROOT_DIR}/include/System/SystemCPUExtensions.h
      ${SYSTEM_PROJECT_ROOT_DIR}/include/System/SystemDetector.h
      ${SYSTEM_PROJECT_ROOT_DIR}/include/System/SystemExports.h
      ${SYSTEM_PROJECT_ROOT_DIR}/include/System/SystemInline.h
      ${SYSTEM_PROJECT_ROOT_DIR}/include/System/String.hpp
      ${SYSTEM_PROJECT_ROOT_DIR}/include/System/ThreadPool.hpp
      ${SYSTEM_PROJECT_ROOT_DIR}/include/System/FunctionName.hpp
      ${SYSTEM_PROJECT_ROOT_DIR}/include/System/Encoding.hpp
      ${SYSTEM_PROJECT_ROOT_DIR}/include/System/ClassEnumUtils.hpp
      ${SYSTEM_PROJECT_ROOT_DIR}/include/System/ConstExpressions.hpp
      ${SYSTEM_PROJECT_ROOT_DIR}/include/System/Endianness.hpp
      ${SYSTEM_PROJECT_ROOT_DIR}/include/System/ScopedLock.hpp
      ${SYSTEM_PROJECT_ROOT_DIR}/include/System/StringSwitch.hpp
      ${SYSTEM_PROJECT_ROOT_DIR}/include/System/LoopBreak.hpp
      ${SYSTEM_PROJECT_ROOT_DIR}/include/System/Guid.hpp
      ${SYSTEM_PROJECT_ROOT_DIR}/include/System/FunctionTraits.hpp
      ${SYSTEM_PROJECT_ROOT_DIR}/include/System/DotNet.hpp
)

# Include directories
target_include_directories(${SYSTEM_TARGET_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${SYSTEM_PROJECT_ROOT_DIR}/include>
    $<BUILD_INTERFACE:${SYSTEM_PROJECT_ROOT_DIR}/deps/utfcpp/include>
    $<INSTALL_INTERFACE:include>
)

# ======================
# Platform-specific settings
# ======================

if(APPLE)
  target_compile_options(${SYSTEM_TARGET_NAME} PRIVATE "-stdlib=libc++")
endif()

# MSVC runtime configuration
set_property(TARGET ${SYSTEM_TARGET_NAME} PROPERTY
  MSVC_RUNTIME_LIBRARY
  "MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<BOOL:${${SYSTEM_PROJECT_NAME_UPPER}_DYNAMIC_RUNTIME}>:DLL>"
)

# Visibility
set_target_properties(${SYSTEM_TARGET_NAME} PROPERTIES
  POSITION_INDEPENDENT_CODE ON
  CXX_VISIBILITY_PRESET hidden
  VISIBILITY_INLINES_HIDDEN YES
)

# Link libraries
target_link_libraries(${SYSTEM_TARGET_NAME}
  PUBLIC
    $<$<PLATFORM_ID:Linux>:dl>
    $<$<PLATFORM_ID:Windows>:ntdll shell32 advapi32>
)

# Compiler options
target_compile_options(${SYSTEM_TARGET_NAME} PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/MP /Zc:__cplusplus>
)

# ======================
# Tests
# ======================

if(${SYSTEM_PROJECT_NAME_UPPER}_BUILD_TESTS)

  add_executable(${SYSTEM_TARGET_NAME}_test
    ${SYSTEM_PROJECT_ROOT_DIR}/tests/main.cpp
  )

  target_link_libraries(${SYSTEM_TARGET_NAME}_test
    PRIVATE
      ${SYSTEM_TARGET_NAME_ALIAS}
  )

  target_compile_definitions(${SYSTEM_TARGET_NAME}_test PRIVATE
    $<$<BOOL:${${SYSTEM_PROJECT_NAME_UPPER}_BUILD_TESTS_DOTNET}>:
      SYSTEM_DOTNET_TEST_ASSEMBLY="${SYSTEM_PROJECT_ROOT_DIR}/tests/TestDotNetLoader/bin/Release/net6.0/publish/TestDotNetLoader.dll">
  )

  set_property(TARGET ${SYSTEM_TARGET_NAME}_test PROPERTY
    MSVC_RUNTIME_LIBRARY
    "MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<BOOL:${${SYSTEM_PROJECT_NAME_UPPER}_DYNAMIC_RUNTIME}>:DLL>"
  )

  # ----------------------
  # Shared test library
  # ----------------------

  add_library(${SYSTEM_TARGET_NAME}_shared SHARED
    ${SYSTEM_PROJECT_ROOT_DIR}/tests/shared_main.cpp
  )

  target_link_libraries(${SYSTEM_TARGET_NAME}_shared
    PRIVATE
      ${SYSTEM_TARGET_NAME_ALIAS}
  )

  # Remove "lib" prefix on Unix-like systems
  set_target_properties(${SYSTEM_TARGET_NAME}_shared PROPERTIES
    PREFIX ""
  )

  # MSVC runtime
  set_property(TARGET ${SYSTEM_TARGET_NAME}_shared PROPERTY
    MSVC_RUNTIME_LIBRARY
    "MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<BOOL:${${SYSTEM_PROJECT_NAME_UPPER}_DYNAMIC_RUNTIME}>:DLL>"
  )
  
  set_property(TARGET ${SYSTEM_TARGET_NAME}_shared PROPERTY
    MSVC_RUNTIME_LIBRARY
    "MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<BOOL:${${SYSTEM_PROJECT_NAME_UPPER}_DYNAMIC_RUNTIME}>:DLL>"
  )

endif()

# ======================
# Installation
# ======================

include(GNUInstallDirs)

install(TARGETS ${SYSTEM_TARGET_NAME}
  EXPORT ${SYSTEM_PROJECT_NAME}Targets
  FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(EXPORT ${SYSTEM_PROJECT_NAME}Targets
  FILE ${SYSTEM_PROJECT_NAME}Config.cmake
  NAMESPACE ${SYSTEM_PROJECT_NAMESPACE}::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${SYSTEM_PROJECT_NAME}
)

cmake_policy(SET CMP0091 NEW)
cmake_minimum_required(VERSION 3.15)
project(system)

if(APPLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

set(CMAKE_C_STANDARD_LIBRARIES "" CACHE STRING "" FORCE)
set(CMAKE_CXX_STANDARD_LIBRARIES "" CACHE STRING "" FORCE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(SYSTEM_DYNAMIC_RUNTIME "Link against dynamic runtime (Windows)" ON)
option(SYSTEM_BUILD_TESTS "Build tests." OFF)
option(SYSTEM_BUILD_TESTS_DOTNET "Build tests dotnet." OFF)

set(SYSTEM_HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/include/System/System.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/System/SystemMacro.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/System/Filesystem.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/System/Date.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/System/Library.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/System/SystemCompiler.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/System/SystemCPUExtensions.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/System/SystemDetector.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/System/SystemExports.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/System/SystemInline.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/System/String.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/System/ThreadPool.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/System/FunctionName.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/System/Encoding.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/System/ClassEnumUtils.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/System/ConstExpressions.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/System/Endianness.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/System/ScopedLock.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/System/StringSwitch.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/System/LoopBreak.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/System/Guid.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/System/FunctionTraits.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/System/DotNet.hpp
)

add_library(system
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CPUExtentions.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Encoding.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Filesystem.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Date.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Guid.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Library.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/String.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/System.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/System_internals.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/DotNet.cpp
  $<$<BOOL:${WIN32}>:${CMAKE_CURRENT_SOURCE_DIR}/src/WinRegister.h>
  $<$<BOOL:${WIN32}>:${CMAKE_CURRENT_SOURCE_DIR}/src/WinRegister.cpp>
  ${SYSTEM_HEADERS}
)

set_target_properties(system PROPERTIES
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<BOOL:${SYSTEM_DYNAMIC_RUNTIME}>:DLL>"
  POSITION_INDEPENDENT_CODE ON
  C_VISIBILITY_PRESET hidden
  CXX_VISIBILITY_PRESET hidden
  VISIBILITY_INLINES_HIDDEN ON
)

target_link_libraries(system
  PUBLIC
  $<$<BOOL:${UNIX}>:dl>
  
  $<$<BOOL:${WIN32}>:ntdll>
  $<$<BOOL:${WIN32}>:shell32>
  $<$<BOOL:${WIN32}>:advapi32>
)

target_compile_options(system
  PRIVATE
  $<$<BOOL:${MSVC}>:/MP>
  $<$<BOOL:${MSVC}>:/Zc:__cplusplus>
)

target_include_directories(system
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/deps/utfcpp/include>
  $<INSTALL_INTERFACE:include>
)

add_library(Nemirtingas::System ALIAS system)
set_target_properties(system PROPERTIES EXPORT_NAME System)

if(${SYSTEM_BUILD_TESTS})
  
add_executable(test_app
  tests/main.cpp
)

target_link_libraries(test_app
  PRIVATE
  Nemirtingas::System
)

target_compile_definitions(test_app
  PRIVATE
  $<$<BOOL:${SYSTEM_BUILD_TESTS_DOTNET}>:SYSTEM_DOTNET_TEST_ASSEMBLY="${CMAKE_CURRENT_SOURCE_DIR}/tests/TestDotNetLoader/bin/Release/net6.0/publish/TestDotNetLoader.dll">
)

set_property(TARGET test_app PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<BOOL:${SYSTEM_DYNAMIC_RUNTIME}>:DLL>")
  
add_library(shared SHARED
  tests/shared_main.cpp
)

target_link_libraries(shared
  PRIVATE
  Nemirtingas::System
)

set_target_properties(shared PROPERTIES PREFIX "")

set_property(TARGET shared PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<BOOL:${SYSTEM_DYNAMIC_RUNTIME}>:DLL>")
  
endif()

##################
## Install rules
install(TARGETS system EXPORT SystemTargets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

install(FILES ${SYSTEM_HEADERS}
  DESTINATION include/System
)

# Export targets
install(
  EXPORT SystemTargets
  FILE SystemConfig.cmake
  NAMESPACE Nemirtingas::
  DESTINATION lib/cmake/System
)


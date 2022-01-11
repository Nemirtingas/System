
cmake_policy(SET CMP0091 NEW)
project(system)
cmake_minimum_required(VERSION 3.15)

if(APPLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

set(CMAKE_C_STANDARD_LIBRARIES "" CACHE STRING "" FORCE)
set(CMAKE_CXX_STANDARD_LIBRARIES "" CACHE STRING "" FORCE)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(SYSTEM_CODE "Add cross-platform usefull functions." ON)
option(FILESYSTEM_CODE "Add some kind of C++17 filesystem code." ON)
option(LIBRARY_CODE "Add a Library class to handle modules loding/unloadling/infos." ON)

set(SYSTEM_HEADERS
  $<$<BOOL:${SYSTEM_CODE}>:${CMAKE_CURRENT_SOURCE_DIR}/include/System/System.h>
  $<$<BOOL:${FILESYSTEM_CODE}>:${CMAKE_CURRENT_SOURCE_DIR}/include/System/Filesystem.h>
  $<$<BOOL:${LIBRARY_CODE}>:${CMAKE_CURRENT_SOURCE_DIR}/include/System/Library.h>
  ${CMAKE_CURRENT_SOURCE_DIR}/include/System/SystemDetector.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/System/SystemExports.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/System/ClassEnumUtils.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/System/ConstExpressions.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/System/Endianness.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/System/ScopedLock.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/System/StringSwitch.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/System/StringUtils.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/System/StringView.hpp
)

if(${SYSTEM_CODE} OR ${FILESYSTEM_CODE} OR ${LIBRARY_CODE})
  add_library(system
    $<$<BOOL:${SYSTEM_CODE}>:${CMAKE_CURRENT_SOURCE_DIR}/src/System.cpp>
    $<$<BOOL:${FILESYSTEM_CODE}>:${CMAKE_CURRENT_SOURCE_DIR}/src/Filesystem.cpp>
    $<$<BOOL:${LIBRARY_CODE}>:${CMAKE_CURRENT_SOURCE_DIR}/src/Library.cpp>
    ${CMAKE_CURRENT_SOURCE_DIR}/src/System_internals.cpp
	${SYSTEM_HEADERS}
  )
  
  set_target_properties(system PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    POSITION_INDEPENDENT_CODE ON
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
  )
  
  target_link_libraries(system
    PUBLIC
    $<$<BOOL:${UNIX}>:dl>
    
    $<$<BOOL:${WIN32}>:shell32>
  )
  
  target_compile_options(system
    PRIVATE
    $<$<BOOL:${MSVC}>:/MP>
  )
  
  target_include_directories(system
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
  )
else()
  add_library(system INTERFACE)

  target_include_directories(system
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
  )
endif()

add_library(System::System ALIAS system)
set_target_properties(system PROPERTIES EXPORT_NAME System)

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
  NAMESPACE System::
  DESTINATION lib/cmake/System
)


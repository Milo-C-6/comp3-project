cmake_minimum_required(VERSION 3.24...3.30)
project(comp3-project CXX)
set(CMAKE_CXX_STANDARD 23)

include(FetchContent)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies
set(CMAKE_TLS_VERIFY false) # fricken school wifi
set(RAYLIB_VERSION 6.0)
find_package(raylib ${RAYLIB_VERSION} QUIET) 
if (NOT raylib_FOUND) 
  include(FetchContent)
  FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
  )
  FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_MakeAvailable(raylib)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) 
  endif()
endif()

set(RAYGUI_VERSION 4.0)
find_package(raygui ${RAYGUI_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raygui_FOUND) # If there's none, fetch and build raylib
  include(FetchContent)
  FetchContent_Declare(
    raygui
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raygui/archive/refs/heads/master.zip
  )
  FetchContent_GetProperties(raygui)
  if (NOT raygui_POPULATED) # Have we downloaded raylib yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_MakeAvailable(raygui)
    set(BUILD_RAYGUI_EXAMPLES OFF CACHE BOOL "" FORCE) 
    add_subdirectory(${raygui_SOURCE_DIR}/projects/CMake ${raygui_BINARY_DIR})
  endif()
endif()

# osdialog
# add_library(osdialog STATIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/osdialog/osdialog.c)

# target_include_directories(osdialog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/osdialog)
# if(WIN32)
  target_sources(osdialog PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/osdialog/osdialog_win.c)
  # target_link_libraries(osdialog PRIVATE "comdlg32")
# elseif(APPLE)
#   target_sources(osdialog PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/osdialog/osdialog_mac.m")
#   target_link_libraries(osdialog PRIVATE "${APPKIT_FRAMEWORK}")
# else()
#   target_sources(osdialog PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/osdialog/osdialog_gtk3.c")
#   target_link_libraries(osdialog PRIVATE PkgConfig::Gtk3)
# endif()

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src/include
  ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/exprtk
)

# Our Project
if(WIN32 AND MINGW) # so we can actually build exprtk
  set(CMAKE_CXX_FLAGS -Wa,-mbig-obj)
endif()

add_executable(${PROJECT_NAME})
add_subdirectory(src)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>)

if ("${PLATFORM}" STREQUAL "Web")
    add_custom_command(
        TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/../resources
    )
    #DEPENDS ${PROJECT_NAME}
else()
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
    )
    #DEPENDS ${PROJECT_NAME}
endif()

#set(raylib_VERBOSE 1)
target_link_libraries(${PROJECT_NAME} raylib raygui)

# Web Configurations
if ("${PLATFORM}" STREQUAL "Web")
    # Tell Emscripten to build an example.html file.
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
    target_link_options(${PROJECT_NAME} PUBLIC -sUSE_GLFW=3 PUBLIC --preload-file resources)
endif()

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
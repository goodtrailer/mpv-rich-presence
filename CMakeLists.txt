cmake_minimum_required(VERSION 3.18)

# Project ----------------------------------------------------------------------

project(mpv-rich-presence
    VERSION 0.0.0
    LANGUAGES CXX C)

add_library(mpv-rich-presence SHARED)

# Options ----------------------------------------------------------------------

option(mpv-rich-presence_INSTALL "Generate target for installing mpv-rich-presence" ${PROJECT_IS_TOP_LEVEL})
option(mpv-rich-presence_CLANG_TIDY "Use clang-tidy on build or for Visual Studio project" false)

# Dependencies -----------------------------------------------------------------

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

set(DISCORD_SOCIAL_SDK_DIR "" CACHE FILEPATH "path to Discord Social SDK root dir")
if(NOT IS_DIRECTORY "${DISCORD_SOCIAL_SDK_DIR}")
    message(FATAL_ERROR "DISCORD_SOCIAL_SDK_DIR is not set to a directory")
endif()
if(WIN32)
    set(DISCORD_LIB "${DISCORD_SOCIAL_SDK_DIR}/lib/release/discord_partner_sdk.lib")
    set(DISCORD_SHARED_LIB_DIR "${DISCORD_SOCIAL_SDK_DIR}/bin/release")
    set(DISCORD_SHARED_LIB "${DISCORD_SHARED_LIB_DIR}/discord_partner_sdk.dll")
elseif(APPLE)
    set(DISCORD_LIB "${DISCORD_SOCIAL_SDK_DIR}/lib/release/libdiscord_partner_sdk.dylib")
    set(DISCORD_SHARED_LIB_DIR "${DISCORD_SOCIAL_SDK_DIR}/lib/release")
    set(DISCORD_SHARED_LIB "${DISCORD_SHARED_LIB_DIR}/libdiscord_partner_sdk.dylib")
else()
    set(DISCORD_LIB "${DISCORD_SOCIAL_SDK_DIR}/lib/release/libdiscord_partner_sdk.so")
    set(DISCORD_SHARED_LIB_DIR "${DISCORD_SOCIAL_SDK_DIR}/lib/release")
    set(DISCORD_SHARED_LIB "${DISCORD_SHARED_LIB_DIR}/libdiscord_partner_sdk.so")
endif()

find_package(Boost CONFIG REQUIRED COMPONENTS dll)
find_package(CMakeRC CONFIG REQUIRED)
    cmrc_add_resource_library(mpv-rich-presence_resources
        ALIAS mpv-rich-presence::rc
        NAMESPACE mpvrp
        WHENCE "${DISCORD_SHARED_LIB_DIR}"
        PREFIX discord_social_sdk
        "${DISCORD_SHARED_LIB}")

# Source -----------------------------------------------------------------------

set(mpv-rich-presence_HPP
    discord_api
    discord_utils
    mpv_utils
    rich_presence_state)

set(mpv-rich-presence_CPP
    discord_api
    discord_utils
    mpv_rich_presence
    mpv_utils)

set(mpv-rich-presence_LUA
    rich-presence-conf)

set(mpv-rich-presence_CONF
    rich-presence)

list(TRANSFORM mpv-rich-presence_HPP
    PREPEND "include/mpv_rich_presence/")
list(TRANSFORM mpv-rich-presence_HPP
    APPEND ".hpp")

list(TRANSFORM mpv-rich-presence_CPP
    PREPEND "src/")
list(TRANSFORM mpv-rich-presence_CPP
    APPEND ".cpp")

list(TRANSFORM mpv-rich-presence_LUA
    PREPEND "scripts/")
list(TRANSFORM mpv-rich-presence_LUA
    APPEND ".lua")

list(TRANSFORM mpv-rich-presence_CONF
    PREPEND "script-opts/")
list(TRANSFORM mpv-rich-presence_CONF
    APPEND ".conf")

# Target mpv-rich-presence -----------------------------------------------------

set(CLANG_TIDY_FLAGS -checks=modernize-*,readability-*,-readability-braces-around-statements,-readability-identifier-length,-readability-magic-numbers,-readability-redundant-member-init,-readability-redundant-string-init,-clang-diagnostic-unused-command-line-argument)

set_target_properties(mpv-rich-presence PROPERTIES
    CXX_EXTENSIONS OFF
    CXX_STANDARD 20
    OUTPUT_NAME "rich-presence"
    PREFIX ""
    POSITION_INDEPENDENT_CODE ON)

if(mpv-rich-presence_CLANG_TIDY)
    set_target_properties(mpv-rich-presence PROPERTIES
        CXX_CLANG_TIDY "clang-tidy ${CLANG_TIDY_FLAGS}")

    if(MSVC)
        find_program(CLANG_TIDY_EXE clang-tidy)

        set_target_properties(mpv-rich-presence PROPERTIES
            VS_GLOBAL_EnableClangTidyCodeAnalysis true
            VS_GLOBAL_ClangTidyToolExe "${CLANG_TIDY_EXE}"
            VS_GLOBAL_ClangTidyChecks "${CLANG_TIDY_FLAGS}")
    endif()
endif()

if(WIN32)
    target_compile_definitions(mpv-rich-presence PRIVATE MPV_CPLUGIN_DYNAMIC_SYM)
endif()

target_sources(mpv-rich-presence PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/${mpv-rich-presence_HPP}
    ${CMAKE_CURRENT_SOURCE_DIR}/${mpv-rich-presence_CPP})

target_include_directories(mpv-rich-presence PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/mpv/include")

target_link_libraries(mpv-rich-presence PRIVATE
    Boost::dll
    mpv-rich-presence::rc)

# Target mpv-rich-presence::rc -------------------------------------------------

set_target_properties(mpv-rich-presence_resources PROPERTIES
    POSITION_INDEPENDENT_CODE ON)

# IDE --------------------------------------------------------------------------

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/${mpv-rich-presence_HPP}
          ${CMAKE_CURRENT_SOURCE_DIR}/${mpv-rich-presence_CPP})

if(MSVC)
    set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT mpv-rich-presence)
endif()

# Install ----------------------------------------------------------------------

set(mpv-rich-presence_INSTALL_DIR "mpv-rich-presence")
set(mpv-rich-presence_INSTALL_SCRIPTS "${mpv-rich-presence_INSTALL_DIR}/scripts")
set(mpv-rich-presence_INSTALL_SCRIPT_OPTS "${mpv-rich-presence_INSTALL_DIR}/script-opts")

if (mpv-rich-presence_INSTALL AND NOT CMAKE_SKIP_INSTALL_RULES)
    install(TARGETS mpv-rich-presence
        LIBRARY DESTINATION "${mpv-rich-presence_INSTALL_SCRIPTS}"
        RUNTIME DESTINATION "${mpv-rich-presence_INSTALL_SCRIPTS}")

    install(FILES ${mpv-rich-presence_LUA}
        DESTINATION "${mpv-rich-presence_INSTALL_SCRIPTS}")

    install(FILES ${mpv-rich-presence_CONF}
        DESTINATION "${mpv-rich-presence_INSTALL_SCRIPT_OPTS}")

    install(FILES
            "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE"
            "${CMAKE_CURRENT_SOURCE_DIR}/NOTICE"
        DESTINATION "${mpv-rich-presence_INSTALL_DIR}")

    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/licenses"
        DESTINATION "${mpv-rich-presence_INSTALL_DIR}")
endif()

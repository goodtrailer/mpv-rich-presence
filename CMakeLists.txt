cmake_minimum_required(VERSION 3.18)

project(mpv-rich-presence
    VERSION 0.0.0
    LANGUAGES CXX C)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

set(DISCORD_SOCIAL_SDK_DIR "" CACHE FILEPATH "path to Discord Social SDK root dir")
if(NOT IS_DIRECTORY "${DISCORD_SOCIAL_SDK_DIR}")
    message(FATAL_ERROR "DISCORD_SOCIAL_SDK_DIR is not set to a directory")
endif()

add_library(mpv-rich-presence SHARED)
set_target_properties(mpv-rich-presence PROPERTIES
    OUTPUT_NAME "rich-presence"
    POSITION_INDEPENDENT_CODE ON)

if(MSVC)
    set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT mpv-rich-presence)
endif()

set(mpv-rich-presence_HPP
    )

set(mpv-rich-presence_CPP
    main)

list(TRANSFORM mpv-rich-presence_HPP
    PREPEND "include/")
list(TRANSFORM mpv-rich-presence_HPP
    APPEND ".hpp")

list(TRANSFORM mpv-rich-presence_CPP
    PREPEND "src/")
list(TRANSFORM mpv-rich-presence_CPP
    APPEND ".cpp")

if(WIN32)
    set(DISCORD_LIB "${DISCORD_SOCIAL_SDK_DIR}/lib/release/discord_partner_sdk.lib")
    set(DISCORD_SHARED_LIB "${DISCORD_SOCIAL_SDK_DIR}/bin/release/discord_partner_sdk.dll")
elseif(APPLE)
    set(DISCORD_LIB "${DISCORD_SOCIAL_SDK_DIR}/lib/release/libdiscord_partner_sdk.dylib")
    set(DISCORD_SHARED_LIB "${DISCORD_SOCIAL_SDK_DIR}/lib/release/libdiscord_partner_sdk.dylib")
else()
    set(DISCORD_LIB "${DISCORD_SOCIAL_SDK_DIR}/lib/release/libdiscord_partner_sdk.so")
    set(DISCORD_SHARED_LIB "${DISCORD_SOCIAL_SDK_DIR}/lib/release/libdiscord_partner_sdk.so")
endif()

target_link_libraries(mpv-rich-presence PRIVATE
    "${DISCORD_LIB}")

if(WIN32)
    target_compile_definitions(mpv-rich-presence PRIVATE MPV_CPLUGIN_DYNAMIC_SYM)
endif()

if(UNIX)
    # Use RPATH when building
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    # Set the RPATH to use the lib directory relative to the executable
    set(CMAKE_INSTALL_RPATH "$ORIGIN")
    if(APPLE)
        set(CMAKE_INSTALL_RPATH "@executable_path")
    endif()
endif()

add_custom_command(TARGET mpv-rich-presence POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
    "${DISCORD_SHARED_LIB}"
    $<TARGET_FILE_DIR:mpv-rich-presence>;
)

add_custom_command(TARGET mpv-rich-presence POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E make_directory
    $<TARGET_FILE_DIR:mpv-rich-presence>/scripts/;)

add_custom_command(TARGET mpv-rich-presence POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
    $<TARGET_FILE:mpv-rich-presence>
    $<TARGET_FILE_DIR:mpv-rich-presence>/scripts/;)

add_custom_command(TARGET mpv-rich-presence POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory_if_different
    ${CMAKE_CURRENT_SOURCE_DIR}/scripts
    $<TARGET_FILE_DIR:mpv-rich-presence>/scripts;)

add_custom_command(TARGET mpv-rich-presence POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory_if_newer
    ${CMAKE_CURRENT_SOURCE_DIR}/script-opts
    $<TARGET_FILE_DIR:mpv-rich-presence>/script-opts;)

target_include_directories(mpv-rich-presence PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${DISCORD_SOCIAL_SDK_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/mpv/include")

target_sources(mpv-rich-presence PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/${mpv-rich-presence_HPP}
    ${CMAKE_CURRENT_SOURCE_DIR}/${mpv-rich-presence_CPP})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/${mpv-rich-presence_HPP}
          ${CMAKE_CURRENT_SOURCE_DIR}/${mpv-rich-presence_CPP})

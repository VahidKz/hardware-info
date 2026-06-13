cmake_minimum_required(VERSION 3.16)

project(
    HardwareInfo
    VERSION 1.0.0
    DESCRIPTION "A small Qt library for reading Linux hardware identifiers"
    LANGUAGES CXX
)

include(CTest)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(HARDWARE_INFO_BUILD_CLI "Build the hardware-info command-line application" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_library(HardwareInfo
    HardwareInfo.cpp
    HardwareInfo.h
    HardwareInfo_p.h
)
add_library(HardwareInfo::HardwareInfo ALIAS HardwareInfo)

target_link_libraries(HardwareInfo PUBLIC Qt${QT_VERSION_MAJOR}::Core)
target_include_directories(
    HardwareInfo
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
set_target_properties(
    HardwareInfo
    PROPERTIES
        PUBLIC_HEADER HardwareInfo.h
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(HardwareInfo PRIVATE -Wall -Wextra -Wpedantic)
elseif(MSVC)
    target_compile_options(HardwareInfo PRIVATE /W4)
endif()

if(HARDWARE_INFO_BUILD_CLI)
    add_executable(hardware-info main.cpp)
    target_link_libraries(hardware-info PRIVATE HardwareInfo::HardwareInfo)
    target_compile_definitions(
        hardware-info
        PRIVATE HARDWARE_INFO_VERSION="${PROJECT_VERSION}"
    )
endif()

if(BUILD_TESTING)
    add_executable(hardware-info-tests tests/HardwareInfoTests.cpp)
    target_link_libraries(hardware-info-tests PRIVATE HardwareInfo::HardwareInfo)
    add_test(NAME hardware-info-tests COMMAND hardware-info-tests)
endif()

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/HardwareInfoConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
configure_package_config_file(
    cmake/HardwareInfoConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/HardwareInfoConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/HardwareInfo"
)

install(
    TARGETS HardwareInfo
    EXPORT HardwareInfoTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
if(HARDWARE_INFO_BUILD_CLI)
    install(TARGETS hardware-info RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
install(
    EXPORT HardwareInfoTargets
    FILE HardwareInfoTargets.cmake
    NAMESPACE HardwareInfo::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/HardwareInfo"
)
install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/HardwareInfoConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/HardwareInfoConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/HardwareInfo"
)

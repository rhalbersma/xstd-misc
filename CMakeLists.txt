#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.28)
project(
    xstd-misc
    VERSION 0.1.0
    DESCRIPTION "Miscellaneous extensions to the C++ Standard Library"
    HOMEPAGE_URL "https://github.com/rhalbersma/xstd-misc"
    LANGUAGES CXX
)

include(GNUInstallDirs)

# The strictly conforming dialect, for every target this project creates.
# Set before any of them; consumers keep control over their own.
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(
    ${PROJECT_NAME} INTERFACE
)

# Consumed as xstd::misc, following Boost: the package keeps the repository's name
# (find_package(xstd-misc)) while the target is a component of one umbrella namespace,
# so xstd::misc sits beside xstd::ints and xstd::bits the way Boost::mp11 sits beside
# Boost::hash2. EXPORT_NAME is what lets the exported name differ from the target name;
# the alias below must spell the same thing, or an add_subdirectory consumer and a
# find_package consumer would end up with two different names for one library.
set_target_properties(${PROJECT_NAME} PROPERTIES EXPORT_NAME misc)
add_library(xstd::misc ALIAS ${PROJECT_NAME})

# No target_include_directories: the HEADERS file set below adds its base
# directory, and install(TARGETS ... FILE_SET) adds the install destination.
target_compile_features(
    ${PROJECT_NAME} INTERFACE
    cxx_std_23
)

target_sources(
    ${PROJECT_NAME} INTERFACE
    FILE_SET HEADERS
    BASE_DIRS include
    FILES
        include/xstd/misc.hpp
        include/xstd/misc/concepts.hpp
        include/xstd/misc/concepts/specialization_of.hpp
        include/xstd/misc/type_traits.hpp
        include/xstd/misc/type_traits/conditional_data_member.hpp
        include/xstd/misc/type_traits/empty_type.hpp
        include/xstd/misc/type_traits/is_specialization_of.hpp
        include/xstd/misc/type_traits/no_unique_address.hpp
        include/xstd/misc/utility.hpp
        include/xstd/misc/utility/to_underlying.hpp
)

# Tests and their Boost.Test dependency only exist when xstd-misc is being
# developed; a consumer vendoring this directory gets the header-only target.
if(PROJECT_IS_TOP_LEVEL)
    include(CTest)
    if(BUILD_TESTING)
        add_subdirectory(test)
    endif()
endif()

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    FILE_SET HEADERS
)
install(
    EXPORT ${PROJECT_NAME}Targets
    NAMESPACE xstd::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

# So consumers can find_package(xstd-misc CONFIG). ARCH_INDEPENDENT because a
# header-only library has no architecture-specific binaries.
include(CMakePackageConfigHelpers)
configure_package_config_file(
    cmake/${PROJECT_NAME}Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    COMPATIBILITY SameMinorVersion
    ARCH_INDEPENDENT
)
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

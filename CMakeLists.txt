cmake_minimum_required(VERSION 3.12)
project(
        piperine
        LANGUAGES CXX
        VERSION 0.0.1
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("-- Enabling debug symbols")
    add_compile_definitions(PIPERINE_ENABLE_VALIDATION PIPERINE_DEBUG)
endif()

set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

#add_executable(Piperine_old src/utility.h src/vulkan_types.cpp src/vulkan_types.h src/vulkan_containers.h src/window.cpp src/window.h src/utility.cpp src/device.cpp src/device.h src/swapchain.cpp src/swapchain.h src/pipeline.cpp src/pipeline.h src/commands.cpp src/commands.h src/application.cpp src/application.h src/main.cpp src/shader_types.h src/attribute_generator.h src/buffers.cpp src/buffers.h src/vma_impl.cpp src/info_types.h src/mesh.h src/mesh.cpp src/input_generator.h src/texture.cpp src/texture.h)
add_library(piperine
        lib/interface.cpp
        lib/utility.cpp
        lib/vma_impl.cpp
        lib/vulkan_types.cpp

        include/piperine/interface.h
        include/piperine/macros.h
        include/piperine/synchronization.h
        include/piperine/utility.h
        include/piperine/vulkan_types.h
        include/piperine/definitions.h
        include/piperine/vulkan_utility.h
        include/piperine/public_utility.h
        lib/public_utility.cpp include/piperine/instance.h lib/instance.cpp include/piperine/messenger.h lib/messenger.cpp include/piperine/device.h lib/device.cpp include/piperine/allocators.h lib/allocators.cpp include/piperine/swapchain.h lib/swapchain.cpp include/piperine/interface_builder.h include/piperine/vulkan_types_forward_declarations.h)
add_library(pip::piperine ALIAS piperine)

option(PIPERINE_BUILD_EXAMPLES "Build sample project with library" OFF)
if(PIPERINE_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()

#target_include_directories(piperine PUBLIC ${Vulkan_INCLUDE_DIRS} ${glfw3_INCLUDE_DIRS} ${glm_INCLUDE_DIRS})
target_include_directories(piperine PRIVATE ${CMAKE_SOURCE_DIR}/extern/include)
target_link_libraries(piperine PUBLIC vulkan glfw glm)

option(BUILD_SHARED_LIBS "Build shared library" ON)
include(GenerateExportHeader)
generate_export_header(piperine
        EXPORT_MACRO_NAME PIPERINE_API
        EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/include/common.h
)

target_compile_features(piperine PUBLIC cxx_std_20)
target_compile_options(piperine PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        -pipe -march=native
        $<$<CONFIG:Debug>:-O0 -Wall -Wextra>>
        $<$<CXX_COMPILER_ID:MSVC>:
        $<$<CONFIG:Debug>:/Od /Wall /Zi>>
)

target_include_directories(piperine
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(piperine PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

include(GNUInstallDirs)

install(TARGETS piperine
        EXPORT piperine-targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${LIBLEGACY_INCLUDE_DIRS}
)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(DIRECTORY ${CMAKE_BINARY_DIR}/include
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT piperine-targets
        FILE piperine-targets.cmake
        NAMESPACE pip::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/piperine
)

include(CMakePackageConfigHelpers)

configure_package_config_file(
        ${CMAKE_SOURCE_DIR}/cmake/piperine-config.cmake
        ${CMAKE_BINARY_DIR}/cmake/piperine-config.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/piperine
)

write_basic_package_version_file(
        ${CMAKE_BINARY_DIR}/cmake/piperine-config-version.cmake
        VERSION ${PIPERINE_VERSION}
        COMPATIBILITY AnyNewerVersion
)

install(
        FILES
        ${CMAKE_BINARY_DIR}/cmake/piperine-config.cmake
        ${CMAKE_BINARY_DIR}/cmake/piperine-config-version.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/piperine
)

export(EXPORT piperine-targets
        FILE ${CMAKE_BINARY_DIR}/cmake/piperine-targets.cmake
        NAMESPACE pip::
)

export(PACKAGE piperine)


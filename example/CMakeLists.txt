cmake_minimum_required(VERSION 3.12)
message("-- Building example project")

add_executable(piperine-example
        "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
)

target_link_libraries(piperine-example PRIVATE
        pip::piperine
)

file(GLOB SHADERS_SRC "shaders/*.frag" "shaders/*.vert")
file(GLOB SHADERS_SPV "shaders/*.spv")

add_custom_command(
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ${SHADERS_SRC}
        OUTPUT ${SHADERS_SPV}
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/etc/pre_compilation.sh
        COMMENT "Compiling shaders"
)

add_custom_target(piperineShaders DEPENDS ${SHADERS_SPV})
add_dependencies(piperine-example piperineShaders)
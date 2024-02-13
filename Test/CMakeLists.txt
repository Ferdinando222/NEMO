cmake_minimum_required(VERSION 3.18 FATAL_ERROR)
project(test_models)

set(CMAKE_PREFIX_PATH "C:/libtorch")
find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

set(RTNEURAL_STL ON CACHE BOOL "Use RTNeural with this backend" FORCE)
add_subdirectory(../RTNeural ${CMAKE_CURRENT_BINARY_DIR}/RTNeural)

include_directories(RTNeural)

add_executable(test_models src/main.cpp)
target_link_libraries(test_models "${TORCH_LIBRARIES}")
target_link_libraries(test_models RTNeural)
set_property(TARGET test_models PROPERTY CXX_STANDARD 17)


# The following code block is suggested to be used on Windows.
# According to https://github.com/pytorch/pytorch/issues/25457,
# the DLLs need to be copied to avoid memory errors.
if (MSVC)
  file(GLOB TORCH_DLLS "${TORCH_INSTALL_PREFIX}/lib/*.dll")
  add_custom_command(TARGET test_models
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy_if_different
                     ${TORCH_DLLS}
                     $<TARGET_FILE_DIR:test_models>)
endif (MSVC)
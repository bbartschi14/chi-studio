cmake_minimum_required(VERSION 3.6)
project(ChiStudio)

message(STATUS "Building ChiStudio")

# Otherwise IMGUI may pick GLEW as GL loader
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DIMGUI_IMPL_OPENGL_LOADER_GLAD")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if (MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2")
else()
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
endif()
if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected; default to release.")
    set(CMAKE_BUILD_TYPE "Release")
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(cxx_warning_flags "-Wall")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(cxx_warning_flags "-Wall")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVCU")
    set(cxx_warning_flags "/W4")
endif()

message("Using CXX compiler: ${CMAKE_CXX_COMPILER}")
message("             flags: ${CMAKE_CXX_FLAGS}")

# Allow custom CMake configurations.
include(${PROJECT_SOURCE_DIR}/CMakeCustomLists.txt OPTIONAL)

# Allow personal CMake configurations.
include(${PROJECT_SOURCE_DIR}/CMakePersonalLists.txt OPTIONAL)

# Set dependencies for external libraries.
set(external_install_dir ${PROJECT_SOURCE_DIR}/external/install/)
set(external_source_dir ${PROJECT_SOURCE_DIR}/external/src/)

set(external_libs "")
set(external_srcs "")

# GLFW
find_package(
    glfw3
    QUIET
    PATHS ${external_install_dir}/lib/cmake/glfw3
    NO_DEFAULT_PATH
)
if (NOT glfw3_FOUND)
    message(STATUS "No installed GLFW found in ${external_install_dir}.
    Include in build.")
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    add_subdirectory(${external_source_dir}/glfw-3.3.2)
else()
    message(STATUS "Found GLFW installed in ${external_install_dir}.")
endif()
list(APPEND external_libs glfw)

# GLAD
include_directories(${external_source_dir}/glad/include)
list(APPEND external_srcs ${external_source_dir}/glad/src/glad.c)

# GLM
find_package(
    glm
    REQUIRED
    PATHS ${external_source_dir}/glm-0.9.9.8/cmake/glm
    NO_DEFAULT_PATH
)
list(APPEND external_libs glm::glm)

#YAML
add_subdirectory(${external_source_dir}/third-party/yaml-cpp)
list(APPEND external_libs yaml-cpp)
include_directories(${external_source_dir}/third-party/yaml-cpp/include)

# ImGui
set(imgui_dir ${external_source_dir}/imgui)
list(APPEND external_srcs
    ${imgui_dir}/imgui.cpp
    ${imgui_dir}/imgui_demo.cpp
    ${imgui_dir}/imgui_draw.cpp
    ${imgui_dir}/imgui_tables.cpp
    ${imgui_dir}/imgui_widgets.cpp
    ${imgui_dir}/imgui_internal.h
    ${imgui_dir}/backends/imgui_impl_glfw.cpp
    ${imgui_dir}/backends/imgui_impl_opengl3.cpp)

include_directories(${imgui_dir} ${imgui_dir}/backends)

#ImGuizmo
set(imguizmo_dir ${external_source_dir}/ImGuizmo)
list(APPEND external_srcs
    ${imguizmo_dir}/ImGuizmo.cpp)

#fmt
set(fmt_dir ${external_source_dir}/fmt)
list(APPEND external_srcs
    ${fmt_dir}/core.h
    ${fmt_dir}/format.cc
    ${fmt_dir}/format.h
    ${fmt_dir}/format-inl.h)

include_directories(${external_source_dir}/fmt)

# stb
include_directories(${external_source_dir}/stb)

###################################################
# Add path macros.
set(graphics_dir ${PROJECT_SOURCE_DIR}/ChiGraphics)

###################################################

include_directories(${PROJECT_SOURCE_DIR})
file(GLOB graphics_srcs
    ${graphics_dir}/*.cpp
    ${graphics_dir}/GL_Wrapper/*.cpp
    ${graphics_dir}/Components/*.cpp
    ${graphics_dir}/Lights/*.cpp
    ${graphics_dir}/Meshes/*.cpp
    ${graphics_dir}/Meshes/Primitives/*.cpp
    ${graphics_dir}/Shaders/*.cpp
    ${graphics_dir}/Shaders/glsl/*.frag
    ${graphics_dir}/Shaders/glsl/*.vert
    ${graphics_dir}/Cameras/*.cpp
    ${graphics_dir}/Materials/*.cpp
    ${graphics_dir}/Textures/*.cpp
    ${graphics_dir}/Collision/*.cpp
    ${graphics_dir}/Collision/Hittables/*.cpp
    ${graphics_dir}/RayTracing/*.cpp
    ${graphics_dir}/Modifiers/*.cpp
)

###################################################

set(core_dir ${PROJECT_SOURCE_DIR}/ChiCore)
include_directories(${core_dir})
file(GLOB core_source_files
    ${core_dir}/*.cpp
    ${core_dir}/UI/*.cpp
    ${core_dir}/Serialization/*.cpp
)


file(GLOB header_files
    ${graphics_dir}/*.h
    ${graphics_dir}/*/*.h
    ${graphics_dir}/*/*/*.h
    ${core_dir}/*.h
    ${core_dir}/*/*.h
    ${imgui_dir}/*.hpp
    ${imgui_dir}/*/*.hpp
    ${imguizmo_dir}/*.h
    ${imguizmo_dir}/*/*.h
)

set(all_files ${core_source_files};${external_srcs};${graphics_srcs};${header_files})

foreach (source IN LISTS all_files)
    file(RELATIVE_PATH source_rel ${CMAKE_CURRENT_LIST_DIR} ${source})
    get_filename_component(source_path "${source_rel}" PATH)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    source_group("${source_path_msvc}" FILES "${source}")
endforeach ()

add_executable(ChiStudio ${graphics_srcs} ${external_srcs} ${core_source_files} ${header_files})
#target_link_libraries(ChiStudio ${YAML_CPP_LIBRARIES}) # This is the "target" exposed by # yaml-cpp's CMakeLists.txt file
target_link_libraries(ChiStudio ${external_libs})
target_compile_options(ChiStudio PRIVATE ${cxx_warning_flags})

if (MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ChiStudio)
endif ()

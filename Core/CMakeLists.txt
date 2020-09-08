#cmake_minimum_required (VERSION 2.8)

#project(Core)

aux_source_directory(. DIR_LIB_SRCS)

add_executable(Core ${DIR_LIB_SRCS}
${PROJECT_SOURCE_DIR}/imgui-master/examples/imgui_impl_opengl3.cpp
${PROJECT_SOURCE_DIR}/imgui-master/examples/imgui_impl_glfw.cpp
${PROJECT_SOURCE_DIR}/imgui-master/imgui_draw.cpp
${PROJECT_SOURCE_DIR}/imgui-master/imgui.cpp
${PROJECT_SOURCE_DIR}/imgui-master/imgui_widgets.cpp
${PROJECT_SOURCE_DIR}/imgui-master/imgui_demo.cpp
)

include_directories(${PROJECT_SOURCE_DIR}/include)

include_directories(${PROJECT_SOURCE_DIR}/imgui-master)

include_directories(${PROJECT_SOURCE_DIR}/imgui-master/examples)

include_directories(${PROJECT_SOURCE_DIR}/assimp-4.0.0/include)

include_directories(${PROJECT_SOURCE_DIR}/assimp-4.0.0/build/include)

target_link_libraries(Core ${PROJECT_BINARY_DIR}/glfw-3.3.2/src/Debug/glfw3.lib)

target_link_libraries(Core ${PROJECT_SOURCE_DIR}/assimp-4.0.0/build/code/Debug/assimp-vc140-mt.lib)
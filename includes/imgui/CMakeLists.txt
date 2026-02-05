add_library(imgui STATIC
        imgui.cpp
        imgui_draw.cpp
        imgui_widgets.cpp
        imgui_tables.cpp
        imgui_demo.cpp
        backends/imgui_impl_glfw.cpp
        backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/backends
        ${GLFW3_INCLUDE_DIR}    # 来自根目录 find_package(GLFW3)
        ${OPENGL_INCLUDE_DIR}   # 来自根目录（部分系统需要）
)

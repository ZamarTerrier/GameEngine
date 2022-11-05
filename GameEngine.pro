TEMPLATE = lib
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += object_parallel_to_source

SOURCES += \
    bindDescription.c \
    buffers.c \
    camera.c \
    debuger.c \
    device.c \
    e_direct.c \
    e_math.c \
    e_resource.c \
    e_widget.c \
    e_widget_button.c \
    e_widget_combobox.c \
    e_widget_entry.c \
    e_widget_entry_area.c \
    e_widget_image.c \
    e_widget_list.c \
    e_widget_range.c \
    e_widget_text.c \
    e_widget_window.c \
    engine.c \
    fbxLoader.c \
    gameObject.c \
    gameObject2D.c \
    gameObject3D.c \
    glTFLoader.c \
    graphicsObject.c \
    models.c \
    objLoader.c \
    particleSystem.c \
    pipeline.c \
    primitiveObject.c \
    projectionPlaneObject.c \
    rayIntersections.c \
    spriteObject.c \
    swapchain.c \
    textObject.c \
    texture.c \
    tools.c \
    transform.c \
    ufbx.c \
    window.c

HEADERS += \
    bindDesciption.h \
    buffers.h \
    camera.h \
    cgltf.h \
    cgltf_write.h \
    debuger.h \
    device.h \
    e_direct.h \
    e_math.h \
    e_resource.h \
    e_widget.h \
    e_widget_button.h \
    e_widget_combobox.h \
    e_widget_entry.h \
    e_widget_entry_area.h \
    e_widget_image.h \
    e_widget_list.h \
    e_widget_range.h \
    e_widget_text.h \
    e_widget_window.h \
    engine.h \
    fbxLoader.h \
    gameObject.h \
    gameObject2D.h \
    gameObject3D.h \
    glTFLoader.h \
    graphicsItems.h \
    graphicsObject.h \
    models.h \
    objLoader.h \
    particleSystem.h \
    pipeline.h \
    primitiveObject.h \
    projectionPlaneObject.h \
    rayIntersections.h \
    spriteObject.h \
    stb_image.h \
    stb_truetype.h \
    std_intersect.h \
    stdinclude.h \
    swapchain.h \
    textObject.h \
    texture.h \
    tinyobj_loader.h \
    tools.h \
    transform.h \
    ufbx.h \
    window.h

OBJECTS += \
    bin/shaders/gui/widget_frag.o \
    bin/shaders/gui/widget_vert.o \
    bin/shaders/gui/widget_window_frag.o \
    bin/shaders/gui/widget_window_vert.o \
    bin/shaders/model/frag.o \
    bin/shaders/model/vert.o \
    bin/fonts/fantazer-normal.o \
    bin/fonts/arial.o \
    bin/shaders/text/frag.o \
    bin/shaders/text/vert.o \
    bin/shaders/sprite/frag.o \
    bin/shaders/sprite/vert.o \
    bin/shaders/3d_object/frag.o \
    bin/shaders/3d_object/vert.o \
    bin/textures/default/error.o \

DEFINES += CGLTF_IMPLEMENTATION STB_IMAGE_IMPLEMENTATION MATH_IMPLEMENTATION TINYOBJ_LOADER_C_IMPLEMENTATION _WIN_BUILD_

unix:!macx: LIBS += -L$$PWD/../cglm/build/ -lcglm

INCLUDEPATH += $$PWD/../cglm/include
DEPENDPATH += $$PWD/../cglm/include

unix|win32: LIBS += -L$$PWD/../glfw-3.3.5.bin.WIN64/lib-mingw-w64/ -lglfw3dll

INCLUDEPATH += $$PWD/../glfw-3.3.5.bin.WIN64/include
DEPENDPATH += $$PWD/../glfw-3.3.5.bin.WIN64/include

unix|win32: LIBS += -LD:/VulkanSDK/1.2.198.0/Lib/ -lvulkan-1

INCLUDEPATH += D:/VulkanSDK/1.2.198.0/Include
DEPENDPATH += D:/VulkanSDK/1.2.198.0/Include

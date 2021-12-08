TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        bindDescription.c \
        buffers.c \
        debuger.c \
        device.c \
        engine.c \
        gameObject.c \
        main.c \
        pipeline.c \
        resource.c \
        swapchain.c \
        texture.c \
        tools.c \
        window.c

LIBS += -lglfw3 -lvulkan -ldl -lpthread -lX11 -lm -lXrandr -lXi

DISTFILES += \
    Graphics.pro.user \
    shaders/shader.frag \
    shaders/shader.vert

HEADERS += \
    bindDesciption.h \
    buffers.h \
    debuger.h \
    device.h \
    engine.h \
    gameObject.h \
    pipeline.h \
    resource.h \
    stdinclude.h \
    swapchain.h \
    texture.h \
    tools.h \
    window.h

DEFINES += STB_IMAGE_IMPLEMENTATION

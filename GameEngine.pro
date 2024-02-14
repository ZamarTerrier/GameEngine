TEMPLATE = lib
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

OBJECTS += data/data1.bin

INCLUDEPATH += includes/ libs/

win32: LIBS += -LD:/VulkanSDK/1.2.198.0/Lib/ -lvulkan-1

INCLUDEPATH += D:/VulkanSDK/1.2.198.0/Include
DEPENDPATH += D:/VulkanSDK/1.2.198.0/Include

HEADERS += \
    includes/Core/e_blue_print.h \
    includes/Core/e_buffer.h \
    includes/Core/e_camera.h \
    includes/Core/e_debuger.h \
    includes/Core/e_descriptor.h \
    includes/Core/e_device.h \
    includes/Core/e_texture.h \
    includes/Core/e_transform.h \
    includes/Core/e_window.h \
    includes/Core/engine.h \
    includes/Core/graphicsItems.h \
    includes/Core/graphicsObject.h \
    includes/Core/pipeline.h \
    includes/Core/swapchain.h \
    includes/Data/e_resource_data.h \
    includes/Data/e_resource_descriptors.h \
    includes/Data/e_resource_engine.h \
    includes/Data/e_resource_export.h \
    includes/Data/e_resource_shapes.h \
    includes/GUI/e_widget.h \
    includes/GUI/e_widget_button.h \
    includes/GUI/e_widget_combobox.h \
    includes/GUI/e_widget_entry.h \
    includes/GUI/e_widget_entry_area.h \
    includes/GUI/e_widget_image.h \
    includes/GUI/e_widget_list.h \
    includes/GUI/e_widget_range.h \
    includes/GUI/e_widget_roller.h \
    includes/GUI/e_widget_scroll.h \
    includes/GUI/e_widget_text.h \
    includes/GUI/e_widget_top_menu.h \
    includes/GUI/e_widget_window.h \
    includes/Objects/e_audioObject.h \
    includes/Objects/e_painter.h \
    includes/Objects/e_render_object.h \
    includes/Objects/gameObject.h \
    includes/Objects/gameObject2D.h \
    includes/Objects/gameObject3D.h \
    includes/Objects/grass_object.h \
    includes/Objects/lightObject.h \
    includes/Objects/marchingcube_object.h \
    includes/Objects/models.h \
    includes/Objects/particleSystem2D.h \
    includes/Objects/particleSystem3D.h \
    includes/Objects/primitiveObject.h \
    includes/Objects/projectionPlaneObject.h \
    includes/Objects/quad_object.h \
    includes/Objects/render_texture.h \
    includes/Objects/shapeObject.h \
    includes/Objects/sky_object.h \
    includes/Objects/spriteObject.h \
    includes/Objects/terrain_object.h \
    includes/Objects/textObject.h \
    includes/Objects/tree_object.h \
    includes/Objects/water_object.h \
    includes/Tools/e_direct.h \
    includes/Tools/e_math.h \
    includes/Tools/e_tools.h \
    includes/Tools/fbxLoader.h \
    includes/Tools/glTFLoader.h \
    includes/Tools/intersections2D.h \
    includes/Tools/intersections3D.h \
    includes/Tools/json_parcer.h \
    includes/Tools/objLoader.h \
    includes/Tools/rayIntersections2D.h \
    includes/Tools/rayIntersections3D.h \
    includes/Tools/shader_builder.h \
    includes/Variabels/e_buffers_variables.h \
    includes/Variabels/e_device_variables.h \
    includes/Variabels/e_math_variables.h \
    includes/Variabels/e_pipeline_variables.h \
    includes/Variabels/e_texture_variables.h \
    includes/Variabels/e_vertex.h \
    includes/Variabels/engine_includes.h \
    includes/Variabels/model_include.h \
    includes/Variabels/std_intersect.h \
    includes/wManager/input_manager.h \
    includes/wManager/manager_includes.h \
    includes/wManager/win_defines.h \
    includes/wManager/window_manager.h \
    includes/wManager/x11_defines.h \
    libs/cgltf.h \
    libs/miniaudio.h \
    libs/spirv.h \
    libs/stb_image.h \
    libs/stb_image_resize.h \
    libs/stb_image_write.h \
    libs/stb_json.h \
    libs/stb_truetype.h \
    libs/tinyobj_loader.h \
    libs/ufbx.h \
    libs/vk_platform.h

SOURCES += \
    libs/ufbx.c \
    src/Core/e_blue_print.c \
    src/Core/e_buffer.c \
    src/Core/e_camera.c \
    src/Core/e_debuger.c \
    src/Core/e_descriptor.c \
    src/Core/e_device.c \
    src/Core/e_texture.c \
    src/Core/e_transform.c \
    src/Core/e_window.c \
    src/Core/engine.c \
    src/Core/graphicsObject.c \
    src/Core/pipeline.c \
    src/Core/swapchain.c \
    src/Data/e_resource_data.c \
    src/Data/e_resource_descriptors.c \
    src/Data/e_resource_engine.c \
    src/Data/e_resource_shapes.c \
    src/GUI/e_widget.c \
    src/GUI/e_widget_button.c \
    src/GUI/e_widget_combobox.c \
    src/GUI/e_widget_entry.c \
    src/GUI/e_widget_entry_area.c \
    src/GUI/e_widget_image.c \
    src/GUI/e_widget_list.c \
    src/GUI/e_widget_range.c \
    src/GUI/e_widget_roller.c \
    src/GUI/e_widget_scroll.c \
    src/GUI/e_widget_text.c \
    src/GUI/e_widget_top_menu.c \
    src/GUI/e_widget_window.c \
    src/Objects/e_audioObject.c \
    src/Objects/e_painter.c \
    src/Objects/e_render_object.c \
    src/Objects/gameObject.c \
    src/Objects/gameObject2D.c \
    src/Objects/gameObject3D.c \
    src/Objects/grass_object.c \
    src/Objects/lightObject.c \
    src/Objects/marchingcube_object.c \
    src/Objects/models.c \
    src/Objects/particleSystem2D.c \
    src/Objects/particleSystem3D.c \
    src/Objects/primitiveObject.c \
    src/Objects/projectionPlaneObject.c \
    src/Objects/quad_object.c \
    src/Objects/render_texture.c \
    src/Objects/shapeObject.c \
    src/Objects/sky_object.c \
    src/Objects/spriteObject.c \
    src/Objects/terrain_object.c \
    src/Objects/textObject.c \
    src/Objects/tree_object.c \
    src/Objects/water_object.c \
    src/Tools/e_direct.c \
    src/Tools/e_math.c \
    src/Tools/e_tools.c \
    src/Tools/fbxLoader.c \
    src/Tools/glTFLoader.c \
    src/Tools/intersections2D.c \
    src/Tools/intersections3D.c \
    src/Tools/objLoader.c \
    src/Tools/rayIntersections2D.c \
    src/Tools/rayIntersections3D.c \
    src/Tools/shader_builder.c \
    src/wManager/input_manager.c \
    src/wManager/posix_poll.c \
    src/wManager/vulkan.c \
    src/wManager/win_init.c \
    src/wManager/win_monitor.c \
    src/wManager/win_tools.c \
    src/wManager/win_window.c \
    src/wManager/window.c \
    src/wManager/window_manager.c \
    src/wManager/x11_init.c \
    src/wManager/x11_monitor.c \
    src/wManager/x11_tools.c \
    src/wManager/x11_window.c


DEFINES += _WIN_

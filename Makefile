OBJECTS := obj/engine.o \
	obj/bindDescription.o \
	obj/buffers.o \
	obj/camera.o \
	obj/debuger.o \
	obj/device.o \
	obj/e_audioObject.o \
	obj/e_direct.o \
	obj/e_math.o \
	obj/e_painter.o \
	obj/e_resource_data.o \
	obj/e_resource_descriptors.o \
	obj/e_resource_engine.o \
	obj/e_resource_shapes.o \
	obj/e_widget_button.o \
	obj/e_widget_combobox.o \
	obj/e_widget_entry_area.o \
	obj/e_widget_entry.o \
	obj/e_widget_image.o \
	obj/e_widget_list.o \
	obj/e_widget_range.o \
	obj/e_widget_roller.o \
	obj/e_widget_scroll.o \
	obj/e_widget_text.o \
	obj/e_widget_top_menu.o \
	obj/e_widget_window.o \
	obj/e_widget.o \
	obj/fbxLoader.o \
	obj/gameObject.o \
	obj/gameObject2D.o \
	obj/gameObject3D.o \
	obj/glTFLoader.o \
	obj/graphicsObject.o \
	obj/intersections2D.o \
	obj/intersections3D.o \
	obj/lightObject.o \
	obj/models.o \
	obj/objLoader.o \
	obj/particleSystem2D.o \
	obj/particleSystem3D.o \
	obj/pipeline.o \
	obj/primitiveObject.o \
	obj/projectionPlaneObject.o \
	obj/rayIntersections2D.o \
	obj/rayIntersections3D.o \
	obj/shapeObject.o \
	obj/spriteObject.o \
	obj/swapchain.o \
	obj/textObject.o \
	obj/texture.o \
	obj/tools.o \
	obj/transform.o \
	obj/window.o \
	obj/ufbx.o \
	
BINFILES += bin/shaders/gui/widget_frag.o \
    bin/shaders/gui/widget_vert.o \
    bin/shaders/gui/widget_window_frag.o \
    bin/shaders/gui/widget_window_vert.o \
    bin/shaders/gui/painter_vert.o \
    bin/shaders/gui/painter_vert.o \
    bin/shaders/model/frag.o \
    bin/shaders/model/vert.o \
    bin/shaders/text/frag.o \
    bin/shaders/text/vert.o \
    bin/shaders/sprite/frag.o \
    bin/shaders/sprite/vert.o \
    bin/shaders/3d_object/frag.o \
    bin/shaders/3d_object/vert.o \
    bin/shaders/3d_object/line_frag.o \
    bin/shaders/3d_object/line_vert.o \
    bin/shaders/particle/frag2D.o \
    bin/shaders/particle/vert2D.o \
    bin/shaders/particle/frag3D.o \
    bin/shaders/particle/vert3D.o \
    bin/fonts/fantazer-normal.o \
    bin/fonts/RobotoBlack.o \
    bin/fonts/TenorSans.o \
    bin/fonts/null-normal.o \
 
all : clean mdir libGameEngine.so

obj/%.o : src/%.c
	gcc -c -fPIC $< -o $@ -Iincludes -Ilibs
	
obj/%.o : libs/%.c
	gcc -c -fPIC $< -o $@ -Iincludes -Ilibs

libGameEngine.so : $(BINFILES) $(OBJECTS)
	gcc -shared $^ -o $@ -Iincludes
	
mdir : 
	mkdir obj

clean : 
	$(RM) -r obj 
	$(RM) libGameEngine.so

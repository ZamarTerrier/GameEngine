#include "engine.h"


void Run(){
	while(!glfwWindowShouldClose(window)){
		glfwPollEvents();
        engineLoop();
	}

    vkDeviceWaitIdle(device);
}

int main(){
    init();

	Run();

    cleanUp();

    return 0;
}

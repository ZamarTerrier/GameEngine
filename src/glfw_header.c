#include "glfw_header.h"

#include <dlfcn.h>

PFN_glfwSetCursorPosCallback glfwSetCursorPosCallback;
PFN_glfwCreateWindow glfwCreateWindow;
PFN_glfwCreateWindowSurface glfwCreateWindowSurface;
PFN_glfwSetFramebufferSizeCallback glfwSetFramebufferSizeCallback;
PFN_glfwGetKey glfwGetKey;
PFN_glfwInit glfwInit;
PFN_glfwGetRequiredInstanceExtensions glfwGetRequiredInstanceExtensions;
PFN_glfwGetFramebufferSize glfwGetFramebufferSize;
PFN_glfwGetTime glfwGetTime;
PFN_glfwSetCharCallback glfwSetCharCallback;
PFN_glfwWaitEvents glfwWaitEvents;
PFN_glfwPollEvents glfwPollEvents;
PFN_glfwGetCursorPos glfwGetCursorPos;
PFN_glfwSetInputMode glfwSetInputMode;
PFN_glfwSetCursorPos glfwSetCursorPos;
PFN_glfwTerminate glfwTerminate;
PFN_glfwDestroyWindow glfwDestroyWindow;
PFN_glfwWindowHint glfwWindowHint;
PFN_glfwSetMouseButtonCallback glfwSetMouseButtonCallback;
PFN_glfwSetKeyCallback glfwSetKeyCallback;
PFN_glfwGetMouseButton glfwGetMouseButton;
PFN_glfwWindowShouldClose glfwWindowShouldClose;

void *libglfw;

void InitGLFWFun()
{

    libglfw = dlopen("libglfw.so", RTLD_LAZY | RTLD_LOCAL);
    if (!libglfw)
        exit(-1);

    glfwSetCursorPosCallback = dlsym(libglfw, "glfwSetCursorPosCallback");
    glfwCreateWindow = dlsym(libglfw, "glfwCreateWindow");
    glfwCreateWindowSurface = dlsym(libglfw, "glfwCreateWindowSurface");
    glfwSetFramebufferSizeCallback = dlsym(libglfw, "glfwSetFramebufferSizeCallback");
    glfwGetKey = dlsym(libglfw, "glfwGetKey");
    glfwInit = dlsym(libglfw, "glfwInit");
    glfwGetRequiredInstanceExtensions = dlsym(libglfw, "glfwGetRequiredInstanceExtensions");
    glfwGetFramebufferSize = dlsym(libglfw, "glfwGetFramebufferSize");
    glfwGetTime = dlsym(libglfw, "glfwGetTime");
    glfwSetCharCallback = dlsym(libglfw, "glfwSetCharCallback");
    glfwWaitEvents = dlsym(libglfw, "glfwWaitEvents");
    glfwPollEvents = dlsym(libglfw, "glfwPollEvents");
    glfwGetCursorPos = dlsym(libglfw, "glfwGetCursorPos");
    glfwSetInputMode = dlsym(libglfw, "glfwSetInputMode");
    glfwSetCursorPos = dlsym(libglfw, "glfwSetCursorPos");
    glfwTerminate = dlsym(libglfw, "glfwTerminate");
    glfwDestroyWindow = dlsym(libglfw, "glfwDestroyWindow");
    glfwWindowHint = dlsym(libglfw, "glfwWindowHint");
    glfwSetMouseButtonCallback = dlsym(libglfw, "glfwSetMouseButtonCallback");
    glfwSetKeyCallback = dlsym(libglfw, "glfwSetKeyCallback");
    glfwGetMouseButton = dlsym(libglfw, "glfwGetMouseButton");
    glfwWindowShouldClose = dlsym(libglfw, "glfwWindowShouldClose");
}

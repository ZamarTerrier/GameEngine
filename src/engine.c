#include "engine.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "e_texture_variables.h"

#include "debuger.h"
#include "device.h"

#include "string.h"
#include "window.h"
#include "swapchain.h"
#include "pipeline.h"
#include "buffers.h"
#include "texture.h"

#include "gameObject.h"
#include "textObject.h"
#include "lightObject.h"

#include "e_widget_entry.h"
#include "e_widget_entry_area.h"

#include "camera.h"

#include "tools.h"

#include "e_resource_data.h"
#include "e_resource_engine.h"

typedef struct{
    GameObject** go;
    uint32_t count;
} renderObjects;

typedef void (*e_charCallback)(GLFWwindow*, uint32_t);
typedef void (*e_keyCallback)(GLFWwindow*, int , int , int , int );

renderObjects objs;

void * RecreateFunc = NULL;

e_charCallback *charCallbacks;
int charCallbackSize;

e_keyCallback *keyCallbacks;
int keyCallbackSize;

void EngineUpdateLine(){
    EntryUpdateLine();
}

void EngineCharacterCallback(GLFWwindow* window, uint32_t codepoint)
{
    EntryWidgetCharacterCallback(window, codepoint);

    for(int i=0; i < charCallbackSize;i++)
        charCallbacks[i](window, codepoint);
}

void EngineKeyCallback(GLFWwindow* window,  int key, int scancode, int action, int mods)
{
    EntryWidgetKeyCallback(window, key, scancode, action, mods);

    for(int i=0; i < keyCallbackSize;i++)
        keyCallbacks[i](window, key, scancode, action, mods);
}

void EngineInitVulkan(){
    createInstance();
    setupDebugMessenger();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    SwapChainCreate();
    SwapChainCreateImageViews();
    PipelineCreateRenderPass();
    BuffersCreateCommandPool();
    ToolsCreateDepthResources();
    BuffersCreateFramebuffers();
    BuffersCreateCommand();
    EngineCreateSyncobjects();

    objs.go = (GameObject **) calloc(0, sizeof(GameObject*));
    objs.count = 0;

    charCallbacks = (e_charCallback *) calloc(0, sizeof(e_charCallback));
    keyCallbacks = (e_keyCallback *) calloc(0, sizeof(e_keyCallback));

    e_var_lights = calloc(0, sizeof(LightObject *));
    e_var_num_lights = 0;

    e_var_images = calloc(MAX_IMAGES, sizeof(engine_buffered_image));
    e_var_num_images = 0;

    e_var_fonts = calloc(MAX_FONTS, sizeof(FontCache));
    e_var_num_fonts = 0;

    engine_buffered_image *images = e_var_images;
    TextureCreateEmptyDefault(&images[e_var_num_images].texture);
    TextureCreateTextureImageView(&images[e_var_num_images].texture);
    TextureCreateSampler(&images[e_var_num_images].texture);
    char *text = "Null texture";
    memcpy(images[e_var_num_images].path, text, strlen(text));
    e_var_num_images ++;
}

void EngineInitSystem(int width, int height, const char* name){
    strcpy(app_name, name);

    WIDTH = width;
    HEIGHT = height;

    viewSize.x = WIDTH;
    viewSize.y = HEIGHT;

    //rootDirPath = e_GetCurrectFilePath();

//    InitGLFWFun();
//    InitVulkanVariables();

    initWindow();
    EngineInitVulkan();

    glfwSetCharCallback(e_window, EngineCharacterCallback);
    glfwSetKeyCallback(e_window, EngineKeyCallback);

    e_var_current_entry = NULL;
}

void EngineFixedCursorCenter(){

    glfwSetCursorPos(e_window, WIDTH / 2, HEIGHT / 2);

}

void EngineGetCursorPos(double *xpos, double *ypos){
    glfwGetCursorPos(e_window, xpos, ypos);
}

void EngineSetCursorPos(float xpos, float ypos){

    glfwSetCursorPos(e_window, xpos, ypos);

}

void EngineHideCursor(char state){
    switch(state){
        case 0 :
            glfwSetInputMode(e_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            break;
        case 1 :
            glfwSetInputMode(e_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
            break;
        case 2 :
            glfwSetInputMode(e_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            break;
    }
}

int EngineGetMousePress(int Key){
    int state = glfwGetMouseButton(e_window, Key);

    return state;
}

int EngineWindowIsClosed()
{
    return glfwWindowShouldClose(e_window);
}

double EngineGetTime()
{
    return glfwGetTime();
}

const char *EngineGetClipBoardString()
{
    return glfwGetClipboardString(e_window);
}

void EngineSetClipBoardString(const char *string)
{
    glfwSetClipboardString( e_window, string);
}

void EnginePoolEvents()
{
    glfwPollEvents();
}

void EngineDeviceWaitIdle()
{
    vkDeviceWaitIdle(e_device);
}

int EngineGetKeyPress(int Key){
    int res = glfwGetKey(e_window, Key);

    return res;
}

void EngineSetMouseKeyCallback(void *callback){
    glfwSetMouseButtonCallback(e_window, callback);
}

void EngineSetKeyCallback(void *callback){
    keyCallbackSize ++;

    keyCallbacks = (e_keyCallback *)realloc(keyCallbacks, keyCallbackSize * sizeof(e_keyCallback));
    keyCallbacks[keyCallbackSize - 1] = (e_keyCallback *)callback;
}

void EngineSetCharCallback(void *callback){
    charCallbackSize ++;

    charCallbacks = (e_charCallback *)realloc(charCallbacks, charCallbackSize * sizeof(e_charCallback));
    charCallbacks[charCallbackSize - 1] = (e_charCallback *)callback;
}

void EngineSetCursorPoscallback(void * callback){
    glfwSetCursorPosCallback(e_window, callback);
}

void EngineSetRecreateFunc(void *func)
{
    RecreateFunc = func;
}

void EngineCleanupSwapChain() {

    vkDestroyImageView(e_device, depthImageView, NULL);
    vkDestroyImage(e_device, depthImage, NULL);
    vkFreeMemory(e_device, depthImageMemory, NULL);

    for (size_t i = 0; i < imagesCount; i++) {
        vkDestroyFramebuffer(e_device, swapChainFramebuffers[i], NULL);
    }
    free(swapChainFramebuffers);
    swapChainFramebuffers = NULL;

    vkFreeCommandBuffers(e_device, commandPool, imagesCount, commandBuffers);
    free(commandBuffers);
    commandBuffers = NULL;

    int temp = objs.count;

    while(temp > 0)
    {
        GameObjectClean(objs.go[temp - 1]);
        temp --;
    }

    vkDestroyRenderPass(e_device, renderPass, NULL);

    for (size_t i = 0; i < imagesCount; i++) {
        vkDestroyImageView(e_device, swapChainImageViews[i], NULL);
    }
    free(swapChainImages);
    free(swapChainImageViews);
    swapChainImageViews = NULL;

    vkDestroySwapchainKHR(e_device, swapChain, NULL);

}

void EnginereRecreateSwapChain() {

    glfwGetFramebufferSize(e_window, &WIDTH, &HEIGHT);

    while (WIDTH == 0 || HEIGHT == 0) {
        glfwGetFramebufferSize(e_window, &WIDTH, &HEIGHT);
        glfwWaitEvents();
    }

    if(RecreateFunc != NULL)
    {
        void (*reZero)(void) = RecreateFunc;

        reZero();
    }

    diffSize.x =  1;
    diffSize.y =  1;

    vkDeviceWaitIdle(e_device);

    EngineCleanupSwapChain();

    SwapChainCreate();
    SwapChainCreateImageViews();
    PipelineCreateRenderPass();
    ToolsCreateDepthResources();
    int temp = 0;

    while(temp < objs.count)
    {
        GameObjectRecreate(objs.go[temp]);
        temp ++;
    }

    BuffersCreateFramebuffers();
    BuffersCreateCommand();


    framebufferwasResized = true;

}

void EngineCreateSyncobjects() {
    imageAvailableSemaphores = (VkSemaphore *) calloc(MAX_FRAMES_IN_FLIGHT, sizeof(VkSemaphore));
    renderFinishedSemaphores = (VkSemaphore *) calloc(MAX_FRAMES_IN_FLIGHT, sizeof(VkSemaphore));
    inFlightFences = (VkFence *) calloc(MAX_FRAMES_IN_FLIGHT, sizeof(VkFence));
    imagesInFlight = (VkFence *) calloc(imagesCount, sizeof(VkFence));

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for(int i=0;i < imagesCount; i++)
        imagesInFlight[i] = VK_NULL_HANDLE;

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(e_device, &semaphoreInfo, NULL, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(e_device, &semaphoreInfo, NULL, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(e_device, &fenceInfo, NULL, &inFlightFences[i]) != VK_SUCCESS) {
            printf("failed to create synchronization objects for a frame!");
            exit(1);
        }
    }
}

void EngineLoop(){

    EngineUpdateLine();

    VkResult result = vkAcquireNextImageKHR(e_device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR && framebufferwasResized) {
        EnginereRecreateSwapChain();
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        printf("failed to acquire swap chain image!");
        exit(1);
    }

    for(int i=0;i < objs.count;i++){
        GameObjectUpdate(objs.go[i]);
    }

    if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(e_device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }

    imagesInFlight[imageIndex] = inFlightFences[currentFrame];

    //vkResetCommandPool(e_device, commandPool, 0);

    EngineDrawFrame();

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    EngineDeviceWaitIdle();
    vkResetFences(e_device, 1, &inFlightFences[currentFrame]);

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
        printf("failed to submit draw command buffer!");
        exit(1);
    }

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(presentQueue, &presentInfo);

    //vkQueueWaitIdle(presentQueue);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || (framebufferResized && framebufferwasResized)) {
        framebufferResized = false;
        framebufferwasResized = false;
        EnginereRecreateSwapChain();
    } else if (result != VK_SUCCESS) {
        printf("failed to present swap chain image!");
        exit(1);
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

    free(objs.go);
    objs.go = (GameObject **) calloc(0, sizeof(GameObject *));
    objs.count = 0;

    free(e_var_lights);
    e_var_lights = calloc(0, sizeof(LightObject *));
    e_var_num_lights = 0;
}

void EngineDrawFrame(){

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
        printf("failed to begin recording command buffer!");
        exit(1);
    }

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset.x = 0;
    renderPassInfo.renderArea.offset.y = 0;
    renderPassInfo.renderArea.extent = *(VkExtent2D*)&swapChainExtent;


    VkClearValue* clearColor = (VkClearValue *) calloc(1, sizeof(VkClearValue));
    clearColor->color.float32[0] = 0.8f;
    clearColor->color.float32[1] = 0.1f;
    clearColor->color.float32[2] = 0.1f;
    clearColor->color.float32[3] = 1.0f;
    VkClearValue* depthColor = (VkClearValue *) calloc(1, sizeof(VkClearValue));
    depthColor->depthStencil.depth = 1.0f;
    depthColor->depthStencil.stencil = 0;
    VkClearValue clearValues[] = {
        *clearColor,
        *depthColor
    };
    renderPassInfo.clearValueCount = 2;
    renderPassInfo.pClearValues = clearValues;

    vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    int temp = 0;

    while(temp < objs.count){
        GameObjectDraw(objs.go[temp]);

        temp ++;
    }

    vkCmdEndRenderPass(commandBuffers[imageIndex]);

    if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
        printf("failed to record command buffer!");
        exit(1);
    }

    free(clearColor);
    clearColor = NULL;
    free(depthColor);
    depthColor = NULL;

}

void EngineDraw(void* arg){

    GameObject* go = (GameObject *)arg;

    if(go->UpdatePoint == NULL)
    {
        perror("False point!\n");
        return;
    }

    for(int i=0;i < objs.count;i++)
    {
        if(objs.go[i] == go)
            return;
    }

    objs.count ++;
    objs.go = (GameObject **) realloc(objs.go,objs.count * sizeof(GameObject*));
    objs.go[objs.count - 1] = go;
}

void EngineCleanUp(){
    EngineCleanupSwapChain();

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(e_device, renderFinishedSemaphores[i], NULL);
        vkDestroySemaphore(e_device, imageAvailableSemaphores[i], NULL);
        vkDestroyFence(e_device, inFlightFences[i], NULL);
    }

    free(renderFinishedSemaphores);
    free(imageAvailableSemaphores);
    free(inFlightFences);
    free(imagesInFlight);

    vkDestroyCommandPool(e_device, commandPool, NULL);


    if(objs.count > 0)
    {
        free(objs.go);
        objs.go = NULL;
    }

    if(e_var_num_fonts > 0)
    {
        FontCache *fonts = e_var_fonts;

        for(int i=0; i < e_var_num_fonts;i++)
        {
            free(fonts[i].cdata);
            free(fonts[i].info);
            ImageDestroyTexture(fonts[i].texture);
            free(fonts[i].texture);
        }
    }

    free(e_var_fonts);
    e_var_fonts = NULL;

    if(e_var_num_images > 0)
    {
        engine_buffered_image *images = e_var_images;

        for(int i=0; i < e_var_num_images;i++)
        {
            ImageDestroyTexture(&images[i].texture);
        }
    }

    free(e_var_images);
    e_var_images = NULL;

    if(e_var_num_lights > 0)
    {
        free(e_var_lights);
        e_var_lights = NULL;
        e_var_num_lights = 0;
    }

    vkDestroyDevice(e_device, NULL);

    if (enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, NULL);
    }

    vkDestroySurfaceKHR(instance, surface, NULL);
    vkDestroyInstance(instance, NULL);

    glfwDestroyWindow(e_window);

    glfwTerminate();

}

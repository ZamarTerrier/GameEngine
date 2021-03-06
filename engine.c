#include "engine.h"

#include "string.h"

#include "gameObject.h"
#include "gameObject3D.h"
#include "textObject.h"
#include "camera.h"

typedef struct{
    GameObject2D** go;
    uint32_t count;
} rndrObj;

typedef struct{
    GameObject3D** go;
    uint32_t count;
} rndrObj3D;

typedef struct{
    TextObject** to;
    uint32_t count;
} textObjs;

rndrObj objs;
rndrObj objs3D;
textObjs tObjs;


void initVulkan(){
    createInstance();
    setupDebugMessenger();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
    createImageViews();
    createRenderPass();
    createCommandPool();
    createDepthResources();
    createFramebuffers();
    createCommandBuffers();
    createSyncObjects();

    objs.go = (GameObject2D *) calloc(0, sizeof(GameObject2D));
    objs3D.go = (GameObject3D *) calloc(0, sizeof(GameObject3D));
    objs.count = objs3D.count = 0;

}

void initEngine(int width, int height, const char* name){
    strcpy(app_name, name);

    WIDTH = width;
    HEIGHT = height;

    viewSize.x = WIDTH;
    viewSize.y = HEIGHT;

    initWindow();
    initVulkan();
}

void EngineGetcursorPos(int *xpos, int *ypos){
    glfwGetCursorPos(window, xpos, ypos);
}

void EngineFixedCursorCenter(){

    glfwSetCursorPos(window, WIDTH / 2, HEIGHT / 2);

}

void EngineSetCursorPos(float xpos, float ypos){

    glfwSetCursorPos(window, xpos, ypos);

}

void EngineHideCursor(char state){
    switch(state){
        case 0 :
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            break;
        case 1 :
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
            break;
        case 2 :
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            break;
    }
}

int EngineGetKeyPress(int Key){
    int res = glfwGetKey(window, Key);

    return res;
}

void EngineSetKeyCallback(void *callback){
    glfwSetKeyCallback(window, callback);
}

void EngineSetcursorPoscallback(void * callback){
    glfwSetCursorPosCallback(window, callback);
}

void cleanupSwapChain() {

    vkDestroyImageView(device, depthImageView, NULL);
    vkDestroyImage(device, depthImage, NULL);
    vkFreeMemory(device, depthImageMemory, NULL);

    for (size_t i = 0; i < imagesCount; i++) {
        vkDestroyFramebuffer(device, swapChainFramebuffers[i], NULL);
    }

    vkFreeCommandBuffers(device, commandPool, imagesCount, commandBuffers);

    int temp = objs.count;

    while(temp > 0)
    {
        cleanGameObject(objs.go[temp - 1]);
        temp --;
    }

    temp = objs3D.count;

    while(temp > 0)
    {
        GameObject3DClean(objs3D.go[temp - 1]);
        temp --;
    }

    temp = tObjs.count;

    while(temp > 0)
    {
        cleanTextObject(tObjs.to[temp - 1]);
        temp --;
    }

    vkDestroyRenderPass(device, renderPass, NULL);

    for (size_t i = 0; i < imagesCount; i++) {
        vkDestroyImageView(device, swapChainImageViews[i], NULL);
    }

    vkDestroySwapchainKHR(device, swapChain, NULL);

}

void recreateSwapChain() {

    glfwGetFramebufferSize(window, &WIDTH, &HEIGHT);

    while (WIDTH == 0 || HEIGHT == 0) {
        glfwGetFramebufferSize(window, &WIDTH, &HEIGHT);
        glfwWaitEvents();
    }

    vec2 newScale;

    newScale.x =  viewSize.x / WIDTH;
    newScale.y =  viewSize.y / HEIGHT;

    setViewScale2D(newScale);

    vkDeviceWaitIdle(device);

    cleanupSwapChain();

    createSwapChain();
    createImageViews();
    createRenderPass();
    createDepthResources();
    int temp = 0;

    while(temp < objs3D.count)
    {

        cleanPipelines(&objs3D.go[temp]->graphObj);

        GameObject3DCreateDrawItems(objs3D.go[temp]);
        temp ++;
    }

    temp = 0;

    while(temp < tObjs.count)
    {

        cleanPipelines(&tObjs.to[temp]->graphObj);

        createDrawItemsTextObject(tObjs.to[temp]);
        temp ++;
    }

    temp = 0;

    while(temp < objs.count)
    {

        cleanPipelines(&objs.go[temp]->graphObj);

        createDrawItemsGameObject(objs.go[temp]);
        temp ++;
    }

    createFramebuffers();
    createCommandBuffers();

    framebufferwasResized = true;

}

void createSyncObjects() {
    imageAvailableSemaphores = (VkSemaphore *) calloc(MAX_FRAMES_IN_FLIGHT, sizeof(VkSemaphore));
    renderFinishedSemaphores = (VkSemaphore *) calloc(MAX_FRAMES_IN_FLIGHT, sizeof(VkSemaphore));
    inFlightFences = (VkFence *) calloc(MAX_FRAMES_IN_FLIGHT, sizeof(VkFence));
    imagesInFlight = (VkFence *) calloc(imagesCount, sizeof(VkFence));

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for(i=0;i < imagesCount; i++)
        imagesInFlight[i] = VK_NULL_HANDLE;

    for (i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(device, &semaphoreInfo, NULL, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(device, &semaphoreInfo, NULL, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(device, &fenceInfo, NULL, &inFlightFences[i]) != VK_SUCCESS) {
            printf("failed to create synchronization objects for a frame!");
            exit(1);
        }
    }
}

void engineLoop(){

    VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR && framebufferwasResized) {
        recreateSwapChain();
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        printf("failed to acquire swap chain image!");
        exit(1);
    }

    int temp = 0;

    while(temp < objs3D.count)
    {
        GameObject3DUpdateUniformBuffer(objs3D.go[temp]);
        temp ++;
    }

    temp = 0;

    while(temp < objs.count)
    {
        updateUniformBuffer(objs.go[temp]);
        temp ++;
    }

    temp = 0;

    while(temp < tObjs.count)
    {
        updateTextUniformBuffer(tObjs.to[temp]);
        temp ++;
    }

    if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }

    imagesInFlight[imageIndex] = inFlightFences[currentFrame];

    //vkResetCommandPool(device, commandPool, 0);

    drawFrame();

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

    vkResetFences(device, 1, &inFlightFences[currentFrame]);

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
        recreateSwapChain();
    } else if (result != VK_SUCCESS) {
        printf("failed to present swap chain image!");
        exit(1);
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

    objs.go = (GameObject2D *) realloc(0, sizeof(GameObject2D));
    objs3D.go = (GameObject3D *) calloc(0, sizeof(GameObject3D));
    tObjs.to = (TextObject *) realloc(0, sizeof(TextObject));

    objs.count = tObjs.count = objs3D.count = 0;

}

void drawFrame(){

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
    renderPassInfo.renderArea.extent = swapChainExtent;


    VkClearValue* clearColor = (VkClearValue *) calloc(1, sizeof(VkClearValue));
    clearColor->color.float32[0] = 0.3f;
    clearColor->color.float32[1] = 0.0f;
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
    while(temp < objs3D.count)
    {
        GameObject3DDraw(objs3D.go[temp]);

        temp ++;
    }

    temp = 0;

    while(temp < objs.count){

        gameObjectDraw(objs.go[temp]);

        temp ++;
    }

    temp = 0;

    while(temp < tObjs.count)
    {
        drawTextObject(tObjs.to[temp]);
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

void engDraw(void* arg){

    GameObject2D* go = (GameObject2D *)arg;

    for(i=0;i < objs.count;i++)
    {
        if(objs.go[i] == go)
            return;
    }

    objs.count ++;
    objs.go = (GameObject2D **) realloc(objs.go,objs.count * sizeof(GameObject2D*));
    objs.go[objs.count - 1] = go;
}

void engDraw3D(void* arg){

    GameObject3D* go = (GameObject3D *)arg;

    for(i=0;i < objs3D.count;i++)
    {
        if(objs3D.go[i] == go)
            return;
    }

    objs3D.count ++;
    objs3D.go = (GameObject3D **) realloc(objs3D.go,objs3D.count * sizeof(GameObject3D*));
    objs3D.go[objs3D.count - 1] = go;
}

void engDrawText(void* arg){

    TextObject* to = (TextObject *)arg;

    for(i=0;i < tObjs.count;i++)
    {
        if(tObjs.to[i] == to)
            return;
    }

    tObjs.count ++;
    tObjs.to = (GameObject2D **) realloc(tObjs.to, tObjs.count * sizeof(GameObject2D*));
    tObjs.to[tObjs.count - 1] = to;
}

void cleanUp(){
    cleanupSwapChain();

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(device, renderFinishedSemaphores[i], NULL);
        vkDestroySemaphore(device, imageAvailableSemaphores[i], NULL);
        vkDestroyFence(device, inFlightFences[i], NULL);
    }

    vkDestroyCommandPool(device, commandPool, NULL);

    vkDestroyDevice(device, NULL);

    if (enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, NULL);
    }

    vkDestroySurfaceKHR(instance, surface, NULL);
    vkDestroyInstance(instance, NULL);

    glfwDestroyWindow(window);

    glfwTerminate();

}

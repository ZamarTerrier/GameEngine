#include "engine.h"

#include "gameObject.h"
#include "textObject.h"
#include "camera.h"

typedef struct{
    GameObject** go;
    uint32_t count;
} rndrObj;

typedef struct{
    TextObject** to;
    uint32_t count;
} textObjs;

rndrObj objs;
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

    objs.go = (GameObject *) calloc(0, sizeof(GameObject));
    objs.count = 0;

    initCamera();

}

void initEngine(){
    initWindow();
    initVulkan();
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

    setViewScale(newScale);

    vkDeviceWaitIdle(device);

    cleanupSwapChain();

    createSwapChain();
    createImageViews();
    createRenderPass();
    createDepthResources();
    int temp = objs.count;

    while(temp > 0)
    {
        createDrawItemsGameObject(objs.go[temp - 1]);
        temp --;
    }

    temp = tObjs.count;

    while(temp > 0)
    {
        createDrawItemsTextObject(tObjs.to[temp - 1]);
        temp --;
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

    int temp;

    temp = tObjs.count;

    while(temp > 0)
    {
        updateTextUniformBuffer(tObjs.to[temp - 1]);
        temp --;
    }

    temp = objs.count;

    while(temp > 0)
    {
        updateUniformBuffer(objs.go[temp - 1]);
        temp --;
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

    objs.go = (GameObject *) realloc(0, sizeof(GameObject));
    objs.count = 0;

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
    clearColor->color.float32[0] = 0.0f;
    clearColor->color.float32[1] = 0.0f;
    clearColor->color.float32[2] = 0.2f;
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

    int temp;

    temp = objs.count;

    while(temp > 0)
    {
        gameObjectDraw(objs.go[temp - 1]);

        temp --;
    }

    temp = tObjs.count;

    while(temp > 0)
    {
        drawTextObject(tObjs.to[temp - 1]);
        temp --;
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

    GameObject* go = (GameObject *)arg;

    for(i=0;i < objs.count;i++)
    {
        if(objs.go[i] == go)
            return;
    }

    objs.count ++;
    objs.go = (GameObject **) realloc(objs.go,objs.count * sizeof(GameObject*));
    objs.go[objs.count - 1] = go;
}

void engDrawText(void* arg){

    TextObject* to = (TextObject *)arg;

    for(i=0;i < tObjs.count;i++)
    {
        if(tObjs.to[i] == to)
            return;
    }

    tObjs.count ++;
    tObjs.to = (GameObject **) realloc(tObjs.to, tObjs.count * sizeof(GameObject*));
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

    free(camObj);

    glfwDestroyWindow(window);

    glfwTerminate();

}

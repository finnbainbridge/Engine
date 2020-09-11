#include "Engine/Engine.hpp"
#include <memory>
#include <string>
#include <glad/glad.h>
#define GLFW_INCLUDE_ES3
#include "GLFW/glfw3.h"

#include "Engine/Renderer/Amber.hpp"

using namespace Engine::Renderer;

Amber::Amber(std::shared_ptr<Document> doc): document(doc)
{
    doc->renderer = std::shared_ptr<Amber>(this);
}

Amber::~Amber()
{
    // TODO: Check if we actually made a window lol
    glfwTerminate();
}

void onFrameBufferSizeChange(GLFWwindow* window, int iwidth, int iheight);

bool Amber::createWindow(int width, int height, std::string title)
{
    std::cout << "Amber Renderer v0.0.1" << std::endl;

    if (!glfwInit())
    {
        std::cerr << "Failed. Could not start GLFW." << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // TODO: Fullscreen
    window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);

    if (window == nullptr)
    {
        std::cerr << "Failed. Could not create window." << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, onFrameBufferSizeChange);

    // 1 for V-sync
    glfwSwapInterval(0);

    if(!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed. Could not start OpenGL" << std::endl;
        glfwTerminate();
        return false;
    }

    std::cout << "OpenGL ES " << GLVersion.major << "." << GLVersion.minor << " on " << glGetString(GL_VENDOR) << " " << glGetString(GL_RENDERER) << std::endl;

    glClearColor(0.23f, 0.38f, 0.47f, 1.0f);
    

    glViewport(0,0,width,height);
    glEnable(GL_DEPTH_TEST);

    return true;
}

void Amber::mainloop()
{
    last_frame_start = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        loop();
    }

    glfwTerminate();
    document->destroy();
    Engine::Threading::cleanup();
    std::cout << "Shut down" << std::endl;
}

void Amber::loop()
{
    float delta = glfwGetTime() - last_frame_start;
    last_frame_start = glfwGetTime();

    // Check for keypresses
    glfwPollEvents();
    
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Run the DOM to get everything to render
    document->tick(delta);

    // Swap the buffers so it actually shows up
    glfwSwapBuffers(window);
}

void onFrameBufferSizeChange(GLFWwindow* window, int iwidth, int iheight)
{
    // width = iwidth;
    // height = iheight;
    glViewport(0,0,iwidth,iheight);
}
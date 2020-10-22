#include "Engine/Engine.hpp"
// #include "Engine/NKAPI.hpp"
// #include "Engine/Log.hpp"
#include "Engine/Log.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "glm/fwd.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/matrix.hpp"
#include <algorithm>
#include <cstddef>
#include <exception>
#include <iostream>
#include <memory>
#include <string>

#ifdef __EMSCRIPTEN__

#include <GLES3/gl3.h>
#include <emscripten.h>
#include <emscripten/html5.h>

#else

#include <glad/glad.h>

#endif


//#define GLFW_INCLUDE_ES3
#include "GLFW/glfw3.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT
#define NK_GLFW_GL3_IMPLEMENTATION
#include "Engine/NKAPI.hpp"

#include "Engine/Renderer/Amber.hpp"
#include "Engine/Input.hpp"
#include "nuklear/nuklear_glfw_gl3.h"


using namespace Engine::Renderer;
using namespace Engine::NKAPI;

GLFWwindow* Engine::Renderer::Amber::window = nullptr;

int Amber::screen_height = 0;
int Amber::screen_width = 0;

float Amber::_scroll_offset = 0;

// Nuklear instance
struct nk_glfw glfw = {0};
#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

std::function<void()> loop_func;

Amber::Amber(std::shared_ptr<Document> doc): document(doc),
offset(),
old_position()
{
    doc->renderer = std::shared_ptr<Amber>(this);
    mouse_mode = Engine::Input::MouseMode::Free;
}

Amber::~Amber()
{
    // TODO: Check if we actually made a window lol
    glfwTerminate();
}

void onFrameBufferSizeChange(GLFWwindow* window, int iwidth, int iheight);
void onScroll(GLFWwindow* window, double xoffset, double yoffset);

bool Amber::createWindow(int width, int height, std::string title)
{
    LOG_SUCCESS("Amber Renderer v0.0.1");

    if (!glfwInit())
    {
        LOG_ASSERT_MESSAGE_FATAL(true, "Failed. Could not start GLFW.");
        return false;
    }

#ifndef EMSCRIPTEN
    //glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    // TODO: Fullscreen
    window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);

    if (window == nullptr)
    {
        LOG_ERROR("Failed. Could not create window.");
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, onFrameBufferSizeChange);

    glfwSetScrollCallback(window, onScroll);

    // 1 for V-sync
    glfwSwapInterval(0);

#ifndef __EMSCRIPTEN__
    if(!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress))
    {
        LOG_ERROR("Failed. Could not start OpenGL");
        glfwTerminate();
        return false;
    }

    LOG_SUCCESS("OpenGL ES " + std::to_string(GLVersion.major) + "." + std::to_string(GLVersion.minor) + " on " + (char*)glGetString(GL_VENDOR) + " " + (char*)glGetString(GL_RENDERER));
#endif


    glClearColor(0.23f, 0.38f, 0.47f, 1.0f);
    

    glViewport(0,0,width,height);
    glEnable(GL_DEPTH_TEST);

    // Create nuklear
    NKAPI::ctx = nk_glfw3_init(&glfw, window, NK_GLFW3_DEFAULT);

    glfwSetCharCallback(window, nk_glfw3_char_callback);
    glfwSetMouseButtonCallback(window, nk_glfw3_mouse_button_callback);

    // Create fonts
    {struct nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(&glfw, &atlas);
    /*struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "../../../extra_font/DroidSans.ttf", 14, 0);*/
    /*struct nk_font *roboto = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Roboto-Regular.ttf", 14, 0);*/
    /*struct nk_font *future = nk_font_atlas_add_from_file(atlas, "../../../extra_font/kenvector_future_thin.ttf", 13, 0);*/
    /*struct nk_font *clean = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyClean.ttf", 12, 0);*/
    /*struct nk_font *tiny = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyTiny.ttf", 10, 0);*/
    /*struct nk_font *cousine = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Cousine-Regular.ttf", 13, 0);*/
    nk_glfw3_font_stash_end(&glfw);
    /*nk_style_load_all_cursors(ctx, atlas->cursors);*/
    /*nk_style_set_font(ctx, &droid->handle);*/}

    screen_width = width;
    screen_height = height;

    // Create function for emscripten main loop
    loop_func = std::bind(&Amber::loop, this);

    return true;
}

#ifdef __EMSCRIPTEN__
void loop_wrapper()
{
    // Amber::loop(amb);
    loop_func();
    // return EM_TRUE;
}

#endif

void Amber::mainloop(std::function<void(float)> func)
{
    last_frame_start = glfwGetTime();
    render_func = func;
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(loop_wrapper, 0, true);
#else
    while (!glfwWindowShouldClose(window))
    {
        loop();
    }
#endif

    destroy();
    document->destroy();
    Engine::Threading::cleanup();
    LOG_SUCCESS("Shut down");
    glfwTerminate();
}

bool Amber::loop()
{
    // Make sure the context is on the right thread
    glfwMakeContextCurrent(window);

    float delta = glfwGetTime() - last_frame_start;
    last_frame_start = glfwGetTime();

    // Check for keypresses
    glfwPollEvents();

    // Start Nuklear frame
    nk_glfw3_new_frame(&glfw);
    
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Deal with input stuff (mostly mouse)
    if (mouse_mode == Engine::Input::MouseMode::Captured)
    {
        // Always set the cursor to the center of the screen
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        offset = glm::vec2(x,y);
        x = screen_width/2;
        y = screen_height/2;
        glfwSetCursorPos(window, x, y);
        old_position = glm::vec2(x,y);
    }
    else if (mouse_mode == Engine::Input::MouseMode::Confined)
    {
        // Keep the cursor within the confines of the screen
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        offset = old_position - glm::vec2(x,y);
        old_position = glm::vec2(x,y);
        glm::vec2 new_position(x,y);

        if (old_position.x < 10)
        {
            new_position.x = screen_width-11;
        }
        else if (old_position.x > screen_width-10)
        {
            new_position.x = 11;
        }

        if (old_position.y < 10)
        {
            new_position.y = screen_height-11;
        }
        else if (old_position.y > screen_height-10)
        {
            new_position.y = 11;
        }

        if (new_position != old_position)
        {
            glfwSetCursorPos(window, new_position.x, new_position.y);
            old_position = new_position;
        }
    }
    else {
        // The cursor can do whatever it wants
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        offset = old_position - glm::vec2(x,y);
        old_position = glm::vec2(x,y);
    }

    // Run our render function
    render_func(delta);

    // Run the DOM to get everything to render
    document->tick(delta);

    // Render nuklear GUI
    // nk_end(NKAPI::ctx);
    nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON);
    glEnable(GL_DEPTH_TEST);

    // Swap the buffers so it actually shows up
    glfwSwapBuffers(window);

    // Deal with the scroll wheel
    _scroll_offset = 0;

    return true;
}

double Amber::getTime()
{
    return glfwGetTime();
}

void AmberShaderProgram::checkCompileErrors(glm::uint32 shader, AmberShaderType type)
{
    int status = 0;

    if (type == AmberShaderType::program)
    {
        glGetProgramiv(handle, GL_LINK_STATUS, &status);

        if (status == GL_FALSE)
        {
            GLint length = 0;
            glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &length);

            std::string errorLog(length, ' ');
            glGetProgramInfoLog(handle, length, &length, &errorLog[0]);
            std::cerr << "Linking failed: " << errorLog << std::endl;
        }
    }
    else // VERTEX or FRAGMENT
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE)
        {
            GLint length = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

            std::string errorLog(length, ' ');
            glGetShaderInfoLog(shader, length, &length, &errorLog[0]);
            std::cerr << "Shader compilation failed: " << errorLog << std::endl;
        }
    }
}

glm::vec2 Amber::getMouseOffset()
{
    return offset;
}

void onScroll(GLFWwindow* window, double xoffset, double yoffset)
{
    Amber::_scroll_offset += yoffset;

    // Call nuklear callbacks
    nk_gflw3_scroll_callback(window, xoffset, yoffset);
}

float Amber::getScrollWheelOffset()
{
    return Amber::_scroll_offset;
}

void AmberShaderProgram::loadShaders(std::shared_ptr<ShaderResource> vert, std::shared_ptr<ShaderResource> frag)
{
    // Make sure we're on the right thread
    Amber::makeCurrent();

    // Turn Shaders into c strings
    const std::string vertex_src_str = vert->getText();
    const char* vertex_src = vertex_src_str.c_str();

    const std::string fragment_src_str = frag->getText();
    const char* fragment_src = fragment_src_str.c_str();

    // Create Shaders
    glm::uint32 vs = glCreateShader(GL_VERTEX_SHADER);
    glm::uint32 fs = glCreateShader(GL_FRAGMENT_SHADER);

    // Attach the source code
    glShaderSource(vs, 1, &vertex_src, NULL);
    glShaderSource(fs, 1, &fragment_src, NULL);

    // Compile shaders and check for errors
    glCompileShader(vs);
    checkCompileErrors(vs, AmberShaderType::shader);

    glCompileShader(fs);
    checkCompileErrors(fs, AmberShaderType::shader);

    // Now create the shader program
    handle = glCreateProgram();
    
    // Attach the shaders
    glAttachShader(handle, vs);
    glAttachShader(handle, fs);

    // Link shaders
    glLinkProgram(handle);
    checkCompileErrors(handle, AmberShaderType::program);

    // Delete shaders (they're useless now)
    glDeleteShader(vs);
    glDeleteShader(fs);

    // delete vertex_src;
    // delete fragment_src;
}

void AmberShaderProgram::use()
{
    if (handle > 0)
    {
        glUseProgram(handle);
    }
}

GLint AmberShaderProgram::getUniformLocation(const std::string name) 
{
    std::map<std::string, GLint>::iterator it = uniform_locations.find(name);
    if (it == uniform_locations.end())
    {
        uniform_locations[name] = glGetUniformLocation(handle, name.c_str());
    }

    return uniform_locations[name];
}

void AmberShaderProgram::setUniform(const std::string name, const glm::vec2& v)
{
    GLint loc = getUniformLocation(name);
    glUniform2f(loc, v.x, v.y);
}
void AmberShaderProgram::setUniform(const std::string name, const glm::vec3& v)
{
    GLint loc = getUniformLocation(name);
    glUniform3f(loc, v.x, v.y, v.z);
}
void AmberShaderProgram::setUniform(const std::string name, const glm::vec4& v)
{
    GLint loc = getUniformLocation(name);
    glUniform4f(loc, v.x, v.y, v.z, v.w);
}
void AmberShaderProgram::setUniform(const std::string name, const glm::mat4& v)
{
    GLint loc = getUniformLocation(name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(v));
}

void AmberShaderProgram::setUniform(const std::string name, const int& v)
{
    GLint loc = getUniformLocation(name);
    glUniform1i(loc, v);
}

void AmberShaderProgram::setUniform(const std::string name, const float& v)
{
    GLint loc = getUniformLocation(name);
    glUniform1f(loc, v);
}

void AmberShaderProgram::setUniform(const std::string name, const bool& v)
{
    GLint loc = getUniformLocation(name);
    glUniform1i(loc, v);
}

void AmberShaderProgram::destroy()
{
    LOG_INFO("Destroying shaders!");
    glDeleteProgram(handle);
}

// Shader program caching
struct CachedShaders
{
    std::shared_ptr<AmberShaderProgram> program;
    std::shared_ptr<ShaderResource> vert;
    std::shared_ptr<ShaderResource> frag;

    bool friend operator == (const CachedShaders& a, const CachedShaders& b)
    {
        return a.vert == b.vert && a.frag == b.frag;
    }
};

std::vector<CachedShaders> shader_cache;

std::shared_ptr<ShaderProgram> Amber::addShaderProgram(std::shared_ptr<ShaderResource> vert, std::shared_ptr<ShaderResource> frag)
{
    CachedShaders new_shaders;
    new_shaders.vert = vert;
    new_shaders.frag = frag;

    auto cached = std::find(shader_cache.begin(), shader_cache.end(), new_shaders);
    std::shared_ptr<AmberShaderProgram> program;

    if (cached == shader_cache.end())
    {
        // Create and add new shader
        program = std::make_shared<AmberShaderProgram>();
        program->loadShaders(vert, frag);
        shaders.push_back(program);

        // Add it to the cache
        new_shaders.program = program;
        shader_cache.push_back(new_shaders);
    }
    else
    {
        program = cached->program;
    }

    return program;
}

void AmberRenderObject::setMeshData(std::vector<glm::float32> vertices, std::vector<glm::uint32> indiciez)
{
    RenderObject::setMeshData(vertices, indiciez);

    // Make sure we're on the right thread
    Amber::makeCurrent();

    // Now put the model into OpenGL

    // First, create the buffers
    glGenVertexArrays(1, &vao);
    // std::cout << glGetError() << std::endl;
    glGenBuffers(1, &vbo);
    // std::cout << glGetError() << std::endl;
    glGenBuffers(1, &ibo);
    // std::cout << glGetError() << std::endl;

    // Use the vertex array
    glBindVertexArray(vao);
    // std::cout << glGetError() << std::endl;

    // Fill up the buffers
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Add the vertex data
    glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(glm::float32), &vertex_data[0], GL_STATIC_DRAW);

    // Add the indicies
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    // std::cout << glGetError() << std::endl;
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(glm::uint32), &indices[0], GL_STATIC_DRAW);
    // std::cout << glGetError() << std::endl;

    // Now, we tell OpenGL where all the data actually _is_

    // In position 1: Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GL_FLOAT), NULL);
    glEnableVertexAttribArray(0);
    // std::cout << glGetError() << std::endl;

    // In position 2: Normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(1);
    // std::cout << glGetError() << std::endl;

    // In position 3: Texture Coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GL_FLOAT), (GLvoid*)(6 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(2);
    // std::cout << glGetError() << std::endl;

    // Cleanup
    glBindVertexArray(0);

}

void AmberRenderObject::draw()
{
    // std::cout << glGetError() << std::endl;
    glBindVertexArray(vao);
    // std::cout << glGetError() << std::endl;
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*) 0);
    // std::cout << glGetError() << std::endl;
    glBindVertexArray(0);
    // std::cout << glGetError() << std::endl;
}

void AmberRenderObject::destroy()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
}

std::shared_ptr<RenderObject> Amber::addRenderObject()
{
    auto object = std::make_shared<AmberRenderObject>();
    objects.push_back(object);
    return object;
}

void Amber::renderRenderObject(std::shared_ptr<RenderObject> model, glm::mat4 global_transform, glm::mat4 local_transform)
{
    Amber::makeCurrent();
    model->shader_program->use();

    if (!has_camera)
    {
        return;
    }

    // Set uniforms
    // TODO: Do this in a more scalable way
    glm::mat4 trans = global_transform * local_transform;
    glm::mat4 mv = camera->_getViewMatrix() * trans;
    model->shader_program->setUniform("projection", glm::perspective((float)0.8726646, (float)screen_width/screen_height, 0.1f, 100.0f));
    model->shader_program->setUniform("view", camera->_getViewMatrix());
    model->shader_program->setUniform("transform", trans);
    model->shader_program->setUniform("model_view", mv);
    // model->shader_program->setUniform("normal_transform", glm::mat3(mv));
    // model->shader_program->setUniform("normal_local", glm::mat3( glm::vec3(local_transform[0]), glm::vec3(local_transform[1]), glm::vec3(local_transform[2])));
    // model->shader_program->setUniform("normal_global", glm::mat3( glm::vec3(global_transform[0]), glm::vec3(global_transform[1]), glm::vec3(global_transform[2])));

    // model->shader_program->setUniform("normal_local", glm::mat3(local_transform));
    // model->shader_program->setUniform("normal_global", glm::mat3(global_transform));

    model->draw();
}

void Amber::setCamera(std::shared_ptr<ICamera> cam)
{
    has_camera = true;
    camera = cam;

    try
    {
        auto cam_element = std::dynamic_pointer_cast<DOM::Element>(cam);
        if (cam_element->inited == false)
        {
            cam_element->init();
            cam_element->inited = true;
        }
    }
    catch (std::exception e)
    {

    }
}

std::shared_ptr<ICamera> Amber::getCamera()
{
    if (!has_camera)
    {
        LOG_WARN("Trying to get camera with no active camera");
        return nullptr;
    }
    return camera;
}

bool Amber::isKeyPressed(int key)
{
    if (glfwGetKey(window, key) == GLFW_PRESS)
    {
        return true;
    }
    
    return false;
}

bool Amber::isMouseButtonPressed(int button)
{
    if (glfwGetMouseButton(window, button) == GLFW_PRESS)
    {
        return true;
    }
    
    return false;
}

// Mouse stuff
void Amber::setCursorMode(Engine::Input::CursorMode mode)
{
    if (mode == Engine::Input::CursorMode::Hidden)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }
    else
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void Amber::setMouseMode(Engine::Input::MouseMode mode)
{
    mouse_mode = mode;
}

void Amber::destroy()
{
    for (size_t i = 0; i < objects.size(); i++) 
    {
        objects[i]->destroy();
    }

    for (size_t i = 0; i < shaders.size(); i++) 
    {
        shaders[i]->destroy();
    }

    nk_free(NKAPI::ctx);
    glfwTerminate();
}

void onFrameBufferSizeChange(GLFWwindow* window, int iwidth, int iheight)
{
    Amber::screen_width = iwidth;
    Amber::screen_height = iheight;
    glViewport(0,0,iwidth,iheight);
}
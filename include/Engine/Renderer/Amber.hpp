#ifndef ENGINE_RENDERER_AMBER_H
#define ENGINE_RENDERER_AMBER_H

// #include "Engine/Engine.hpp"
// #include "Engine/DOM.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Res.hpp"
#include "GLFW/glfw3.h"
#include <map>
#include <string>
#include "glm/fwd.hpp"
#include <glm/glm.hpp>


namespace Engine {

    namespace Renderer {

        enum AmberShaderType {program, shader};
        class AmberShaderProgram: public ShaderProgram
        {
            private:
                glm::uint32 handle;
                void checkCompileErrors(glm::uint32 shader, AmberShaderType type);

                GLint getUniformLocation(const std::string name);

                std::map<std::string, GLint> uniform_locations;

            public:
                AmberShaderProgram() : handle(0) {};
                virtual void loadShaders(std::shared_ptr<ShaderResource> vert, std::shared_ptr<ShaderResource> frag);

                virtual void use();

                virtual void destroy();

                virtual void setUniform(const std::string label, const glm::vec2& value);
                virtual void setUniform(const std::string label, const glm::vec3& value);
                virtual void setUniform(const std::string label, const glm::vec4& value);
                virtual void setUniform(const std::string label, const glm::mat4& value);
        };

        class AmberRenderObject: public RenderObject
        {
            private:
                glm::uint32 vao;
                glm::uint32 vbo;
                glm::uint32 ibo;

            public:
                AmberRenderObject(): vao(0), vbo(0), ibo(0) {};
                virtual void setMeshData(std::vector<glm::vec3> position, std::vector<glm::vec3> normals, std::vector<glm::vec2> texture_coords, std::vector<glm::uint32> indiciez);
                virtual void draw();
                virtual void destroy();
        };

        class Amber: public IRenderer
        {
            private:
                static GLFWwindow* window;
                std::shared_ptr<Document> document;
                double last_frame_start;
                int width;
                int height;

                std::vector<std::shared_ptr<AmberShaderProgram>> shaders;
                std::vector<std::shared_ptr<AmberRenderObject>> objects;

                bool has_camera = false;
                std::shared_ptr<ICamera> camera;

                std::function<void(float)> render_func;

                Engine::Input::MouseMode mouse_mode;
                glm::vec2 offset;
                glm::vec2 old_position;

            public:
                Amber(std::shared_ptr<Document> doc);
                ~Amber();

                virtual bool createWindow(int width, int height, std::string title);
                static int screen_width;
                static int screen_height;

                virtual void mainloop(std::function<void(float)> func);
                bool loop();

                virtual double getTime();

                virtual std::shared_ptr<ShaderProgram> addShaderProgram(std::shared_ptr<ShaderResource> vert, std::shared_ptr<ShaderResource> frag);

                virtual std::shared_ptr<RenderObject> addRenderObject();
                virtual void renderRenderObject(std::shared_ptr<RenderObject> model, glm::mat4 global_transform, glm::mat4 local_transform);

                virtual void destroy();

                virtual void setCamera(std::shared_ptr<ICamera> cam);
                virtual std::shared_ptr<ICamera> getCamera();

                virtual bool isKeyPressed(int key);

                virtual bool isMouseButtonPressed(int button);

                virtual glm::vec2 getMousePosition() {return glm::vec2();};
                virtual glm::vec2 getMouseOffset();

                virtual float getScrollWheelOffset();
                static float _scroll_offset;

                virtual void setMouseMode(Engine::Input::MouseMode mode);
                virtual void setCursorMode(Engine::Input::CursorMode mode);

                virtual int getHeight() {return screen_height;};
                virtual int getWidth() {return screen_width;};

                static void makeCurrent()
                {
                    glfwMakeContextCurrent(window);
                }
        };
    }
}

#endif

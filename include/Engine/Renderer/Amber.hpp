#ifndef ENGINE_RENDERER_AMBER_H
#define ENGINE_RENDERER_AMBER_H

// #include "Engine/Engine.hpp"
// #include "Engine/DOM.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Res.hpp"
#include "GLFW/glfw3.h"
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
            public:
                AmberShaderProgram() : handle(0) {};
                virtual void loadShaders(std::shared_ptr<ShaderResource> vert, std::shared_ptr<ShaderResource> frag);

                virtual void use();

                virtual void destroy();
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

                void loop();

            public:
                Amber(std::shared_ptr<Document> doc);
                ~Amber();
                virtual bool createWindow(int width, int height, std::string title);

                virtual void mainloop();

                virtual std::shared_ptr<ShaderProgram> addShaderProgram(std::shared_ptr<ShaderResource> vert, std::shared_ptr<ShaderResource> frag);

                virtual std::shared_ptr<RenderObject> addRenderObject();
                virtual void renderRenderObject(std::shared_ptr<RenderObject> model);

                virtual void destroy();

                static void makeCurrent()
                {
                    glfwMakeContextCurrent(window);
                }
        };
    }
}

#endif

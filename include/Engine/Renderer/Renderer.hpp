#ifndef ENGINE_RENDERER_H
#define ENGINE_RENDERER_H

#include "Engine/Engine.hpp"
// #include "Engine/DOM.hpp"
#include "Engine/Res.hpp"
#include "glm/fwd.hpp"
#include <functional>
#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include <sstream>

#include "Engine/Input.hpp"

namespace Engine {
    namespace Renderer {
        
        class ShaderResource: public Res::IResource
        {
            private:
                std::string text = "";
            public:
                ShaderResource(): text("") {};
                virtual void loadFile(std::shared_ptr<std::stringstream> data)
                {
                    text = data->str();
                }

                std::string getText() const {
                    return text;
                }
        };

        class ShaderProgram 
        {
            public:
                ShaderProgram() {};
                virtual void loadShaders(std::shared_ptr<ShaderResource> vert, std::shared_ptr<ShaderResource> frag) {}
                virtual void destroy() {};
                virtual void use() {};

                virtual void setUniform(const std::string label, const glm::vec2& value) {};
                virtual void setUniform(const std::string label, const glm::vec3& value) {};
                virtual void setUniform(const std::string label, const glm::vec4& value) {};
                virtual void setUniform(const std::string label, const glm::mat4& value) {};
        };

        class RenderObject
        {
            public:
                RenderObject(): shader_program(nullptr), global_transform(), local_transform(), vertex_data(), indicies() {};
                glm::mat4 global_transform;
                glm::mat4 local_transform;

                std::shared_ptr<ShaderProgram> shader_program;

                // This is the mesh data
                // Format: Tightly packed float32
                // 3 floats for position (x y z) 3 floats for normals (x y z) 2 floats for texture coords (x y)
                std::vector<glm::float32> vertex_data;

                // Indicies
                std::vector<glm::uint32> indicies;

                virtual void setMeshData(std::vector<glm::vec3> position, std::vector<glm::vec3> normals, std::vector<glm::vec2> texture_coords, std::vector<glm::uint32> indiciez)
                {
                    indicies = indiciez;

                    if (position.size() != normals.size() && normals.size() != texture_coords.size())
                    {
                        std::cerr << "Error: Positions, normals, and texture coords must be the same length" << std::endl;
                    }

                    vertex_data = std::vector<glm::float32>();

                    for (size_t i = 0; i < position.size(); i++) {
                        vertex_data.push_back(position[i].x);
                        vertex_data.push_back(position[i].y);
                        vertex_data.push_back(position[i].z);

                        vertex_data.push_back(normals[i].x);
                        vertex_data.push_back(normals[i].y);
                        vertex_data.push_back(normals[i].z);

                        vertex_data.push_back(texture_coords[i].x);
                        vertex_data.push_back(texture_coords[i].y);
                    }
                }

                virtual void setShaderProgram(std::shared_ptr<ShaderProgram> shaders) {shader_program = shaders;};

                virtual void draw() {};
                virtual void destroy() {};
        };

        class ICamera
        {
            public:
                virtual glm::mat4 _getViewMatrix() {return glm::mat4();};
        };

        class IRenderer
        {
            public:
                virtual void mainloop(std::function<void(float)> func) {};
                virtual void getFps() {};
                virtual bool createWindow(int width, int height, std::string title) {return true;};

                virtual double getTime() {return 0;}

                virtual std::shared_ptr<ShaderProgram> addShaderProgram(std::shared_ptr<ShaderResource> vert, std::shared_ptr<ShaderResource> frag) {return nullptr;};

                virtual std::shared_ptr<RenderObject> addRenderObject() {return nullptr;};
                virtual void renderRenderObject(std::shared_ptr<RenderObject> model, glm::mat4 global_transform, glm::mat4 local_transform) {};

                virtual void cleanup() {};

                virtual void setCamera(std::shared_ptr<ICamera> cam) {};
                virtual std::shared_ptr<ICamera> getCamera() {return nullptr;};

                virtual bool isKeyPressed(int key) {return false;};
                virtual bool isMouseButtonPressed(int button) {return false;};

                virtual glm::vec2 getMousePosition() {return glm::vec2();};
                virtual glm::vec2 getMouseOffset() {return glm::vec2();};

                virtual float getScrollWheelOffset() {return 0.0f;};

                virtual void setMouseMode(Engine::Input::MouseMode mode) {};
                virtual void setCursorMode(Engine::Input::CursorMode mode) {};

                virtual int getHeight() {return 0;};
                virtual int getWidth() {return 0;};
        };

    }
}

#endif
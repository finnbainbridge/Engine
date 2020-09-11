#ifndef ENGINE_RENDERER_H
#define ENGINE_RENDERER_H

#include "Engine/Engine.hpp"
// #include "Engine/DOM.hpp"
#include "glm/fwd.hpp"
#include <glm/glm.hpp>
#include <iostream>

namespace Engine {
    namespace Renderer {

        class IRenderObject
        {
            public:
                glm::mat4 global_transform;
                glm::mat4 local_transform;

                // This is the mesh data
                // Format: Tightly packed float32
                // 3 floats for position (x y z) 3 floats for normals (x y z) 2 floats for texture coords (x y)
                std::vector<glm::float32> vertex_data;

                // Indicies
                std::vector<glm::uint32> indicies;

                virtual void setMeshData(std::vector<glm::vec3> position, std::vector<glm::vec3> normals, std::vector<glm::vec2> texture_coords, std::vector<glm::uint32> indiciez)
                {
                    indicies = indiciez;

                    if (position.size() != normals.size() != texture_coords.size())
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
        };

        class IRenderer
        {
            public:
                virtual void mainloop() {};
                virtual void getFps() {};
                virtual bool createWindow(int width, int height, std::string title) {return true;};

                virtual std::shared_ptr<IRenderObject> addModel() {return nullptr;};
                virtual void renderModel(std::shared_ptr<IRenderObject> model) {};

                virtual void cleanup() {};
        };
    }
}

#endif
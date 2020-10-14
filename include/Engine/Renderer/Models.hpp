#ifndef ENGINE_MODELS_H
#define ENGINE_MODELS_H

#include "Engine/Engine.hpp"
#include "Engine/Log.hpp"
#include "Engine/Res.hpp"
#include "glm/fwd.hpp"
#include <cstdlib>
#include <utility>


namespace Engine
{
    namespace Models
    {
        struct _MeshFile
        {
            glm::uint32 version;
            glm::uint32 num_vertices;
            glm::uint32 num_indices;
            glm::uint32 size;
        };

        class MeshResource: public Res::IResource
        {
            private:
                std::vector<glm::float32> vertices;
                std::vector<glm::uint32> indices;
            public:
                const glm::uint32 file_format_version = 1;
                MeshResource()
                {
                    file_type = Res::FileType::binary;
                }

                // Returns vertices, a vector of sets of 8 floats in the order as follows: position x, position y, position z, normal x, normal y, normal z, texture coord x, texture coord y
                std::vector<glm::float32> getVertices() const
                {
                    return vertices;
                };

                // Returns indices, a vector of ints telling the renderer which order to render the vertices
                std::vector<glm::uint32> getIndices() const
                {
                    return indices;
                };

                void setVertices(std::vector<glm::float32> arg)
                {
                    vertices = arg;
                }

                void setIndices(std::vector<glm::uint32> arg)
                {
                    indices = arg;
                }

                virtual void loadFile(std::shared_ptr<std::stringstream> data);

                virtual void saveFile(std::shared_ptr<std::stringstream> data);
        };
    }
}

#endif
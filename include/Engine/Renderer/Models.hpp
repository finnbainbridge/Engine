#ifndef ENGINE_MODELS_H
#define ENGINE_MODELS_H

#include "Engine/Engine.hpp"
#include "Engine/Log.hpp"
#include "Engine/Res.hpp"
#include "glm/fwd.hpp"
#include <cstdlib>


namespace Engine
{
    namespace Models
    {
        class MeshResource: Res::IResource
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

                virtual void loadFile(std::shared_ptr<std::ifstream> data)
                {
                    glm::uint32 format_version, vertices_len, indices_len;

                    // Make sure this is actually a version we can read
                    data->read((char *)&format_version, sizeof(glm::uint32));
                    LOG_ASSERT_MESSAGE_FATAL(format_version != file_format_version, "Cannot load Mesh: File format is wrong. Please re-import your assets");

                    // Find length of verticies
                    data->read((char *)&vertices_len, sizeof(glm::uint32));

                    LOG_ASSERT_MESSAGE(data->fail(), "Mesh loader: Failed to load file");

                    // Allocate memory for vertex data, then fill it from the file
                    glm::float32 *vertex_data = (glm::float32*)malloc(vertices_len * sizeof(glm::float32) * 8);

                    // Read in from file
                    data->read((char *)vertex_data, vertices_len * sizeof(glm::float32) * 8);
                    
                    // Convert to vector and save
                    vertices = std::vector<glm::float32>(vertex_data, vertex_data + vertices_len*8);

                    LOG_ASSERT_MESSAGE(data->fail(), "Mesh loader: Failed to load vertices");

                    // Now do the same thing, but with indices

                    // Find length of indices
                    data->read((char *)&indices_len, sizeof(glm::uint32));

                    LOG_ASSERT_MESSAGE(data->fail(), "Mesh loader: Failed to load file");

                    // Allocate memory for vertex data, then fill it from the file
                    glm::uint32 *index_data = (glm::uint32*)malloc(vertices_len * sizeof(glm::uint32));

                    // Read in from file
                    data->read((char *)index_data, indices_len * sizeof(glm::uint32));
                    
                    // Convert to vector and save
                    indices = std::vector<glm::uint32>(index_data, index_data + indices_len);

                    LOG_ASSERT_MESSAGE(data->fail(), "Mesh loader: Failed to load indices");

                    // Cleanup...
                    delete vertex_data;
                    delete index_data;
                    // And we're done!
                }

                // TODO: Saving
        };
    }
}

#endif
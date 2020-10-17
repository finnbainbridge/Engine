#include "Engine/Renderer/Models.hpp"
#include "Engine/Log.hpp"
#include "glm/fwd.hpp"
#include <ios>

using namespace Engine::Models;

void MeshResource::loadFile(std::shared_ptr<std::stringstream> data)
{
    data->seekg(0, std::ios::beg);
    auto begin = data->tellg();
    data->seekg(0, std::ios::end);
    auto end = data->tellg();
    data->seekg(0, std::ios::beg);

    auto size = end - begin;

    // Load header
    _MeshFile header;
    data->read((char *) &header, sizeof(header));

    // Version & size checks
    LOG_ASSERT_MESSAGE_FATAL(header.version != file_format_version, "Mesh file is of incorrect version");
    LOG_ASSERT_MESSAGE_FATAL(header.size != size - sizeof(header), "Mesh data malformed: Make sure compression is correct");

    // Load vertices
    glm::float32* new_vertices = new glm::float32[header.num_vertices * 8];
    data->read((char *) new_vertices, header.num_vertices * 8 * sizeof(glm::float32));

    vertices = std::vector<glm::float32>(new_vertices, new_vertices + header.num_vertices * 8);

    // Now load indices
    glm::uint32* new_indices = new glm::uint32[header.num_indices];
    data->read((char *) new_indices, header.num_indices * sizeof(glm::uint32));

    indices = std::vector<glm::uint32>(new_indices, new_indices + header.num_indices);

    // And we're done!
    // Now we cleanup

    delete[] new_vertices;
    delete[] new_indices;
}

void MeshResource::saveFile(std::shared_ptr<std::stringstream> data)
{
    data->seekg(0, std::ios::beg);

    // Create header
    _MeshFile header;
    header.version = file_format_version;
    header.num_indices = indices.size();
    header.num_vertices = vertices.size() / 8;
    header.size = vertices.size() * sizeof(glm::float32) + indices.size() * sizeof(glm::uint32);

    // Write header
    data->write((char *) &header, sizeof(header));

    // Write vertices
    data->write((char *) vertices.data(), vertices.size() * sizeof(glm::float32));

    // Write indices
    data->write((char *) indices.data(), indices.size() * sizeof(glm::uint32));

}
#include "Engine/Element3D.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Log.hpp"
#include "Engine/Renderer/Models.hpp"
#include "Engine/Res.hpp"
#include "Engine/Tools/AssimpImporter.hpp"
#include "assimp/matrix4x4.h"
#include "glm/fwd.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <filesystem>
#include <memory>

std::string fname;

std::shared_ptr<Engine::Document> document;

void process_meshes(const aiScene* scene)
{
    if (!scene->HasMeshes())
    {
        LOG_WARN("Warning: File has no meshes");
        return;
    }

    for (int i = 0; i < scene->mNumMeshes; i++)
    {
        std::string mpath = fname + "_" + std::string(scene->mMeshes[i]->mName.C_Str()) + ".emesh";
        LOG_INFO("Extracting mesh " + std::string(scene->mMeshes[i]->mName.C_Str()) + " to file " + mpath);

        auto mesh = scene->mMeshes[i];

        auto mres = std::make_shared<Engine::Models::MeshResource>();

        std::vector<glm::float32> vertices;
        std::vector<glm::uint32> indices;

        // Now iterate through all the vertices and save them
        for (int j = 0; j < mesh->mNumVertices; j ++)
        {
            // Format: X Y Z NX NY NZ TX TY
            vertices.push_back(mesh->mVertices[j].x);
            vertices.push_back(mesh->mVertices[j].y);
            vertices.push_back(mesh->mVertices[j].z);

            vertices.push_back(mesh->mNormals[j].x);
            vertices.push_back(mesh->mNormals[j].y);
            vertices.push_back(mesh->mNormals[j].z);

            // Texture coords are weird
            if (mesh->mTextureCoords[0] != nullptr)
            {
                vertices.push_back(mesh->mTextureCoords[0][j].x);
                vertices.push_back(mesh->mTextureCoords[0][j].y);
            }
            else
            {
                // No texture coords :?
                vertices.push_back(0);
                vertices.push_back(0);
            }
        }

        // And now the indices
        for (int j = 0; j < mesh->mNumFaces; j++)
        {
            for (int k = 0; k < mesh->mFaces[j].mNumIndices; k++)
            {
                indices.push_back(mesh->mFaces[j].mIndices[k]);
            }
        }

        // Now dump these in the resource, and save them
        mres->setVertices(vertices);
        mres->setIndices(indices);

        // TODO: Change this
        // For now, this program must be run from the base directory of the project
        Engine::Res::ResourceManager::save(mpath, mres, true);
        
    }
}

std::shared_ptr<Engine::E3D::Element3D> convert_node(const aiScene* scene, aiNode* node, std::shared_ptr<Engine::E3D::Element3D> parent)
{
    std::shared_ptr<Engine::E3D::Element3D> element;

    if (node->mNumMeshes > 0)
    {
        // It's a MeshElement3D
        auto mesh_ele = std::make_shared<Engine::E3D::MeshElement3D>(document);

        // For now, only do the first mesh
        std::string mpath = fname + "_" + std::string(scene->mMeshes[node->mMeshes[0]]->mName.C_Str()) + ".emesh";
        mesh_ele->_setResourceDry(Engine::Res::ResourceManager::load<Engine::Models::MeshResource>(mpath, true));

        element = mesh_ele;

        if (node->mNumMeshes > 1 && parent != nullptr)
        {
            // Now do the rest
            for (int i = 1; i < node->mNumMeshes; i++)
            {
                auto n_mesh_ele = std::make_shared<Engine::E3D::MeshElement3D>(document);
                std::string n_mpath = fname + "_" + std::string(scene->mMeshes[node->mMeshes[i]]->mName.C_Str()) + ".emesh";
                n_mesh_ele->_setResourceDry(Engine::Res::ResourceManager::load<Engine::Models::MeshResource>(n_mpath, true));
                parent->appendChild(n_mesh_ele);
            }
        }
    }
    else
    {
        // Just a regular old Element3D
        element = std::make_shared<Engine::E3D::Element3D>(document);
    }

    float arr[] = {
        node->mTransformation.a1, node->mTransformation.a2, node->mTransformation.a3, node->mTransformation.a4,
        node->mTransformation.b1, node->mTransformation.b2, node->mTransformation.b3, node->mTransformation.b4,
        node->mTransformation.c1, node->mTransformation.c2, node->mTransformation.c3, node->mTransformation.c4,
        node->mTransformation.d1, node->mTransformation.d2, node->mTransformation.d3, node->mTransformation.d4
    };

    // Add transform
    glm::mat4 transform = glm::make_mat4(arr);
    element->setTransform(transform);

    // Now do the same to the children
    for (int i = 0; i < node->mNumChildren; i++)
    {
        element->appendChild(convert_node(scene, node->mChildren[i], element));
    }

    return element;
}

void create_scene(const aiScene* scene)
{
    // Create document
    // Don't fully create it, though
    document = std::make_shared<Engine::Document>();
    document->addExtension(std::make_shared<Engine::E3D::E3DExtension>());
    // Load meshes as actual elements
    // RECURSION!!!
    auto val = convert_node(scene, scene->mRootNode, nullptr);

    LOG_INFO("Saving to file: " + fname + ".xml");
    val->saveToFile(fname + ".xml");
    LOG_INFO("Done");
}

void assimp_import(std::string filename)
{
    // Create an importer
    Assimp::Importer importer;

    size_t lastindex = filename.find_last_of("."); 
    fname = filename.substr(0, lastindex); 

    LOG_INFO("Loading file: " + Engine::Res::ResourceManager::getDirname() + "/" + filename);

    const struct aiScene* scene = importer.ReadFile(Engine::Res::ResourceManager::getDirname() + "/" + filename, aiProcess_CalcTangentSpace |
                aiProcess_Triangulate            |
                aiProcess_JoinIdenticalVertices  |
                aiProcess_SortByPType);
    
    // Check if it failed
    if (!scene)
    {
        LOG_ERROR("Assimp import failed with this message: " + std::string(importer.GetErrorString()));
        return;
    }

    /*
    Game plan:
    Step 1: Convert all the meshes into .emesh files
    Step 2: Go through the file's scene tree and create an xml file 
            representing it
    */

    process_meshes(scene);

    create_scene(scene);
}
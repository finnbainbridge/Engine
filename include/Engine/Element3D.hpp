#ifndef ENGINE_ELEMENT3D_H
#define ENGINE_ELEMENT3D_H

#include "Engine/Engine.hpp"
#include "Engine/Renderer/Models.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "glm/fwd.hpp"
#include <glm/glm.hpp>
#include <mutex>
#include <thread>

namespace Engine
{
    namespace E3D
    {
        enum CoordinateType {Local, Global};
        /*
        Ascii Diagram of how Transformations generally work for Element3Ds:

        +-------------+
        |             +------+ +-------+
        |  Element3D  | Local| |Global |
        |             +------+ +-------+
        +-----+-------+ Transformations
            ^                      ^
            |                      |
            |                      |
        +-----+-------+            |
        |             |      +-----+
        | Element     |      |
        |             |      |
        +-----+-------+      +
            ^             +->* <---+
            |             |        |
        +-----+-------+   |        |
        |             +---+--+ +---+---+
        |  Element3D  | Local| |Global |
        |             +------+ +-------+
        +-------------+ Transformations
        
        */

        /*
        The base element of all things 3D. Element3D implements local and global coordinates, as well as transformations in 3d space.
        */
        class Element3D: public DOM::Element
        {
            private:
                

            protected:
                glm::mat4 transform;
                std::mutex transform_lock;
                
                glm::mat4 global_transform;
                std::mutex global_transform_lock;
            public:
                Element3D(std::shared_ptr<Document> parent_document);
                glm::mat4 getTransform() const;
                glm::mat4 getGlobalTransform() const;

                // This tells this element that it's parent has been updated, and it must update it's position accordingly
                void updateGlobalTransform();

                // This tells the Renderer if this element should always be in the global coordinate system, or in it's parent's coordinate system
                CoordinateType coord_type = Local;

                // Rotates this Element3D and all its children by _angle_ (in radians) along _axis_
                void rotate(float angle, glm::vec3 axis);

                // Rotates this Element 3D and all it's children on the given *global* axis
                void rotateGlobal(float angle, glm::vec3 axis);

                // Translates this Element3D and all its children by the given offset
                void translate(glm::vec3 offset);

                // Scales this Element3D and all it's children by the given scaler for each axis
                void scale(glm::vec3 scaler);

                void setTransform(glm::mat4 trans);

                // This tells all this element's children to update their positions. This should be called after 
                // you modify the transformation matricies yourself
                void callChildUpdate();

                // Tells the devtools orbit camera to target this element
                // This should *only* be called while devtools is open
                void devtoolsOrbit();

                virtual void onLoad();
                virtual void onSave();

        };


        class CameraElement3D: public Element3D, public Renderer::ICamera
        {
            private:
            public:
                CameraElement3D(std::shared_ptr<Document> document);
                virtual glm::mat4 _getViewMatrix();
        };

        /*
        The most basic Mesh element. You give it points, it renders them.
        */
        class ManualMeshElement3D: public Element3D
        {
            private:
                bool given_points = false;
            protected:
                std::shared_ptr<Renderer::RenderObject> render_object;
            public:
                ManualMeshElement3D(std::shared_ptr<Document> parent_document);
                
                void setMesh(std::vector<glm::vec3> points, std::vector<glm::vec3> normals, std::vector<glm::vec2> tex_coords, std::vector<glm::uint32> indicies, std::shared_ptr<Renderer::ShaderProgram> shaders);

                virtual void render(float delta);
        };

        /*
        The element that is used to display most things. MeshElement3D is capable of rendering 3d models
        */
        class MeshElement3D: public Element3D
        {
            private:
                bool has_data = false;
            protected:
                std::shared_ptr<Renderer::RenderObject> render_object;
                std::shared_ptr<Models::MeshResource> resource;
                std::shared_ptr<Renderer::ShaderProgram> shaders;
            public:
                MeshElement3D(std::shared_ptr<Document> doc);
                virtual void init();

                void setResource(std::shared_ptr<Models::MeshResource> res);

                // Sets the reource of the mesh without actually adding an openhl object.
                // Intended for use when building scenes offline
                void _setResourceDry(std::shared_ptr<Models::MeshResource> res);

                void setShaders(std::shared_ptr<Renderer::ShaderProgram> shaders);
                virtual void render(float delta);

                virtual void onSave();
                virtual void onLoad();
        };

        // The extension class
        class E3DExtension: public IExtension
        {
            public:
                virtual void start(std::shared_ptr<Document> doc);
        };
    }
}

#endif
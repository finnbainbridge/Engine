#ifndef ENGINE_ELEMENT3D_H
#define ENGINE_ELEMENT3D_H

#include "Engine/Engine.hpp"
#include "Engine/Log.hpp"
#include "Engine/Renderer/Models.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "glm/fwd.hpp"
#include <glm/glm.hpp>
#include <math.h>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

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

                virtual void appendChild(std::shared_ptr<DOM::Element> elem);
                virtual void onParentAdded();

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

        /* The basic light class. All it does is emits light in a given radius */
        class LightElement3D: public Element3D
        {
            private:
                std::string vectorToString(glm::vec3 vec)
                {
                    return std::to_string(vec.x) + "," + std::to_string(vec.y) + "," + std::to_string(vec.z);
                }

                glm::vec3 stringToVector(std::string vec)
                {
                    std::vector<float> tokens;
                    std::string token;
                    std::istringstream token_stream(vec);
                    while (std::getline(token_stream, token, ','))
                    {
                        tokens.push_back(std::stof(token));
                    }

                    if (tokens.size() != 3)
                    {
                        LOG_ERROR("Vec3 must contain 3 floats: not this: " + vec);
                        return glm::vec3(0, 0, 0);
                    }

                    return glm::vec3(token[0], token[1], token[2]);
                };

            public:
                LightElement3D(std::shared_ptr<Document> document): Element3D(document)
                {
                    setTagName("light3d");
                }

                // glm::vec3 diffuse = glm::vec3(1, 1, 1);
                glm::vec3 ambient = glm::vec3(0.5, 0.5, 0.5);
                // glm::vec3 specular = glm::vec3(0.2, 0.2, 0.2);
                glm::vec3 intensity = glm::vec3(0.5, 0.5, 0.5);
                int radius = 5;

                virtual void render(float delta) 
                {
                    document->renderer->addLight(std::dynamic_pointer_cast<LightElement3D>(shared_from_this()));
                };

                virtual void onSave()
                {
                    // setAttribute("diffuse", vectorToString(diffuse));
                    setAttribute("ambient", vectorToString(ambient));
                    setAttribute("intensity", vectorToString(intensity));
                    // setAttribute("specular", vectorToString(specular));
                    setAttribute("radius", radius);
                };

                virtual void onLoad()
                {
                    // diffuse = stringToVector(std::get<std::string>(getAttribute("diffuse")));
                    ambient = stringToVector(std::get<std::string>(getAttribute("ambient")));
                    // specular = stringToVector(std::get<std::string>(getAttribute("specular")));
                    intensity = stringToVector(std::get<std::string>(getAttribute("intensity")));
                    radius = std::get<int>(getAttribute("radius"));
                };
        };

        enum ShadingMode
        {
            Fragment,
            Vertex
        };

        class MeshMaterial: public Renderer::UniformObject
        {
            public:
                // Colour of the material (will be overriden by textures)
                glm::vec3 diffuse;

                // Ambient colour
                glm::vec3 ambient;

                // Specular
                glm::vec3 specular;

                // Shininess
                float shininess;

                // Culling mode
                Renderer::CullingMode culling_mode = Renderer::CullingMode::Front;

                // Shading mode:
                // For most things, per fragment shading produces better results.
                // But for objects far away, or not very important, per vertex shading is more efficient
                ShadingMode shading_mode = ShadingMode::Fragment;

                // Tells the renderer if it should light tie inside of the shape as well.
                // Needed for shapes with holes in them
                // bool two_sided;

                virtual void setUniforms(std::shared_ptr<Renderer::ShaderProgram> sp, std::shared_ptr<Renderer::RenderObject> re, std::vector<std::shared_ptr<LightElement3D>> lights, glm::mat4 global_position)
                {
                    // Setting shader uniforms goes here
                    int on_num = 0;

                    // TODO: Default lights
                    // TODO: Detect if only half of an object needs to be shaded

                    // Find every light that effects this object
                    for (int i = 0; i < lights.size(); i++) {
                        // if (i > lights.size()-1)
                        // {
                        //     sp->setUniform("light["+std::to_string(i)+"].position", glm::vec4(0, 0, 2, 1));

                        //     // Diffuse, ambient, and specular, of light
                        //     sp->setUniform("light["+std::to_string(i)+"].diffuse", glm::vec3(0.5, 0.5, 0.5));
                        //     sp->setUniform("light["+std::to_string(i)+"].ambient", glm::vec3(0.5, 0.5, 0.5));
                        //     sp->setUniform("light["+std::to_string(i)+"].specular", glm::vec3(0.5, 0.5, 0.5));
                        //     sp->setUniform("light["+std::to_string(i)+"].radius", 0);

                        //     // Set existance
                        //     sp->setUniform("light["+std::to_string(i)+"].exists", 0);

                        //     continue;
                        // }
                        
                        auto lpos = lights[i]->getGlobalTransform() * lights[i]->getTransform() * glm::vec4(0,0,0,1);

                        if (glm::distance(lpos, global_position * glm::vec4(0, 0, 0, 1)) <= lights[i]->radius)
                        {
                            // Within radius
                            sp->setUniform("light["+std::to_string(i)+"].position", lpos);

                            // Diffuse, ambient, and specular, of light
                            // sp->setUniform("light["+std::to_string(i)+"].diffuse", lights[i]->diffuse);
                            sp->setUniform("light["+std::to_string(i)+"].ambient", lights[i]->ambient);
                            // sp->setUniform("light["+std::to_string(i)+"].specular", lights[i]->specular);
                            sp->setUniform("light["+std::to_string(i)+"].intensity", lights[i]->intensity);
                            sp->setUniform("light["+std::to_string(i)+"].radius", lights[i]->radius);

                            if (shading_mode == ShadingMode::Fragment)
                            {
                                sp->setUniform("shading_mode", 1);
                                sp->setUniform("light["+std::to_string(i)+"].attenv", 1.0f - glm::distance(global_position * glm::vec4(0, 0, 0, 1), lpos)/lights[i]->radius);
                            }
                            else
                            {
                                // For vertex shading, we do this per vertex
                                // Which looks better
                                // But that's not viable per fragment
                                sp->setUniform("light["+std::to_string(i)+"].attenv", 0.0f);
                                sp->setUniform("shading_mode", 0);
                            }

                            // Set existance
                            sp->setUniform("light["+std::to_string(i)+"].exists", 1);

                            on_num ++;
                            if (on_num > 31)
                            {
                                break;
                            }
                        }
                    }

                    for (int i = on_num; i < 32; i++)
                    {
                        sp->setUniform("light["+std::to_string(i)+"].position", glm::vec4(0, 0, 2, 1));

                        // Diffuse, ambient, and specular, of light
                        sp->setUniform("light["+std::to_string(i)+"].diffuse", glm::vec3(0.5, 0.5, 0.5));
                        sp->setUniform("light["+std::to_string(i)+"].ambient", glm::vec3(0.5, 0.5, 0.5));
                        sp->setUniform("light["+std::to_string(i)+"].specular", glm::vec3(0.5, 0.5, 0.5));
                        sp->setUniform("light["+std::to_string(i)+"].radius", 0);

                        // Set existance
                        sp->setUniform("light["+std::to_string(i)+"].exists", 0);
                    }

                    // Material stuff
                    sp->setUniform("material.diffuse", diffuse);
                    sp->setUniform("material.ambient", ambient);
                    sp->setUniform("material.specular", specular);
                    sp->setUniform("material.shininess", shininess);
                }
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

                std::shared_ptr<MeshMaterial> material;
            public:
                MeshElement3D(std::shared_ptr<Document> doc);
                virtual void init();

                void setResource(std::shared_ptr<Models::MeshResource> res);

                // Sets the reource of the mesh without actually adding an openhl object.
                // Intended for use when building scenes offline
                void _setResourceDry(std::shared_ptr<Models::MeshResource> res);

                void setMaterial(std::shared_ptr<MeshMaterial> mat)
                {
                    material = mat;
                };

                std::shared_ptr<MeshMaterial> getMaterial() const
                {
                    return material;
                };

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
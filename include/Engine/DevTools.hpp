#ifndef ENGINE_DEVTOOLS_H
#define ENGINE_DEVTOOLS_H

#include "Engine/Engine.hpp"
#include "Engine/Element3D.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "glm/fwd.hpp"
#include <thread>
#include <mutex>


namespace Engine {

    namespace DevTools {
        
        class OrbitCamera3D: public Renderer::ICamera, public E3D::Element3D
        {
            private:
                glm::vec3 target;
                float yaw;
                float pitch;
                float radius;

                float angle_x;
                float angle_y;

                std::mutex cam_lock;

                glm::vec3 cam_pos;

                std::shared_ptr<E3D::CameraElement3D> camera;
            public:
                OrbitCamera3D(std::shared_ptr<Document> new_document);
                ~OrbitCamera3D() {};
                virtual void init();
                virtual void process(float delta);
                virtual glm::mat4 _getViewMatrix();

                void setPosition(glm::vec3 pos);
        };

        class DevTools: public DOM::Element
        {
            private:
                // Integer defining which tab we're currently on
                // The first subelement of DevTools is 0, the next 1, etc
                int current_tab;
                double cooldown_time = 0;
                std::shared_ptr<OrbitCamera3D> camera;
                std::shared_ptr<Renderer::ICamera> old_camera = nullptr;

                std::mutex cam_switch_lock;
                int camera_switch = 0;
            public:
                DevTools(std::shared_ptr<Document> document);
                virtual void init();
                virtual void render(float delta);
                virtual void process(float delta);

                bool is_active = false;

                void targetCamera(std::shared_ptr<E3D::Element3D> element);
        };

        /*
        Base class for DevTools tab. Make sure you call setTabName in the constructor.
        Add one of these to DevTools to add another tab to the devtools UI
        */
        class DevToolsTab: public DOM::Element
        {
            private:
                std::string tab_name;
            public:
                DevToolsTab(std::shared_ptr<Document> doc);
                virtual void render(float delta);
                virtual void renderUI(float delta);

                virtual void setTabName(std::string name);
                virtual std::string getTabName();
        };

        /*
        Tab for the DevTools that shows the DOM in tree view format
        */
        class DevToolsTree: public DevToolsTab
        {
            private:
                int recursiveTreeRender(std::shared_ptr<DOM::Element> element, int id);
            public:
                DevToolsTree(std::shared_ptr<Document> doc);
                virtual void renderUI(float delta);
        };

    }
}

#endif
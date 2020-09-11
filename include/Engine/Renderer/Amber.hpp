#ifndef ENGINE_RENDERER_AMBER_H
#define ENGINE_RENDERER_AMBER_H

// #include "Engine/Engine.hpp"
// #include "Engine/DOM.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "GLFW/glfw3.h"
#include <string>

namespace Engine {

    namespace Renderer {

        class Amber: public IRenderer
        {
            private:
                GLFWwindow* window;
                std::shared_ptr<Document> document;
                double last_frame_start;
                int width;
                int height;

                void loop();

            public:
                Amber(std::shared_ptr<Document> doc);
                ~Amber();
                virtual bool createWindow(int width, int height, std::string title);

                virtual void mainloop();

                // void onFrameBufferSizeChange(GLFWwindow* window, int iwidth, int iheight);
        };
    }
}

#endif
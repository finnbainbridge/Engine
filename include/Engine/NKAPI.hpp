#ifndef ENGINE_NKAPI_H
#define ENGINE_NKAPI_H


#include "Engine/Engine.hpp"
#include "nuklear/nuklear.h"

struct nk_context;

namespace Engine
{
    namespace NKAPI
    {
        // Global nuklear context
        inline nk_context *ctx;

        // Nulkear element
        class NuklearElement: public DOM::Element
        {
            public:
                NuklearElement(std::shared_ptr<Document> document): DOM::Element(document)
                {
                    setTagName("nuklear");
                }
                virtual void render(float delta);
        };
    }
}


#endif
#ifndef ENGINE_H
#define ENGINE_H

#include "Engine/DOM.hpp"

namespace Engine
{
    class Document
    {
    private:
        /* data */
    public:
        Document();
        ~Document();

        std::shared_ptr<DOM::Element> createElement(DOM::Element* element);
    };
}

#endif
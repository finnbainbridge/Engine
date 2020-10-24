#ifndef ENGINE_H
#define ENGINE_H

#ifdef __EMSCRIPTEN__
#define ENGINE_NO_THREADING
#endif

// #include "Engine/DOM.hpp"

// #include "Engine/Threading.hpp"
// #include "Engine/Renderer/Renderer.hpp"
#include <exception>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#include <thread>
#include <functional>
#include <variant>
#include <string>
#include <map>

#include "glm/fwd.hpp"
#include "Engine/Log.hpp"

// So I don't have header hell
namespace tinyxml2
{
    class XMLElement;
    class XMLDocument;
}

namespace Engine
{
    class Document;

    namespace Renderer {
        class IRenderer;
    }

    namespace DevTools
    {
        class DevTools;
    }

    namespace Types
    {
        class TypeContainer
        {
            private:
                glm::uint64 storage [8] = {0,0,0,0,0,0,0,0};
            public:
                void setType(int new_type)
                {
                    if (new_type > 511)
                    {
                        LOG_ERROR("Error: Only 511 types are supported");
                        return;
                    }
                    if (isType(new_type))
                    {
                        // Don't unset ourselves
                        return;
                    }
                    int num = new_type/64;
                    int bit = new_type % 64;

                    storage[num] |= 1UL << bit;
                }

                bool isType(int new_type)
                {
                    if (new_type > 511)
                    {
                        LOG_ERROR("Error: Only 511 types are supported");
                        return false;
                    }

                    int num = new_type/64;
                    int bit = new_type % 64;

                    int value = (storage[num] >> bit) & 1U;
                    return value;
                }
        };

        class ElementTypes
        {
            private:
                int on;
                std::map<std::string, int> types;
            public:
                ElementTypes()
                {
                    on = 0;
                    types = std::map<std::string, int>();
                }

                int getTypeOfElement(std::string element)
                {
                    // Check if it exists
                    if (types.find(element) == types.end()) {
                        // We have to create a new type
                        on ++;
                        types[element] = on;
                        return on;
                    }
                    else
                    {
                        return types[element];
                    }
                }
        };
    }

    namespace Threading
    {
        struct Task
        {
            std::function<void()> function;
        };

        // Static class
        // class ThreadPool
        // {
        // private:
            // ThreadPool(/* args */);
            // ~ThreadPool();

            

        // public:
            void startThreads();
            void threadWorker();
            void lesserThreadWorker();
            void cleanup();

            void waitForCompletion();

            void addTask(std::function<void()> function);
            void addLesserTask(std::function<void()> function);
            
        // };
    } // namespace Threading

    namespace DOM
    {
        class ClassList
        {
        private:
            
        public:
            ClassList();
            ~ClassList();

            std::vector<std::string> classes;

            void add(std::string element_class);
            void remove(std::string element_class);
            bool has(std::string element_class);
        };

        typedef std::variant<unsigned int, int, float, std::string> AttrVariant;
        class Element: public std::enable_shared_from_this<Element>
        {
        private:
            std::vector<std::shared_ptr<Element>> children;
            std::map<std::string, AttrVariant> attributes;

            std::shared_ptr<Element> parent;

            bool visible = true;
            bool do_process = true;

            std::map<std::string, std::function<void()>> devtools_buttons;

            tinyxml2::XMLElement* elementToXMLElement(std::shared_ptr<Element> elem, tinyxml2::XMLDocument* doc);

        public:
            Element(std::shared_ptr<Document> parent_document);
            ~Element();

            // Get the name used to refer to this tag
            std::string getTagName();

            // Find an element using the id attribute. This will only look through this element's children, and their children, etc
            std::shared_ptr<Element> getElementById(std::string id);

            // Find a vector of elements which are of the tag `tag`. This will only look through this element's children, and their children, etc
            std::vector<std::shared_ptr<Element>> getElementsByTagName(std::string tag, bool derived = false);

            // Finds parents of this element which are a certain tag
            std::vector<std::shared_ptr<Element>> getParentsByTagName(std::string tag, bool derived = false);

            // Find a vector of elements which have the class `class`. This will only look through this element's children, and their children, etc
            std::vector<std::shared_ptr<Element>> getElementsByClassName(std::string clas);

            // Returns this element's children
            std::vector<std::shared_ptr<Element>> getChildren();

            // Add a child to this element. This will remove them from their previous parent
            virtual void appendChild(std::shared_ptr<Element> child);

            // Function called when a new parent is added
            virtual void onParentAdded() {};

            // Remove a child from this element
            virtual void removeChild(std::shared_ptr<Element> child);

            // Function called when a new parent is removed
            virtual void onParentRemoved() {};

            // Checks if a given element is this element's child
            bool hasChild(std::shared_ptr<Element> child);

            // Changes this elements parent element. *Note:* This function does not properly remove this node from it's parent and should only be used internally
            void setParent(std::shared_ptr<Element> element);

            // Returns this element's parent element
            std::shared_ptr<Element> getParent();

            // Returns true if this element has a parent
            bool hasParent();

            // Returns true if the given element is a descendant of this element
            bool contains(std::shared_ptr<Element> element);

            std::string getId();
            void setId(std::string id);

            // This function is called the first frame after a node is added
            virtual void init() {};
            bool inited = false;
            
            // The main loop. This function will be called every frame. This function will be called in it's own thread
            virtual void process(float delta) {};

            // The _other_ main loop. This function will be called syncrinously. Mostly intended for rendering
            virtual void render(float delta) {};

            // This gets called when this element is loaded from an xml file. Runs syncrinously, before init()
            // Use this to load all your data from attributes
            virtual void onLoad()
            {

            }

            // This gets called when this element is being saved to a file. 
            // Store all your important data into attributes
            virtual void onSave()
            {

            }

            // Saves this element (and all it's children) to the specified file
            // The produced XML can be loaded with document.loadFromFile
            void saveToFile(std::string filename);

            // Sets the visibility of this element. If it's invisible, the render function of this element and it's children will not be called
            void setVisible(bool new_vis)
            {
                visible = new_vis;
            }

            // Gets this element's visibility
            bool getVisible() const
            {
                return visible;
            }

            // Sets the processability of this element. If it's inprocessable, the process function of this element and it's children will not be called
            void setProcess(bool new_proc)
            {
                do_process = new_proc;
            }

            // Gets this element's processability
            bool getProcess() const
            {
                return do_process;
            }

            // Sets one of the element's attributes
            void setAttribute(std::string attribute, AttrVariant value);

            // Gets an attribute from the element. Returns something if failed
            AttrVariant getAttribute(std::string attribute);

            // Returns true is that attribute exists, otherwise false
            bool hasAttribute(std::string attribute);

            // Destroys the element and all it's children
            void destroy();

            // Sets the tag name
            void setTagName(std::string tag);

            // Object that stores this Element's type
            Engine::Types::TypeContainer type_container;

            // Object containing which this element's classes
            ClassList classList;

            // A smart pointer to the document
            std::shared_ptr<Engine::Document> document;

            // DevTools buttons
            void addDevToolsButton(std::string name, std::function<void()> func)
            {
                devtools_buttons[name] = func;
            }

            std::map<std::string, std::function<void()>> getDevToolsButtons() const
            {
                return devtools_buttons;
            }

        protected:
            // Set this first thing
            std::string tag_name = "element";
            std::string id; // Id will also be an attribute, but we put it here for easy access
        };

        // Template black magic to get this to work
        class ElementClass
        {
            public:
                virtual std::shared_ptr<Element> getNewInstance(std::shared_ptr<Document> doc) {return nullptr;};
        };

        // Class that stores an element type. Can be used to make more instances of that type
        template<typename T>
        class ElementClassFactory: public ElementClass
        {
            public:
                virtual std::shared_ptr<Element> getNewInstance(std::shared_ptr<Document> doc)
                {
                    return std::dynamic_pointer_cast<Element>(std::make_shared<T>(doc));
                }
        };
    } // namespace DOM

    /*
    A small class where you can put your initialisation code, for example adding all your elements to the Document
    */
    class IExtension
    {
        private:
        public:
            virtual void start(std::shared_ptr<Document> doc) {};
    };

    class Document: public std::enable_shared_from_this<Document>
    {
    private:
        std::shared_ptr<DOM::Element> base;

        // We still have to create this even when theading if off
        // This means we will have to compile with threads
        // But hopefully the code will still run on platforms without 
        // Thread support
        std::vector<std::thread*> operations;

        std::shared_ptr<Document> self_ptr;

        void executeElement(float delta, std::shared_ptr<DOM::Element> element);
        void renderElement(float delta, std::shared_ptr<DOM::Element> element);
        std::shared_ptr<Engine::DOM::Element> xmlElementToElement(tinyxml2::XMLElement* node);

    public:
        Document();
        ~Document();

        Types::ElementTypes element_types;

        // A map which contains the classes of each element. 
        // Mainly used when loading XML to instanciate the correct classes
        std::map<std::string, std::shared_ptr<DOM::ElementClass>> element_classes;

        // Adds an element to the central database. Only added elements will be able to be loaded from files
        void addElement(std::string name, std::shared_ptr<DOM::ElementClass> type);

        // Loads elements from an XML file. returns a shared pointer to the base element of the file
        // Elements will only be properly loaded if they've beed added to the document using addElement
        std::shared_ptr<DOM::Element> loadFromFile(std::string filename);

        std::shared_ptr<DOM::Element> head;
        std::shared_ptr<DOM::Element> body;
        std::shared_ptr<DevTools::DevTools> devtools;

        void setup();

        // Adds an extension to the document. The E3D extension (for 3d related elements) is added automatically
        void addExtension(std::shared_ptr<IExtension> ext)
        {
            ext->start(shared_from_this());
        }

        // std::shared_ptr<DOM::Element> createElement(DOM::Element* element);

        void tick(float delta);

        void destroy();

        static std::shared_ptr<Document> createDocument() {
            Engine::Threading::startThreads();
            auto document = std::make_shared<Engine::Document>();
            document->setup();
            return document;
        }

        std::shared_ptr<Renderer::IRenderer> renderer;
    };
}

#endif
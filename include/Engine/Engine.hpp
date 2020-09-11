#ifndef ENGINE_H
#define ENGINE_H

// #include "Engine/DOM.hpp"

// #include "Engine/Threading.hpp"
// #include "Engine/Renderer/Renderer.hpp"
#include <memory>
#include <vector>

#include <thread>
#include <functional>
#include <variant>
#include <string>
#include <map>


namespace Engine
{
    class Document;

    namespace Renderer {
        class IRenderer;
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
            std::vector<std::string> classes;
        public:
            ClassList();
            ~ClassList();

            void add(std::string element_class);
            void remove(std::string element_class);
            bool has(std::string element_class);
        };
        class Element;
        typedef std::variant<uint, int, float, std::string> AttrVariant;
        class Element: public std::enable_shared_from_this<Element>
        {
        private:
            std::vector<std::shared_ptr<Element>> children;
            std::map<std::string, AttrVariant> attributes;

            std::shared_ptr<Element> parent;

        public:
            Element(std::shared_ptr<Document> parent_document);
            ~Element();

            // Get the name used to refer to this tag
            std::string getTagName();

            // Find an element using the id attribute. This will only look through this element's children, and their children, etc
            std::shared_ptr<Element> getElementById(std::string id);

            // Find a vector of elements which are of the tag `tag`. This will only look through this element's children, and their children, etc
            std::vector<std::shared_ptr<Element>> getElementsByTagName(std::string tag);

            // Find a vector of elements which have the class `class`. This will only look through this element's children, and their children, etc
            std::vector<std::shared_ptr<Element>> getElementsByClassName(std::string clas);

            // Returns this element's children
            std::vector<std::shared_ptr<Element>> getChildren();

            // Add a child to this element. This will remove them from their previous parent
            void appendChild(std::shared_ptr<Element> child);

            // Remove a child from this element
            void removeChild(std::shared_ptr<Element> child);

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
            
            // The main loop. This function will be called every frame. This function will be called in it's own thread
            virtual void process(float delta) {};

            // The _other_ main loop. This function will be called syncrinously. Mostly intended for rendering
            virtual void render(float delta) {};

            // Sets one of the element's attributes
            void setAttribute(std::string attribute, AttrVariant value);

            // Gets an attribute from the element. Returns something if failed
            AttrVariant getAttribute(std::string attribute);

            // Returns true is that attribute exists, otherwise false
            bool hasAttribute(std::string attribute);

            // Destroys the element and all it's children
            void destroy();

            // Sets the tag name
            void setTagName(std::string tag)
            {
                tag_name = tag;
            }

            // Object containing which this element's classes
            ClassList classList;

            // A smart pointer to the document
            std::shared_ptr<Engine::Document> document;

        protected:
            // Set this first thing
            std::string tag_name = "element";
            std::string id; // Id will also be an attribute, but we put it here for easy access
        };
    } // namespace DOM

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

    public:
        Document();
        ~Document();

        std::shared_ptr<DOM::Element> head;
        std::shared_ptr<DOM::Element> body;

        void setup();

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
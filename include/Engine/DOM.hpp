#ifndef ENGINE_DOM_H
#define ENGINE_DOM_H

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <list>
#include <map>
#include <variant>

namespace Engine
{
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

        typedef std::variant<uint, int, float, std::string> AttrVariant;
        class Element
        {
        private:
            std::vector<std::shared_ptr<Element>> children;
            std::map<std::string, AttrVariant> attributes;

            std::shared_ptr<Element> parent;

        public:
            Element();
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
            
            // The main loop. This function will be called every frame
            virtual void process(float delta) {};

            // Sets one of the element's attributes
            void setAttribute(std::string attribute, AttrVariant value);

            // Gets an attribute from the element. Returns something if failed
            AttrVariant getAttribute(std::string attribute);

            // Returns true is that attribute exists, otherwise false
            bool hasAttribute(std::string attribute);

            // Object containing which this element's classes
            ClassList classList;

            // A smart pointer to itself. This is to be used whenever it is nessesary to point to this element
            std::shared_ptr<Element> self_ptr;

        protected:
            // Set this first thing
            std::string tag_name = "element";
            std::string id; // Id will also be an attribute, but we put it here for easy access

            void setTagName(std::string tag)
            {
                tag_name = tag;
            }
        };
    } // namespace DOM

} // namespace Engine



#endif
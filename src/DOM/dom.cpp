// #include "Engine/DOM.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Log.hpp"
#include "Engine/Res.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <memory>
#include <variant>
#include <vector>
#include <map>
#include <algorithm>

#include <tinyxml2.h>

using namespace Engine::DOM;

Element::Element(std::shared_ptr<Engine::Document> parent_document)
: children(),
// attributes(),
id(""),
document(parent_document),
classList()
{
    setTagName("element");
}

Element::~Element()
{
    
}

std::string Element::getTagName()
{
    return tag_name;
}

void Element::appendChild(std::shared_ptr<Element> child)
{
    if (child->hasParent())
    {
        // Make sure it's not the a child of 2 elements
        child->getParent()->removeChild(child);
    }

    child->setParent(shared_from_this());
    children.push_back(child);

    // Call the event
    child->onParentAdded();
}

void Element::removeChild(std::shared_ptr<Element> child)
{
    //children.remove(child);
    for (size_t i = 0; i < children.size(); i++)
    {
        if (children[i] == child)
        {
            children.erase(children.begin() + i);
            break;
        }
    }
    
    child->setParent(nullptr);

    // Call the Evant
    child->onParentRemoved();
}

bool Element::hasChild(std::shared_ptr<Element> child)
{
    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}

std::vector<std::shared_ptr<Element>> Element::getChildren()
{
    return children;
}

void Element::setParent(std::shared_ptr<Element> new_parent)
{
    LOG_ASSERT_MESSAGE_FATAL(hasParent() && new_parent != nullptr, "Could not run setParent: element has parent and new parent isn't nullptr");
    // Do not run removeChild here 
    parent = new_parent;
}

std::shared_ptr<Element> Element::getParent()
{
    return parent;
}

bool Element::hasParent()
{
    if (parent == nullptr)
    {
        return false;
    }
    else
    {
        return true;
    }
}

std::string Element::getId()
{
    return id;
}

void Element::setId(std::string new_id)
{
    id = new_id;
}

void Element::destroy()
{
    for (size_t i = 0; i < children.size(); i++)
    {
        children[i]->destroy();
    }

    // This probably frees this element, but I'm not really sure
    // self_ptr.reset();
}

void Element::setTagName(std::string tag)
{
    tag_name = tag;

    // Add it to the types database
    type_container.setType(document->element_types.getTypeOfElement(tag_name));
}

// ==============================================
// Selectors

std::shared_ptr<Element> Element::getElementById(std::string id)
{
    for (size_t i = 0; i < children.size(); i++)
    {
        if (children[i]->getId() == id)
        {
            return children[i];
        }
        else {
            auto res = children[i]->getElementById(id);
            if (res != nullptr)
            {
                return res;
            }
        }
    }
    LOG_WARN("Could not find element with given id");
    return nullptr;
}

std::vector<std::shared_ptr<Element>> Element::getElementsByTagName(std::string tag, bool derived)
{
    std::vector<std::shared_ptr<Element>> output;

    for (size_t i = 0; i < children.size(); i++)
    {
        if (derived == false)
        {
            if (children[i]->getTagName() == tag)
            {
                output.push_back(children[i]);
            }
        }
        else
        {
            if (children[i]->type_container.isType(document->element_types.getTypeOfElement(tag)))
            {
                output.push_back(children[i]);
            }
        }

        auto more = children[i]->getElementsByTagName(tag, derived);
        if (more.size() > 0)
        {
            output.insert(output.end(), more.begin(), more.end());
        }
    }

    return output;
}

std::vector<std::shared_ptr<Element>> Element::getParentsByTagName(std::string tag, bool derived)
{
    std::vector<std::shared_ptr<Element>> output;
    
    if (getParent() != nullptr)
    {
        if (derived == false)
        {
            if (getParent()->getTagName() == tag_name)
            {
                output.push_back(getParent());
                auto more = getParent()->getParentsByTagName(tag);
                if (more.size() > 0)
                {
                    output.insert(output.end(), more.begin(), more.end());
                }
            }
        }
        else {
            if (getParent()->type_container.isType(document->element_types.getTypeOfElement(tag)))
            {
                output.push_back(getParent());
                auto more = getParent()->getParentsByTagName(tag);
                if (more.size() > 0)
                {
                    output.insert(output.end(), more.begin(), more.end());
                }
            }
        }
    }

    return output;
}

bool Element::contains(std::shared_ptr<Element> element)
{
    for (size_t i = 0; i < children.size(); i++)
    {
        if (children[i] == element)
        {
            return true;
        }
        
        if (children[i]->contains(element))
        {
            return true;
        }
    }

    return false;
}

// =======================================================
// Attributes

void Element::setAttribute(std::string attribute, AttrVariant value)
{
    attributes[attribute] = value;
}

AttrVariant Element::getAttribute(std::string attribute)
{
    return attributes.at(attribute);
}

bool Element::hasAttribute(std::string attribute)
{
    try
    {
        attributes.at(attribute);
        return true;
    }
    catch(const std::exception& e)
    {
        return false;
    }
    
}

// =======================================================
// ClassList

ClassList::ClassList()
: classes()
{

}

ClassList::~ClassList()
{

}

void ClassList::add(std::string element_class)
{
    if (!has(element_class))
    {
        classes.push_back(element_class);
    }
}

void ClassList::remove(std::string element_class)
{
    if (!has(element_class))
    {
        LOG_ERROR("Cannot remove class " + element_class + ": Element does not contain this class");
        return;
    }

    for (size_t i = 0; i < classes.size(); i++)
    {
        if (classes[i] == element_class)
        {
            classes.erase(classes.begin() + i);
            break;
        }
    }
}

bool ClassList::has(std::string element_class)
{
    for (size_t i = 0; i < classes.size(); i++)
    {
        if (classes[i] == element_class)
        {
            return true;
        }
    }

    return false;
}


// ==============================================================
// Saving
void Element::saveToFile(std::string filename)
{
    auto doc = new tinyxml2::XMLDocument();

    doc->InsertEndChild(elementToXMLElement(shared_from_this(), doc));

    tinyxml2::XMLPrinter printer;
    doc->Print(&printer);

    std::string output = printer.CStr();

    delete doc;

    // Now save the file
    auto res = std::make_shared<Res::TextResource>();
    res->setText(output);
    Res::ResourceManager::save(filename, res);
}

// I accidentally coded this really weirdly
tinyxml2::XMLElement* Element::elementToXMLElement(std::shared_ptr<Element> ele, tinyxml2::XMLDocument* doc)
{
    ele->onSave();
    auto new_ele = doc->NewElement(ele->getTagName().c_str());

    for (auto i = ele->attributes.begin(); i != ele->attributes.end(); i++)
    {
        if (std::get_if<int>(&i->second))
        {
            new_ele->SetAttribute(i->first.c_str(), std::get<int>(i->second));
        }
        else if (std::get_if<float>(&i->second))
        {
            new_ele->SetAttribute(i->first.c_str(), std::get<float>(i->second));
        }
        // else if (std::get_if<bool>(&i->second))
        // {
        //     new_ele->SetAttribute(i->first.c_str(), std::get<bool>(i->second));
        // }
        else if (std::get_if<std::string>(&i->second))
        {
            new_ele->SetAttribute(i->first.c_str(), std::get<std::string>(i->second).c_str());
        }
    }

    if (ele->getId() != "")
    {
        new_ele->SetAttribute("id", ele->getId().c_str());
    }

    std::string classes = "";
    for (int i = 0; i < ele->classList.classes.size(); i++) 
    {
        classes += ele->classList.classes[i];
        if (i != ele->classList.classes.size()-1)
        {
            classes += " ";
        }
    }

    for (int i = 0; i < ele->children.size(); i ++) 
    {
        new_ele->InsertEndChild(elementToXMLElement(ele->children[i], doc));
    }

    return new_ele;
}


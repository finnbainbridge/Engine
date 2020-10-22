#include "Engine/DevTools.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Log.hpp"
#include "Engine/NKAPI.hpp"
#include <memory>

using namespace Engine::DevTools;

/*
This function is not normally part of nuklear - it comes from https://github.com/vurtun/nuklear/issues/828
*/
NK_API int nk_tab (struct nk_context *ctx, const char *title, int active, float widget_width)
{
	const struct nk_user_font *f = ctx->style.font;
	float text_width = f->width(f->userdata, f->height, title, nk_strlen(title));
	// float widget_width = text_width + 3 * ctx->style.button.padding.x;
	nk_layout_row_push(ctx, widget_width);
	struct nk_style_item c = ctx->style.button.normal;
	if (active) {ctx->style.button.normal = ctx->style.button.active;}
	int r = nk_button_label (ctx, title);
	ctx->style.button.normal = c;
	return r;
}


DevTools::DevTools(std::shared_ptr<Document> document): DOM::Element(document),
current_tab(0)
{
    setTagName("devtools");
}

void DevTools::init()
{
    camera = std::make_shared<OrbitCamera3D>(document);
    appendChild(camera);
    camera->setProcess(false);
}

void DevTools::render(float delta)
{

    cam_switch_lock.lock();
    if (camera_switch == 1)
    {
        // Switch to orbitcam
        old_camera = document->renderer->getCamera();
        document->renderer->setCamera(camera);
        camera->setProcess(true);
        camera_switch = 0;
    }
    else if (camera_switch == 2)
    {
        // Switch back to the old camera
        document->renderer->setCamera(old_camera);
        camera->setProcess(false);
        camera_switch = 0;
        setVisible(false);
    }
    cam_switch_lock.unlock();


    if (nk_begin(NKAPI::ctx, "DevTools", nk_rect(10, (document->renderer->getHeight()/3*2)-10, document->renderer->getWidth()-20, document->renderer->getHeight()/3), 
                NK_WINDOW_BORDER | NK_WINDOW_BACKGROUND))
    {
        nk_style_push_vec2(NKAPI::ctx, &NKAPI::ctx->style.window.spacing, nk_vec2(0,0));
        nk_style_push_float(NKAPI::ctx, &NKAPI::ctx->style.button.rounding, 0);

        auto tabs = getElementsByTagName("devtoolstab", true);

        nk_layout_row_begin(NKAPI::ctx, NK_STATIC, 30, tabs.size());

        std::shared_ptr<Engine::DevTools::DevToolsTab> active_tab = nullptr;

        for (int i = 0; i < tabs.size(); i++)
        {
            auto tab = std::dynamic_pointer_cast<DevToolsTab>(tabs[i]);
            if (nk_tab (NKAPI::ctx, tab->getTabName().c_str(), current_tab == i, (document->renderer->getWidth()-45)/tabs.size())) 
            {
                current_tab = i;
            }

            if (current_tab == i)
            {
                active_tab = tab;
            }
        }
        
        nk_style_pop_float(NKAPI::ctx);
        nk_style_pop_vec2(NKAPI::ctx);

        nk_end(NKAPI::ctx);

        // A stupid hack to get tabs and scroll bars working
        if (active_tab != nullptr)
        {
            if (nk_begin(NKAPI::ctx, active_tab->getTabName().c_str(), nk_rect(30, (document->renderer->getHeight()/3*2)+(40 - 10),
                 document->renderer->getWidth()-60, document->renderer->getHeight()/3 -40), 
                0))
            {
                active_tab->renderUI(delta);
                nk_end(NKAPI::ctx);
            }
        }
    }
}

void DevTools::process(float delta)
{
    if (document->renderer->isKeyPressed(ENGINE_KEY_BACKSLASH) && document->renderer->getTime() > cooldown_time)
    {
        // setVisible(!getVisible());
        if (getVisible() == false)
        {
            setVisible(true);

            // TODO: Move this to "administrative" DevTools tab
            document->body->setProcess(false);
            document->head->setProcess(false);

            cam_switch_lock.lock();
            camera_switch = 1;
            cam_switch_lock.unlock();

            is_active = true;
        }
        else
        {

            // TODO: Move this to "administrative" DevTools tab
            document->body->setProcess(true);
            document->head->setProcess(true);

            cam_switch_lock.lock();
            camera_switch = 2;
            cam_switch_lock.unlock();

            is_active = false;
        }
        cooldown_time = document->renderer->getTime() + 0.2;
    }
}

void DevTools::targetCamera(std::shared_ptr<Engine::E3D::Element3D> element)
{
    camera->setPosition(element->getGlobalTransform() * element->getTransform() * glm::vec4(0,0,0,1));
}

// =====================================================
// Dev Tools Tab

DevToolsTab::DevToolsTab(std::shared_ptr<Document> doc): DOM::Element(doc)
{
    setTagName("devtoolstab");
}

void DevToolsTab::setTabName(std::string name)
{
    tab_name = name;
}

std::string DevToolsTab::getTabName()
{
    return tab_name;
}

void DevToolsTab::render(float delta)
{
    // Do absolutely nothing
}

void DevToolsTab::renderUI(float delta)
{
    // Actually render the UI here
}

// =================================================
// DevToolsTree
// Element tree browser
DevToolsTree::DevToolsTree(std::shared_ptr<Document> doc): DevToolsTab(doc)
{
    setTagName("devtoolstree");
    setTabName("Element Tree");
}

void DevToolsTree::renderUI(float delta)
{
    // Get root node
    auto root = document->body->getParent();
    recursiveTreeRender(root, 0);
}

int DevToolsTree::recursiveTreeRender(std::shared_ptr<DOM::Element> element, int id)
{
    std::string name = element->getTagName();
    if (element->getId() != "")
    {
        name += "#" + element->getId();
    }
    bool worked = nk_tree_push_id(NKAPI::ctx, NK_TREE_NODE, name.c_str(), NK_MINIMIZED, id);
    if (worked)
    {
        auto buttons = element->getDevToolsButtons();
        if (buttons.size() > 0)
        {
            nk_layout_row_dynamic(NKAPI::ctx, 20, buttons.size());
            for (std::map<std::string, std::function<void()>>::iterator it = buttons.begin(); it != buttons.end(); it++)
            {
                if (nk_button_label(NKAPI::ctx, it->first.c_str()))
                {
                    it->second();
                }
            }
        }
    }
    id ++;
    for (int i = 0; i < element->getChildren().size(); i++)
    {
        if (worked)
        {
            
            id = recursiveTreeRender(element->getChildren()[i], id);
        }
        else
        {
            id ++;
        }
    }

    if (worked)
    {
        nk_tree_pop(NKAPI::ctx);
    }

    return id;
}

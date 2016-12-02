#include "UIScrollBarLinkSystem.h"

#include "UI/Scroll/UIScrollBarDelegateComponent.h"
#include "UI/UIControl.h"
#include "UI/UIControlHelpers.h"
#include "UI/UIScrollBar.h"

namespace DAVA
{
UIScrollBarLinkSystem::UIScrollBarLinkSystem()
{
}

UIScrollBarLinkSystem::~UIScrollBarLinkSystem()
{
}

void UIScrollBarLinkSystem::RegisterControl(UIControl* control)
{
    UIScrollBarDelegateComponent* component = control->GetComponent<UIScrollBarDelegateComponent>();
    if (component)
    {
        LinkScrollBar(component);
    }
    else
    {
        LinkDelegate(control);
    }
}

void UIScrollBarLinkSystem::UnregisterControl(UIControl* control)
{
    UIScrollBarDelegateComponent* component = control->GetComponent<UIScrollBarDelegateComponent>();
    if (component)
    {
        UnlinkScrollBar(component);
    }
    else
    {
        UnlinkDelegate(control);
    }
}

void UIScrollBarLinkSystem::RegisterComponent(UIControl* control, UIComponent* component)
{
    if (component->GetType() == UIScrollBarDelegateComponent::C_TYPE)
    {
        LinkScrollBar(static_cast<UIScrollBarDelegateComponent*>(component));
    }
}

void UIScrollBarLinkSystem::UnregisterComponent(UIControl* control, UIComponent* component)
{
    if (component->GetType() == UIScrollBarDelegateComponent::C_TYPE)
    {
        UnlinkScrollBar(static_cast<UIScrollBarDelegateComponent*>(component));
    }
}

void UIScrollBarLinkSystem::Process(DAVA::float32 elapsedTime)
{
    for (Link& link : links)
    {
        if (((link.type & Link::SCROLL_BAR_LINKED) != 0) && (link.linkedControl == nullptr || link.component->IsPathToDelegateDirty()))
        {
            UIControl* control = link.component->GetControl()->FindByPath(link.component->GetPathToDelegate());
            if (control != nullptr)
            {
                SetupLink(&link, control);
            }
            else
            {
                BreakLink(&link);
                link.type &= ~Link::DELEGATE_LINKED;
                link.linkedControl = nullptr;
            }
            link.component->ResetPathToDelegateDirty();
        }
        else if (restoreLinks && link.type == (Link::SCROLL_BAR_LINKED | Link::DELEGATE_LINKED))
        {
            link.component->SetPathToDelegate(UIControlHelpers::GetPathToOtherControl(link.component->GetControl(), link.linkedControl));
        }
    }
}

bool UIScrollBarLinkSystem::IsRestoreLinks() const
{
    return restoreLinks;
}

void UIScrollBarLinkSystem::SetRestoreLinks(bool restore)
{
    restoreLinks = restore;
}

void UIScrollBarLinkSystem::SetupLink(Link* link, UIControl* control)
{
    UIScrollBarDelegate* delegate = dynamic_cast<UIScrollBarDelegate*>(control);
    DVASSERT(delegate != nullptr);

    UIScrollBar* scrollBar = dynamic_cast<UIScrollBar*>(link->component->GetControl());
    DVASSERT(scrollBar != nullptr);

    if (scrollBar && delegate)
    {
        link->linkedControl = control;
        link->type = Link::DELEGATE_LINKED | Link::SCROLL_BAR_LINKED;
        scrollBar->SetDelegate(delegate);
    }
}

void UIScrollBarLinkSystem::BreakLink(Link* link)
{
    UIScrollBar* scrollBar = dynamic_cast<UIScrollBar*>(link->component->GetControl());
    if (scrollBar)
    {
        scrollBar->SetDelegate(nullptr);
    }
}

template <typename Predicate>
bool UIScrollBarLinkSystem::RestoreLink(int32 linkType, Predicate predicate)
{
    if (restoreLinks)
    {
        auto it = std::find_if(links.begin(), links.end(), predicate);
        if (it != links.end())
        {
            it->type |= linkType;
            it->component->SetPathToDelegate(UIControlHelpers::GetPathToOtherControl(it->component->GetControl(), it->linkedControl));
            SetupLink(&(*it), it->linkedControl);

            return true;
        }
    }
    return false;
}

void UIScrollBarLinkSystem::LinkScrollBar(UIScrollBarDelegateComponent* component)
{
    if (!RestoreLink(Link::SCROLL_BAR_LINKED, [component](Link& l) { return l.component == component; }))
    {
        links.push_back(Link(component));
    }
}

void UIScrollBarLinkSystem::LinkDelegate(UIControl* linkedControl)
{
    RestoreLink(Link::DELEGATE_LINKED, [linkedControl](Link& l) { return l.linkedControl == linkedControl; });
}

template <typename Predicate>
bool UIScrollBarLinkSystem::Unlink(int32 linkType, Predicate predicate)
{
    auto it = std::find_if(links.begin(), links.end(), predicate);
    if (it != links.end())
    {
        it->type &= ~linkType;
        BreakLink(&(*it));

        if (!restoreLinks || it->type == 0)
        {
            links.erase(it);
        }
        return true;
    }
    return false;
}

bool UIScrollBarLinkSystem::UnlinkScrollBar(UIScrollBarDelegateComponent* component)
{
    return Unlink(Link::SCROLL_BAR_LINKED, [component](Link& l) { return l.component == component; });
}

bool UIScrollBarLinkSystem::UnlinkDelegate(UIControl* linkedControl)
{
    return Unlink(Link::DELEGATE_LINKED, [linkedControl](Link& l) { return l.linkedControl == linkedControl; });
}
}

#include "Infrastructure/TestListScreen.h"
#include <Utils/UTF8Utils.h>

using namespace DAVA;

TestListScreen::TestListScreen()
    : UIScreen()
    , testsGrid(nullptr)
    , cellHeight(50)
{
}

TestListScreen::~TestListScreen()
{
    for (auto screen : testScreens)
    {
        SafeRelease(screen);
    }
    testScreens.clear();
}

void TestListScreen::AddTestScreen(BaseScreen* screen)
{
    screen->Retain();
    testScreens.push_back(screen);
}

void TestListScreen::LoadResources()
{
    UIScreen::LoadResources();

    Size2i screenSize = VirtualCoordinatesSystem::Instance()->GetVirtualScreenSize();

    testsGrid = new UIList(Rect(0.0, 0.0, static_cast<DAVA::float32>(screenSize.dx), static_cast<DAVA::float32>(screenSize.dy)), UIList::ORIENTATION_VERTICAL);
    testsGrid->SetDelegate(this);
    AddControl(testsGrid);
}

void TestListScreen::UnloadResources()
{
    UIScreen::UnloadResources();
    RemoveAllControls();

    SafeRelease(testsGrid);
}

int32 TestListScreen::ElementsCount(UIList* list)
{
    return static_cast<int32>(testScreens.size());
}

float32 TestListScreen::CellHeight(UIList* list, int32 index)
{
    return cellHeight;
}

UIListCell* TestListScreen::CellAtIndex(UIList* list, int32 index)
{
    const char8 buttonName[] = "CellButton";
    const char8 cellName[] = "TestButtonCell";
    UIStaticText* buttonText = nullptr;
    UIListCell* c = list->GetReusableCell(cellName); //try to get cell from the reusable cells store
    if (!c)
    { //if cell of requested type isn't find in the store create new cell
        c = new UIListCell(Rect(0., 0., static_cast<float32>(list->size.x), CellHeight(list, index)), cellName);

        buttonText = new UIStaticText(Rect(0., 0., static_cast<float32>(list->size.x), CellHeight(list, index)));
        buttonText->SetName(buttonName);
        c->AddControl(buttonText);

        Font* font = FTFont::Create("~res:/Fonts/korinna.ttf");
        DVASSERT(font);

        font->SetSize(static_cast<float32>(20));
        buttonText->SetFont(font);
        buttonText->SetDebugDraw(true);

        SafeRelease(font);
        c->GetBackground()->SetColor(Color(0.75, 0.75, 0.75, 0.5));
    }

    auto screen = testScreens.at(index);

    buttonText = (UIStaticText*)c->FindByName(buttonName);
    if (nullptr != buttonText)
    {
        buttonText->SetText(UTF8Utils::EncodeToWideString(screen->GetName().c_str()));
    }

    return c; //returns cell
}

void TestListScreen::OnCellSelected(UIList* forList, UIListCell* selectedCell)
{
    UIScreenManager::Instance()->SetScreen(selectedCell->GetIndex() + 1);
}

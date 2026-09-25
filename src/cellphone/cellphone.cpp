#include "cellphone.h"

#include "../config.h"
#include "../container/container.h"
#include "../container/containerLoader.h"
#include "aml-psdk/gta_base/Vector.h"

#include "../utils/utils.h"
#include "cellphone.h"

#include "../config.h"
#include "../container/container.h"
#include "../container/containerLoader.h"
#include "aml-psdk/gta_base/Vector.h"
#include "cellphone.h"

#include "../config.h"
#include "../container/container.h"
#include "../container/containerLoader.h"
#include "aml-psdk/gta_base/Vector.h"

#include "../utils/effects.h"

#include "../utils/drawUtils.h"
#include "mod/logger.h"
#include <string>

Cellphone *Cellphone::ScriptsCellphone = nullptr;

Cellphone *Cellphone::CreateScriptsCellphone()
{
    auto cellphone = new Cellphone();
    ScriptsCellphone = cellphone;
    // ScriptsCellphone->SetVisible(false);
    return ScriptsCellphone;
}

Cellphone::Cellphone()
{
    auto self = this;

    auto container = Container::MainContainer->AddChild("");
    LoadContainerFromFile(container, GetMenuLayoutPath("cellphone.json"));

    _container = container;

    container->canBlockTouchEvents = true;

    auto content = container->FindChild("contentPanel");
    auto closeBtn = container->FindChild("closeBtn");
    auto pageInfo = container->FindChild("pageInfo");
    auto appsInfo = container->FindChild("appsInfo");
    auto leftButton = container->FindChild("leftButton");
    auto rightButton = container->FindChild("rightButton");

    _contentPanel = content;
    _pageInfo = pageInfo;
    _appsInfo = appsInfo;
    _page = 0;

    // AddItem("Menu Veiculos", testImagePath, []() {});

    UpdatePage();

    leftButton->onClick->Add(
        [this]()
        {
            const int maxPage = (_items.size() - 1) / 5;

            _page--;

            if (_page < 0)
                _page = maxPage;

            UpdatePage();
        });

    rightButton->onClick->Add(
        [this]()
        {
            const int maxPage = (_items.size() - 1) / 5;

            _page++;

            if (_page > maxPage)
                _page = 0;

            UpdatePage();
        });

    closeBtn->onClick->Add([this]() { FadeOut(); });
}

Cellphone::~Cellphone() { Container::MainContainer->RemoveChild(_container, true); }

void Cellphone::UpdatePage()
{
    auto cellphone = this;

    logger->Info("Cellphone::UpdatePage - START");
    logger->Info("Page: %d, Items: %d", _page, static_cast<int>(_items.size()));

    if (!_contentPanel)
    {
        logger->Error("Cellphone::UpdatePage - _contentPanel is NULL");
        return;
    }

    logger->Info("Removing children...");
    _contentPanel->RemoveChildren();
    logger->Info("Children removed");

    const int start = _page * 5;
    const int end = std::min(start + 5, static_cast<int>(_items.size()));

    const int maxPage = (_items.size() - 1) / 5;

    _pageInfo->text = "Page " + std::to_string(_page + 1) + "/" + std::to_string(maxPage + 1);

    _appsInfo->text = std::to_string(_items.size()) + " scripts found";

    for (int i = start; i < end; i++)
    {
        const auto &data = _items[i];

        auto item = _contentPanel->AddChild("");

        auto layoutPath = GetMenuLayoutPath("cellphone_item.json");

        LoadContainerFromFile(item, layoutPath);

        item->onClick->Add(
            [this, dataFunction = data.function, cellphone]()
            {
                logger->Info("doing fadeout");

                FadeOut();

                logger->Info("calling dataFunction");

                if (dataFunction)
                    dataFunction();
            });

        item->UpdateTransformFromRoot();

        auto currentSize = item->GetCurrentSize();

        item->style.left = "0px";
        item->style.top = std::to_string((i - start) * currentSize.y) + "px";

        auto image = item->FindChild("img");
        auto text = item->FindChild("appName");

        if (image)
        {
            if (!data.image.empty())
                image->style.backgroundImage = data.image;
        }

        if (text)
        {
            text->text = data.text;
        }

        if (data.text.find("Crash game") != std::string::npos)
        {
            item->Dump();

            // SetTimeout([item]() { item->Dump(); }, 3000);
        }
    }

    logger->Info("Cellphone::UpdatePage - END");
}

void Cellphone::AddItem(std::string text, std::string image, std::function<void()> function)
{
    if (image.empty())
    {
        image = GetMenuAssetPath("icons/script.png");
    }

    CellphoneItem item;
    item.text = text;
    item.image = image;
    item.function = function;

    _items.push_back(item);

    std::sort(_items.begin(), _items.end(),
        [](const CellphoneItem &a, const CellphoneItem &b)
        {
            auto getSortText = [](const std::string &text)
            {
                if (text.rfind("~", 0) == 0)
                {
                    size_t end = text.find("~", 1);

                    if (end != std::string::npos)
                        return text.substr(end + 1);
                }

                return text;
            };

            return getSortText(a.text) < getSortText(b.text);
        });

    UpdatePage();
}

void Cellphone::SetVisible(bool visible) { _container->visible = visible; }

void Cellphone::FadeIn()
{
    auto baseResolution = DrawUtils::GetBaseResolution();

    const CVector2D screenCenter = CVector2D(baseResolution.width / 2.0f, baseResolution.height / 2.0f);
    const CVector2D screenBottom = CVector2D(baseResolution.width / 2.0f, baseResolution.height);

    const CVector2D startPosition = screenBottom;
    const CVector2D endPosition = screenCenter;

    const CVector2D startScale(0, 0);
    const CVector2D endScale(1, 1);

    const float startOpacity = 0.0f;
    const float endOpacity = 1.0f;

    const int duration = 600;

    SetVisible(true);

    const auto onComplete = [this]() {};

    Ease_Simple(
        _container, startPosition, endPosition, startScale, endScale, startOpacity, endOpacity, duration, onComplete);
}

void Cellphone::FadeOut()
{
    logger->Info("FadeOut called");

    _container->style.opacity = 1;
    SetVisible(true);

    auto baseResolution = DrawUtils::GetBaseResolution();

    const CVector2D screenCenter = CVector2D(baseResolution.width / 2.0f, baseResolution.height / 2.0f);
    const CVector2D screenBottom = CVector2D(baseResolution.width / 2.0f, baseResolution.height);

    const CVector2D startPosition = _container->GetCenterPosition();
    const CVector2D endPosition = screenBottom;

    const CVector2D startScale(1, 1);
    const CVector2D endScale(0, 0);

    const float startOpacity = 1.0f;
    const float endOpacity = 0.0f;

    const int duration = 600;

    const auto onComplete = [this]() { SetVisible(false); };

    Ease_Simple(
        _container, startPosition, endPosition, startScale, endScale, startOpacity, endOpacity, duration, onComplete);
}
#include "infoMessage.h"
#include "src/container/container.h"

#include "../container/containerLoader.h"
#include "../utils/effects.h"

InfoMessage* g_bottomInfoMessage = nullptr;

InfoMessage::InfoMessage(Container* parent)
{
    auto infoMessage = parent->AddChild("infoMessage");
    LoadContainerFromFile(infoMessage, GetMenuLayoutPath("infoMessage.json"));

    _container = infoMessage;

    menuSZK->onMenuProcess->AddRef(this,
        [this](unsigned int deltaTime)
        {
            if (_timeLeft > 0)
            {
                _timeLeft -= deltaTime;
                if (_timeLeft <= 0)
                {
                    _timeLeft = 0;
                    FadeOut();
                }
            }
        });
}

InfoMessage::~InfoMessage()
{
    menuSZK->onMenuProcess->Remove(this);
}

void InfoMessage::FadeIn()
{
    _container->visible = true;
}

void InfoMessage::FadeOut()
{
    _container->visible = false;
}

void InfoMessage::Pop()
{
    Ease_Scale(_container, 1.2f, 300);
}

void InfoMessage::SetMessage(std::string message, int duration)
{
    _container->text = message;
    _timeLeft = duration;

    FadeIn();
    Pop();
}

InfoMessage* InfoMessage::GetBottom()
{
    if (g_bottomInfoMessage != nullptr) { return g_bottomInfoMessage; }

    g_bottomInfoMessage = new InfoMessage(Container::MainContainer);

    return g_bottomInfoMessage;
}
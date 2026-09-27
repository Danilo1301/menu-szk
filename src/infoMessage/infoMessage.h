#pragma once

#include "src/container/container.h"
#include <string>
class InfoMessage
{
  private:
    Container *_container;
    int _timeLeft = 0;

  public:
    InfoMessage(Container *parent);
    ~InfoMessage();

    void FadeIn();
    void FadeOut();
    void Pop();

    void SetMessage(std::string message, int duration);

    static InfoMessage *GetBottom();
};
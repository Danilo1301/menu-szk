#include "input_old.h"

#include "hooks.h"
#include "input.h"
#include "pch.h"

void Input_old::Update()
{
    static bool wasDown = false;

    bool isDown = *CTouchInterface_m_bTouchDown;
    CVector2D pos = *m_vecCachedPos;

    //LOGI("[Input_old] Update | wasDown=%d isDown=%d pos=(%.2f, %.2f)", wasDown, isDown, pos.x, pos.y);

    int state = 0;

    if (isDown)
    {
        if (!wasDown)
        {
            state = 2;

            //LOGI("[Input_old] TOUCH DOWN | pos=(%.2f, %.2f)", pos.x, pos.y);
        }
        else
        {
            state = 3;

            //LOGI("[Input_old] TOUCH MOVE | pos=(%.2f, %.2f)", pos.x, pos.y);
        }
    }
    else
    {
        if (wasDown)
        {
            state = 1;

            //LOGI("[Input_old] TOUCH UP | pos=(%.2f, %.2f)", pos.x, pos.y);
        }
        else
        {
            //LOGI("[Input_old] NO TOUCH");
        }
    }

    wasDown = isDown;

    if (state == 0) return;

    int touchId = 0;
    int x = static_cast<int>(pos.x);
    int y = static_cast<int>(pos.y);

    //LOGI("[Input_old] Sending OnTouchEvent | state=%d id=%d x=%d y=%d", state, touchId, x, y);

    Input::OnTouchEvent(state, touchId, x, y, g_timeInMilliseconds);

    //LOGI("[Input_old] OnTouchEvent finished");
}
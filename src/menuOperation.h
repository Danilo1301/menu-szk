#pragma once

enum MenuOperation : unsigned int
{
    TEST_OP = 75235,
    op_ModLoad,
    op_ResolveDependecies,
    op_ModPreload,
    op_WebServerThread,
    op_TimerUpdate,
    op_ProcessMenuEvents,
    op_ProcessScriptEvents,
    op_GameProcces,
    op_PreRenderEnd,
    op_TouchEvent,
    op_DrawRadarGangOverlay,
    op_Container_destroy,
    op_Container_addChild,
    op_Test,
    op_Container_onClick
};
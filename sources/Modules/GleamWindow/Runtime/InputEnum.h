#pragma once
#include <GLFW/glfw3.h>
#include <type_traits>

namespace Gleam
{
    constexpr int KeyCodeCount = 349;
    enum class KeyCode:uint16_t
    {
        Backspace = GLFW_KEY_BACKSPACE,
        Delete = GLFW_KEY_DELETE,
        Tab = GLFW_KEY_TAB,
        Return = GLFW_KEY_ENTER,
        Esc = GLFW_KEY_ESCAPE,
        Space = GLFW_KEY_SPACE,
        LeftShift = GLFW_KEY_LEFT_SHIFT,
        LeftCtrl = GLFW_KEY_LEFT_CONTROL,
        LeftAlt = GLFW_KEY_LEFT_ALT,
        F1 = GLFW_KEY_F1,
        F2 = GLFW_KEY_F2,
        F3 = GLFW_KEY_F3,
        F4 = GLFW_KEY_F4,
        F5 = GLFW_KEY_F5,
        F6 = GLFW_KEY_F6,
        F7 = GLFW_KEY_F7,
        F8 = GLFW_KEY_F8,
        F9 = GLFW_KEY_F9,
        F10 = GLFW_KEY_F10,
        F11 = GLFW_KEY_F11,
        F12 = GLFW_KEY_F12,
        Alpha0 = GLFW_KEY_0,
        Alpha1 = GLFW_KEY_1,
        Alpha2 = GLFW_KEY_2,
        Alpha3 = GLFW_KEY_3,
        Alpha4 = GLFW_KEY_4,
        Alpha5 = GLFW_KEY_5,
        Alpha6 = GLFW_KEY_6,
        Alpha7 = GLFW_KEY_7,
        Alpha8 = GLFW_KEY_8,
        Alpha9 = GLFW_KEY_9,
        A = GLFW_KEY_A,
        B = GLFW_KEY_B,
        C = GLFW_KEY_C,
        D = GLFW_KEY_D,
        E = GLFW_KEY_E,
        F = GLFW_KEY_F,
        G = GLFW_KEY_G,
        H = GLFW_KEY_H,
        I = GLFW_KEY_I,
        J = GLFW_KEY_J,
        K = GLFW_KEY_K,
        L = GLFW_KEY_L,
        M = GLFW_KEY_M,
        N = GLFW_KEY_N,
        O = GLFW_KEY_O,
        P = GLFW_KEY_P,
        Q = GLFW_KEY_Q,
        R = GLFW_KEY_R,
        S = GLFW_KEY_S,
        T = GLFW_KEY_T,
        U = GLFW_KEY_U,
        V = GLFW_KEY_V,
        W = GLFW_KEY_W,
        X = GLFW_KEY_X,
        Y = GLFW_KEY_Y,
        Z = GLFW_KEY_Z,
        BackQuote = GLFW_KEY_GRAVE_ACCENT, //反引号 `
        Minus = GLFW_KEY_MINUS, //减号 -
        Equals = GLFW_KEY_EQUAL, //等于号 =
        LeftBracket = GLFW_KEY_LEFT_BRACKET, //左方括号 [
        RightBracket = GLFW_KEY_RIGHT_BRACKET, //右方括号 ]
        Semicolon = GLFW_KEY_SEMICOLON, //分号 ;
        Quote = GLFW_KEY_APOSTROPHE, //引号 '
        Backslash = GLFW_KEY_BACKSLASH, /* 反斜杠 \ */
        Comma = GLFW_KEY_COMMA, //逗号 ,
        Period = GLFW_KEY_PERIOD, //句号 .
        Slash = GLFW_KEY_SLASH, //斜杠 /
    };

    constexpr int MouseButtonCount = 3;
    enum class MouseButton:uint8_t
    {
        Left = GLFW_MOUSE_BUTTON_LEFT,
        Right = GLFW_MOUSE_BUTTON_RIGHT,
        Middle = GLFW_MOUSE_BUTTON_MIDDLE,
    };
}
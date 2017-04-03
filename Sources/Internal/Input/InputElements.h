#pragma once

#include "Base/BaseTypes.h"
#include "Debug/DVAssert.h"

namespace DAVA
{
/**
    List of all supported input elements.
    An input element is a part of a device which can be used for input. For example, a keyboard button, a mouse button, a mouse wheel, gamepad's stick etc.
*/
enum eInputElements : uint32
{
    // Order of elements should not be changed,
    // since some functions rely on it (i.e. when converting from virtual to scancode keyboard key)

    NONE = 0,

    // Keyboard virtual keys

    KB_1_VIRTUAL,
    KB_2_VIRTUAL,
    KB_3_VIRTUAL,
    KB_4_VIRTUAL,
    KB_5_VIRTUAL,
    KB_6_VIRTUAL,
    KB_7_VIRTUAL,
    KB_8_VIRTUAL,
    KB_9_VIRTUAL,
    KB_0_VIRTUAL,
    KB_A_VIRTUAL,
    KB_B_VIRTUAL,
    KB_C_VIRTUAL,
    KB_D_VIRTUAL,
    KB_E_VIRTUAL,
    KB_F_VIRTUAL,
    KB_G_VIRTUAL,
    KB_H_VIRTUAL,
    KB_I_VIRTUAL,
    KB_J_VIRTUAL,
    KB_K_VIRTUAL,
    KB_L_VIRTUAL,
    KB_M_VIRTUAL,
    KB_N_VIRTUAL,
    KB_O_VIRTUAL,
    KB_P_VIRTUAL,
    KB_Q_VIRTUAL,
    KB_R_VIRTUAL,
    KB_S_VIRTUAL,
    KB_T_VIRTUAL,
    KB_U_VIRTUAL,
    KB_V_VIRTUAL,
    KB_W_VIRTUAL,
    KB_X_VIRTUAL,
    KB_Y_VIRTUAL,
    KB_Z_VIRTUAL,
    KB_F1_VIRTUAL,
    KB_F2_VIRTUAL,
    KB_F3_VIRTUAL,
    KB_F4_VIRTUAL,
    KB_F5_VIRTUAL,
    KB_F6_VIRTUAL,
    KB_F7_VIRTUAL,
    KB_F8_VIRTUAL,
    KB_F9_VIRTUAL,
    KB_F10_VIRTUAL,
    KB_F11_VIRTUAL,
    KB_F12_VIRTUAL,
    KB_NONUSBACKSLASH_VIRTUAL,
    KB_COMMA_VIRTUAL,
    KB_PERIOD_VIRTUAL,
    KB_SLASH_VIRTUAL,
    KB_BACKSLASH_VIRTUAL,
    KB_APOSTROPHE_VIRTUAL,
    KB_SEMICOLON_VIRTUAL,
    KB_RBRACKET_VIRTUAL,
    KB_LBRACKET_VIRTUAL,
    KB_BACKSPACE_VIRTUAL,
    KB_EQUALS_VIRTUAL,
    KB_MINUS_VIRTUAL,
    KB_ESCAPE_VIRTUAL,
    KB_GRAVE_VIRTUAL,
    KB_TAB_VIRTUAL,
    KB_CAPSLOCK_VIRTUAL,
    KB_LSHIFT_VIRTUAL,
    KB_LCTRL_VIRTUAL,
    KB_LWIN_VIRTUAL,
    KB_LALT_VIRTUAL,
    KB_SPACE_VIRTUAL,
    KB_RALT_VIRTUAL,
    KB_RWIN_VIRTUAL,
    KB_MENU_VIRTUAL,
    KB_RCTRL_VIRTUAL,
    KB_RSHIFT_VIRTUAL,
    KB_ENTER_VIRTUAL,
    KB_PRINTSCREEN_VIRTUAL,
    KB_SCROLLLOCK_VIRTUAL,
    KB_PAUSE_VIRTUAL,
    KB_INSERT_VIRTUAL,
    KB_HOME_VIRTUAL,
    KB_PAGEUP_VIRTUAL,
    KB_DELETE_VIRTUAL,
    KB_END_VIRTUAL,
    KB_PAGEDOWN_VIRTUAL,
    KB_UP_VIRTUAL,
    KB_LEFT_VIRTUAL,
    KB_DOWN_VIRTUAL,
    KB_RIGHT_VIRTUAL,
    KB_NUMLOCK_VIRTUAL,
    KB_DIVIDE_VIRTUAL,
    KB_MULTIPLY_VIRTUAL,
    KB_NUMPAD_MINUS_VIRTUAL,
    KB_NUMPAD_PLUS_VIRTUAL,
    KB_NUMPAD_ENTER_VIRTUAL,
    KB_NUMPAD_DELETE_VIRTUAL,
    KB_NUMPAD_1_VIRTUAL,
    KB_NUMPAD_2_VIRTUAL,
    KB_NUMPAD_3_VIRTUAL,
    KB_NUMPAD_4_VIRTUAL,
    KB_NUMPAD_5_VIRTUAL,
    KB_NUMPAD_6_VIRTUAL,
    KB_NUMPAD_7_VIRTUAL,
    KB_NUMPAD_8_VIRTUAL,
    KB_NUMPAD_9_VIRTUAL,
    KB_NUMPAD_0_VIRTUAL,
    KB_LCMD_VIRTUAL,
    KB_RCMD_VIRTUAL,
    KB_BACK_VIRTUAL, // Android only

    // Keyboard scancode keys

    KB_1,
    KB_2,
    KB_3,
    KB_4,
    KB_5,
    KB_6,
    KB_7,
    KB_8,
    KB_9,
    KB_0,
    KB_A,
    KB_B,
    KB_C,
    KB_D,
    KB_E,
    KB_F,
    KB_G,
    KB_H,
    KB_I,
    KB_J,
    KB_K,
    KB_L,
    KB_M,
    KB_N,
    KB_O,
    KB_P,
    KB_Q,
    KB_R,
    KB_S,
    KB_T,
    KB_U,
    KB_V,
    KB_W,
    KB_X,
    KB_Y,
    KB_Z,
    KB_F1,
    KB_F2,
    KB_F3,
    KB_F4,
    KB_F5,
    KB_F6,
    KB_F7,
    KB_F8,
    KB_F9,
    KB_F10,
    KB_F11,
    KB_F12,
    KB_NONUSBACKSLASH,
    KB_COMMA,
    KB_PERIOD,
    KB_SLASH,
    KB_BACKSLASH,
    KB_APOSTROPHE,
    KB_SEMICOLON,
    KB_RBRACKET,
    KB_LBRACKET,
    KB_BACKSPACE,
    KB_EQUALS,
    KB_MINUS,
    KB_ESCAPE,
    KB_GRAVE,
    KB_TAB,
    KB_CAPSLOCK,
    KB_LSHIFT,
    KB_LCTRL,
    KB_LWIN,
    KB_LALT,
    KB_SPACE,
    KB_RALT,
    KB_RWIN,
    KB_MENU,
    KB_RCTRL,
    KB_RSHIFT,
    KB_ENTER,
    KB_PRINTSCREEN,
    KB_SCROLLLOCK,
    KB_PAUSE,
    KB_INSERT,
    KB_HOME,
    KB_PAGEUP,
    KB_DELETE,
    KB_END,
    KB_PAGEDOWN,
    KB_UP,
    KB_LEFT,
    KB_DOWN,
    KB_RIGHT,
    KB_NUMLOCK,
    KB_DIVIDE,
    KB_MULTIPLY,
    KB_NUMPAD_MINUS,
    KB_NUMPAD_PLUS,
    KB_NUMPAD_ENTER,
    KB_NUMPAD_DELETE,
    KB_NUMPAD_1,
    KB_NUMPAD_2,
    KB_NUMPAD_3,
    KB_NUMPAD_4,
    KB_NUMPAD_5,
    KB_NUMPAD_6,
    KB_NUMPAD_7,
    KB_NUMPAD_8,
    KB_NUMPAD_9,
    KB_NUMPAD_0,
    KB_LCMD,
    KB_RCMD,
    KB_BACK, // Android only

    // Mouse

    MOUSE_LBUTTON,
    MOUSE_RBUTTON,
    MOUSE_MBUTTON,
    MOUSE_EXT1BUTTON,
    MOUSE_EXT2BUTTON,
    MOUSE_WHEEL,
    MOUSE_POSITION,

    // Counters

    FIRST = NONE,
    LAST = MOUSE_POSITION,

    MOUSE_FIRST = MOUSE_LBUTTON,
    MOUSE_LAST = MOUSE_POSITION,
    MOUSE_FIRST_BUTTON = MOUSE_LBUTTON,
    MOUSE_LAST_BUTTON = MOUSE_EXT2BUTTON,

    KB_FIRST = KB_1_VIRTUAL,
    KB_LAST = KB_BACK,

    KB_FIRST_SCANCODE = KB_1,
    KB_LAST_SCANCODE = KB_BACK,

    KB_FIRST_VIRTUAL = KB_1_VIRTUAL,
    KB_LAST_VIRTUAL = KB_BACK_VIRTUAL
};

enum
{
    INPUT_ELEMENTS_COUNT = eInputElements::LAST - eInputElements::FIRST + 1,
    INPUT_ELEMENTS_KB_COUNT = eInputElements::KB_LAST - eInputElements::KB_FIRST + 1,
    INPUT_ELEMENTS_KB_COUNT_SCANCODE = eInputElements::KB_LAST_SCANCODE - eInputElements::KB_FIRST_SCANCODE + 1,
    INPUT_ELEMENTS_KB_COUNT_VIRTUAL = eInputElements::KB_LAST_VIRTUAL - eInputElements::KB_FIRST_VIRTUAL + 1,
};

// Each virtual key should have a scancode counterpart
static_assert(INPUT_ELEMENTS_KB_COUNT_VIRTUAL == INPUT_ELEMENTS_KB_COUNT_SCANCODE, "Amount of virtual keyboard keys does not match amount of scancode keys");

/** List of element types */
enum eInputElementTypes
{
    /** Basically, a button, which can just be pressed and released. */
    DIGITAL,

    /**
		Element whose state can only be described using multiple float values.
		For example, gamepad's stick position can be described using normalized x and y values.
	*/
    ANALOG
};

/** Contains additional information about an element. */
struct InputElementInfo final
{
    String name;
    eInputElementTypes type;
};

/** Return true if specified `element` is a mouse element */
inline bool IsMouseInputElement(eInputElements element)
{
    return eInputElements::MOUSE_FIRST <= element && element <= eInputElements::MOUSE_LAST;
}

/** Return true if specified `element` is a keyboard element */
inline bool IsKeyboardInputElement(eInputElements element)
{
    return eInputElements::KB_FIRST <= element && element <= eInputElements::KB_LAST;
}

/** Return true if specified `element` is a virtual keyboard element */
inline bool IsKeyboardVirtualInputElement(eInputElements element)
{
    return eInputElements::KB_FIRST_VIRTUAL <= element && element <= eInputElements::KB_LAST_VIRTUAL;
}

/** Return true if specified `element` is a scancode keyboard element */
inline bool IsKeyboardScancodeInputElement(eInputElements element)
{
    return eInputElements::KB_FIRST_SCANCODE <= element && element <= eInputElements::KB_LAST_SCANCODE;
}

/** Return true if specified keyboard `element` is a keyboard modifier element */
inline bool IsKeyboardModifierInputElement(eInputElements element)
{
    // Check both virtual and scancodes since these key are not remappable
    return (element == eInputElements::KB_LSHIFT_VIRTUAL || element == eInputElements::KB_LSHIFT ||
            element == eInputElements::KB_LCTRL_VIRTUAL || element == eInputElements::KB_LCTRL ||
            element == eInputElements::KB_LALT_VIRTUAL || element == eInputElements::KB_LALT ||
            element == eInputElements::KB_RSHIFT_VIRTUAL || element == eInputElements::KB_RSHIFT ||
            element == eInputElements::KB_RCTRL_VIRTUAL || element == eInputElements::KB_RCTRL ||
            element == eInputElements::KB_RALT_VIRTUAL || element == eInputElements::KB_RALT);
}

/** Return true if specified keyboard `element` is a keyboard system element */
inline bool IsKeyboardSystemInputElement(eInputElements element)
{
    // Check both virtual and scancodes since these key are not remappable
    return (element == eInputElements::KB_ESCAPE_VIRTUAL || element == eInputElements::KB_ESCAPE ||
            element == eInputElements::KB_CAPSLOCK_VIRTUAL || element == eInputElements::KB_CAPSLOCK ||
            element == eInputElements::KB_LWIN_VIRTUAL || element == eInputElements::KB_LWIN ||
            element == eInputElements::KB_RWIN_VIRTUAL || element == eInputElements::KB_RWIN ||
            element == eInputElements::KB_LCMD_VIRTUAL || element == eInputElements::KB_LCMD ||
            element == eInputElements::KB_RCMD_VIRTUAL || element == eInputElements::KB_RCMD ||
            element == eInputElements::KB_PRINTSCREEN_VIRTUAL || element == eInputElements::KB_PRINTSCREEN ||
            element == eInputElements::KB_SCROLLLOCK_VIRTUAL || element == eInputElements::KB_SCROLLLOCK ||
            element == eInputElements::KB_PAUSE_VIRTUAL || element == eInputElements::KB_PAUSE ||
            element == eInputElements::KB_INSERT_VIRTUAL || element == eInputElements::KB_INSERT ||
            element == eInputElements::KB_HOME_VIRTUAL || element == eInputElements::KB_HOME ||
            element == eInputElements::KB_PAGEUP_VIRTUAL || element == eInputElements::KB_PAGEUP ||
            element == eInputElements::KB_PAGEDOWN_VIRTUAL || element == eInputElements::KB_PAGEDOWN ||
            element == eInputElements::KB_DELETE_VIRTUAL || element == eInputElements::KB_DELETE ||
            element == eInputElements::KB_END_VIRTUAL || element == eInputElements::KB_END ||
            element == eInputElements::KB_PAGEDOWN_VIRTUAL || element == eInputElements::KB_PAGEDOWN ||
            element == eInputElements::KB_NUMLOCK_VIRTUAL || element == eInputElements::KB_NUMLOCK ||
            element == eInputElements::KB_MENU_VIRTUAL || element == eInputElements::KB_MENU);
}

/** Get additional information about an element */
const InputElementInfo& GetInputElementInfo(eInputElements element);
} // namespace DAVA

#include "wManager/window_manager.h"
#include "wManager/key_defines.h"
#include "wManager/x11_defines.h"

#ifdef _X11_

#include <dlfcn.h>

extern wManagerWindow _wMWindow;

void* _wManagerPlatformLoadModule(const char* path)
{
    return dlopen(path, RTLD_LAZY | RTLD_LOCAL);
}

void _wManagerPlatformFreeModule(void* module)
{
    dlclose(module);
}

void * _wManagerPlatformGetModuleSymbol(void* module, const char* name)
{
    return dlsym(module, name);
}

void inputMethodInstantiateCallback(Display* display,
                                           XPointer clientData,
                                           XPointer callData)
{

   /* wManagerX11 *wX11 = _wMWindow.WindowData;

    if (wX11->im)
        return;

    wX11->im = XOpenIM(wX11->display, 0, NULL, NULL);
    if (wX11->im)
    {
        if (!hasUsableInputMethodStyle())
        {
            XCloseIM(wX11->im);
            wX11->im = NULL;
        }
    }

    if (wX11->im)
    {
        XIMCallback callback;
        callback.callback = (XIMProc) inputMethodDestroyCallback;
        callback.client_data = NULL;
        XSetIMValues(wX11->im, XNDestroyCallback, &callback, NULL);

        for (wManagerWindow* window = _glfw.windowListHead;  window;  window = window->next)
            _glfwCreateInputContextX11(window);
    }*/
}

int translateKeySyms(const KeySym* keysyms, int width)
{
    if (width > 1)
    {
        switch (keysyms[1])
        {
            case XK_KP_0:           return ENGINE_KEY_KP_0;
            case XK_KP_1:           return ENGINE_KEY_KP_1;
            case XK_KP_2:           return ENGINE_KEY_KP_2;
            case XK_KP_3:           return ENGINE_KEY_KP_3;
            case XK_KP_4:           return ENGINE_KEY_KP_4;
            case XK_KP_5:           return ENGINE_KEY_KP_5;
            case XK_KP_6:           return ENGINE_KEY_KP_6;
            case XK_KP_7:           return ENGINE_KEY_KP_7;
            case XK_KP_8:           return ENGINE_KEY_KP_8;
            case XK_KP_9:           return ENGINE_KEY_KP_9;
            case XK_KP_Separator:
            case XK_KP_Decimal:     return ENGINE_KEY_KP_DECIMAL;
            case XK_KP_Equal:       return ENGINE_KEY_KP_EQUAL;
            case XK_KP_Enter:       return ENGINE_KEY_KP_ENTER;
            default:                break;
        }
    }

    switch (keysyms[0])
    {
        case XK_Escape:         return ENGINE_KEY_ESCAPE;
        case XK_Tab:            return ENGINE_KEY_TAB;
        case XK_Shift_L:        return ENGINE_KEY_LEFT_SHIFT;
        case XK_Shift_R:        return ENGINE_KEY_RIGHT_SHIFT;
        case XK_Control_L:      return ENGINE_KEY_LEFT_CONTROL;
        case XK_Control_R:      return ENGINE_KEY_RIGHT_CONTROL;
        case XK_Meta_L:
        case XK_Alt_L:          return ENGINE_KEY_LEFT_ALT;
        case XK_Mode_switch: // Mapped to Alt_R on many keyboards
        case XK_ISO_Level3_Shift: // AltGr on at least some machines
        case XK_Meta_R:
        case XK_Alt_R:          return ENGINE_KEY_RIGHT_ALT;
        case XK_Super_L:        return ENGINE_KEY_LEFT_SUPER;
        case XK_Super_R:        return ENGINE_KEY_RIGHT_SUPER;
        case XK_Menu:           return ENGINE_KEY_MENU;
        case XK_Num_Lock:       return ENGINE_KEY_NUM_LOCK;
        case XK_Caps_Lock:      return ENGINE_KEY_CAPS_LOCK;
        case XK_Print:          return ENGINE_KEY_PRINT_SCREEN;
        case XK_Scroll_Lock:    return ENGINE_KEY_SCROLL_LOCK;
        case XK_Pause:          return ENGINE_KEY_PAUSE;
        case XK_Delete:         return ENGINE_KEY_DELETE;
        case XK_BackSpace:      return ENGINE_KEY_BACKSPACE;
        case XK_Return:         return ENGINE_KEY_ENTER;
        case XK_Home:           return ENGINE_KEY_HOME;
        case XK_End:            return ENGINE_KEY_END;
        case XK_Page_Up:        return ENGINE_KEY_PAGE_UP;
        case XK_Page_Down:      return ENGINE_KEY_PAGE_DOWN;
        case XK_Insert:         return ENGINE_KEY_INSERT;
        case XK_Left:           return ENGINE_KEY_LEFT;
        case XK_Right:          return ENGINE_KEY_RIGHT;
        case XK_Down:           return ENGINE_KEY_DOWN;
        case XK_Up:             return ENGINE_KEY_UP;
        case XK_F1:             return ENGINE_KEY_F1;
        case XK_F2:             return ENGINE_KEY_F2;
        case XK_F3:             return ENGINE_KEY_F3;
        case XK_F4:             return ENGINE_KEY_F4;
        case XK_F5:             return ENGINE_KEY_F5;
        case XK_F6:             return ENGINE_KEY_F6;
        case XK_F7:             return ENGINE_KEY_F7;
        case XK_F8:             return ENGINE_KEY_F8;
        case XK_F9:             return ENGINE_KEY_F9;
        case XK_F10:            return ENGINE_KEY_F10;
        case XK_F11:            return ENGINE_KEY_F11;
        case XK_F12:            return ENGINE_KEY_F12;
        case XK_F13:            return ENGINE_KEY_F13;
        case XK_F14:            return ENGINE_KEY_F14;
        case XK_F15:            return ENGINE_KEY_F15;
        case XK_F16:            return ENGINE_KEY_F16;
        case XK_F17:            return ENGINE_KEY_F17;
        case XK_F18:            return ENGINE_KEY_F18;
        case XK_F19:            return ENGINE_KEY_F19;
        case XK_F20:            return ENGINE_KEY_F20;
        case XK_F21:            return ENGINE_KEY_F21;
        case XK_F22:            return ENGINE_KEY_F22;
        case XK_F23:            return ENGINE_KEY_F23;
        case XK_F24:            return ENGINE_KEY_F24;
        case XK_F25:            return ENGINE_KEY_F25;

        // Numeric keypad
        case XK_KP_Divide:      return ENGINE_KEY_KP_DIVIDE;
        case XK_KP_Multiply:    return ENGINE_KEY_KP_MULTIPLY;
        case XK_KP_Subtract:    return ENGINE_KEY_KP_SUBTRACT;
        case XK_KP_Add:         return ENGINE_KEY_KP_ADD;

        // These should have been detected in secondary keysym test above!
        case XK_KP_Insert:      return ENGINE_KEY_KP_0;
        case XK_KP_End:         return ENGINE_KEY_KP_1;
        case XK_KP_Down:        return ENGINE_KEY_KP_2;
        case XK_KP_Page_Down:   return ENGINE_KEY_KP_3;
        case XK_KP_Left:        return ENGINE_KEY_KP_4;
        case XK_KP_Right:       return ENGINE_KEY_KP_6;
        case XK_KP_Home:        return ENGINE_KEY_KP_7;
        case XK_KP_Up:          return ENGINE_KEY_KP_8;
        case XK_KP_Page_Up:     return ENGINE_KEY_KP_9;
        case XK_KP_Delete:      return ENGINE_KEY_KP_DECIMAL;
        case XK_KP_Equal:       return ENGINE_KEY_KP_EQUAL;
        case XK_KP_Enter:       return ENGINE_KEY_KP_ENTER;

        // Last resort: Check for printable keys (should not happen if the XKB
        // extension is available). This will give a layout dependent mapping
        // (which is wrong, and we may miss some keys, especially on non-US
        // keyboards), but it's better than nothing...
        case XK_a:              return ENGINE_KEY_A;
        case XK_b:              return ENGINE_KEY_B;
        case XK_c:              return ENGINE_KEY_C;
        case XK_d:              return ENGINE_KEY_D;
        case XK_e:              return ENGINE_KEY_E;
        case XK_f:              return ENGINE_KEY_F;
        case XK_g:              return ENGINE_KEY_G;
        case XK_h:              return ENGINE_KEY_H;
        case XK_i:              return ENGINE_KEY_I;
        case XK_j:              return ENGINE_KEY_J;
        case XK_k:              return ENGINE_KEY_K;
        case XK_l:              return ENGINE_KEY_L;
        case XK_m:              return ENGINE_KEY_M;
        case XK_n:              return ENGINE_KEY_N;
        case XK_o:              return ENGINE_KEY_O;
        case XK_p:              return ENGINE_KEY_P;
        case XK_q:              return ENGINE_KEY_Q;
        case XK_r:              return ENGINE_KEY_R;
        case XK_s:              return ENGINE_KEY_S;
        case XK_t:              return ENGINE_KEY_T;
        case XK_u:              return ENGINE_KEY_U;
        case XK_v:              return ENGINE_KEY_V;
        case XK_w:              return ENGINE_KEY_W;
        case XK_x:              return ENGINE_KEY_X;
        case XK_y:              return ENGINE_KEY_Y;
        case XK_z:              return ENGINE_KEY_Z;
        case XK_1:              return ENGINE_KEY_1;
        case XK_2:              return ENGINE_KEY_2;
        case XK_3:              return ENGINE_KEY_3;
        case XK_4:              return ENGINE_KEY_4;
        case XK_5:              return ENGINE_KEY_5;
        case XK_6:              return ENGINE_KEY_6;
        case XK_7:              return ENGINE_KEY_7;
        case XK_8:              return ENGINE_KEY_8;
        case XK_9:              return ENGINE_KEY_9;
        case XK_0:              return ENGINE_KEY_0;
        case XK_space:          return ENGINE_KEY_SPACE;
        case XK_minus:          return ENGINE_KEY_MINUS;
        case XK_equal:          return ENGINE_KEY_EQUAL;
        case XK_bracketleft:    return ENGINE_KEY_LEFT_BRACKET;
        case XK_bracketright:   return ENGINE_KEY_RIGHT_BRACKET;
        case XK_backslash:      return ENGINE_KEY_BACKSLASH;
        case XK_semicolon:      return ENGINE_KEY_SEMICOLON;
        case XK_apostrophe:     return ENGINE_KEY_APOSTROPHE;
        case XK_grave:          return ENGINE_KEY_GRAVE_ACCENT;
        case XK_comma:          return ENGINE_KEY_COMMA;
        case XK_period:         return ENGINE_KEY_PERIOD;
        case XK_slash:          return ENGINE_KEY_SLASH;
        case XK_less:           return ENGINE_KEY_WORLD_1; // At least in some layouts...
        default:                break;
    }

    // No matching translation was found
    return ENGINE_KEY_UNKNOWN;
}

void createKeyTables(void *wData)
{
    int scancodeMin, scancodeMax;


    wManagerX11 *wX11 = _wMWindow.WindowData;

        memset(wX11->keycodes, -1, sizeof(wX11->keycodes));
        memset(wX11->scancodes, -1, sizeof(wX11->scancodes));

        if (wX11->xkb.available)
        {
            // Use XKB to determine physical key locations independently of the
            // current keyboard layout

            XkbDescPtr desc = XkbGetMap(wX11->display, 0, XkbUseCoreKbd);
            XkbGetNames(wX11->display, XkbKeyNamesMask | XkbKeyAliasesMask, desc);

            scancodeMin = desc->min_key_code;
            scancodeMax = desc->max_key_code;

            const struct
            {
                int key;
                char* name;
            } keymap[] =
            {
                { ENGINE_KEY_GRAVE_ACCENT, "TLDE" },
                { ENGINE_KEY_1, "AE01" },
                { ENGINE_KEY_2, "AE02" },
                { ENGINE_KEY_3, "AE03" },
                { ENGINE_KEY_4, "AE04" },
                { ENGINE_KEY_5, "AE05" },
                { ENGINE_KEY_6, "AE06" },
                { ENGINE_KEY_7, "AE07" },
                { ENGINE_KEY_8, "AE08" },
                { ENGINE_KEY_9, "AE09" },
                { ENGINE_KEY_0, "AE10" },
                { ENGINE_KEY_MINUS, "AE11" },
                { ENGINE_KEY_EQUAL, "AE12" },
                { ENGINE_KEY_Q, "AD01" },
                { ENGINE_KEY_W, "AD02" },
                { ENGINE_KEY_E, "AD03" },
                { ENGINE_KEY_R, "AD04" },
                { ENGINE_KEY_T, "AD05" },
                { ENGINE_KEY_Y, "AD06" },
                { ENGINE_KEY_U, "AD07" },
                { ENGINE_KEY_I, "AD08" },
                { ENGINE_KEY_O, "AD09" },
                { ENGINE_KEY_P, "AD10" },
                { ENGINE_KEY_LEFT_BRACKET, "AD11" },
                { ENGINE_KEY_RIGHT_BRACKET, "AD12" },
                { ENGINE_KEY_A, "AC01" },
                { ENGINE_KEY_S, "AC02" },
                { ENGINE_KEY_D, "AC03" },
                { ENGINE_KEY_F, "AC04" },
                { ENGINE_KEY_G, "AC05" },
                { ENGINE_KEY_H, "AC06" },
                { ENGINE_KEY_J, "AC07" },
                { ENGINE_KEY_K, "AC08" },
                { ENGINE_KEY_L, "AC09" },
                { ENGINE_KEY_SEMICOLON, "AC10" },
                { ENGINE_KEY_APOSTROPHE, "AC11" },
                { ENGINE_KEY_Z, "AB01" },
                { ENGINE_KEY_X, "AB02" },
                { ENGINE_KEY_C, "AB03" },
                { ENGINE_KEY_V, "AB04" },
                { ENGINE_KEY_B, "AB05" },
                { ENGINE_KEY_N, "AB06" },
                { ENGINE_KEY_M, "AB07" },
                { ENGINE_KEY_COMMA, "AB08" },
                { ENGINE_KEY_PERIOD, "AB09" },
                { ENGINE_KEY_SLASH, "AB10" },
                { ENGINE_KEY_BACKSLASH, "BKSL" },
                { ENGINE_KEY_WORLD_1, "LSGT" },
                { ENGINE_KEY_SPACE, "SPCE" },
                { ENGINE_KEY_ESCAPE, "ESC" },
                { ENGINE_KEY_ENTER, "RTRN" },
                { ENGINE_KEY_TAB, "TAB" },
                { ENGINE_KEY_BACKSPACE, "BKSP" },
                { ENGINE_KEY_INSERT, "INS" },
                { ENGINE_KEY_DELETE, "DELE" },
                { ENGINE_KEY_RIGHT, "RGHT" },
                { ENGINE_KEY_LEFT, "LEFT" },
                { ENGINE_KEY_DOWN, "DOWN" },
                { ENGINE_KEY_UP, "UP" },
                { ENGINE_KEY_PAGE_UP, "PGUP" },
                { ENGINE_KEY_PAGE_DOWN, "PGDN" },
                { ENGINE_KEY_HOME, "HOME" },
                { ENGINE_KEY_END, "END" },
                { ENGINE_KEY_CAPS_LOCK, "CAPS" },
                { ENGINE_KEY_SCROLL_LOCK, "SCLK" },
                { ENGINE_KEY_NUM_LOCK, "NMLK" },
                { ENGINE_KEY_PRINT_SCREEN, "PRSC" },
                { ENGINE_KEY_PAUSE, "PAUS" },
                { ENGINE_KEY_F1, "FK01" },
                { ENGINE_KEY_F2, "FK02" },
                { ENGINE_KEY_F3, "FK03" },
                { ENGINE_KEY_F4, "FK04" },
                { ENGINE_KEY_F5, "FK05" },
                { ENGINE_KEY_F6, "FK06" },
                { ENGINE_KEY_F7, "FK07" },
                { ENGINE_KEY_F8, "FK08" },
                { ENGINE_KEY_F9, "FK09" },
                { ENGINE_KEY_F10, "FK10" },
                { ENGINE_KEY_F11, "FK11" },
                { ENGINE_KEY_F12, "FK12" },
                { ENGINE_KEY_F13, "FK13" },
                { ENGINE_KEY_F14, "FK14" },
                { ENGINE_KEY_F15, "FK15" },
                { ENGINE_KEY_F16, "FK16" },
                { ENGINE_KEY_F17, "FK17" },
                { ENGINE_KEY_F18, "FK18" },
                { ENGINE_KEY_F19, "FK19" },
                { ENGINE_KEY_F20, "FK20" },
                { ENGINE_KEY_F21, "FK21" },
                { ENGINE_KEY_F22, "FK22" },
                { ENGINE_KEY_F23, "FK23" },
                { ENGINE_KEY_F24, "FK24" },
                { ENGINE_KEY_F25, "FK25" },
                { ENGINE_KEY_KP_0, "KP0" },
                { ENGINE_KEY_KP_1, "KP1" },
                { ENGINE_KEY_KP_2, "KP2" },
                { ENGINE_KEY_KP_3, "KP3" },
                { ENGINE_KEY_KP_4, "KP4" },
                { ENGINE_KEY_KP_5, "KP5" },
                { ENGINE_KEY_KP_6, "KP6" },
                { ENGINE_KEY_KP_7, "KP7" },
                { ENGINE_KEY_KP_8, "KP8" },
                { ENGINE_KEY_KP_9, "KP9" },
                { ENGINE_KEY_KP_DECIMAL, "KPDL" },
                { ENGINE_KEY_KP_DIVIDE, "KPDV" },
                { ENGINE_KEY_KP_MULTIPLY, "KPMU" },
                { ENGINE_KEY_KP_SUBTRACT, "KPSU" },
                { ENGINE_KEY_KP_ADD, "KPAD" },
                { ENGINE_KEY_KP_ENTER, "KPEN" },
                { ENGINE_KEY_KP_EQUAL, "KPEQ" },
                { ENGINE_KEY_LEFT_SHIFT, "LFSH" },
                { ENGINE_KEY_LEFT_CONTROL, "LCTL" },
                { ENGINE_KEY_LEFT_ALT, "LALT" },
                { ENGINE_KEY_LEFT_SUPER, "LWIN" },
                { ENGINE_KEY_RIGHT_SHIFT, "RTSH" },
                { ENGINE_KEY_RIGHT_CONTROL, "RCTL" },
                { ENGINE_KEY_RIGHT_ALT, "RALT" },
                { ENGINE_KEY_RIGHT_ALT, "LVL3" },
                { ENGINE_KEY_RIGHT_ALT, "MDSW" },
                { ENGINE_KEY_RIGHT_SUPER, "RWIN" },
                { ENGINE_KEY_MENU, "MENU" }
            };

            // Find the X11 key code -> GLFW key code mapping
            for (int scancode = scancodeMin;  scancode <= scancodeMax;  scancode++)
            {
                int key = ENGINE_KEY_UNKNOWN;

                // Map the key name to a GLFW key code. Note: We use the US
                // keyboard layout. Because function keys aren't mapped correctly
                // when using traditional KeySym translations, they are mapped
                // here instead.
                for (int i = 0;  i < sizeof(keymap) / sizeof(keymap[0]);  i++)
                {
                    if (strncmp(desc->names->keys[scancode].name,
                                keymap[i].name,
                                XkbKeyNameLength) == 0)
                    {
                        key = keymap[i].key;
                        break;
                    }
                }

                // Fall back to key aliases in case the key name did not match
                for (int i = 0;  i < desc->names->num_key_aliases;  i++)
                {
                    if (key != ENGINE_KEY_UNKNOWN)
                        break;

                    if (strncmp(desc->names->key_aliases[i].real,
                                desc->names->keys[scancode].name,
                                XkbKeyNameLength) != 0)
                    {
                        continue;
                    }

                    for (int j = 0;  j < sizeof(keymap) / sizeof(keymap[0]);  j++)
                    {
                        if (strncmp(desc->names->key_aliases[i].alias,
                                    keymap[j].name,
                                    XkbKeyNameLength) == 0)
                        {
                            key = keymap[j].key;
                            break;
                        }
                    }
                }

                wX11->keycodes[scancode] = key;
            }

            XkbFreeNames(desc, XkbKeyNamesMask, True);
            XkbFreeKeyboard(desc, 0, True);
        }
        else
            XDisplayKeycodes(wX11->display, &scancodeMin, &scancodeMax);

        int width;
        KeySym* keysyms = XGetKeyboardMapping(wX11->display,
                                              scancodeMin,
                                              scancodeMax - scancodeMin + 1,
                                              &width);

        for (int scancode = scancodeMin;  scancode <= scancodeMax;  scancode++)
        {
            // Translate the un-translated key codes using traditional X11 KeySym
            // lookups
            if (wX11->keycodes[scancode] < 0)
            {
                const size_t base = (scancode - scancodeMin) * width;
                wX11->keycodes[scancode] = translateKeySyms(&keysyms[base], width);
            }

            // Store the reverse translation for faster key name lookup
            if (wX11->keycodes[scancode] > 0)
                wX11->scancodes[wX11->keycodes[scancode]] = scancode;
        }

        XFree(keysyms);
}

extern void _wManagerGrabErrorHandlerX11(void);
extern void _wManagerReleaseErrorHandlerX11(void);
extern unsigned long _wManagerGetWindowPropertyX11(Window window,
                                                   Atom property,
                                                   Atom type,
                                                   unsigned char** value);
// Return the atom ID only if it is listed in the specified array
//
Atom getAtomIfSupported(Atom* supportedAtoms,
                               unsigned long atomCount,
                               const char* atomName)
{
    wManagerX11 *wX11 = _wMWindow.WindowData;

    const Atom atom = XInternAtom(wX11->display, atomName, False);

    for (unsigned long i = 0;  i < atomCount;  i++)
    {
        if (supportedAtoms[i] == atom)
            return atom;
    }

    return None;
}


// Check whether the running window manager is EWMH-compliant
//
static void detectEWMH(void)
{

    wManagerX11 *wX11 = _wMWindow.WindowData;

    // First we read the _NET_SUPPORTING_WM_CHECK property on the root window

    Window* windowFromRoot = NULL;
    if (!_wManagerGetWindowPropertyX11(wX11->root,
                                   wX11->NET_SUPPORTING_WM_CHECK,
                                   XA_WINDOW,
                                   (unsigned char**) &windowFromRoot))
    {
        return;
    }

    _wManagerGrabErrorHandlerX11();

    // If it exists, it should be the XID of a top-level window
    // Then we look for the same property on that window

    Window* windowFromChild = NULL;
    if (!_wManagerGetWindowPropertyX11(*windowFromRoot,
                                   wX11->NET_SUPPORTING_WM_CHECK,
                                   XA_WINDOW,
                                   (unsigned char**) &windowFromChild))
    {
        XFree(windowFromRoot);
        return;
    }

    _wManagerReleaseErrorHandlerX11();

    // If the property exists, it should contain the XID of the window

    if (*windowFromRoot != *windowFromChild)
    {
        XFree(windowFromRoot);
        XFree(windowFromChild);
        return;
    }

    XFree(windowFromRoot);
    XFree(windowFromChild);

    // We are now fairly sure that an EWMH-compliant WM is currently running
    // We can now start querying the WM about what features it supports by
    // looking in the _NET_SUPPORTED property on the root window
    // It should contain a list of supported EWMH protocol and state atoms

    Atom* supportedAtoms = NULL;
    const unsigned long atomCount =
        _wManagerGetWindowPropertyX11(wX11->root,
                                  wX11->NET_SUPPORTED,
                                  XA_ATOM,
                                  (unsigned char**) &supportedAtoms);

    // See which of the atoms we support that are supported by the WM

    wX11->NET_WM_STATE =
        getAtomIfSupported(supportedAtoms, atomCount, "_NET_WM_STATE");
    wX11->NET_WM_STATE_ABOVE =
        getAtomIfSupported(supportedAtoms, atomCount, "_NET_WM_STATE_ABOVE");
    wX11->NET_WM_STATE_FULLSCREEN =
        getAtomIfSupported(supportedAtoms, atomCount, "_NET_WM_STATE_FULLSCREEN");
    wX11->NET_WM_STATE_MAXIMIZED_VERT =
        getAtomIfSupported(supportedAtoms, atomCount, "_NET_WM_STATE_MAXIMIZED_VERT");
    wX11->NET_WM_STATE_MAXIMIZED_HORZ =
        getAtomIfSupported(supportedAtoms, atomCount, "_NET_WM_STATE_MAXIMIZED_HORZ");
    wX11->NET_WM_STATE_DEMANDS_ATTENTION =
        getAtomIfSupported(supportedAtoms, atomCount, "_NET_WM_STATE_DEMANDS_ATTENTION");
    wX11->NET_WM_FULLSCREEN_MONITORS =
        getAtomIfSupported(supportedAtoms, atomCount, "_NET_WM_FULLSCREEN_MONITORS");
    wX11->NET_WM_WINDOW_TYPE =
        getAtomIfSupported(supportedAtoms, atomCount, "_NET_WM_WINDOW_TYPE");
    wX11->NET_WM_WINDOW_TYPE_NORMAL =
        getAtomIfSupported(supportedAtoms, atomCount, "_NET_WM_WINDOW_TYPE_NORMAL");
    wX11->NET_WORKAREA =
        getAtomIfSupported(supportedAtoms, atomCount, "_NET_WORKAREA");
    wX11->NET_CURRENT_DESKTOP =
        getAtomIfSupported(supportedAtoms, atomCount, "_NET_CURRENT_DESKTOP");
    wX11->NET_ACTIVE_WINDOW =
        getAtomIfSupported(supportedAtoms, atomCount, "_NET_ACTIVE_WINDOW");
    wX11->NET_FRAME_EXTENTS =
        getAtomIfSupported(supportedAtoms, atomCount, "_NET_FRAME_EXTENTS");
    wX11->NET_REQUEST_FRAME_EXTENTS =
        getAtomIfSupported(supportedAtoms, atomCount, "_NET_REQUEST_FRAME_EXTENTS");

    if (supportedAtoms)
        XFree(supportedAtoms);
}

// Look for and initialize supported X11 extensions
//
static int32_t initExtensions(void)
{

    wManagerX11 *wX11 = _wMWindow.WindowData;

/*#if defined(__OpenBSD__) || defined(__NetBSD__)
    wX11->vidmode.handle = _wManagerPlatformLoadModule("libXxf86vm.so");
#else
    wX11->vidmode.handle = _wManagerPlatformLoadModule("libXxf86vm.so.1");
#endif
    if (wX11->vidmode.handle)
    {
        wX11->vidmode.QueryExtension = (PFN_XF86VidModeQueryExtension)
            _wManagerPlatformGetModuleSymbol(wX11->vidmode.handle, "XF86VidModeQueryExtension");
        wX11->vidmode.GetGammaRamp = (PFN_XF86VidModeGetGammaRamp)
            _wManagerPlatformGetModuleSymbol(wX11->vidmode.handle, "XF86VidModeGetGammaRamp");
        wX11->vidmode.SetGammaRamp = (PFN_XF86VidModeSetGammaRamp)
            _wManagerPlatformGetModuleSymbol(wX11->vidmode.handle, "XF86VidModeSetGammaRamp");
        wX11->vidmode.GetGammaRampSize = (PFN_XF86VidModeGetGammaRampSize)
            _wManagerPlatformGetModuleSymbol(wX11->vidmode.handle, "XF86VidModeGetGammaRampSize");

        wX11->vidmode.available =
            XF86VidModeQueryExtension(wX11->display,
                                      &wX11->vidmode.eventBase,
                                      &wX11->vidmode.errorBase);
    }*/

#if defined(__CYGWIN__)
    wX11->xi.handle = _wManagerPlatformLoadModule("libXi-6.so");
#elif defined(__OpenBSD__) || defined(__NetBSD__)
    wX11->xi.handle = _wManagerPlatformLoadModule("libXi.so");
#else
    wX11->xi.handle = _wManagerPlatformLoadModule("libXi.so.6");
#endif
    if (wX11->xi.handle)
    {
        wX11->xi.QueryVersion = (PFN_XIQueryVersion)
            _wManagerPlatformGetModuleSymbol(wX11->xi.handle, "XIQueryVersion");
        wX11->xi.SelectEvents = (PFN_XISelectEvents)
            _wManagerPlatformGetModuleSymbol(wX11->xi.handle, "XISelectEvents");

        if (XQueryExtension(wX11->display,
                            "XInputExtension",
                            &wX11->xi.majorOpcode,
                            &wX11->xi.eventBase,
                            &wX11->xi.errorBase))
        {
            wX11->xi.major = 2;
            wX11->xi.minor = 0;

            if (XIQueryVersion(wX11->display,
                               &wX11->xi.major,
                               &wX11->xi.minor) == Success)
            {
                wX11->xi.available = true;
            }
        }
    }

#if defined(__CYGWIN__)
    wX11->randr.handle = _wManagerPlatformLoadModule("libXrandr-2.so");
#elif defined(__OpenBSD__) || defined(__NetBSD__)
    wX11->randr.handle = _wManagerPlatformLoadModule("libXrandr.so");
#else
    wX11->randr.handle = _wManagerPlatformLoadModule("libXrandr.so.2");
#endif
    if (wX11->randr.handle)
    {
        wX11->randr.AllocGamma = (PFN_XRRAllocGamma)
            _wManagerPlatformGetModuleSymbol(wX11->randr.handle, "XRRAllocGamma");
        wX11->randr.FreeGamma = (PFN_XRRFreeGamma)
            _wManagerPlatformGetModuleSymbol(wX11->randr.handle, "XRRFreeGamma");
        wX11->randr.FreeCrtcInfo = (PFN_XRRFreeCrtcInfo)
            _wManagerPlatformGetModuleSymbol(wX11->randr.handle, "XRRFreeCrtcInfo");
        wX11->randr.FreeGamma = (PFN_XRRFreeGamma)
            _wManagerPlatformGetModuleSymbol(wX11->randr.handle, "XRRFreeGamma");
        wX11->randr.FreeOutputInfo = (PFN_XRRFreeOutputInfo)
            _wManagerPlatformGetModuleSymbol(wX11->randr.handle, "XRRFreeOutputInfo");
        wX11->randr.FreeScreenResources = (PFN_XRRFreeScreenResources)
            _wManagerPlatformGetModuleSymbol(wX11->randr.handle, "XRRFreeScreenResources");
        wX11->randr.GetCrtcGamma = (PFN_XRRGetCrtcGamma)
            _wManagerPlatformGetModuleSymbol(wX11->randr.handle, "XRRGetCrtcGamma");
        wX11->randr.GetCrtcGammaSize = (PFN_XRRGetCrtcGammaSize)
            _wManagerPlatformGetModuleSymbol(wX11->randr.handle, "XRRGetCrtcGammaSize");
        wX11->randr.GetCrtcInfo = (PFN_XRRGetCrtcInfo)
            _wManagerPlatformGetModuleSymbol(wX11->randr.handle, "XRRGetCrtcInfo");
        wX11->randr.GetOutputInfo = (PFN_XRRGetOutputInfo)
            _wManagerPlatformGetModuleSymbol(wX11->randr.handle, "XRRGetOutputInfo");
        wX11->randr.GetOutputPrimary = (PFN_XRRGetOutputPrimary)
            _wManagerPlatformGetModuleSymbol(wX11->randr.handle, "XRRGetOutputPrimary");
        wX11->randr.GetScreenResourcesCurrent = (PFN_XRRGetScreenResourcesCurrent)
            _wManagerPlatformGetModuleSymbol(wX11->randr.handle, "XRRGetScreenResourcesCurrent");
        wX11->randr.QueryExtension = (PFN_XRRQueryExtension)
            _wManagerPlatformGetModuleSymbol(wX11->randr.handle, "XRRQueryExtension");
        wX11->randr.QueryVersion = (PFN_XRRQueryVersion)
            _wManagerPlatformGetModuleSymbol(wX11->randr.handle, "XRRQueryVersion");
        wX11->randr.SelectInput = (PFN_XRRSelectInput)
            _wManagerPlatformGetModuleSymbol(wX11->randr.handle, "XRRSelectInput");
        wX11->randr.SetCrtcConfig = (PFN_XRRSetCrtcConfig)
            _wManagerPlatformGetModuleSymbol(wX11->randr.handle, "XRRSetCrtcConfig");
        wX11->randr.SetCrtcGamma = (PFN_XRRSetCrtcGamma)
            _wManagerPlatformGetModuleSymbol(wX11->randr.handle, "XRRSetCrtcGamma");
        wX11->randr.UpdateConfiguration = (PFN_XRRUpdateConfiguration)
            _wManagerPlatformGetModuleSymbol(wX11->randr.handle, "XRRUpdateConfiguration");

        if (XRRQueryExtension(wX11->display,
                              &wX11->randr.eventBase,
                              &wX11->randr.errorBase))
        {
            if (XRRQueryVersion(wX11->display,
                                &wX11->randr.major,
                                &wX11->randr.minor))
            {
                // The GLFW RandR path requires at least version 1.3
                if (wX11->randr.major > 1 || wX11->randr.minor >= 3)
                    wX11->randr.available = true;
            }
            else
            {
                printf("X11: Failed to query RandR version\n");
            }
        }
    }

    if (wX11->randr.available)
    {
        XRRScreenResources* sr = XRRGetScreenResourcesCurrent(wX11->display,
                                                              wX11->root);

        if (!sr->ncrtc || !XRRGetCrtcGammaSize(wX11->display, sr->crtcs[0]))
        {
            // This is likely an older Nvidia driver with broken gamma support
            // Flag it as useless and fall back to xf86vm gamma, if available
            wX11->randr.gammaBroken = true;
        }

        if (!sr->ncrtc)
        {
            // A system without CRTCs is likely a system with broken RandR
            // Disable the RandR monitor path and fall back to core functions
            wX11->randr.monitorBroken = true;
        }

        XRRFreeScreenResources(sr);
    }

    if (wX11->randr.available && !wX11->randr.monitorBroken)
    {
        XRRSelectInput(wX11->display, wX11->root,
                       RROutputChangeNotifyMask);
    }

#if defined(__CYGWIN__)
    wX11->xcursor.handle = _wManagerPlatformLoadModule("libXcursor-1.so");
#elif defined(__OpenBSD__) || defined(__NetBSD__)
    wX11->xcursor.handle = _wManagerPlatformLoadModule("libXcursor.so");
#else
    wX11->xcursor.handle = _wManagerPlatformLoadModule("libXcursor.so.1");
#endif
    if (wX11->xcursor.handle)
    {
        wX11->xcursor.ImageCreate = (PFN_XcursorImageCreate)
            _wManagerPlatformGetModuleSymbol(wX11->xcursor.handle, "XcursorImageCreate");
        wX11->xcursor.ImageDestroy = (PFN_XcursorImageDestroy)
            _wManagerPlatformGetModuleSymbol(wX11->xcursor.handle, "XcursorImageDestroy");
        wX11->xcursor.ImageLoadCursor = (PFN_XcursorImageLoadCursor)
            _wManagerPlatformGetModuleSymbol(wX11->xcursor.handle, "XcursorImageLoadCursor");
        wX11->xcursor.GetTheme = (PFN_XcursorGetTheme)
            _wManagerPlatformGetModuleSymbol(wX11->xcursor.handle, "XcursorGetTheme");
        wX11->xcursor.GetDefaultSize = (PFN_XcursorGetDefaultSize)
            _wManagerPlatformGetModuleSymbol(wX11->xcursor.handle, "XcursorGetDefaultSize");
        wX11->xcursor.LibraryLoadImage = (PFN_XcursorLibraryLoadImage)
            _wManagerPlatformGetModuleSymbol(wX11->xcursor.handle, "XcursorLibraryLoadImage");
    }

/*#if defined(__CYGWIN__)
    wX11->xinerama.handle = _wManagerPlatformLoadModule("libXinerama-1.so");
#elif defined(__OpenBSD__) || defined(__NetBSD__)
    wX11->xinerama.handle = _wManagerPlatformLoadModule("libXinerama.so");
#else
    wX11->xinerama.handle = _wManagerPlatformLoadModule("libXinerama.so.1");
#endif
    if (wX11->xinerama.handle)
    {
        wX11->xinerama.IsActive = (PFN_XineramaIsActive)
            _wManagerPlatformGetModuleSymbol(wX11->xinerama.handle, "XineramaIsActive");
        wX11->xinerama.QueryExtension = (PFN_XineramaQueryExtension)
            _wManagerPlatformGetModuleSymbol(wX11->xinerama.handle, "XineramaQueryExtension");
        wX11->xinerama.QueryScreens = (PFN_XineramaQueryScreens)
            _wManagerPlatformGetModuleSymbol(wX11->xinerama.handle, "XineramaQueryScreens");

        if (XineramaQueryExtension(wX11->display,
                                   &wX11->xinerama.major,
                                   &wX11->xinerama.minor))
        {
            if (XineramaIsActive(wX11->display))
                wX11->xinerama.available = true;
        }
    }*/

    wX11->xkb.major = 1;
    wX11->xkb.minor = 0;
    wX11->xkb.available =
        XkbQueryExtension(wX11->display,
                          &wX11->xkb.majorOpcode,
                          &wX11->xkb.eventBase,
                          &wX11->xkb.errorBase,
                          &wX11->xkb.major,
                          &wX11->xkb.minor);

    if (wX11->xkb.available)
    {
        Bool supported;

        if (XkbSetDetectableAutoRepeat(wX11->display, True, &supported))
        {
            if (supported)
                wX11->xkb.detectable = true;
        }

        XkbStateRec state;
        if (XkbGetState(wX11->display, XkbUseCoreKbd, &state) == Success)
            wX11->xkb.group = (unsigned int)state.group;

        XkbSelectEventDetails(wX11->display, XkbUseCoreKbd, XkbStateNotify,
                              XkbGroupStateMask, XkbGroupStateMask);
    }

    if (_wMWindow.hints.init.x11.xcbVulkanSurface)
    {
#if defined(__CYGWIN__)
        wX11->x11xcb.handle = _wManagerPlatformLoadModule("libX11-xcb-1.so");
#elif defined(__OpenBSD__) || defined(__NetBSD__)
        wX11->x11xcb.handle = _wManagerPlatformLoadModule("libX11-xcb.so");
#else
        wX11->x11xcb.handle = _wManagerPlatformLoadModule("libX11-xcb.so.1");
#endif
    }

    if (wX11->x11xcb.handle)
    {
        wX11->x11xcb.GetXCBConnection = (PFN_XGetXCBConnection)
            _wManagerPlatformGetModuleSymbol(wX11->x11xcb.handle, "XGetXCBConnection");
    }

#if defined(__CYGWIN__)
    wX11->xrender.handle = _wManagerPlatformLoadModule("libXrender-1.so");
#elif defined(__OpenBSD__) || defined(__NetBSD__)
    wX11->xrender.handle = _wManagerPlatformLoadModule("libXrender.so");
#else
    wX11->xrender.handle = _wManagerPlatformLoadModule("libXrender.so.1");
#endif
    if (wX11->xrender.handle)
    {
        wX11->xrender.QueryExtension = (PFN_XRenderQueryExtension)
            _wManagerPlatformGetModuleSymbol(wX11->xrender.handle, "XRenderQueryExtension");
        wX11->xrender.QueryVersion = (PFN_XRenderQueryVersion)
            _wManagerPlatformGetModuleSymbol(wX11->xrender.handle, "XRenderQueryVersion");
        wX11->xrender.FindVisualFormat = (PFN_XRenderFindVisualFormat)
            _wManagerPlatformGetModuleSymbol(wX11->xrender.handle, "XRenderFindVisualFormat");

        if (XRenderQueryExtension(wX11->display,
                                  &wX11->xrender.errorBase,
                                  &wX11->xrender.eventBase))
        {
            if (XRenderQueryVersion(wX11->display,
                                    &wX11->xrender.major,
                                    &wX11->xrender.minor))
            {
                wX11->xrender.available = true;
            }
        }
    }

#if defined(__CYGWIN__)
    wX11->xshape.handle = _wManagerPlatformLoadModule("libXext-6.so");
#elif defined(__OpenBSD__) || defined(__NetBSD__)
    wX11->xshape.handle = _wManagerPlatformLoadModule("libXext.so");
#else
    wX11->xshape.handle = _wManagerPlatformLoadModule("libXext.so.6");
#endif
    if (wX11->xshape.handle)
    {
        wX11->xshape.QueryExtension = (PFN_XShapeQueryExtension)
            _wManagerPlatformGetModuleSymbol(wX11->xshape.handle, "XShapeQueryExtension");
        wX11->xshape.ShapeCombineRegion = (PFN_XShapeCombineRegion)
            _wManagerPlatformGetModuleSymbol(wX11->xshape.handle, "XShapeCombineRegion");
        wX11->xshape.QueryVersion = (PFN_XShapeQueryVersion)
            _wManagerPlatformGetModuleSymbol(wX11->xshape.handle, "XShapeQueryVersion");
        wX11->xshape.ShapeCombineMask = (PFN_XShapeCombineMask)
            _wManagerPlatformGetModuleSymbol(wX11->xshape.handle, "XShapeCombineMask");

        if (XShapeQueryExtension(wX11->display,
            &wX11->xshape.errorBase,
            &wX11->xshape.eventBase))
        {
            if (XShapeQueryVersion(wX11->display,
                &wX11->xshape.major,
                &wX11->xshape.minor))
            {
                wX11->xshape.available = true;
            }
        }
    }

    // Update the key code LUT
    // FIXME: We should listen to XkbMapNotify events to track changes to
    // the keyboard mapping.
    createKeyTables(_wMWindow.WindowData);

    // String format atoms
    wX11->NULL_ = XInternAtom(wX11->display, "NULL", False);
    wX11->UTF8_STRING = XInternAtom(wX11->display, "UTF8_STRING", False);
    wX11->ATOM_PAIR = XInternAtom(wX11->display, "ATOM_PAIR", False);

    // Custom selection property atom
    wX11->ENGINE_SELECTION =
        XInternAtom(wX11->display, "GLFW_SELECTION", False);

    // ICCCM standard clipboard atoms
    wX11->TARGETS = XInternAtom(wX11->display, "TARGETS", False);
    wX11->MULTIPLE = XInternAtom(wX11->display, "MULTIPLE", False);
    wX11->PRIMARY = XInternAtom(wX11->display, "PRIMARY", False);
    wX11->INCR = XInternAtom(wX11->display, "INCR", False);
    wX11->CLIPBOARD = XInternAtom(wX11->display, "CLIPBOARD", False);

    // Clipboard manager atoms
    wX11->CLIPBOARD_MANAGER =
        XInternAtom(wX11->display, "CLIPBOARD_MANAGER", False);
    wX11->SAVE_TARGETS =
        XInternAtom(wX11->display, "SAVE_TARGETS", False);

    // Xdnd (drag and drop) atoms
    wX11->XdndAware = XInternAtom(wX11->display, "XdndAware", False);
    wX11->XdndEnter = XInternAtom(wX11->display, "XdndEnter", False);
    wX11->XdndPosition = XInternAtom(wX11->display, "XdndPosition", False);
    wX11->XdndStatus = XInternAtom(wX11->display, "XdndStatus", False);
    wX11->XdndActionCopy = XInternAtom(wX11->display, "XdndActionCopy", False);
    wX11->XdndDrop = XInternAtom(wX11->display, "XdndDrop", False);
    wX11->XdndFinished = XInternAtom(wX11->display, "XdndFinished", False);
    wX11->XdndSelection = XInternAtom(wX11->display, "XdndSelection", False);
    wX11->XdndTypeList = XInternAtom(wX11->display, "XdndTypeList", False);
    wX11->text_uri_list = XInternAtom(wX11->display, "text/uri-list", False);

    // ICCCM, EWMH and Motif window property atoms
    // These can be set safely even without WM support
    // The EWMH atoms that require WM support are handled in detectEWMH
    wX11->WM_PROTOCOLS =
        XInternAtom(wX11->display, "WM_PROTOCOLS", False);
    wX11->WM_STATE =
        XInternAtom(wX11->display, "WM_STATE", False);
    wX11->WM_DELETE_WINDOW =
        XInternAtom(wX11->display, "WM_DELETE_WINDOW", False);
    wX11->NET_SUPPORTED =
        XInternAtom(wX11->display, "_NET_SUPPORTED", False);
    wX11->NET_SUPPORTING_WM_CHECK =
        XInternAtom(wX11->display, "_NET_SUPPORTING_WM_CHECK", False);
    wX11->NET_WM_ICON =
        XInternAtom(wX11->display, "_NET_WM_ICON", False);
    wX11->NET_WM_PING =
        XInternAtom(wX11->display, "_NET_WM_PING", False);
    wX11->NET_WM_PID =
        XInternAtom(wX11->display, "_NET_WM_PID", False);
    wX11->NET_WM_NAME =
        XInternAtom(wX11->display, "_NET_WM_NAME", False);
    wX11->NET_WM_ICON_NAME =
        XInternAtom(wX11->display, "_NET_WM_ICON_NAME", False);
    wX11->NET_WM_BYPASS_COMPOSITOR =
        XInternAtom(wX11->display, "_NET_WM_BYPASS_COMPOSITOR", False);
    wX11->NET_WM_WINDOW_OPACITY =
        XInternAtom(wX11->display, "_NET_WM_WINDOW_OPACITY", False);
    wX11->MOTIF_WM_HINTS =
        XInternAtom(wX11->display, "_MOTIF_WM_HINTS", False);

    // The compositing manager selection name contains the screen number
    {
        char name[32];
        snprintf(name, sizeof(name), "_NET_WM_CM_S%u", wX11->screen);
        wX11->NET_WM_CM_Sx = XInternAtom(wX11->display, name, False);
    }

    // Detect whether an EWMH-conformant window manager is running
    detectEWMH();

    return true;
}

// Retrieve system content scale via folklore heuristics
//
void getSystemContentScale(float* xscale, float* yscale)
{
    // Start by assuming the default X11 DPI
    // NOTE: Some desktop environments (KDE) may remove the Xft.dpi field when it
    //       would be set to 96, so assume that is the case if we cannot find it
    float xdpi = 96.f, ydpi = 96.f;

    // NOTE: Basing the scale on Xft.dpi where available should provide the most
    //       consistent user experience (matches Qt, Gtk, etc), although not
    //       always the most accurate one
    char* rms = XResourceManagerString(((wManagerX11 *)_wMWindow.WindowData)->display);
    if (rms)
    {
        XrmDatabase db = XrmGetStringDatabase(rms);
        if (db)
        {
            XrmValue value;
            char* type = NULL;

            if (XrmGetResource(db, "Xft.dpi", "Xft.Dpi", &type, &value))
            {
                if (type && strcmp(type, "String") == 0)
                    xdpi = ydpi = atof(value.addr);
            }

            XrmDestroyDatabase(db);
        }
    }

    *xscale = xdpi / 96.f;
    *yscale = ydpi / 96.f;
}

// Create the pipe for empty events without assumuing the OS has pipe2(2)
//
int32_t createEmptyEventPipe(void)
{

    wManagerX11 *wX11 = _wMWindow.WindowData;

    if (pipe(wX11->emptyEventPipe) != 0)
    {
        printf("X11: Failed to create empty event pipe: %s\n",
                        strerror(errno));
        return false;
    }

    for (int i = 0; i < 2; i++)
    {
        const int sf = fcntl(wX11->emptyEventPipe[i], F_GETFL, 0);
        const int df = fcntl(wX11->emptyEventPipe[i], F_GETFD, 0);

        if (sf == -1 || df == -1 ||
            fcntl(wX11->emptyEventPipe[i], F_SETFL, sf | O_NONBLOCK) == -1 ||
            fcntl(wX11->emptyEventPipe[i], F_SETFD, df | FD_CLOEXEC) == -1)
        {
            printf("X11: Failed to set flags for empty event pipe: %s\n", strerror(errno));
            return false;
        }
    }

    return true;
}

// Creates a native cursor object from the specified image and hotspot
//
Cursor _wManagerCreateNativeCursorX11(const wManagerImage* image, int xhot, int yhot)
{

    wManagerX11 *wX11 = _wMWindow.WindowData;

    Cursor cursor;

    if (!wX11->xcursor.handle)
        return None;

    XcursorImage* native = wX11->xcursor.ImageCreate(image->width, image->height);
    if (native == NULL)
        return None;

    native->xhot = xhot;
    native->yhot = yhot;

    unsigned char* source = (unsigned char*) image->pixels;
    XcursorPixel* target = native->pixels;

    for (int i = 0;  i < image->width * image->height;  i++, target++, source += 4)
    {
        unsigned int alpha = source[3];

        *target = (alpha << 24) |
                  ((unsigned char) ((source[0] * alpha) / 255) << 16) |
                  ((unsigned char) ((source[1] * alpha) / 255) <<  8) |
                  ((unsigned char) ((source[2] * alpha) / 255) <<  0);
    }

    cursor = wX11->xcursor.ImageLoadCursor(wX11->display, native);
    wX11->xcursor.ImageDestroy(native);

    return cursor;
}

// Create a blank cursor for hidden and disabled cursor modes
//
Cursor createHiddenCursor(void)
{
    unsigned char pixels[16 * 16 * 4] = { 0 };
    wManagerImage image = { 16, 16, pixels };
    return _wManagerCreateNativeCursorX11(&image, 0, 0);
}


int _wManagerInitX11(void)
{

    wManagerX11 *wX11 = _wMWindow.WindowData;

    //Library initialiations
    {
    wX11->xlib.AllocClassHint = (PFN_XAllocClassHint)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XAllocClassHint");
    wX11->xlib.AllocSizeHints = (PFN_XAllocSizeHints)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XAllocSizeHints");
    wX11->xlib.AllocWMHints = (PFN_XAllocWMHints)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XAllocWMHints");
    wX11->xlib.ChangeProperty = (PFN_XChangeProperty)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XChangeProperty");
    wX11->xlib.ChangeWindowAttributes = (PFN_XChangeWindowAttributes)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XChangeWindowAttributes");
    wX11->xlib.CheckIfEvent = (PFN_XCheckIfEvent)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XCheckIfEvent");
    wX11->xlib.CheckTypedWindowEvent = (PFN_XCheckTypedWindowEvent)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XCheckTypedWindowEvent");
    wX11->xlib.CloseDisplay = (PFN_XCloseDisplay)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XCloseDisplay");
    wX11->xlib.CloseIM = (PFN_XCloseIM)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XCloseIM");
    wX11->xlib.ConvertSelection = (PFN_XConvertSelection)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XConvertSelection");
    wX11->xlib.CreateColormap = (PFN_XCreateColormap)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XCreateColormap");
    wX11->xlib.CreateFontCursor = (PFN_XCreateFontCursor)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XCreateFontCursor");
    wX11->xlib.CreateIC = (PFN_XCreateIC)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XCreateIC");
    wX11->xlib.CreateRegion = (PFN_XCreateRegion)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XCreateRegion");
    wX11->xlib.CreateWindow = (PFN_XCreateWindow)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XCreateWindow");
    wX11->xlib.DefineCursor = (PFN_XDefineCursor)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XDefineCursor");
    wX11->xlib.DeleteContext = (PFN_XDeleteContext)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XDeleteContext");
    wX11->xlib.DeleteProperty = (PFN_XDeleteProperty)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XDeleteProperty");
    wX11->xlib.DestroyIC = (PFN_XDestroyIC)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XDestroyIC");
    wX11->xlib.DestroyRegion = (PFN_XDestroyRegion)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XDestroyRegion");
    wX11->xlib.DestroyWindow = (PFN_XDestroyWindow)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XDestroyWindow");
    wX11->xlib.DisplayKeycodes = (PFN_XDisplayKeycodes)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XDisplayKeycodes");
    wX11->xlib.EventsQueued = (PFN_XEventsQueued)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XEventsQueued");
    wX11->xlib.FilterEvent = (PFN_XFilterEvent)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XFilterEvent");
    wX11->xlib.FindContext = (PFN_XFindContext)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XFindContext");
    wX11->xlib.Flush = (PFN_XFlush)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XFlush");
    wX11->xlib.Free = (PFN_XFree)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XFree");
    wX11->xlib.FreeColormap = (PFN_XFreeColormap)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XFreeColormap");
    wX11->xlib.FreeCursor = (PFN_XFreeCursor)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XFreeCursor");
    wX11->xlib.FreeEventData = (PFN_XFreeEventData)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XFreeEventData");
    wX11->xlib.GetErrorText = (PFN_XGetErrorText)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XGetErrorText");
    wX11->xlib.GetEventData = (PFN_XGetEventData)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XGetEventData");
    wX11->xlib.GetICValues = (PFN_XGetICValues)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XGetICValues");
    wX11->xlib.GetIMValues = (PFN_XGetIMValues)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XGetIMValues");
    wX11->xlib.GetInputFocus = (PFN_XGetInputFocus)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XGetInputFocus");
    wX11->xlib.GetKeyboardMapping = (PFN_XGetKeyboardMapping)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XGetKeyboardMapping");
    wX11->xlib.GetScreenSaver = (PFN_XGetScreenSaver)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XGetScreenSaver");
    wX11->xlib.GetSelectionOwner = (PFN_XGetSelectionOwner)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XGetSelectionOwner");
    wX11->xlib.GetVisualInfo = (PFN_XGetVisualInfo)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XGetVisualInfo");
    wX11->xlib.GetWMNormalHints = (PFN_XGetWMNormalHints)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XGetWMNormalHints");
    wX11->xlib.GetWindowAttributes = (PFN_XGetWindowAttributes)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XGetWindowAttributes");
    wX11->xlib.GetWindowProperty = (PFN_XGetWindowProperty)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XGetWindowProperty");
    wX11->xlib.GrabPointer = (PFN_XGrabPointer)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XGrabPointer");
    wX11->xlib.IconifyWindow = (PFN_XIconifyWindow)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XIconifyWindow");
    wX11->xlib.InternAtom = (PFN_XInternAtom)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XInternAtom");
    wX11->xlib.LookupString = (PFN_XLookupString)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XLookupString");
    wX11->xlib.MapRaised = (PFN_XMapRaised)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XMapRaised");
    wX11->xlib.MapWindow = (PFN_XMapWindow)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XMapWindow");
    wX11->xlib.MoveResizeWindow = (PFN_XMoveResizeWindow)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XMoveResizeWindow");
    wX11->xlib.MoveWindow = (PFN_XMoveWindow)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XMoveWindow");
    wX11->xlib.NextEvent = (PFN_XNextEvent)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XNextEvent");
    wX11->xlib.OpenIM = (PFN_XOpenIM)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XOpenIM");
    wX11->xlib.PeekEvent = (PFN_XPeekEvent)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XPeekEvent");
    wX11->xlib.Pending = (PFN_XPending)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XPending");
    wX11->xlib.QueryExtension = (PFN_XQueryExtension)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XQueryExtension");
    wX11->xlib.QueryPointer = (PFN_XQueryPointer)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XQueryPointer");
    wX11->xlib.RaiseWindow = (PFN_XRaiseWindow)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XRaiseWindow");
    wX11->xlib.RegisterIMInstantiateCallback = (PFN_XRegisterIMInstantiateCallback)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XRegisterIMInstantiateCallback");
    wX11->xlib.ResizeWindow = (PFN_XResizeWindow)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XResizeWindow");
    wX11->xlib.ResourceManagerString = (PFN_XResourceManagerString)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XResourceManagerString");
    wX11->xlib.SaveContext = (PFN_XSaveContext)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XSaveContext");
    wX11->xlib.SelectInput = (PFN_XSelectInput)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XSelectInput");
    wX11->xlib.SendEvent = (PFN_XSendEvent)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XSendEvent");
    wX11->xlib.SetClassHint = (PFN_XSetClassHint)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XSetClassHint");
    wX11->xlib.SetErrorHandler = (PFN_XSetErrorHandler)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XSetErrorHandler");
    wX11->xlib.SetICFocus = (PFN_XSetICFocus)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XSetICFocus");
    wX11->xlib.SetIMValues = (PFN_XSetIMValues)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XSetIMValues");
    wX11->xlib.SetInputFocus = (PFN_XSetInputFocus)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XSetInputFocus");
    wX11->xlib.SetLocaleModifiers = (PFN_XSetLocaleModifiers)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XSetLocaleModifiers");
    wX11->xlib.SetScreenSaver = (PFN_XSetScreenSaver)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XSetScreenSaver");
    wX11->xlib.SetSelectionOwner = (PFN_XSetSelectionOwner)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XSetSelectionOwner");
    wX11->xlib.SetWMHints = (PFN_XSetWMHints)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XSetWMHints");
    wX11->xlib.SetWMNormalHints = (PFN_XSetWMNormalHints)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XSetWMNormalHints");
    wX11->xlib.SetWMProtocols = (PFN_XSetWMProtocols)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XSetWMProtocols");
    wX11->xlib.SupportsLocale = (PFN_XSupportsLocale)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XSupportsLocale");
    wX11->xlib.Sync = (PFN_XSync)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XSync");
    wX11->xlib.TranslateCoordinates = (PFN_XTranslateCoordinates)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XTranslateCoordinates");
    wX11->xlib.UndefineCursor = (PFN_XUndefineCursor)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XUndefineCursor");
    wX11->xlib.UngrabPointer = (PFN_XUngrabPointer)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XUngrabPointer");
    wX11->xlib.UnmapWindow = (PFN_XUnmapWindow)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XUnmapWindow");
    wX11->xlib.UnsetICFocus = (PFN_XUnsetICFocus)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XUnsetICFocus");
    wX11->xlib.VisualIDFromVisual = (PFN_XVisualIDFromVisual)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XVisualIDFromVisual");
    wX11->xlib.WarpPointer = (PFN_XWarpPointer)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XWarpPointer");
    wX11->xkb.FreeKeyboard = (PFN_XkbFreeKeyboard)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XkbFreeKeyboard");
    wX11->xkb.FreeNames = (PFN_XkbFreeNames)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XkbFreeNames");
    wX11->xkb.GetMap = (PFN_XkbGetMap)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XkbGetMap");
    wX11->xkb.GetNames = (PFN_XkbGetNames)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XkbGetNames");
    wX11->xkb.GetState = (PFN_XkbGetState)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XkbGetState");
    wX11->xkb.KeycodeToKeysym = (PFN_XkbKeycodeToKeysym)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XkbKeycodeToKeysym");
    wX11->xkb.QueryExtension = (PFN_XkbQueryExtension)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XkbQueryExtension");
    wX11->xkb.SelectEventDetails = (PFN_XkbSelectEventDetails)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XkbSelectEventDetails");
    wX11->xkb.SetDetectableAutoRepeat = (PFN_XkbSetDetectableAutoRepeat)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XkbSetDetectableAutoRepeat");
    wX11->xrm.DestroyDatabase = (PFN_XrmDestroyDatabase)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XrmDestroyDatabase");
    wX11->xrm.GetResource = (PFN_XrmGetResource)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XrmGetResource");
    wX11->xrm.GetStringDatabase = (PFN_XrmGetStringDatabase)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XrmGetStringDatabase");
    wX11->xrm.UniqueQuark = (PFN_XrmUniqueQuark)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XrmUniqueQuark");
    wX11->xlib.UnregisterIMInstantiateCallback = (PFN_XUnregisterIMInstantiateCallback)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "XUnregisterIMInstantiateCallback");
    wX11->xlib.utf8LookupString = (PFN_Xutf8LookupString)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "Xutf8LookupString");
    wX11->xlib.utf8SetWMProperties = (PFN_Xutf8SetWMProperties)
        _wManagerPlatformGetModuleSymbol(wX11->xlib.handle, "Xutf8SetWMProperties");
    }

    if (wX11->xlib.utf8LookupString && wX11->xlib.utf8SetWMProperties)
        wX11->xlib.utf8 = true;

    wX11->screen = DefaultScreen(wX11->display);
    wX11->root = RootWindow(wX11->display, wX11->screen);
    wX11->context = XUniqueContext();

    getSystemContentScale(&wX11->contentScaleX, &wX11->contentScaleY);

    if (!createEmptyEventPipe())
        return false;

    if (!initExtensions())
        return false;

    /*wX11->helperWindowHandle = createHelperWindow();*/
    wX11->hiddenCursorHandle = createHiddenCursor();

    if (XSupportsLocale() && wX11->xlib.utf8)
    {
        XSetLocaleModifiers("");

        // If an IM is already present our callback will be called right away
        XRegisterIMInstantiateCallback(wX11->display,
                                       NULL, NULL, NULL,
                                       inputMethodInstantiateCallback,
                                       NULL);
    }

    //_wManagerPollMonitorsX11();
    return true;
}

void _wManagerTerminateX11(void)
{

    wManagerX11 *wX11 = _wMWindow.WindowData;

    /*if (wX11->helperWindowHandle)
    {
        if (XGetSelectionOwner(wX11->display, wX11->CLIPBOARD) ==
            wX11->helperWindowHandle)
        {
            _glfwPushSelectionToManagerX11();
        }

        XDestroyWindow(wX11->display, wX11->helperWindowHandle);
        wX11->helperWindowHandle = None;
    }*/

    if (wX11->hiddenCursorHandle)
    {
        XFreeCursor(wX11->display, wX11->hiddenCursorHandle);
        wX11->hiddenCursorHandle = (Cursor) 0;
    }

    free(wX11->primarySelectionString);
    free(wX11->clipboardString);

    XUnregisterIMInstantiateCallback(wX11->display,
                                     NULL, NULL, NULL,
                                     inputMethodInstantiateCallback,
                                     NULL);

    if (wX11->im)
    {
        XCloseIM(wX11->im);
        wX11->im = NULL;
    }

    if (wX11->display)
    {
        XCloseDisplay(wX11->display);
        wX11->display = NULL;
    }

    if (wX11->x11xcb.handle)
    {
        _wManagerPlatformFreeModule(wX11->x11xcb.handle);
        wX11->x11xcb.handle = NULL;
    }

    if (wX11->xcursor.handle)
    {
        _wManagerPlatformFreeModule(wX11->xcursor.handle);
        wX11->xcursor.handle = NULL;
    }

    if (wX11->randr.handle)
    {
        _wManagerPlatformFreeModule(wX11->randr.handle);
        wX11->randr.handle = NULL;
    }

    /*if (wX11->xinerama.handle)
    {
        _wManagerPlatformFreeModule(wX11->xinerama.handle);
        wX11->xinerama.handle = NULL;
    }*/

    if (wX11->xrender.handle)
    {
        _wManagerPlatformFreeModule(wX11->xrender.handle);
        wX11->xrender.handle = NULL;
    }

    /*if (wX11->vidmode.handle)
    {
        _wManagerPlatformFreeModule(wX11->vidmode.handle);
        wX11->vidmode.handle = NULL;
    }*/

    if (wX11->xi.handle)
    {
        _wManagerPlatformFreeModule(wX11->xi.handle);
        wX11->xi.handle = NULL;
    }

    /*_glfwTerminateOSMesa();
    // NOTE: These need to be unloaded after XCloseDisplay, as they register
    //       cleanup callbacks that get called by that function
    _glfwTerminateEGL();
    _glfwTerminateGLX();*/

    if (wX11->xlib.handle)
    {
        _wManagerPlatformFreeModule(wX11->xlib.handle);
        wX11->xlib.handle = NULL;
    }

    if (wX11->emptyEventPipe[0] || wX11->emptyEventPipe[1])
    {
        close(wX11->emptyEventPipe[0]);
        close(wX11->emptyEventPipe[1]);
    }

    free(_wMWindow.WindowData);
}

extern void _wManagerGetCursorPosX11(wManagerWindow* window, double* xpos, double* ypos);
extern void _wManagerSetCursorPosX11(wManagerWindow* window, double x, double y);
extern void _wManagerSetCursorModeX11(wManagerWindow* window, int mode);
extern void _wManagerSetRawMouseMotionX11(wManagerWindow *window, uint32_t enabled);
extern uint32_t _wManagerRawMouseMotionSupportedX11(void);
extern const char* _wManagerGetScancodeNameX11(int scancode);
extern int _wManagerGetKeyScancodeX11(int key);
extern uint32_t _wManagerCreateWindowX11(wManagerWindow* window,
                              const _wManagerwndconfig* wndconfig,
                              const _wManagerctxconfig* ctxconfig,
                              const _wManagerfbconfig* fbconfig);
extern void _wManagerDestroyWindowX11(wManagerWindow* window);
extern void _wManagerSetWindowTitleX11(wManagerWindow* window, const char* title);
extern void _wManagerGetWindowPosX11(wManagerWindow* window, int* xpos, int* ypos);
extern void _wManagerSetWindowPosX11(wManagerWindow* window, int xpos, int ypos);
extern void _wManagerGetWindowSizeX11(wManagerWindow* window, int* width, int* height);
extern void _wManagerSetWindowSizeX11(wManagerWindow* window, int width, int height);
extern void _wManagerGetWindowSizeX11(wManagerWindow* window, int* width, int* height);
extern void _wManagerSetWindowSizeLimitsX11(wManagerWindow* window,
                                            int minwidth, int minheight,
                                            int maxwidth, int maxheight);
extern void _wManagerSetWindowAspectRatioX11(wManagerWindow* window, int numer, int denom);
extern void _wManagerGetFramebufferSizeX11(wManagerWindow* window, int* width, int* height);
extern void _wManagerGetWindowFrameSizeX11(wManagerWindow* window,
                                           int* left, int* top,
                                           int* right, int* bottom);
extern void _wManagerGetWindowContentScaleX11(wManagerWindow* window, float* xscale, float* yscale);
extern void _wManagerIconifyWindowX11(wManagerWindow* window);
extern void _wManagerRestoreWindowX11(wManagerWindow* window);
extern void _wManagerMaximizeWindowX11(wManagerWindow* window);
extern void _wManagerShowWindowX11(wManagerWindow* window);
extern void _wManagerHideWindowX11(wManagerWindow* window);
extern void _wManagerRequestWindowAttentionX11(wManagerWindow* window);
extern void _wManagerFocusWindowX11(wManagerWindow* window);
extern void _wManagerSetWindowMonitorX11(wManagerWindow* window,
                                         void* monitor,
                                         int xpos, int ypos,
                                         int width, int height,
                                         int refreshRate);
extern uint32_t _wManagerWindowFocusedX11(wManagerWindow* window);
extern uint32_t _wManagerWindowIconifiedX11(wManagerWindow* window);
extern uint32_t _wManagerWindowVisibleX11(wManagerWindow* window);
extern uint32_t _wManagerWindowMaximizedX11(wManagerWindow* window);
extern uint32_t _wManagerWindowHoveredX11(wManagerWindow* window);
extern uint32_t _wManagerFramebufferTransparentX11(wManagerWindow* window);
extern void _wManagerSetWindowResizableX11(wManagerWindow* window, uint32_t enabled);
extern void _wManagerSetWindowDecoratedX11(wManagerWindow* window, uint32_t enabled);
extern void _wManagerSetWindowFloatingX11(wManagerWindow* window, uint32_t enabled);
extern void _wManagerSetWindowMousePassthroughX11(wManagerWindow* window, uint32_t enabled);
extern float _wManagerGetWindowOpacityX11(wManagerWindow* window);
extern void _wManagerSetWindowOpacityX11(wManagerWindow* window, float opacity);
extern void _wManagerPollEventsX11(void);
extern void _wManagerWaitEventsX11(void);
extern void _wManagerWaitEventsTimeoutX11(double timeout);
extern void _wManagerPostEmptyEventX11(void);
extern void _wManagerGetRequiredInstanceExtensionsX11(char** extensions);
extern uint32_t _wManagerGetPhysicalDevicePresentationSupportX11(VkInstance instance,
                                                             VkPhysicalDevice device,
                                                             uint32_t queuefamily);
extern VkResult _wManagerCreateWindowSurfaceX11(VkInstance instance,
                                                wManagerWindow* window,
                                                const VkAllocationCallbacks* allocator,
                                                VkSurfaceKHR* surface);

uint32_t _wManagerConnectX11(_wManagerPlatform* platform){

    const _wManagerPlatform x11 =
        {
            .platformID = 1,
            .init = _wManagerInitX11,
            .terminate = _wManagerTerminateX11,
            .getCursorPos = _wManagerGetCursorPosX11,
            .setCursorPos = _wManagerSetCursorPosX11,
            .setCursorMode = _wManagerSetCursorModeX11,
            .setRawMouseMotion = _wManagerSetRawMouseMotionX11,
            .rawMouseMotionSupported = _wManagerRawMouseMotionSupportedX11,
            //.createCursor = NULL,//_wManagerCreateCursorX11,
            //.createStandardCursor = NULL,//_wManagerCreateStandardCursorX11,
            //.destroyCursor = NULL,//_wManagerDestroyCursorX11,
            //.setCursor = NULL,//_wManagerSetCursorX11,
            .getScancodeName = _wManagerGetScancodeNameX11,
            .getKeyScancode = _wManagerGetKeyScancodeX11,
            .setClipboardString = NULL,//_wManagerSetClipboardStringX11,
            .getClipboardString = NULL,//_wManagerGetClipboardStringX11,
    /*#if defined(GLFW_BUILD_LINUX_JOYSTICK)
            .initJoysticks = _wManagerInitJoysticksLinux,
            .terminateJoysticks = _wManagerTerminateJoysticksLinux,
            .pollJoystick = _wManagerPollJoystickLinux,
            .getMappingName = _wManagerGetMappingNameLinux,
            .updateGamepadGUID = _wManagerUpdateGamepadGUIDLinux,
    #else
            .initJoysticks = _wManagerInitJoysticksNull,
            .terminateJoysticks = _wManagerTerminateJoysticksNull,
            .pollJoystick = _wManagerPollJoystickNull,
            .getMappingName = _wManagerGetMappingNameNull,
            .updateGamepadGUID = _wManagerUpdateGamepadGUIDNull,
    #endif*/
            /*.freeMonitor = _wManagerFreeMonitorX11,
            .getMonitorPos = _wManagerGetMonitorPosX11,
            .getMonitorContentScale = _wManagerGetMonitorContentScaleX11,
            .getMonitorWorkarea = _wManagerGetMonitorWorkareaX11,
            .getVideoModes = _wManagerGetVideoModesX11,
            .getVideoMode = _wManagerGetVideoModeX11,
            .getGammaRamp = _wManagerGetGammaRampX11,
            .setGammaRamp = _wManagerSetGammaRampX11,*/
            .createWindow = _wManagerCreateWindowX11,
            .destroyWindow = _wManagerDestroyWindowX11,
            .setWindowTitle = _wManagerSetWindowTitleX11,
            //.setWindowIcon = _wManagerSetWindowIconX11,
            .getWindowPos = _wManagerGetWindowPosX11,
            .setWindowPos = _wManagerSetWindowPosX11,
            .getWindowSize = _wManagerGetWindowSizeX11,
            .setWindowSize = _wManagerSetWindowSizeX11,
            .setWindowSizeLimits = _wManagerSetWindowSizeLimitsX11,
            .setWindowAspectRatio = _wManagerSetWindowAspectRatioX11,
            .getFramebufferSize = _wManagerGetFramebufferSizeX11,
            .getWindowFrameSize = _wManagerGetWindowFrameSizeX11,
            .getWindowContentScale = _wManagerGetWindowContentScaleX11,
            .iconifyWindow = _wManagerIconifyWindowX11,
            .restoreWindow = _wManagerRestoreWindowX11,
            .maximizeWindow = _wManagerMaximizeWindowX11,
            .showWindow = _wManagerShowWindowX11,
            .hideWindow = _wManagerHideWindowX11,
            .requestWindowAttention = _wManagerRequestWindowAttentionX11,
            .focusWindow = _wManagerFocusWindowX11,
            //.setWindowMonitor = _wManagerSetWindowMonitorX11,
            .windowFocused = _wManagerWindowFocusedX11,
            .windowIconified = _wManagerWindowIconifiedX11,
            .windowVisible = _wManagerWindowVisibleX11,
            .windowMaximized = _wManagerWindowMaximizedX11,
            .windowHovered = _wManagerWindowHoveredX11,
            .framebufferTransparent = _wManagerFramebufferTransparentX11,
            .getWindowOpacity = _wManagerGetWindowOpacityX11,
            .setWindowResizable = _wManagerSetWindowResizableX11,
            .setWindowDecorated = _wManagerSetWindowDecoratedX11,
            .setWindowFloating = _wManagerSetWindowFloatingX11,
            .setWindowOpacity = _wManagerSetWindowOpacityX11,
            .setWindowMousePassthrough = _wManagerSetWindowMousePassthroughX11,
            .pollEvents = _wManagerPollEventsX11,
            .waitEvents = _wManagerWaitEventsX11,
            .waitEventsTimeout = _wManagerWaitEventsTimeoutX11,
            .postEmptyEvent = _wManagerPostEmptyEventX11,
            /*.getEGLPlatform = _wManagerGetEGLPlatformX11,
            .getEGLNativeDisplay = _wManagerGetEGLNativeDisplayX11,
            .getEGLNativeWindow = _wManagerGetEGLNativeWindowX11,*/
            .getRequiredInstanceExtensions = _wManagerGetRequiredInstanceExtensionsX11,
            .getPhysicalDevicePresentationSupport = _wManagerGetPhysicalDevicePresentationSupportX11,
            .createWindowSurface = _wManagerCreateWindowSurfaceX11
        };

    void* module = _wManagerPlatformLoadModule("libX11.so.6");

    if (!module)
    {
        printf("X11: Failed to load Xlib\n");

        return false;
    }

    PFN_XInitThreads XInitThreads = (PFN_XInitThreads)
        _wManagerPlatformGetModuleSymbol(module, "XInitThreads");
    PFN_XrmInitialize XrmInitialize = (PFN_XrmInitialize)
        _wManagerPlatformGetModuleSymbol(module, "XrmInitialize");
    PFN_XOpenDisplay XOpenDisplay = (PFN_XOpenDisplay)
        _wManagerPlatformGetModuleSymbol(module, "XOpenDisplay");
    if (!XInitThreads || !XrmInitialize || !XOpenDisplay)
    {
        printf("X11: Failed to load Xlib entry point\n");

        _wManagerPlatformFreeModule(module);
        return false;
    }

    XInitThreads();
    XrmInitialize();

    Display* display = XOpenDisplay(NULL);
    if (!display)
    {
        printf("X11: The DISPLAY environment variable is missing\n");

        return false;
    }

    wManagerX11 *wX11 = _wMWindow.WindowData;

    wX11->display = display;
    wX11->xlib.handle = module;

    *platform = x11;
    return true;
}

#endif

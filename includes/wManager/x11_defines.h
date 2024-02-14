#ifndef X11_DEFINES_H
#define X11_DEFINES_H

#ifdef _X11_
    #include <X11/Xlib.h>
    #include <X11/Xmd.h>
    #include <X11/keysym.h>
    #include <X11/Xatom.h>
    #include <X11/Xresource.h>
    #include <X11/Xcursor/Xcursor.h>

    #include <fcntl.h>
    #include <poll.h>
    #include <limits.h>
    #include <signal.h>
    #include <time.h>
    #include <errno.h>

    // The XRandR extension provides mode setting and gamma control
    #include <X11/extensions/Xrandr.h>

    // The Xkb extension provides improved keyboard support
    #include <X11/XKBlib.h>

    // The Xinerama extension provides legacy monitor indices
    #include <X11/extensions/Xinerama.h>

    // The XInput extension provides raw mouse motion input
    #include <X11/extensions/XInput2.h>

    // The Shape extension provides custom window shapes
    #include <X11/extensions/shape.h>

    #include <vulkan/vulkan.h>

    #include <stdint.h>
    #include <stdbool.h>

    typedef struct wManagerWindow wManagerWindow;

    typedef Bool (* PFN_XRenderQueryExtension)(Display*,int*,int*);
    typedef Status (* PFN_XRenderQueryVersion)(Display*dpy,int*,int*);
    typedef XRenderPictFormat* (* PFN_XRenderFindVisualFormat)(Display*,Visual const*);

    typedef Bool (* PFN_XShapeQueryExtension)(Display*,int*,int*);
    typedef Status (* PFN_XShapeQueryVersion)(Display*dpy,int*,int*);
    typedef void (* PFN_XShapeCombineRegion)(Display*,Window,int,int,int,Region,int);
    typedef void (* PFN_XShapeCombineMask)(Display*,Window,int,int,int,Pixmap,int);

    typedef XClassHint* (* PFN_XAllocClassHint)(void);
    typedef XSizeHints* (* PFN_XAllocSizeHints)(void);
    typedef XWMHints* (* PFN_XAllocWMHints)(void);
    typedef int (* PFN_XChangeProperty)(Display*,Window,Atom,Atom,int,int,const unsigned char*,int);
    typedef int (* PFN_XChangeWindowAttributes)(Display*,Window,unsigned long,XSetWindowAttributes*);
    typedef Bool (* PFN_XCheckIfEvent)(Display*,XEvent*,Bool(*)(Display*,XEvent*,XPointer),XPointer);
    typedef Bool (* PFN_XCheckTypedWindowEvent)(Display*,Window,int,XEvent*);
    typedef int (* PFN_XCloseDisplay)(Display*);
    typedef Status (* PFN_XCloseIM)(XIM);
    typedef int (* PFN_XConvertSelection)(Display*,Atom,Atom,Atom,Window,Time);
    typedef Colormap (* PFN_XCreateColormap)(Display*,Window,Visual*,int);
    typedef Cursor (* PFN_XCreateFontCursor)(Display*,unsigned int);
    typedef XIC (* PFN_XCreateIC)(XIM,...);
    typedef Region (* PFN_XCreateRegion)(void);
    typedef Window (* PFN_XCreateWindow)(Display*,Window,int,int,unsigned int,unsigned int,unsigned int,int,unsigned int,Visual*,unsigned long,XSetWindowAttributes*);
    typedef int (* PFN_XDefineCursor)(Display*,Window,Cursor);
    typedef int (* PFN_XDeleteContext)(Display*,XID,XContext);
    typedef int (* PFN_XDeleteProperty)(Display*,Window,Atom);
    typedef void (* PFN_XDestroyIC)(XIC);
    typedef int (* PFN_XDestroyRegion)(Region);
    typedef int (* PFN_XDestroyWindow)(Display*,Window);
    typedef int (* PFN_XDisplayKeycodes)(Display*,int*,int*);
    typedef int (* PFN_XEventsQueued)(Display*,int);
    typedef Bool (* PFN_XFilterEvent)(XEvent*,Window);
    typedef int (* PFN_XFindContext)(Display*,XID,XContext,XPointer*);
    typedef int (* PFN_XFlush)(Display*);
    typedef int (* PFN_XFree)(void*);
    typedef int (* PFN_XFreeColormap)(Display*,Colormap);
    typedef int (* PFN_XFreeCursor)(Display*,Cursor);
    typedef void (* PFN_XFreeEventData)(Display*,XGenericEventCookie*);
    typedef int (* PFN_XGetErrorText)(Display*,int,char*,int);
    typedef Bool (* PFN_XGetEventData)(Display*,XGenericEventCookie*);
    typedef char* (* PFN_XGetICValues)(XIC,...);
    typedef char* (* PFN_XGetIMValues)(XIM,...);
    typedef int (* PFN_XGetInputFocus)(Display*,Window*,int*);
    typedef KeySym* (* PFN_XGetKeyboardMapping)(Display*,KeyCode,int,int*);
    typedef int (* PFN_XGetScreenSaver)(Display*,int*,int*,int*,int*);
    typedef Window (* PFN_XGetSelectionOwner)(Display*,Atom);
    typedef XVisualInfo* (* PFN_XGetVisualInfo)(Display*,long,XVisualInfo*,int*);
    typedef Status (* PFN_XGetWMNormalHints)(Display*,Window,XSizeHints*,long*);
    typedef Status (* PFN_XGetWindowAttributes)(Display*,Window,XWindowAttributes*);
    typedef int (* PFN_XGetWindowProperty)(Display*,Window,Atom,long,long,Bool,Atom,Atom*,int*,unsigned long*,unsigned long*,unsigned char**);
    typedef int (* PFN_XGrabPointer)(Display*,Window,Bool,unsigned int,int,int,Window,Cursor,Time);
    typedef Status (* PFN_XIconifyWindow)(Display*,Window,int);
    typedef Status (* PFN_XInitThreads)(void);
    typedef Atom (* PFN_XInternAtom)(Display*,const char*,Bool);
    typedef int (* PFN_XLookupString)(XKeyEvent*,char*,int,KeySym*,XComposeStatus*);
    typedef int (* PFN_XMapRaised)(Display*,Window);
    typedef int (* PFN_XMapWindow)(Display*,Window);
    typedef int (* PFN_XMoveResizeWindow)(Display*,Window,int,int,unsigned int,unsigned int);
    typedef int (* PFN_XMoveWindow)(Display*,Window,int,int);
    typedef int (* PFN_XNextEvent)(Display*,XEvent*);
    typedef Display* (* PFN_XOpenDisplay)(const char*);
    typedef XIM (* PFN_XOpenIM)(Display*,XrmDatabase*,char*,char*);
    typedef int (* PFN_XPeekEvent)(Display*,XEvent*);
    typedef int (* PFN_XPending)(Display*);
    typedef Bool (* PFN_XQueryExtension)(Display*,const char*,int*,int*,int*);
    typedef Bool (* PFN_XQueryPointer)(Display*,Window,Window*,Window*,int*,int*,int*,int*,unsigned int*);
    typedef int (* PFN_XRaiseWindow)(Display*,Window);
    typedef Bool (* PFN_XRegisterIMInstantiateCallback)(Display*,void*,char*,char*,XIDProc,XPointer);
    typedef int (* PFN_XResizeWindow)(Display*,Window,unsigned int,unsigned int);
    typedef char* (* PFN_XResourceManagerString)(Display*);
    typedef int (* PFN_XSaveContext)(Display*,XID,XContext,const char*);
    typedef int (* PFN_XSelectInput)(Display*,Window,long);
    typedef Status (* PFN_XSendEvent)(Display*,Window,Bool,long,XEvent*);
    typedef int (* PFN_XSetClassHint)(Display*,Window,XClassHint*);
    typedef XErrorHandler (* PFN_XSetErrorHandler)(XErrorHandler);
    typedef void (* PFN_XSetICFocus)(XIC);
    typedef char* (* PFN_XSetIMValues)(XIM,...);
    typedef int (* PFN_XSetInputFocus)(Display*,Window,int,Time);
    typedef char* (* PFN_XSetLocaleModifiers)(const char*);
    typedef int (* PFN_XSetScreenSaver)(Display*,int,int,int,int);
    typedef int (* PFN_XSetSelectionOwner)(Display*,Atom,Window,Time);
    typedef int (* PFN_XSetWMHints)(Display*,Window,XWMHints*);
    typedef void (* PFN_XSetWMNormalHints)(Display*,Window,XSizeHints*);
    typedef Status (* PFN_XSetWMProtocols)(Display*,Window,Atom*,int);
    typedef Bool (* PFN_XSupportsLocale)(void);
    typedef int (* PFN_XSync)(Display*,Bool);
    typedef Bool (* PFN_XTranslateCoordinates)(Display*,Window,Window,int,int,int*,int*,Window*);
    typedef int (* PFN_XUndefineCursor)(Display*,Window);
    typedef int (* PFN_XUngrabPointer)(Display*,Time);
    typedef int (* PFN_XUnmapWindow)(Display*,Window);
    typedef void (* PFN_XUnsetICFocus)(XIC);
    typedef VisualID (* PFN_XVisualIDFromVisual)(Visual*);
    typedef int (* PFN_XWarpPointer)(Display*,Window,Window,int,int,unsigned int,unsigned int,int,int);
    typedef void (* PFN_XkbFreeKeyboard)(XkbDescPtr,unsigned int,Bool);
    typedef void (* PFN_XkbFreeNames)(XkbDescPtr,unsigned int,Bool);
    typedef XkbDescPtr (* PFN_XkbGetMap)(Display*,unsigned int,unsigned int);
    typedef Status (* PFN_XkbGetNames)(Display*,unsigned int,XkbDescPtr);
    typedef Status (* PFN_XkbGetState)(Display*,unsigned int,XkbStatePtr);
    typedef KeySym (* PFN_XkbKeycodeToKeysym)(Display*,KeyCode,int,int);
    typedef Bool (* PFN_XkbQueryExtension)(Display*,int*,int*,int*,int*,int*);
    typedef Bool (* PFN_XkbSelectEventDetails)(Display*,unsigned int,unsigned int,unsigned long,unsigned long);
    typedef Bool (* PFN_XkbSetDetectableAutoRepeat)(Display*,Bool,Bool*);
    typedef void (* PFN_XrmDestroyDatabase)(XrmDatabase);
    typedef Bool (* PFN_XrmGetResource)(XrmDatabase,const char*,const char*,char**,XrmValue*);
    typedef XrmDatabase (* PFN_XrmGetStringDatabase)(const char*);
    typedef void (* PFN_XrmInitialize)(void);
    typedef XrmQuark (* PFN_XrmUniqueQuark)(void);
    typedef Bool (* PFN_XUnregisterIMInstantiateCallback)(Display*,void*,char*,char*,XIDProc,XPointer);
    typedef int (* PFN_Xutf8LookupString)(XIC,XKeyPressedEvent*,char*,int,KeySym*,Status*);
    typedef void (* PFN_Xutf8SetWMProperties)(Display*,Window,const char*,const char*,char**,int,XSizeHints*,XWMHints*,XClassHint*);

    typedef XRRCrtcGamma* (* PFN_XRRAllocGamma)(int);
    typedef void (* PFN_XRRFreeCrtcInfo)(XRRCrtcInfo*);
    typedef void (* PFN_XRRFreeGamma)(XRRCrtcGamma*);
    typedef void (* PFN_XRRFreeOutputInfo)(XRROutputInfo*);
    typedef void (* PFN_XRRFreeScreenResources)(XRRScreenResources*);
    typedef XRRCrtcGamma* (* PFN_XRRGetCrtcGamma)(Display*,RRCrtc);
    typedef int (* PFN_XRRGetCrtcGammaSize)(Display*,RRCrtc);
    typedef XRRCrtcInfo* (* PFN_XRRGetCrtcInfo) (Display*,XRRScreenResources*,RRCrtc);
    typedef XRROutputInfo* (* PFN_XRRGetOutputInfo)(Display*,XRRScreenResources*,RROutput);
    typedef RROutput (* PFN_XRRGetOutputPrimary)(Display*,Window);
    typedef XRRScreenResources* (* PFN_XRRGetScreenResourcesCurrent)(Display*,Window);
    typedef Bool (* PFN_XRRQueryExtension)(Display*,int*,int*);
    typedef Status (* PFN_XRRQueryVersion)(Display*,int*,int*);
    typedef void (* PFN_XRRSelectInput)(Display*,Window,int);
    typedef Status (* PFN_XRRSetCrtcConfig)(Display*,XRRScreenResources*,RRCrtc,Time,int,int,RRMode,Rotation,RROutput*,int);
    typedef void (* PFN_XRRSetCrtcGamma)(Display*,RRCrtc,XRRCrtcGamma*);
    typedef int (* PFN_XRRUpdateConfiguration)(XEvent*);


    typedef Status (* PFN_XIQueryVersion)(Display*,int*,int*);
    typedef int (* PFN_XISelectEvents)(Display*,Window,XIEventMask*,int);

    typedef struct xcb_connection_t xcb_connection_t;
    typedef xcb_connection_t* (* PFN_XGetXCBConnection)(Display*);

    typedef Status (* PFN_XInitThreads)(void);
    typedef void (* PFN_XrmInitialize)(void);
    typedef Display* (* PFN_XOpenDisplay)(const char*);

    typedef struct wManagerX11{
        Colormap        colormap;
        Window          handle;
        Window          parent;
        XIC             ic;

        Display*        display;

        uint32_t        overrideRedirect;
        uint32_t        iconified;
        uint32_t        maximized;

        // Whether the visual supports framebuffer transparency
        uint32_t        transparent;

        // Cached position and size used to filter out duplicate events
        int             width, height;
        int             xpos, ypos;

        // The last received cursor position, regardless of source
        int             lastCursorPosX, lastCursorPosY;
        // The last position the cursor was warped to by GLFW
        int             warpCursorPosX, warpCursorPosY;

        // The time of the last KeyPress event per keycode, for discarding
        // duplicate key events generated for some keys by ibus
        Time            keyPressTimes[256];

        int             screen;
        Window          root;

        // System content scale
        float           contentScaleX, contentScaleY;
        // Helper window for IPC
        Window          helperWindowHandle;
        // Invisible cursor for hidden cursor mode
        Cursor          hiddenCursorHandle;
        // Context for mapping window XIDs to _GLFWwindow pointers
        XContext        context;
        // XIM input method
        XIM             im;
        // The previous X error handler, to be restored later
        XErrorHandler   errorHandler;
        // Most recent error code received by X error handler
        int             errorCode;
        // Primary selection string (while the primary selection is owned)
        char*           primarySelectionString;
        // Clipboard string (while the selection is owned)
        char*           clipboardString;
        // Key name string
        char            keynames[ENGINE_KEY_LAST + 1][5];
        // X11 keycode to GLFW key LUT
        short int       keycodes[256];
        // GLFW key to X11 keycode LUT
        short int       scancodes[ENGINE_KEY_LAST + 1];
        // Where to place the cursor when re-enabled
        double          restoreCursorPosX, restoreCursorPosY;
        // The window whose disabled cursor mode is active
        wManagerWindow*    disabledCursorWindow;
        int             emptyEventPipe[2];

        // Window manager atoms
        Atom            NET_SUPPORTED;
        Atom            NET_SUPPORTING_WM_CHECK;
        Atom            WM_PROTOCOLS;
        Atom            WM_STATE;
        Atom            WM_DELETE_WINDOW;
        Atom            NET_WM_NAME;
        Atom            NET_WM_ICON_NAME;
        Atom            NET_WM_ICON;
        Atom            NET_WM_PID;
        Atom            NET_WM_PING;
        Atom            NET_WM_WINDOW_TYPE;
        Atom            NET_WM_WINDOW_TYPE_NORMAL;
        Atom            NET_WM_STATE;
        Atom            NET_WM_STATE_ABOVE;
        Atom            NET_WM_STATE_FULLSCREEN;
        Atom            NET_WM_STATE_MAXIMIZED_VERT;
        Atom            NET_WM_STATE_MAXIMIZED_HORZ;
        Atom            NET_WM_STATE_DEMANDS_ATTENTION;
        Atom            NET_WM_BYPASS_COMPOSITOR;
        Atom            NET_WM_FULLSCREEN_MONITORS;
        Atom            NET_WM_WINDOW_OPACITY;
        Atom            NET_WM_CM_Sx;
        Atom            NET_WORKAREA;
        Atom            NET_CURRENT_DESKTOP;
        Atom            NET_ACTIVE_WINDOW;
        Atom            NET_FRAME_EXTENTS;
        Atom            NET_REQUEST_FRAME_EXTENTS;
        Atom            MOTIF_WM_HINTS;

        // Xdnd (drag and drop) atoms
        Atom            XdndAware;
        Atom            XdndEnter;
        Atom            XdndPosition;
        Atom            XdndStatus;
        Atom            XdndActionCopy;
        Atom            XdndDrop;
        Atom            XdndFinished;
        Atom            XdndSelection;
        Atom            XdndTypeList;
        Atom            text_uri_list;

        // Selection (clipboard) atoms
        Atom            TARGETS;
        Atom            MULTIPLE;
        Atom            INCR;
        Atom            CLIPBOARD;
        Atom            PRIMARY;
        Atom            CLIPBOARD_MANAGER;
        Atom            SAVE_TARGETS;
        Atom            NULL_;
        Atom            UTF8_STRING;
        Atom            COMPOUND_STRING;
        Atom            ATOM_PAIR;
        Atom            ENGINE_SELECTION;


        struct {
            void*       handle;
            uint32_t    utf8;
            PFN_XGetXCBConnection GetXCBConnection;
            PFN_XAllocClassHint AllocClassHint;
            PFN_XAllocSizeHints AllocSizeHints;
            PFN_XAllocWMHints AllocWMHints;
            PFN_XChangeProperty ChangeProperty;
            PFN_XChangeWindowAttributes ChangeWindowAttributes;
            PFN_XCheckIfEvent CheckIfEvent;
            PFN_XCheckTypedWindowEvent CheckTypedWindowEvent;
            PFN_XCloseDisplay CloseDisplay;
            PFN_XCloseIM CloseIM;
            PFN_XConvertSelection ConvertSelection;
            PFN_XCreateColormap CreateColormap;
            PFN_XCreateFontCursor CreateFontCursor;
            PFN_XCreateIC CreateIC;
            PFN_XCreateRegion CreateRegion;
            PFN_XCreateWindow CreateWindow;
            PFN_XDefineCursor DefineCursor;
            PFN_XDeleteContext DeleteContext;
            PFN_XDeleteProperty DeleteProperty;
            PFN_XDestroyIC DestroyIC;
            PFN_XDestroyRegion DestroyRegion;
            PFN_XDestroyWindow DestroyWindow;
            PFN_XDisplayKeycodes DisplayKeycodes;
            PFN_XEventsQueued EventsQueued;
            PFN_XFilterEvent FilterEvent;
            PFN_XFindContext FindContext;
            PFN_XFlush Flush;
            PFN_XFree Free;
            PFN_XFreeColormap FreeColormap;
            PFN_XFreeCursor FreeCursor;
            PFN_XFreeEventData FreeEventData;
            PFN_XGetErrorText GetErrorText;
            PFN_XGetEventData GetEventData;
            PFN_XGetICValues GetICValues;
            PFN_XGetIMValues GetIMValues;
            PFN_XGetInputFocus GetInputFocus;
            PFN_XGetKeyboardMapping GetKeyboardMapping;
            PFN_XGetScreenSaver GetScreenSaver;
            PFN_XGetSelectionOwner GetSelectionOwner;
            PFN_XGetVisualInfo GetVisualInfo;
            PFN_XGetWMNormalHints GetWMNormalHints;
            PFN_XGetWindowAttributes GetWindowAttributes;
            PFN_XGetWindowProperty GetWindowProperty;
            PFN_XGrabPointer GrabPointer;
            PFN_XIconifyWindow IconifyWindow;
            PFN_XInternAtom InternAtom;
            PFN_XLookupString LookupString;
            PFN_XMapRaised MapRaised;
            PFN_XMapWindow MapWindow;
            PFN_XMoveResizeWindow MoveResizeWindow;
            PFN_XMoveWindow MoveWindow;
            PFN_XNextEvent NextEvent;
            PFN_XOpenIM OpenIM;
            PFN_XPeekEvent PeekEvent;
            PFN_XPending Pending;
            PFN_XQueryExtension QueryExtension;
            PFN_XQueryPointer QueryPointer;
            PFN_XRaiseWindow RaiseWindow;
            PFN_XRegisterIMInstantiateCallback RegisterIMInstantiateCallback;
            PFN_XResizeWindow ResizeWindow;
            PFN_XResourceManagerString ResourceManagerString;
            PFN_XSaveContext SaveContext;
            PFN_XSelectInput SelectInput;
            PFN_XSendEvent SendEvent;
            PFN_XSetClassHint SetClassHint;
            PFN_XSetErrorHandler SetErrorHandler;
            PFN_XSetICFocus SetICFocus;
            PFN_XSetIMValues SetIMValues;
            PFN_XSetInputFocus SetInputFocus;
            PFN_XSetLocaleModifiers SetLocaleModifiers;
            PFN_XSetScreenSaver SetScreenSaver;
            PFN_XSetSelectionOwner SetSelectionOwner;
            PFN_XSetWMHints SetWMHints;
            PFN_XSetWMNormalHints SetWMNormalHints;
            PFN_XSetWMProtocols SetWMProtocols;
            PFN_XSupportsLocale SupportsLocale;
            PFN_XSync Sync;
            PFN_XTranslateCoordinates TranslateCoordinates;
            PFN_XUndefineCursor UndefineCursor;
            PFN_XUngrabPointer UngrabPointer;
            PFN_XUnmapWindow UnmapWindow;
            PFN_XUnsetICFocus UnsetICFocus;
            PFN_XVisualIDFromVisual VisualIDFromVisual;
            PFN_XWarpPointer WarpPointer;
            PFN_XUnregisterIMInstantiateCallback UnregisterIMInstantiateCallback;
            PFN_Xutf8LookupString utf8LookupString;
            PFN_Xutf8SetWMProperties utf8SetWMProperties;
        } xlib;

        struct {
            PFN_XrmDestroyDatabase DestroyDatabase;
            PFN_XrmGetResource GetResource;
            PFN_XrmGetStringDatabase GetStringDatabase;
            PFN_XrmUniqueQuark UniqueQuark;
        } xrm;

        struct {
           int32_t    available;
           void*       handle;
           int         eventBase;
           int         errorBase;
           int         major;
           int         minor;
           int32_t    gammaBroken;
           int32_t    monitorBroken;
           PFN_XRRAllocGamma AllocGamma;
           PFN_XRRFreeCrtcInfo FreeCrtcInfo;
           PFN_XRRFreeGamma FreeGamma;
           PFN_XRRFreeOutputInfo FreeOutputInfo;
           PFN_XRRFreeScreenResources FreeScreenResources;
           PFN_XRRGetCrtcGamma GetCrtcGamma;
           PFN_XRRGetCrtcGammaSize GetCrtcGammaSize;
           PFN_XRRGetCrtcInfo GetCrtcInfo;
           PFN_XRRGetOutputInfo GetOutputInfo;
           PFN_XRRGetOutputPrimary GetOutputPrimary;
           PFN_XRRGetScreenResourcesCurrent GetScreenResourcesCurrent;
           PFN_XRRQueryExtension QueryExtension;
           PFN_XRRQueryVersion QueryVersion;
           PFN_XRRSelectInput SelectInput;
           PFN_XRRSetCrtcConfig SetCrtcConfig;
           PFN_XRRSetCrtcGamma SetCrtcGamma;
           PFN_XRRUpdateConfiguration UpdateConfiguration;
        }randr;

        struct {
            uint32_t     available;
            uint32_t     detectable;
            int          majorOpcode;
            int          eventBase;
            int          errorBase;
            int          major;
            int          minor;
            unsigned int group;
            PFN_XkbFreeKeyboard FreeKeyboard;
            PFN_XkbFreeNames FreeNames;
            PFN_XkbGetMap GetMap;
            PFN_XkbGetNames GetNames;
            PFN_XkbGetState GetState;
            PFN_XkbKeycodeToKeysym KeycodeToKeysym;
            PFN_XkbQueryExtension QueryExtension;
            PFN_XkbSelectEventDetails SelectEventDetails;
            PFN_XkbSetDetectableAutoRepeat SetDetectableAutoRepeat;
        } xkb;

        struct {
            int         version;
            Window      source;
            Atom        format;
        } xdnd;

        struct {
            uint32_t    available;
            void*       handle;
            int         majorOpcode;
            int         eventBase;
            int         errorBase;
            int         major;
            int         minor;
            PFN_XIQueryVersion QueryVersion;
            PFN_XISelectEvents SelectEvents;
        } xi;

        struct {
            uint32_t    available;
            void*       handle;
            int         major;
            int         minor;
            int         eventBase;
            int         errorBase;
            PFN_XRenderQueryExtension QueryExtension;
            PFN_XRenderQueryVersion QueryVersion;
            PFN_XRenderFindVisualFormat FindVisualFormat;
        } xrender;

        struct {
            void*       handle;
            PFN_XGetXCBConnection GetXCBConnection;
        } x11xcb;

        struct {
            uint32_t    available;
            void*       handle;
            int         major;
            int         minor;
            int         eventBase;
            int         errorBase;
            PFN_XShapeQueryExtension QueryExtension;
            PFN_XShapeCombineRegion ShapeCombineRegion;
            PFN_XShapeQueryVersion QueryVersion;
            PFN_XShapeCombineMask ShapeCombineMask;
        } xshape;

    } wManagerX11;

    typedef XID xcb_window_t;
    typedef XID xcb_visualid_t;

    typedef VkFlags VkXlibSurfaceCreateFlagsKHR;
    typedef VkFlags VkXcbSurfaceCreateFlagsKHR;
    typedef VkFlags VkWin32SurfaceCreateFlagsKHR;

    typedef struct VkXlibSurfaceCreateInfoKHR
    {
        VkStructureType             sType;
        const void*                 pNext;
        VkXlibSurfaceCreateFlagsKHR flags;
        Display*                    dpy;
        Window                      window;
    } VkXlibSurfaceCreateInfoKHR;

    typedef struct VkXcbSurfaceCreateInfoKHR
    {
        VkStructureType             sType;
        const void*                 pNext;
        VkXcbSurfaceCreateFlagsKHR  flags;
        xcb_connection_t*           connection;
        xcb_window_t                window;
    } VkXcbSurfaceCreateInfoKHR;

    typedef VkResult (APIENTRY *PFN_vkCreateXlibSurfaceKHR)(VkInstance,const VkXlibSurfaceCreateInfoKHR*,const VkAllocationCallbacks*,VkSurfaceKHR*);
    typedef VkBool32 (APIENTRY *PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR)(VkPhysicalDevice,uint32_t,Display*,VisualID);
    typedef VkResult (APIENTRY *PFN_vkCreateXcbSurfaceKHR)(VkInstance,const VkXcbSurfaceCreateInfoKHR*,const VkAllocationCallbacks*,VkSurfaceKHR*);
    typedef VkBool32 (APIENTRY *PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR)(VkPhysicalDevice,uint32_t,xcb_connection_t*,xcb_visualid_t);


static const struct codepair {
  unsigned short keysym;
  unsigned short ucs;
} keysymtab[] = {
  { 0x01a1, 0x0104 },
  { 0x01a2, 0x02d8 },
  { 0x01a3, 0x0141 },
  { 0x01a5, 0x013d },
  { 0x01a6, 0x015a },
  { 0x01a9, 0x0160 },
  { 0x01aa, 0x015e },
  { 0x01ab, 0x0164 },
  { 0x01ac, 0x0179 },
  { 0x01ae, 0x017d },
  { 0x01af, 0x017b },
  { 0x01b1, 0x0105 },
  { 0x01b2, 0x02db },
  { 0x01b3, 0x0142 },
  { 0x01b5, 0x013e },
  { 0x01b6, 0x015b },
  { 0x01b7, 0x02c7 },
  { 0x01b9, 0x0161 },
  { 0x01ba, 0x015f },
  { 0x01bb, 0x0165 },
  { 0x01bc, 0x017a },
  { 0x01bd, 0x02dd },
  { 0x01be, 0x017e },
  { 0x01bf, 0x017c },
  { 0x01c0, 0x0154 },
  { 0x01c3, 0x0102 },
  { 0x01c5, 0x0139 },
  { 0x01c6, 0x0106 },
  { 0x01c8, 0x010c },
  { 0x01ca, 0x0118 },
  { 0x01cc, 0x011a },
  { 0x01cf, 0x010e },
  { 0x01d0, 0x0110 },
  { 0x01d1, 0x0143 },
  { 0x01d2, 0x0147 },
  { 0x01d5, 0x0150 },
  { 0x01d8, 0x0158 },
  { 0x01d9, 0x016e },
  { 0x01db, 0x0170 },
  { 0x01de, 0x0162 },
  { 0x01e0, 0x0155 },
  { 0x01e3, 0x0103 },
  { 0x01e5, 0x013a },
  { 0x01e6, 0x0107 },
  { 0x01e8, 0x010d },
  { 0x01ea, 0x0119 },
  { 0x01ec, 0x011b },
  { 0x01ef, 0x010f },
  { 0x01f0, 0x0111 },
  { 0x01f1, 0x0144 },
  { 0x01f2, 0x0148 },
  { 0x01f5, 0x0151 },
  { 0x01f8, 0x0159 },
  { 0x01f9, 0x016f },
  { 0x01fb, 0x0171 },
  { 0x01fe, 0x0163 },
  { 0x01ff, 0x02d9 },
  { 0x02a1, 0x0126 },
  { 0x02a6, 0x0124 },
  { 0x02a9, 0x0130 },
  { 0x02ab, 0x011e },
  { 0x02ac, 0x0134 },
  { 0x02b1, 0x0127 },
  { 0x02b6, 0x0125 },
  { 0x02b9, 0x0131 },
  { 0x02bb, 0x011f },
  { 0x02bc, 0x0135 },
  { 0x02c5, 0x010a },
  { 0x02c6, 0x0108 },
  { 0x02d5, 0x0120 },
  { 0x02d8, 0x011c },
  { 0x02dd, 0x016c },
  { 0x02de, 0x015c },
  { 0x02e5, 0x010b },
  { 0x02e6, 0x0109 },
  { 0x02f5, 0x0121 },
  { 0x02f8, 0x011d },
  { 0x02fd, 0x016d },
  { 0x02fe, 0x015d },
  { 0x03a2, 0x0138 },
  { 0x03a3, 0x0156 },
  { 0x03a5, 0x0128 },
  { 0x03a6, 0x013b },
  { 0x03aa, 0x0112 },
  { 0x03ab, 0x0122 },
  { 0x03ac, 0x0166 },
  { 0x03b3, 0x0157 },
  { 0x03b5, 0x0129 },
  { 0x03b6, 0x013c },
  { 0x03ba, 0x0113 },
  { 0x03bb, 0x0123 },
  { 0x03bc, 0x0167 },
  { 0x03bd, 0x014a },
  { 0x03bf, 0x014b },
  { 0x03c0, 0x0100 },
  { 0x03c7, 0x012e },
  { 0x03cc, 0x0116 },
  { 0x03cf, 0x012a },
  { 0x03d1, 0x0145 },
  { 0x03d2, 0x014c },
  { 0x03d3, 0x0136 },
  { 0x03d9, 0x0172 },
  { 0x03dd, 0x0168 },
  { 0x03de, 0x016a },
  { 0x03e0, 0x0101 },
  { 0x03e7, 0x012f },
  { 0x03ec, 0x0117 },
  { 0x03ef, 0x012b },
  { 0x03f1, 0x0146 },
  { 0x03f2, 0x014d },
  { 0x03f3, 0x0137 },
  { 0x03f9, 0x0173 },
  { 0x03fd, 0x0169 },
  { 0x03fe, 0x016b },
  { 0x047e, 0x203e },
  { 0x04a1, 0x3002 },
  { 0x04a2, 0x300c },
  { 0x04a3, 0x300d },
  { 0x04a4, 0x3001 },
  { 0x04a5, 0x30fb },
  { 0x04a6, 0x30f2 },
  { 0x04a7, 0x30a1 },
  { 0x04a8, 0x30a3 },
  { 0x04a9, 0x30a5 },
  { 0x04aa, 0x30a7 },
  { 0x04ab, 0x30a9 },
  { 0x04ac, 0x30e3 },
  { 0x04ad, 0x30e5 },
  { 0x04ae, 0x30e7 },
  { 0x04af, 0x30c3 },
  { 0x04b0, 0x30fc },
  { 0x04b1, 0x30a2 },
  { 0x04b2, 0x30a4 },
  { 0x04b3, 0x30a6 },
  { 0x04b4, 0x30a8 },
  { 0x04b5, 0x30aa },
  { 0x04b6, 0x30ab },
  { 0x04b7, 0x30ad },
  { 0x04b8, 0x30af },
  { 0x04b9, 0x30b1 },
  { 0x04ba, 0x30b3 },
  { 0x04bb, 0x30b5 },
  { 0x04bc, 0x30b7 },
  { 0x04bd, 0x30b9 },
  { 0x04be, 0x30bb },
  { 0x04bf, 0x30bd },
  { 0x04c0, 0x30bf },
  { 0x04c1, 0x30c1 },
  { 0x04c2, 0x30c4 },
  { 0x04c3, 0x30c6 },
  { 0x04c4, 0x30c8 },
  { 0x04c5, 0x30ca },
  { 0x04c6, 0x30cb },
  { 0x04c7, 0x30cc },
  { 0x04c8, 0x30cd },
  { 0x04c9, 0x30ce },
  { 0x04ca, 0x30cf },
  { 0x04cb, 0x30d2 },
  { 0x04cc, 0x30d5 },
  { 0x04cd, 0x30d8 },
  { 0x04ce, 0x30db },
  { 0x04cf, 0x30de },
  { 0x04d0, 0x30df },
  { 0x04d1, 0x30e0 },
  { 0x04d2, 0x30e1 },
  { 0x04d3, 0x30e2 },
  { 0x04d4, 0x30e4 },
  { 0x04d5, 0x30e6 },
  { 0x04d6, 0x30e8 },
  { 0x04d7, 0x30e9 },
  { 0x04d8, 0x30ea },
  { 0x04d9, 0x30eb },
  { 0x04da, 0x30ec },
  { 0x04db, 0x30ed },
  { 0x04dc, 0x30ef },
  { 0x04dd, 0x30f3 },
  { 0x04de, 0x309b },
  { 0x04df, 0x309c },
  { 0x05ac, 0x060c },
  { 0x05bb, 0x061b },
  { 0x05bf, 0x061f },
  { 0x05c1, 0x0621 },
  { 0x05c2, 0x0622 },
  { 0x05c3, 0x0623 },
  { 0x05c4, 0x0624 },
  { 0x05c5, 0x0625 },
  { 0x05c6, 0x0626 },
  { 0x05c7, 0x0627 },
  { 0x05c8, 0x0628 },
  { 0x05c9, 0x0629 },
  { 0x05ca, 0x062a },
  { 0x05cb, 0x062b },
  { 0x05cc, 0x062c },
  { 0x05cd, 0x062d },
  { 0x05ce, 0x062e },
  { 0x05cf, 0x062f },
  { 0x05d0, 0x0630 },
  { 0x05d1, 0x0631 },
  { 0x05d2, 0x0632 },
  { 0x05d3, 0x0633 },
  { 0x05d4, 0x0634 },
  { 0x05d5, 0x0635 },
  { 0x05d6, 0x0636 },
  { 0x05d7, 0x0637 },
  { 0x05d8, 0x0638 },
  { 0x05d9, 0x0639 },
  { 0x05da, 0x063a },
  { 0x05e0, 0x0640 },
  { 0x05e1, 0x0641 },
  { 0x05e2, 0x0642 },
  { 0x05e3, 0x0643 },
  { 0x05e4, 0x0644 },
  { 0x05e5, 0x0645 },
  { 0x05e6, 0x0646 },
  { 0x05e7, 0x0647 },
  { 0x05e8, 0x0648 },
  { 0x05e9, 0x0649 },
  { 0x05ea, 0x064a },
  { 0x05eb, 0x064b },
  { 0x05ec, 0x064c },
  { 0x05ed, 0x064d },
  { 0x05ee, 0x064e },
  { 0x05ef, 0x064f },
  { 0x05f0, 0x0650 },
  { 0x05f1, 0x0651 },
  { 0x05f2, 0x0652 },
  { 0x06a1, 0x0452 },
  { 0x06a2, 0x0453 },
  { 0x06a3, 0x0451 },
  { 0x06a4, 0x0454 },
  { 0x06a5, 0x0455 },
  { 0x06a6, 0x0456 },
  { 0x06a7, 0x0457 },
  { 0x06a8, 0x0458 },
  { 0x06a9, 0x0459 },
  { 0x06aa, 0x045a },
  { 0x06ab, 0x045b },
  { 0x06ac, 0x045c },
  { 0x06ae, 0x045e },
  { 0x06af, 0x045f },
  { 0x06b0, 0x2116 },
  { 0x06b1, 0x0402 },
  { 0x06b2, 0x0403 },
  { 0x06b3, 0x0401 },
  { 0x06b4, 0x0404 },
  { 0x06b5, 0x0405 },
  { 0x06b6, 0x0406 },
  { 0x06b7, 0x0407 },
  { 0x06b8, 0x0408 },
  { 0x06b9, 0x0409 },
  { 0x06ba, 0x040a },
  { 0x06bb, 0x040b },
  { 0x06bc, 0x040c },
  { 0x06be, 0x040e },
  { 0x06bf, 0x040f },
  { 0x06c0, 0x044e },
  { 0x06c1, 0x0430 },
  { 0x06c2, 0x0431 },
  { 0x06c3, 0x0446 },
  { 0x06c4, 0x0434 },
  { 0x06c5, 0x0435 },
  { 0x06c6, 0x0444 },
  { 0x06c7, 0x0433 },
  { 0x06c8, 0x0445 },
  { 0x06c9, 0x0438 },
  { 0x06ca, 0x0439 },
  { 0x06cb, 0x043a },
  { 0x06cc, 0x043b },
  { 0x06cd, 0x043c },
  { 0x06ce, 0x043d },
  { 0x06cf, 0x043e },
  { 0x06d0, 0x043f },
  { 0x06d1, 0x044f },
  { 0x06d2, 0x0440 },
  { 0x06d3, 0x0441 },
  { 0x06d4, 0x0442 },
  { 0x06d5, 0x0443 },
  { 0x06d6, 0x0436 },
  { 0x06d7, 0x0432 },
  { 0x06d8, 0x044c },
  { 0x06d9, 0x044b },
  { 0x06da, 0x0437 },
  { 0x06db, 0x0448 },
  { 0x06dc, 0x044d },
  { 0x06dd, 0x0449 },
  { 0x06de, 0x0447 },
  { 0x06df, 0x044a },
  { 0x06e0, 0x042e },
  { 0x06e1, 0x0410 },
  { 0x06e2, 0x0411 },
  { 0x06e3, 0x0426 },
  { 0x06e4, 0x0414 },
  { 0x06e5, 0x0415 },
  { 0x06e6, 0x0424 },
  { 0x06e7, 0x0413 },
  { 0x06e8, 0x0425 },
  { 0x06e9, 0x0418 },
  { 0x06ea, 0x0419 },
  { 0x06eb, 0x041a },
  { 0x06ec, 0x041b },
  { 0x06ed, 0x041c },
  { 0x06ee, 0x041d },
  { 0x06ef, 0x041e },
  { 0x06f0, 0x041f },
  { 0x06f1, 0x042f },
  { 0x06f2, 0x0420 },
  { 0x06f3, 0x0421 },
  { 0x06f4, 0x0422 },
  { 0x06f5, 0x0423 },
  { 0x06f6, 0x0416 },
  { 0x06f7, 0x0412 },
  { 0x06f8, 0x042c },
  { 0x06f9, 0x042b },
  { 0x06fa, 0x0417 },
  { 0x06fb, 0x0428 },
  { 0x06fc, 0x042d },
  { 0x06fd, 0x0429 },
  { 0x06fe, 0x0427 },
  { 0x06ff, 0x042a },
  { 0x07a1, 0x0386 },
  { 0x07a2, 0x0388 },
  { 0x07a3, 0x0389 },
  { 0x07a4, 0x038a },
  { 0x07a5, 0x03aa },
  { 0x07a7, 0x038c },
  { 0x07a8, 0x038e },
  { 0x07a9, 0x03ab },
  { 0x07ab, 0x038f },
  { 0x07ae, 0x0385 },
  { 0x07af, 0x2015 },
  { 0x07b1, 0x03ac },
  { 0x07b2, 0x03ad },
  { 0x07b3, 0x03ae },
  { 0x07b4, 0x03af },
  { 0x07b5, 0x03ca },
  { 0x07b6, 0x0390 },
  { 0x07b7, 0x03cc },
  { 0x07b8, 0x03cd },
  { 0x07b9, 0x03cb },
  { 0x07ba, 0x03b0 },
  { 0x07bb, 0x03ce },
  { 0x07c1, 0x0391 },
  { 0x07c2, 0x0392 },
  { 0x07c3, 0x0393 },
  { 0x07c4, 0x0394 },
  { 0x07c5, 0x0395 },
  { 0x07c6, 0x0396 },
  { 0x07c7, 0x0397 },
  { 0x07c8, 0x0398 },
  { 0x07c9, 0x0399 },
  { 0x07ca, 0x039a },
  { 0x07cb, 0x039b },
  { 0x07cc, 0x039c },
  { 0x07cd, 0x039d },
  { 0x07ce, 0x039e },
  { 0x07cf, 0x039f },
  { 0x07d0, 0x03a0 },
  { 0x07d1, 0x03a1 },
  { 0x07d2, 0x03a3 },
  { 0x07d4, 0x03a4 },
  { 0x07d5, 0x03a5 },
  { 0x07d6, 0x03a6 },
  { 0x07d7, 0x03a7 },
  { 0x07d8, 0x03a8 },
  { 0x07d9, 0x03a9 },
  { 0x07e1, 0x03b1 },
  { 0x07e2, 0x03b2 },
  { 0x07e3, 0x03b3 },
  { 0x07e4, 0x03b4 },
  { 0x07e5, 0x03b5 },
  { 0x07e6, 0x03b6 },
  { 0x07e7, 0x03b7 },
  { 0x07e8, 0x03b8 },
  { 0x07e9, 0x03b9 },
  { 0x07ea, 0x03ba },
  { 0x07eb, 0x03bb },
  { 0x07ec, 0x03bc },
  { 0x07ed, 0x03bd },
  { 0x07ee, 0x03be },
  { 0x07ef, 0x03bf },
  { 0x07f0, 0x03c0 },
  { 0x07f1, 0x03c1 },
  { 0x07f2, 0x03c3 },
  { 0x07f3, 0x03c2 },
  { 0x07f4, 0x03c4 },
  { 0x07f5, 0x03c5 },
  { 0x07f6, 0x03c6 },
  { 0x07f7, 0x03c7 },
  { 0x07f8, 0x03c8 },
  { 0x07f9, 0x03c9 },
  { 0x08a1, 0x23b7 },
  { 0x08a2, 0x250c },
  { 0x08a3, 0x2500 },
  { 0x08a4, 0x2320 },
  { 0x08a5, 0x2321 },
  { 0x08a6, 0x2502 },
  { 0x08a7, 0x23a1 },
  { 0x08a8, 0x23a3 },
  { 0x08a9, 0x23a4 },
  { 0x08aa, 0x23a6 },
  { 0x08ab, 0x239b },
  { 0x08ac, 0x239d },
  { 0x08ad, 0x239e },
  { 0x08ae, 0x23a0 },
  { 0x08af, 0x23a8 },
  { 0x08b0, 0x23ac },
  { 0x08bc, 0x2264 },
  { 0x08bd, 0x2260 },
  { 0x08be, 0x2265 },
  { 0x08bf, 0x222b },
  { 0x08c0, 0x2234 },
  { 0x08c1, 0x221d },
  { 0x08c2, 0x221e },
  { 0x08c5, 0x2207 },
  { 0x08c8, 0x223c },
  { 0x08c9, 0x2243 },
  { 0x08cd, 0x21d4 },
  { 0x08ce, 0x21d2 },
  { 0x08cf, 0x2261 },
  { 0x08d6, 0x221a },
  { 0x08da, 0x2282 },
  { 0x08db, 0x2283 },
  { 0x08dc, 0x2229 },
  { 0x08dd, 0x222a },
  { 0x08de, 0x2227 },
  { 0x08df, 0x2228 },
  { 0x08ef, 0x2202 },
  { 0x08f6, 0x0192 },
  { 0x08fb, 0x2190 },
  { 0x08fc, 0x2191 },
  { 0x08fd, 0x2192 },
  { 0x08fe, 0x2193 },
  { 0x09e0, 0x25c6 },
  { 0x09e1, 0x2592 },
  { 0x09e2, 0x2409 },
  { 0x09e3, 0x240c },
  { 0x09e4, 0x240d },
  { 0x09e5, 0x240a },
  { 0x09e8, 0x2424 },
  { 0x09e9, 0x240b },
  { 0x09ea, 0x2518 },
  { 0x09eb, 0x2510 },
  { 0x09ec, 0x250c },
  { 0x09ed, 0x2514 },
  { 0x09ee, 0x253c },
  { 0x09ef, 0x23ba },
  { 0x09f0, 0x23bb },
  { 0x09f1, 0x2500 },
  { 0x09f2, 0x23bc },
  { 0x09f3, 0x23bd },
  { 0x09f4, 0x251c },
  { 0x09f5, 0x2524 },
  { 0x09f6, 0x2534 },
  { 0x09f7, 0x252c },
  { 0x09f8, 0x2502 },
  { 0x0aa1, 0x2003 },
  { 0x0aa2, 0x2002 },
  { 0x0aa3, 0x2004 },
  { 0x0aa4, 0x2005 },
  { 0x0aa5, 0x2007 },
  { 0x0aa6, 0x2008 },
  { 0x0aa7, 0x2009 },
  { 0x0aa8, 0x200a },
  { 0x0aa9, 0x2014 },
  { 0x0aaa, 0x2013 },
  { 0x0aae, 0x2026 },
  { 0x0aaf, 0x2025 },
  { 0x0ab0, 0x2153 },
  { 0x0ab1, 0x2154 },
  { 0x0ab2, 0x2155 },
  { 0x0ab3, 0x2156 },
  { 0x0ab4, 0x2157 },
  { 0x0ab5, 0x2158 },
  { 0x0ab6, 0x2159 },
  { 0x0ab7, 0x215a },
  { 0x0ab8, 0x2105 },
  { 0x0abb, 0x2012 },
  { 0x0abc, 0x2329 },
  { 0x0abe, 0x232a },
  { 0x0ac3, 0x215b },
  { 0x0ac4, 0x215c },
  { 0x0ac5, 0x215d },
  { 0x0ac6, 0x215e },
  { 0x0ac9, 0x2122 },
  { 0x0aca, 0x2613 },
  { 0x0acc, 0x25c1 },
  { 0x0acd, 0x25b7 },
  { 0x0ace, 0x25cb },
  { 0x0acf, 0x25af },
  { 0x0ad0, 0x2018 },
  { 0x0ad1, 0x2019 },
  { 0x0ad2, 0x201c },
  { 0x0ad3, 0x201d },
  { 0x0ad4, 0x211e },
  { 0x0ad6, 0x2032 },
  { 0x0ad7, 0x2033 },
  { 0x0ad9, 0x271d },
  { 0x0adb, 0x25ac },
  { 0x0adc, 0x25c0 },
  { 0x0add, 0x25b6 },
  { 0x0ade, 0x25cf },
  { 0x0adf, 0x25ae },
  { 0x0ae0, 0x25e6 },
  { 0x0ae1, 0x25ab },
  { 0x0ae2, 0x25ad },
  { 0x0ae3, 0x25b3 },
  { 0x0ae4, 0x25bd },
  { 0x0ae5, 0x2606 },
  { 0x0ae6, 0x2022 },
  { 0x0ae7, 0x25aa },
  { 0x0ae8, 0x25b2 },
  { 0x0ae9, 0x25bc },
  { 0x0aea, 0x261c },
  { 0x0aeb, 0x261e },
  { 0x0aec, 0x2663 },
  { 0x0aed, 0x2666 },
  { 0x0aee, 0x2665 },
  { 0x0af0, 0x2720 },
  { 0x0af1, 0x2020 },
  { 0x0af2, 0x2021 },
  { 0x0af3, 0x2713 },
  { 0x0af4, 0x2717 },
  { 0x0af5, 0x266f },
  { 0x0af6, 0x266d },
  { 0x0af7, 0x2642 },
  { 0x0af8, 0x2640 },
  { 0x0af9, 0x260e },
  { 0x0afa, 0x2315 },
  { 0x0afb, 0x2117 },
  { 0x0afc, 0x2038 },
  { 0x0afd, 0x201a },
  { 0x0afe, 0x201e },
  { 0x0ba3, 0x003c },
  { 0x0ba6, 0x003e },
  { 0x0ba8, 0x2228 },
  { 0x0ba9, 0x2227 },
  { 0x0bc0, 0x00af },
  { 0x0bc2, 0x22a5 },
  { 0x0bc3, 0x2229 },
  { 0x0bc4, 0x230a },
  { 0x0bc6, 0x005f },
  { 0x0bca, 0x2218 },
  { 0x0bcc, 0x2395 },
  { 0x0bce, 0x22a4 },
  { 0x0bcf, 0x25cb },
  { 0x0bd3, 0x2308 },
  { 0x0bd6, 0x222a },
  { 0x0bd8, 0x2283 },
  { 0x0bda, 0x2282 },
  { 0x0bdc, 0x22a2 },
  { 0x0bfc, 0x22a3 },
  { 0x0cdf, 0x2017 },
  { 0x0ce0, 0x05d0 },
  { 0x0ce1, 0x05d1 },
  { 0x0ce2, 0x05d2 },
  { 0x0ce3, 0x05d3 },
  { 0x0ce4, 0x05d4 },
  { 0x0ce5, 0x05d5 },
  { 0x0ce6, 0x05d6 },
  { 0x0ce7, 0x05d7 },
  { 0x0ce8, 0x05d8 },
  { 0x0ce9, 0x05d9 },
  { 0x0cea, 0x05da },
  { 0x0ceb, 0x05db },
  { 0x0cec, 0x05dc },
  { 0x0ced, 0x05dd },
  { 0x0cee, 0x05de },
  { 0x0cef, 0x05df },
  { 0x0cf0, 0x05e0 },
  { 0x0cf1, 0x05e1 },
  { 0x0cf2, 0x05e2 },
  { 0x0cf3, 0x05e3 },
  { 0x0cf4, 0x05e4 },
  { 0x0cf5, 0x05e5 },
  { 0x0cf6, 0x05e6 },
  { 0x0cf7, 0x05e7 },
  { 0x0cf8, 0x05e8 },
  { 0x0cf9, 0x05e9 },
  { 0x0cfa, 0x05ea },
  { 0x0da1, 0x0e01 },
  { 0x0da2, 0x0e02 },
  { 0x0da3, 0x0e03 },
  { 0x0da4, 0x0e04 },
  { 0x0da5, 0x0e05 },
  { 0x0da6, 0x0e06 },
  { 0x0da7, 0x0e07 },
  { 0x0da8, 0x0e08 },
  { 0x0da9, 0x0e09 },
  { 0x0daa, 0x0e0a },
  { 0x0dab, 0x0e0b },
  { 0x0dac, 0x0e0c },
  { 0x0dad, 0x0e0d },
  { 0x0dae, 0x0e0e },
  { 0x0daf, 0x0e0f },
  { 0x0db0, 0x0e10 },
  { 0x0db1, 0x0e11 },
  { 0x0db2, 0x0e12 },
  { 0x0db3, 0x0e13 },
  { 0x0db4, 0x0e14 },
  { 0x0db5, 0x0e15 },
  { 0x0db6, 0x0e16 },
  { 0x0db7, 0x0e17 },
  { 0x0db8, 0x0e18 },
  { 0x0db9, 0x0e19 },
  { 0x0dba, 0x0e1a },
  { 0x0dbb, 0x0e1b },
  { 0x0dbc, 0x0e1c },
  { 0x0dbd, 0x0e1d },
  { 0x0dbe, 0x0e1e },
  { 0x0dbf, 0x0e1f },
  { 0x0dc0, 0x0e20 },
  { 0x0dc1, 0x0e21 },
  { 0x0dc2, 0x0e22 },
  { 0x0dc3, 0x0e23 },
  { 0x0dc4, 0x0e24 },
  { 0x0dc5, 0x0e25 },
  { 0x0dc6, 0x0e26 },
  { 0x0dc7, 0x0e27 },
  { 0x0dc8, 0x0e28 },
  { 0x0dc9, 0x0e29 },
  { 0x0dca, 0x0e2a },
  { 0x0dcb, 0x0e2b },
  { 0x0dcc, 0x0e2c },
  { 0x0dcd, 0x0e2d },
  { 0x0dce, 0x0e2e },
  { 0x0dcf, 0x0e2f },
  { 0x0dd0, 0x0e30 },
  { 0x0dd1, 0x0e31 },
  { 0x0dd2, 0x0e32 },
  { 0x0dd3, 0x0e33 },
  { 0x0dd4, 0x0e34 },
  { 0x0dd5, 0x0e35 },
  { 0x0dd6, 0x0e36 },
  { 0x0dd7, 0x0e37 },
  { 0x0dd8, 0x0e38 },
  { 0x0dd9, 0x0e39 },
  { 0x0dda, 0x0e3a },
  { 0x0ddf, 0x0e3f },
  { 0x0de0, 0x0e40 },
  { 0x0de1, 0x0e41 },
  { 0x0de2, 0x0e42 },
  { 0x0de3, 0x0e43 },
  { 0x0de4, 0x0e44 },
  { 0x0de5, 0x0e45 },
  { 0x0de6, 0x0e46 },
  { 0x0de7, 0x0e47 },
  { 0x0de8, 0x0e48 },
  { 0x0de9, 0x0e49 },
  { 0x0dea, 0x0e4a },
  { 0x0deb, 0x0e4b },
  { 0x0dec, 0x0e4c },
  { 0x0ded, 0x0e4d },
  { 0x0df0, 0x0e50 },
  { 0x0df1, 0x0e51 },
  { 0x0df2, 0x0e52 },
  { 0x0df3, 0x0e53 },
  { 0x0df4, 0x0e54 },
  { 0x0df5, 0x0e55 },
  { 0x0df6, 0x0e56 },
  { 0x0df7, 0x0e57 },
  { 0x0df8, 0x0e58 },
  { 0x0df9, 0x0e59 },
  { 0x0ea1, 0x3131 },
  { 0x0ea2, 0x3132 },
  { 0x0ea3, 0x3133 },
  { 0x0ea4, 0x3134 },
  { 0x0ea5, 0x3135 },
  { 0x0ea6, 0x3136 },
  { 0x0ea7, 0x3137 },
  { 0x0ea8, 0x3138 },
  { 0x0ea9, 0x3139 },
  { 0x0eaa, 0x313a },
  { 0x0eab, 0x313b },
  { 0x0eac, 0x313c },
  { 0x0ead, 0x313d },
  { 0x0eae, 0x313e },
  { 0x0eaf, 0x313f },
  { 0x0eb0, 0x3140 },
  { 0x0eb1, 0x3141 },
  { 0x0eb2, 0x3142 },
  { 0x0eb3, 0x3143 },
  { 0x0eb4, 0x3144 },
  { 0x0eb5, 0x3145 },
  { 0x0eb6, 0x3146 },
  { 0x0eb7, 0x3147 },
  { 0x0eb8, 0x3148 },
  { 0x0eb9, 0x3149 },
  { 0x0eba, 0x314a },
  { 0x0ebb, 0x314b },
  { 0x0ebc, 0x314c },
  { 0x0ebd, 0x314d },
  { 0x0ebe, 0x314e },
  { 0x0ebf, 0x314f },
  { 0x0ec0, 0x3150 },
  { 0x0ec1, 0x3151 },
  { 0x0ec2, 0x3152 },
  { 0x0ec3, 0x3153 },
  { 0x0ec4, 0x3154 },
  { 0x0ec5, 0x3155 },
  { 0x0ec6, 0x3156 },
  { 0x0ec7, 0x3157 },
  { 0x0ec8, 0x3158 },
  { 0x0ec9, 0x3159 },
  { 0x0eca, 0x315a },
  { 0x0ecb, 0x315b },
  { 0x0ecc, 0x315c },
  { 0x0ecd, 0x315d },
  { 0x0ece, 0x315e },
  { 0x0ecf, 0x315f },
  { 0x0ed0, 0x3160 },
  { 0x0ed1, 0x3161 },
  { 0x0ed2, 0x3162 },
  { 0x0ed3, 0x3163 },
  { 0x0ed4, 0x11a8 },
  { 0x0ed5, 0x11a9 },
  { 0x0ed6, 0x11aa },
  { 0x0ed7, 0x11ab },
  { 0x0ed8, 0x11ac },
  { 0x0ed9, 0x11ad },
  { 0x0eda, 0x11ae },
  { 0x0edb, 0x11af },
  { 0x0edc, 0x11b0 },
  { 0x0edd, 0x11b1 },
  { 0x0ede, 0x11b2 },
  { 0x0edf, 0x11b3 },
  { 0x0ee0, 0x11b4 },
  { 0x0ee1, 0x11b5 },
  { 0x0ee2, 0x11b6 },
  { 0x0ee3, 0x11b7 },
  { 0x0ee4, 0x11b8 },
  { 0x0ee5, 0x11b9 },
  { 0x0ee6, 0x11ba },
  { 0x0ee7, 0x11bb },
  { 0x0ee8, 0x11bc },
  { 0x0ee9, 0x11bd },
  { 0x0eea, 0x11be },
  { 0x0eeb, 0x11bf },
  { 0x0eec, 0x11c0 },
  { 0x0eed, 0x11c1 },
  { 0x0eee, 0x11c2 },
  { 0x0eef, 0x316d },
  { 0x0ef0, 0x3171 },
  { 0x0ef1, 0x3178 },
  { 0x0ef2, 0x317f },
  { 0x0ef3, 0x3181 },
  { 0x0ef4, 0x3184 },
  { 0x0ef5, 0x3186 },
  { 0x0ef6, 0x318d },
  { 0x0ef7, 0x318e },
  { 0x0ef8, 0x11eb },
  { 0x0ef9, 0x11f0 },
  { 0x0efa, 0x11f9 },
  { 0x0eff, 0x20a9 },
  { 0x13a4, 0x20ac },
  { 0x13bc, 0x0152 },
  { 0x13bd, 0x0153 },
  { 0x13be, 0x0178 },
  { 0x20ac, 0x20ac },
  { 0xfe50,    '`' },
  { 0xfe51, 0x00b4 },
  { 0xfe52,    '^' },
  { 0xfe53,    '~' },
  { 0xfe54, 0x00af },
  { 0xfe55, 0x02d8 },
  { 0xfe56, 0x02d9 },
  { 0xfe57, 0x00a8 },
  { 0xfe58, 0x02da },
  { 0xfe59, 0x02dd },
  { 0xfe5a, 0x02c7 },
  { 0xfe5b, 0x00b8 },
  { 0xfe5c, 0x02db },
  { 0xfe5d, 0x037a },
  { 0xfe5e, 0x309b },
  { 0xfe5f, 0x309c },
  { 0xfe63,    '/' },
  { 0xfe64, 0x02bc },
  { 0xfe65, 0x02bd },
  { 0xfe66, 0x02f5 },
  { 0xfe67, 0x02f3 },
  { 0xfe68, 0x02cd },
  { 0xfe69, 0xa788 },
  { 0xfe6a, 0x02f7 },
  { 0xfe6e,    ',' },
  { 0xfe6f, 0x00a4 },
  { 0xfe80,    'a' }, // XK_dead_a
  { 0xfe81,    'A' }, // XK_dead_A
  { 0xfe82,    'e' }, // XK_dead_e
  { 0xfe83,    'E' }, // XK_dead_E
  { 0xfe84,    'i' }, // XK_dead_i
  { 0xfe85,    'I' }, // XK_dead_I
  { 0xfe86,    'o' }, // XK_dead_o
  { 0xfe87,    'O' }, // XK_dead_O
  { 0xfe88,    'u' }, // XK_dead_u
  { 0xfe89,    'U' }, // XK_dead_U
  { 0xfe8a, 0x0259 },
  { 0xfe8b, 0x018f },
  { 0xfe8c, 0x00b5 },
  { 0xfe90,    '_' },
  { 0xfe91, 0x02c8 },
  { 0xfe92, 0x02cc },
  { 0xff80 /*XKB_KEY_KP_Space*/,     ' ' },
  { 0xff95 /*XKB_KEY_KP_7*/, 0x0037 },
  { 0xff96 /*XKB_KEY_KP_4*/, 0x0034 },
  { 0xff97 /*XKB_KEY_KP_8*/, 0x0038 },
  { 0xff98 /*XKB_KEY_KP_6*/, 0x0036 },
  { 0xff99 /*XKB_KEY_KP_2*/, 0x0032 },
  { 0xff9a /*XKB_KEY_KP_9*/, 0x0039 },
  { 0xff9b /*XKB_KEY_KP_3*/, 0x0033 },
  { 0xff9c /*XKB_KEY_KP_1*/, 0x0031 },
  { 0xff9d /*XKB_KEY_KP_5*/, 0x0035 },
  { 0xff9e /*XKB_KEY_KP_0*/, 0x0030 },
  { 0xffaa /*XKB_KEY_KP_Multiply*/,  '*' },
  { 0xffab /*XKB_KEY_KP_Add*/,       '+' },
  { 0xffac /*XKB_KEY_KP_Separator*/, ',' },
  { 0xffad /*XKB_KEY_KP_Subtract*/,  '-' },
  { 0xffae /*XKB_KEY_KP_Decimal*/,   '.' },
  { 0xffaf /*XKB_KEY_KP_Divide*/,    '/' },
  { 0xffb0 /*XKB_KEY_KP_0*/, 0x0030 },
  { 0xffb1 /*XKB_KEY_KP_1*/, 0x0031 },
  { 0xffb2 /*XKB_KEY_KP_2*/, 0x0032 },
  { 0xffb3 /*XKB_KEY_KP_3*/, 0x0033 },
  { 0xffb4 /*XKB_KEY_KP_4*/, 0x0034 },
  { 0xffb5 /*XKB_KEY_KP_5*/, 0x0035 },
  { 0xffb6 /*XKB_KEY_KP_6*/, 0x0036 },
  { 0xffb7 /*XKB_KEY_KP_7*/, 0x0037 },
  { 0xffb8 /*XKB_KEY_KP_8*/, 0x0038 },
  { 0xffb9 /*XKB_KEY_KP_9*/, 0x0039 },
  { 0xffbd /*XKB_KEY_KP_Equal*/,     '=' }
};

#endif

#endif // X11_DEFINES_H

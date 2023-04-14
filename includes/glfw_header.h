#ifndef GLFW_HEADER_H
#define GLFW_HEADER_H

#include "vulkan_header.h"

/* GLFWAPI is used to declare public API functions for export
 * from the DLL / shared library / dynamic library.
 */
#if defined(_WIN32) && defined(_GLFW_BUILD_DLL)
 /* We are building GLFW as a Win32 DLL */
 #define GLFWAPI __declspec(dllexport)
#elif defined(_WIN32) && defined(GLFW_DLL)
 /* We are calling GLFW as a Win32 DLL */
 #define GLFWAPI __declspec(dllimport)
#elif defined(__GNUC__) && defined(_GLFW_BUILD_DLL)
 /* We are building GLFW as a shared / dynamic library */
 #define GLFWAPI __attribute__((visibility("default")))
#else
 /* We are building or calling GLFW as a static library */
 #define GLFWAPI
#endif

#define GLFW_TRUE                   1
/*! @brief Zero.
 *
 *  This is only semantic sugar for the number 0.  You can instead use `0` or
 *  `false` or `_False` or `GL_FALSE` or `VK_FALSE` or anything else that is
 *  equal to zero.
 *
 *  @ingroup init
 */
#define GLFW_FALSE                  0

/*! @name Key and button actions
 *  @{ */
/*! @brief The key or mouse button was released.
 *
 *  The key or mouse button was released.
 *
 *  @ingroup input
 */
#define GLFW_RELEASE                0
/*! @brief The key or mouse button was pressed.
 *
 *  The key or mouse button was pressed.
 *
 *  @ingroup input
 */
#define GLFW_PRESS                  1
/*! @brief The key was held down until it repeated.
 *
 *  The key was held down until it repeated.
 *
 *  @ingroup input
 */
#define GLFW_REPEAT                 2
/*! @} */

/*! @defgroup hat_state Joystick hat states
 *  @brief Joystick hat states.
 *
 *  See [joystick hat input](@ref joystick_hat) for how these are used.
 *
 *  @ingroup input
 *  @{ */
#define GLFW_HAT_CENTERED           0
#define GLFW_HAT_UP                 1
#define GLFW_HAT_RIGHT              2
#define GLFW_HAT_DOWN               4
#define GLFW_HAT_LEFT               8
#define GLFW_HAT_RIGHT_UP           (GLFW_HAT_RIGHT | GLFW_HAT_UP)
#define GLFW_HAT_RIGHT_DOWN         (GLFW_HAT_RIGHT | GLFW_HAT_DOWN)
#define GLFW_HAT_LEFT_UP            (GLFW_HAT_LEFT  | GLFW_HAT_UP)
#define GLFW_HAT_LEFT_DOWN          (GLFW_HAT_LEFT  | GLFW_HAT_DOWN)

/* The unknown key */
#define GLFW_KEY_UNKNOWN            -1

/* Printable keys */
#define GLFW_KEY_SPACE              32
#define GLFW_KEY_APOSTROPHE         39  /* ' */
#define GLFW_KEY_COMMA              44  /* , */
#define GLFW_KEY_MINUS              45  /* - */
#define GLFW_KEY_PERIOD             46  /* . */
#define GLFW_KEY_SLASH              47  /* / */
#define GLFW_KEY_0                  48
#define GLFW_KEY_1                  49
#define GLFW_KEY_2                  50
#define GLFW_KEY_3                  51
#define GLFW_KEY_4                  52
#define GLFW_KEY_5                  53
#define GLFW_KEY_6                  54
#define GLFW_KEY_7                  55
#define GLFW_KEY_8                  56
#define GLFW_KEY_9                  57
#define GLFW_KEY_SEMICOLON          59  /* ; */
#define GLFW_KEY_EQUAL              61  /* = */
#define GLFW_KEY_A                  65
#define GLFW_KEY_B                  66
#define GLFW_KEY_C                  67
#define GLFW_KEY_D                  68
#define GLFW_KEY_E                  69
#define GLFW_KEY_F                  70
#define GLFW_KEY_G                  71
#define GLFW_KEY_H                  72
#define GLFW_KEY_I                  73
#define GLFW_KEY_J                  74
#define GLFW_KEY_K                  75
#define GLFW_KEY_L                  76
#define GLFW_KEY_M                  77
#define GLFW_KEY_N                  78
#define GLFW_KEY_O                  79
#define GLFW_KEY_P                  80
#define GLFW_KEY_Q                  81
#define GLFW_KEY_R                  82
#define GLFW_KEY_S                  83
#define GLFW_KEY_T                  84
#define GLFW_KEY_U                  85
#define GLFW_KEY_V                  86
#define GLFW_KEY_W                  87
#define GLFW_KEY_X                  88
#define GLFW_KEY_Y                  89
#define GLFW_KEY_Z                  90
#define GLFW_KEY_LEFT_BRACKET       91  /* [ */
#define GLFW_KEY_BACKSLASH          92  /* \ */
#define GLFW_KEY_RIGHT_BRACKET      93  /* ] */
#define GLFW_KEY_GRAVE_ACCENT       96  /* ` */
#define GLFW_KEY_WORLD_1            161 /* non-US #1 */
#define GLFW_KEY_WORLD_2            162 /* non-US #2 */

/* Function keys */
#define GLFW_KEY_ESCAPE             256
#define GLFW_KEY_ENTER              257
#define GLFW_KEY_TAB                258
#define GLFW_KEY_BACKSPACE          259
#define GLFW_KEY_INSERT             260
#define GLFW_KEY_DELETE             261
#define GLFW_KEY_RIGHT              262
#define GLFW_KEY_LEFT               263
#define GLFW_KEY_DOWN               264
#define GLFW_KEY_UP                 265
#define GLFW_KEY_PAGE_UP            266
#define GLFW_KEY_PAGE_DOWN          267
#define GLFW_KEY_HOME               268
#define GLFW_KEY_END                269
#define GLFW_KEY_CAPS_LOCK          280
#define GLFW_KEY_SCROLL_LOCK        281
#define GLFW_KEY_NUM_LOCK           282
#define GLFW_KEY_PRINT_SCREEN       283
#define GLFW_KEY_PAUSE              284
#define GLFW_KEY_F1                 290
#define GLFW_KEY_F2                 291
#define GLFW_KEY_F3                 292
#define GLFW_KEY_F4                 293
#define GLFW_KEY_F5                 294
#define GLFW_KEY_F6                 295
#define GLFW_KEY_F7                 296
#define GLFW_KEY_F8                 297
#define GLFW_KEY_F9                 298
#define GLFW_KEY_F10                299
#define GLFW_KEY_F11                300
#define GLFW_KEY_F12                301
#define GLFW_KEY_F13                302
#define GLFW_KEY_F14                303
#define GLFW_KEY_F15                304
#define GLFW_KEY_F16                305
#define GLFW_KEY_F17                306
#define GLFW_KEY_F18                307
#define GLFW_KEY_F19                308
#define GLFW_KEY_F20                309
#define GLFW_KEY_F21                310
#define GLFW_KEY_F22                311
#define GLFW_KEY_F23                312
#define GLFW_KEY_F24                313
#define GLFW_KEY_F25                314
#define GLFW_KEY_KP_0               320
#define GLFW_KEY_KP_1               321
#define GLFW_KEY_KP_2               322
#define GLFW_KEY_KP_3               323
#define GLFW_KEY_KP_4               324
#define GLFW_KEY_KP_5               325
#define GLFW_KEY_KP_6               326
#define GLFW_KEY_KP_7               327
#define GLFW_KEY_KP_8               328
#define GLFW_KEY_KP_9               329
#define GLFW_KEY_KP_DECIMAL         330
#define GLFW_KEY_KP_DIVIDE          331
#define GLFW_KEY_KP_MULTIPLY        332
#define GLFW_KEY_KP_SUBTRACT        333
#define GLFW_KEY_KP_ADD             334
#define GLFW_KEY_KP_ENTER           335
#define GLFW_KEY_KP_EQUAL           336
#define GLFW_KEY_LEFT_SHIFT         340
#define GLFW_KEY_LEFT_CONTROL       341
#define GLFW_KEY_LEFT_ALT           342
#define GLFW_KEY_LEFT_SUPER         343
#define GLFW_KEY_RIGHT_SHIFT        344
#define GLFW_KEY_RIGHT_CONTROL      345
#define GLFW_KEY_RIGHT_ALT          346
#define GLFW_KEY_RIGHT_SUPER        347
#define GLFW_KEY_MENU               348

#define GLFW_KEY_LAST               GLFW_KEY_MENU


/*! @} */

/*! @defgroup mods Modifier key flags
 *  @brief Modifier key flags.
 *
 *  See [key input](@ref input_key) for how these are used.
 *
 *  @ingroup input
 *  @{ */

/*! @brief If this bit is set one or more Shift keys were held down.
 *
 *  If this bit is set one or more Shift keys were held down.
 */
#define GLFW_MOD_SHIFT           0x0001
/*! @brief If this bit is set one or more Control keys were held down.
 *
 *  If this bit is set one or more Control keys were held down.
 */
#define GLFW_MOD_CONTROL         0x0002
/*! @brief If this bit is set one or more Alt keys were held down.
 *
 *  If this bit is set one or more Alt keys were held down.
 */
#define GLFW_MOD_ALT             0x0004
/*! @brief If this bit is set one or more Super keys were held down.
 *
 *  If this bit is set one or more Super keys were held down.
 */
#define GLFW_MOD_SUPER           0x0008
/*! @brief If this bit is set the Caps Lock key is enabled.
 *
 *  If this bit is set the Caps Lock key is enabled and the @ref
 *  GLFW_LOCK_KEY_MODS input mode is set.
 */
#define GLFW_MOD_CAPS_LOCK       0x0010
/*! @brief If this bit is set the Num Lock key is enabled.
 *
 *  If this bit is set the Num Lock key is enabled and the @ref
 *  GLFW_LOCK_KEY_MODS input mode is set.
 */
#define GLFW_MOD_NUM_LOCK        0x0020

/*! @} */

/*! @defgroup buttons Mouse buttons
 *  @brief Mouse button IDs.
 *
 *  See [mouse button input](@ref input_mouse_button) for how these are used.
 *
 *  @ingroup input
 *  @{ */
#define GLFW_MOUSE_BUTTON_1         0
#define GLFW_MOUSE_BUTTON_2         1
#define GLFW_MOUSE_BUTTON_3         2
#define GLFW_MOUSE_BUTTON_4         3
#define GLFW_MOUSE_BUTTON_5         4
#define GLFW_MOUSE_BUTTON_6         5
#define GLFW_MOUSE_BUTTON_7         6
#define GLFW_MOUSE_BUTTON_8         7
#define GLFW_MOUSE_BUTTON_LAST      GLFW_MOUSE_BUTTON_8
#define GLFW_MOUSE_BUTTON_LEFT      GLFW_MOUSE_BUTTON_1
#define GLFW_MOUSE_BUTTON_RIGHT     GLFW_MOUSE_BUTTON_2
#define GLFW_MOUSE_BUTTON_MIDDLE    GLFW_MOUSE_BUTTON_3
/*! @} */

/*! @defgroup joysticks Joysticks
 *  @brief Joystick IDs.
 *
 *  See [joystick input](@ref joystick) for how these are used.
 *
 *  @ingroup input
 *  @{ */
#define GLFW_JOYSTICK_1             0
#define GLFW_JOYSTICK_2             1
#define GLFW_JOYSTICK_3             2
#define GLFW_JOYSTICK_4             3
#define GLFW_JOYSTICK_5             4
#define GLFW_JOYSTICK_6             5
#define GLFW_JOYSTICK_7             6
#define GLFW_JOYSTICK_8             7
#define GLFW_JOYSTICK_9             8
#define GLFW_JOYSTICK_10            9
#define GLFW_JOYSTICK_11            10
#define GLFW_JOYSTICK_12            11
#define GLFW_JOYSTICK_13            12
#define GLFW_JOYSTICK_14            13
#define GLFW_JOYSTICK_15            14
#define GLFW_JOYSTICK_16            15
#define GLFW_JOYSTICK_LAST          GLFW_JOYSTICK_16
/*! @} */

/*! @defgroup gamepad_buttons Gamepad buttons
 *  @brief Gamepad buttons.
 *
 *  See @ref gamepad for how these are used.
 *
 *  @ingroup input
 *  @{ */
#define GLFW_GAMEPAD_BUTTON_A               0
#define GLFW_GAMEPAD_BUTTON_B               1
#define GLFW_GAMEPAD_BUTTON_X               2
#define GLFW_GAMEPAD_BUTTON_Y               3
#define GLFW_GAMEPAD_BUTTON_LEFT_BUMPER     4
#define GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER    5
#define GLFW_GAMEPAD_BUTTON_BACK            6
#define GLFW_GAMEPAD_BUTTON_START           7
#define GLFW_GAMEPAD_BUTTON_GUIDE           8
#define GLFW_GAMEPAD_BUTTON_LEFT_THUMB      9
#define GLFW_GAMEPAD_BUTTON_RIGHT_THUMB     10
#define GLFW_GAMEPAD_BUTTON_DPAD_UP         11
#define GLFW_GAMEPAD_BUTTON_DPAD_RIGHT      12
#define GLFW_GAMEPAD_BUTTON_DPAD_DOWN       13
#define GLFW_GAMEPAD_BUTTON_DPAD_LEFT       14
#define GLFW_GAMEPAD_BUTTON_LAST            GLFW_GAMEPAD_BUTTON_DPAD_LEFT

#define GLFW_GAMEPAD_BUTTON_CROSS       GLFW_GAMEPAD_BUTTON_A
#define GLFW_GAMEPAD_BUTTON_CIRCLE      GLFW_GAMEPAD_BUTTON_B
#define GLFW_GAMEPAD_BUTTON_SQUARE      GLFW_GAMEPAD_BUTTON_X
#define GLFW_GAMEPAD_BUTTON_TRIANGLE    GLFW_GAMEPAD_BUTTON_Y
/*! @} */

/*! @defgroup gamepad_axes Gamepad axes
 *  @brief Gamepad axes.
 *
 *  See @ref gamepad for how these are used.
 *
 *  @ingroup input
 *  @{ */
#define GLFW_GAMEPAD_AXIS_LEFT_X        0
#define GLFW_GAMEPAD_AXIS_LEFT_Y        1
#define GLFW_GAMEPAD_AXIS_RIGHT_X       2
#define GLFW_GAMEPAD_AXIS_RIGHT_Y       3
#define GLFW_GAMEPAD_AXIS_LEFT_TRIGGER  4
#define GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER 5
#define GLFW_GAMEPAD_AXIS_LAST          GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER
/*! @} */

/*! @defgroup errors Error codes
 *  @brief Error codes.
 *
 *  See [error handling](@ref error_handling) for how these are used.
 *
 *  @ingroup init
 *  @{ */
/*! @brief No error has occurred.
 *
 *  No error has occurred.
 *
 *  @analysis Yay.
 */
#define GLFW_NO_ERROR               0
/*! @brief GLFW has not been initialized.
 *
 *  This occurs if a GLFW function was called that must not be called unless the
 *  library is [initialized](@ref intro_init).
 *
 *  @analysis Application programmer error.  Initialize GLFW before calling any
 *  function that requires initialization.
 */
#define GLFW_NOT_INITIALIZED        0x00010001
/*! @brief No context is current for this thread.
 *
 *  This occurs if a GLFW function was called that needs and operates on the
 *  current OpenGL or OpenGL ES context but no context is current on the calling
 *  thread.  One such function is @ref glfwSwapInterval.
 *
 *  @analysis Application programmer error.  Ensure a context is current before
 *  calling functions that require a current context.
 */
#define GLFW_NO_CURRENT_CONTEXT     0x00010002
/*! @brief One of the arguments to the function was an invalid enum value.
 *
 *  One of the arguments to the function was an invalid enum value, for example
 *  requesting @ref GLFW_RED_BITS with @ref glfwGetWindowAttrib.
 *
 *  @analysis Application programmer error.  Fix the offending call.
 */
#define GLFW_INVALID_ENUM           0x00010003
/*! @brief One of the arguments to the function was an invalid value.
 *
 *  One of the arguments to the function was an invalid value, for example
 *  requesting a non-existent OpenGL or OpenGL ES version like 2.7.
 *
 *  Requesting a valid but unavailable OpenGL or OpenGL ES version will instead
 *  result in a @ref GLFW_VERSION_UNAVAILABLE error.
 *
 *  @analysis Application programmer error.  Fix the offending call.
 */
#define GLFW_INVALID_VALUE          0x00010004
/*! @brief A memory allocation failed.
 *
 *  A memory allocation failed.
 *
 *  @analysis A bug in GLFW or the underlying operating system.  Report the bug
 *  to our [issue tracker](https://github.com/glfw/glfw/issues).
 */
#define GLFW_OUT_OF_MEMORY          0x00010005
/*! @brief GLFW could not find support for the requested API on the system.
 *
 *  GLFW could not find support for the requested API on the system.
 *
 *  @analysis The installed graphics driver does not support the requested
 *  API, or does not support it via the chosen context creation backend.
 *  Below are a few examples.
 *
 *  @par
 *  Some pre-installed Windows graphics drivers do not support OpenGL.  AMD only
 *  supports OpenGL ES via EGL, while Nvidia and Intel only support it via
 *  a WGL or GLX extension.  macOS does not provide OpenGL ES at all.  The Mesa
 *  EGL, OpenGL and OpenGL ES libraries do not interface with the Nvidia binary
 *  driver.  Older graphics drivers do not support Vulkan.
 */
#define GLFW_API_UNAVAILABLE        0x00010006
/*! @brief The requested OpenGL or OpenGL ES version is not available.
 *
 *  The requested OpenGL or OpenGL ES version (including any requested context
 *  or framebuffer hints) is not available on this machine.
 *
 *  @analysis The machine does not support your requirements.  If your
 *  application is sufficiently flexible, downgrade your requirements and try
 *  again.  Otherwise, inform the user that their machine does not match your
 *  requirements.
 *
 *  @par
 *  Future invalid OpenGL and OpenGL ES versions, for example OpenGL 4.8 if 5.0
 *  comes out before the 4.x series gets that far, also fail with this error and
 *  not @ref GLFW_INVALID_VALUE, because GLFW cannot know what future versions
 *  will exist.
 */
#define GLFW_VERSION_UNAVAILABLE    0x00010007
/*! @brief A platform-specific error occurred that does not match any of the
 *  more specific categories.
 *
 *  A platform-specific error occurred that does not match any of the more
 *  specific categories.
 *
 *  @analysis A bug or configuration error in GLFW, the underlying operating
 *  system or its drivers, or a lack of required resources.  Report the issue to
 *  our [issue tracker](https://github.com/glfw/glfw/issues).
 */
#define GLFW_PLATFORM_ERROR         0x00010008
/*! @brief The requested format is not supported or available.
 *
 *  If emitted during window creation, the requested pixel format is not
 *  supported.
 *
 *  If emitted when querying the clipboard, the contents of the clipboard could
 *  not be converted to the requested format.
 *
 *  @analysis If emitted during window creation, one or more
 *  [hard constraints](@ref window_hints_hard) did not match any of the
 *  available pixel formats.  If your application is sufficiently flexible,
 *  downgrade your requirements and try again.  Otherwise, inform the user that
 *  their machine does not match your requirements.
 *
 *  @par
 *  If emitted when querying the clipboard, ignore the error or report it to
 *  the user, as appropriate.
 */
#define GLFW_FORMAT_UNAVAILABLE     0x00010009
/*! @brief The specified window does not have an OpenGL or OpenGL ES context.
 *
 *  A window that does not have an OpenGL or OpenGL ES context was passed to
 *  a function that requires it to have one.
 *
 *  @analysis Application programmer error.  Fix the offending call.
 */
#define GLFW_NO_WINDOW_CONTEXT      0x0001000A
/*! @} */

/*! @addtogroup window
 *  @{ */
/*! @brief Input focus window hint and attribute
 *
 *  Input focus [window hint](@ref GLFW_FOCUSED_hint) or
 *  [window attribute](@ref GLFW_FOCUSED_attrib).
 */
#define GLFW_FOCUSED                0x00020001
/*! @brief Window iconification window attribute
 *
 *  Window iconification [window attribute](@ref GLFW_ICONIFIED_attrib).
 */
#define GLFW_ICONIFIED              0x00020002
/*! @brief Window resize-ability window hint and attribute
 *
 *  Window resize-ability [window hint](@ref GLFW_RESIZABLE_hint) and
 *  [window attribute](@ref GLFW_RESIZABLE_attrib).
 */
#define GLFW_RESIZABLE              0x00020003
/*! @brief Window visibility window hint and attribute
 *
 *  Window visibility [window hint](@ref GLFW_VISIBLE_hint) and
 *  [window attribute](@ref GLFW_VISIBLE_attrib).
 */
#define GLFW_VISIBLE                0x00020004
/*! @brief Window decoration window hint and attribute
 *
 *  Window decoration [window hint](@ref GLFW_DECORATED_hint) and
 *  [window attribute](@ref GLFW_DECORATED_attrib).
 */
#define GLFW_DECORATED              0x00020005
/*! @brief Window auto-iconification window hint and attribute
 *
 *  Window auto-iconification [window hint](@ref GLFW_AUTO_ICONIFY_hint) and
 *  [window attribute](@ref GLFW_AUTO_ICONIFY_attrib).
 */
#define GLFW_AUTO_ICONIFY           0x00020006
/*! @brief Window decoration window hint and attribute
 *
 *  Window decoration [window hint](@ref GLFW_FLOATING_hint) and
 *  [window attribute](@ref GLFW_FLOATING_attrib).
 */
#define GLFW_FLOATING               0x00020007
/*! @brief Window maximization window hint and attribute
 *
 *  Window maximization [window hint](@ref GLFW_MAXIMIZED_hint) and
 *  [window attribute](@ref GLFW_MAXIMIZED_attrib).
 */
#define GLFW_MAXIMIZED              0x00020008
/*! @brief Cursor centering window hint
 *
 *  Cursor centering [window hint](@ref GLFW_CENTER_CURSOR_hint).
 */
#define GLFW_CENTER_CURSOR          0x00020009
/*! @brief Window framebuffer transparency hint and attribute
 *
 *  Window framebuffer transparency
 *  [window hint](@ref GLFW_TRANSPARENT_FRAMEBUFFER_hint) and
 *  [window attribute](@ref GLFW_TRANSPARENT_FRAMEBUFFER_attrib).
 */
#define GLFW_TRANSPARENT_FRAMEBUFFER 0x0002000A
/*! @brief Mouse cursor hover window attribute.
 *
 *  Mouse cursor hover [window attribute](@ref GLFW_HOVERED_attrib).
 */
#define GLFW_HOVERED                0x0002000B
/*! @brief Input focus on calling show window hint and attribute
 *
 *  Input focus [window hint](@ref GLFW_FOCUS_ON_SHOW_hint) or
 *  [window attribute](@ref GLFW_FOCUS_ON_SHOW_attrib).
 */
#define GLFW_FOCUS_ON_SHOW          0x0002000C

/*! @brief Framebuffer bit depth hint.
 *
 *  Framebuffer bit depth [hint](@ref GLFW_RED_BITS).
 */
#define GLFW_RED_BITS               0x00021001
/*! @brief Framebuffer bit depth hint.
 *
 *  Framebuffer bit depth [hint](@ref GLFW_GREEN_BITS).
 */
#define GLFW_GREEN_BITS             0x00021002
/*! @brief Framebuffer bit depth hint.
 *
 *  Framebuffer bit depth [hint](@ref GLFW_BLUE_BITS).
 */
#define GLFW_BLUE_BITS              0x00021003
/*! @brief Framebuffer bit depth hint.
 *
 *  Framebuffer bit depth [hint](@ref GLFW_ALPHA_BITS).
 */
#define GLFW_ALPHA_BITS             0x00021004
/*! @brief Framebuffer bit depth hint.
 *
 *  Framebuffer bit depth [hint](@ref GLFW_DEPTH_BITS).
 */
#define GLFW_DEPTH_BITS             0x00021005
/*! @brief Framebuffer bit depth hint.
 *
 *  Framebuffer bit depth [hint](@ref GLFW_STENCIL_BITS).
 */
#define GLFW_STENCIL_BITS           0x00021006
/*! @brief Framebuffer bit depth hint.
 *
 *  Framebuffer bit depth [hint](@ref GLFW_ACCUM_RED_BITS).
 */
#define GLFW_ACCUM_RED_BITS         0x00021007
/*! @brief Framebuffer bit depth hint.
 *
 *  Framebuffer bit depth [hint](@ref GLFW_ACCUM_GREEN_BITS).
 */
#define GLFW_ACCUM_GREEN_BITS       0x00021008
/*! @brief Framebuffer bit depth hint.
 *
 *  Framebuffer bit depth [hint](@ref GLFW_ACCUM_BLUE_BITS).
 */
#define GLFW_ACCUM_BLUE_BITS        0x00021009
/*! @brief Framebuffer bit depth hint.
 *
 *  Framebuffer bit depth [hint](@ref GLFW_ACCUM_ALPHA_BITS).
 */
#define GLFW_ACCUM_ALPHA_BITS       0x0002100A
/*! @brief Framebuffer auxiliary buffer hint.
 *
 *  Framebuffer auxiliary buffer [hint](@ref GLFW_AUX_BUFFERS).
 */
#define GLFW_AUX_BUFFERS            0x0002100B
/*! @brief OpenGL stereoscopic rendering hint.
 *
 *  OpenGL stereoscopic rendering [hint](@ref GLFW_STEREO).
 */
#define GLFW_STEREO                 0x0002100C
/*! @brief Framebuffer MSAA samples hint.
 *
 *  Framebuffer MSAA samples [hint](@ref GLFW_SAMPLES).
 */
#define GLFW_SAMPLES                0x0002100D
/*! @brief Framebuffer sRGB hint.
 *
 *  Framebuffer sRGB [hint](@ref GLFW_SRGB_CAPABLE).
 */
#define GLFW_SRGB_CAPABLE           0x0002100E
/*! @brief Monitor refresh rate hint.
 *
 *  Monitor refresh rate [hint](@ref GLFW_REFRESH_RATE).
 */
#define GLFW_REFRESH_RATE           0x0002100F
/*! @brief Framebuffer double buffering hint.
 *
 *  Framebuffer double buffering [hint](@ref GLFW_DOUBLEBUFFER).
 */
#define GLFW_DOUBLEBUFFER           0x00021010

/*! @brief Context client API hint and attribute.
 *
 *  Context client API [hint](@ref GLFW_CLIENT_API_hint) and
 *  [attribute](@ref GLFW_CLIENT_API_attrib).
 */
#define GLFW_CLIENT_API             0x00022001
/*! @brief Context client API major version hint and attribute.
 *
 *  Context client API major version [hint](@ref GLFW_CONTEXT_VERSION_MAJOR_hint)
 *  and [attribute](@ref GLFW_CONTEXT_VERSION_MAJOR_attrib).
 */
#define GLFW_CONTEXT_VERSION_MAJOR  0x00022002
/*! @brief Context client API minor version hint and attribute.
 *
 *  Context client API minor version [hint](@ref GLFW_CONTEXT_VERSION_MINOR_hint)
 *  and [attribute](@ref GLFW_CONTEXT_VERSION_MINOR_attrib).
 */
#define GLFW_CONTEXT_VERSION_MINOR  0x00022003
/*! @brief Context client API revision number hint and attribute.
 *
 *  Context client API revision number
 *  [attribute](@ref GLFW_CONTEXT_REVISION_attrib).
 */
#define GLFW_CONTEXT_REVISION       0x00022004
/*! @brief Context robustness hint and attribute.
 *
 *  Context client API revision number [hint](@ref GLFW_CONTEXT_ROBUSTNESS_hint)
 *  and [attribute](@ref GLFW_CONTEXT_ROBUSTNESS_attrib).
 */
#define GLFW_CONTEXT_ROBUSTNESS     0x00022005
/*! @brief OpenGL forward-compatibility hint and attribute.
 *
 *  OpenGL forward-compatibility [hint](@ref GLFW_OPENGL_FORWARD_COMPAT_hint)
 *  and [attribute](@ref GLFW_OPENGL_FORWARD_COMPAT_attrib).
 */
#define GLFW_OPENGL_FORWARD_COMPAT  0x00022006
/*! @brief OpenGL debug context hint and attribute.
 *
 *  OpenGL debug context [hint](@ref GLFW_OPENGL_DEBUG_CONTEXT_hint) and
 *  [attribute](@ref GLFW_OPENGL_DEBUG_CONTEXT_attrib).
 */
#define GLFW_OPENGL_DEBUG_CONTEXT   0x00022007
/*! @brief OpenGL profile hint and attribute.
 *
 *  OpenGL profile [hint](@ref GLFW_OPENGL_PROFILE_hint) and
 *  [attribute](@ref GLFW_OPENGL_PROFILE_attrib).
 */
#define GLFW_OPENGL_PROFILE         0x00022008
/*! @brief Context flush-on-release hint and attribute.
 *
 *  Context flush-on-release [hint](@ref GLFW_CONTEXT_RELEASE_BEHAVIOR_hint) and
 *  [attribute](@ref GLFW_CONTEXT_RELEASE_BEHAVIOR_attrib).
 */
#define GLFW_CONTEXT_RELEASE_BEHAVIOR 0x00022009
/*! @brief Context error suppression hint and attribute.
 *
 *  Context error suppression [hint](@ref GLFW_CONTEXT_NO_ERROR_hint) and
 *  [attribute](@ref GLFW_CONTEXT_NO_ERROR_attrib).
 */
#define GLFW_CONTEXT_NO_ERROR       0x0002200A
/*! @brief Context creation API hint and attribute.
 *
 *  Context creation API [hint](@ref GLFW_CONTEXT_CREATION_API_hint) and
 *  [attribute](@ref GLFW_CONTEXT_CREATION_API_attrib).
 */
#define GLFW_CONTEXT_CREATION_API   0x0002200B
/*! @brief Window content area scaling window
 *  [window hint](@ref GLFW_SCALE_TO_MONITOR).
 */
#define GLFW_SCALE_TO_MONITOR       0x0002200C
/*! @brief macOS specific
 *  [window hint](@ref GLFW_COCOA_RETINA_FRAMEBUFFER_hint).
 */
#define GLFW_COCOA_RETINA_FRAMEBUFFER 0x00023001
/*! @brief macOS specific
 *  [window hint](@ref GLFW_COCOA_FRAME_NAME_hint).
 */
#define GLFW_COCOA_FRAME_NAME         0x00023002
/*! @brief macOS specific
 *  [window hint](@ref GLFW_COCOA_GRAPHICS_SWITCHING_hint).
 */
#define GLFW_COCOA_GRAPHICS_SWITCHING 0x00023003
/*! @brief X11 specific
 *  [window hint](@ref GLFW_X11_CLASS_NAME_hint).
 */
#define GLFW_X11_CLASS_NAME         0x00024001
/*! @brief X11 specific
 *  [window hint](@ref GLFW_X11_CLASS_NAME_hint).
 */
#define GLFW_X11_INSTANCE_NAME      0x00024002
/*! @} */

#define GLFW_NO_API                          0
#define GLFW_OPENGL_API             0x00030001
#define GLFW_OPENGL_ES_API          0x00030002

#define GLFW_NO_ROBUSTNESS                   0
#define GLFW_NO_RESET_NOTIFICATION  0x00031001
#define GLFW_LOSE_CONTEXT_ON_RESET  0x00031002

#define GLFW_OPENGL_ANY_PROFILE              0
#define GLFW_OPENGL_CORE_PROFILE    0x00032001
#define GLFW_OPENGL_COMPAT_PROFILE  0x00032002

#define GLFW_CURSOR                 0x00033001
#define GLFW_STICKY_KEYS            0x00033002
#define GLFW_STICKY_MOUSE_BUTTONS   0x00033003
#define GLFW_LOCK_KEY_MODS          0x00033004
#define GLFW_RAW_MOUSE_MOTION       0x00033005

#define GLFW_CURSOR_NORMAL          0x00034001
#define GLFW_CURSOR_HIDDEN          0x00034002
#define GLFW_CURSOR_DISABLED        0x00034003

#define GLFW_ANY_RELEASE_BEHAVIOR            0
#define GLFW_RELEASE_BEHAVIOR_FLUSH 0x00035001
#define GLFW_RELEASE_BEHAVIOR_NONE  0x00035002

#define GLFW_NATIVE_CONTEXT_API     0x00036001
#define GLFW_EGL_CONTEXT_API        0x00036002
#define GLFW_OSMESA_CONTEXT_API     0x00036003

typedef struct GLFWwindow GLFWwindow;
typedef struct GLFWmonitor GLFWmonitor;

typedef void (*GLFWglproc)(void);
typedef void (* GLFWcursorposfun)(GLFWwindow*,double,double);
typedef void (* GLFWframebuffersizefun)(GLFWwindow*,int,int);
typedef void (* GLFWcharfun)(GLFWwindow*,unsigned int);
typedef void (* GLFWmousebuttonfun)(GLFWwindow*,int,int,int);
typedef void (* GLFWkeyfun)(GLFWwindow*,int,int,int,int);

typedef GLFWcursorposfun (*PFN_glfwSetCursorPosCallback)(GLFWwindow* window, GLFWcursorposfun callback);
typedef GLFWwindow* (*PFN_glfwCreateWindow)(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share);
typedef VkResult (*PFN_glfwCreateWindowSurface)(VkInstance instance, GLFWwindow* window, const VkAllocationCallbacks* allocator, VkSurfaceKHR* surface);
typedef GLFWframebuffersizefun (*PFN_glfwSetFramebufferSizeCallback)(GLFWwindow* window, GLFWframebuffersizefun callback);
typedef int (*PFN_glfwGetKey)(GLFWwindow* window, int key);
typedef int (*PFN_glfwInit)(void);
typedef const char** (*PFN_glfwGetRequiredInstanceExtensions)(uint32_t* count);
typedef void (*PFN_glfwGetFramebufferSize)(GLFWwindow* window, int* width, int* height);
typedef double (*PFN_glfwGetTime)(void);
typedef GLFWcharfun (*PFN_glfwSetCharCallback)(GLFWwindow* window, GLFWcharfun callback);
typedef void (*PFN_glfwWaitEvents)(void);
typedef void (*PFN_glfwPollEvents)(void);
typedef void (*PFN_glfwGetCursorPos)(GLFWwindow* window, double* xpos, double* ypos);
typedef void (*PFN_glfwSetInputMode)(GLFWwindow* window, int mode, int value);
typedef void (*PFN_glfwSetCursorPos)(GLFWwindow* window, double xpos, double ypos);
typedef void (*PFN_glfwTerminate)(void);
typedef void (*PFN_glfwDestroyWindow)(GLFWwindow* window);
typedef GLFWwindow* (*PFN_glfwCreateWindow)(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share);
typedef void (*PFN_glfwWindowHint)(int hint, int value);
typedef GLFWmousebuttonfun (*PFN_glfwSetMouseButtonCallback)(GLFWwindow* window, GLFWmousebuttonfun callback);
typedef GLFWkeyfun (*PFN_glfwSetKeyCallback)(GLFWwindow* window, GLFWkeyfun callback);
typedef int (*PFN_glfwGetMouseButton)(GLFWwindow* window, int button);
typedef int (*PFN_glfwWindowShouldClose)(GLFWwindow* window);

extern PFN_glfwSetCursorPosCallback glfwSetCursorPosCallback;
extern PFN_glfwCreateWindow glfwCreateWindow;
extern PFN_glfwCreateWindowSurface glfwCreateWindowSurface;
extern PFN_glfwSetFramebufferSizeCallback glfwSetFramebufferSizeCallback;
extern PFN_glfwGetKey glfwGetKey;
extern PFN_glfwInit glfwInit;
extern PFN_glfwGetRequiredInstanceExtensions glfwGetRequiredInstanceExtensions;
extern PFN_glfwGetFramebufferSize glfwGetFramebufferSize;
extern PFN_glfwGetTime glfwGetTime;
extern PFN_glfwSetCharCallback glfwSetCharCallback;
extern PFN_glfwWaitEvents glfwWaitEvents;
extern PFN_glfwPollEvents glfwPollEvents;
extern PFN_glfwGetCursorPos glfwGetCursorPos;
extern PFN_glfwSetInputMode glfwSetInputMode;
extern PFN_glfwSetCursorPos glfwSetCursorPos;
extern PFN_glfwTerminate glfwTerminate;
extern PFN_glfwDestroyWindow glfwDestroyWindow;
extern PFN_glfwWindowHint glfwWindowHint;
extern PFN_glfwSetMouseButtonCallback glfwSetMouseButtonCallback;
extern PFN_glfwSetKeyCallback glfwSetKeyCallback;
extern PFN_glfwGetMouseButton glfwGetMouseButton;
extern PFN_glfwWindowShouldClose glfwWindowShouldClose;

void InitGLFWFun();

#endif // GLFW_HEADER_H

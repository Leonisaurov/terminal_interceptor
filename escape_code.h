#ifndef KEYEXPAND_ESC_CODES
#define KEYEXPAND_ESC_CODES

#include <stdio.h>

typedef enum {
    GRAPHICAL_RENDITION_NO_STYLE = 0b0,
    GRAPHICAL_RENDITION_BG_COLOR = 0b1,
    GRAPHICAL_RENDITION_FG_COLOR = 0b10,
    GRAPHICAL_RENDITION_STYLE_BOLD = 0b100,
    GRAPHICAL_RENDITION_STYLE_ITALIC = 0b1000,
    GRAPHICAL_RENDITION_STYLE_UNDERLINE = 0b10000,
    GRAPHICAL_RENDITION_STYLE_BLINK = 0b100000,
    GRAPHICAL_RENDITION_STYLE_INVERSE = 0b1000000,
    GRAPHICAL_RENDITION_STYLE_INVISIBLE = 0b10000000,
    GRAPHICAL_RENDITION_STYLE_STRIKE_THROUGHT = 0b100000000,
} GRAPHICAL_RENDITION_Type;

typedef enum {
    REQUEST_CURSOR_POSITION,
} REQUEST_STATUS_Type;

typedef enum {
    STATUS_REPORT_CURSOR_POSITION,
} STATUS_REPORT_Type;

typedef enum {
    FOCUS_IN,
    FOCUS_OUT,
} FOCUS_REPORT;

typedef enum {
    MOUSE_BTN_LEFT,
    MOUSE_BTN_RIGHT,
    MOUSE_BTN_MIDDLE,
    MOUSE_BTN_UNDEF,
} MOUSE_BUTTON;

typedef enum {
    MOUSE_PRESS,
    MOUSE_RELEASE,
    MOUSE_DRAG,
    MOUSE_WHEEL_UP,
    MOUSE_WHEEL_DOWN,
} MOUSE_EVENT;

typedef union {
    unsigned short color[8];
    unsigned short position[2];
    unsigned short size[2];
    struct {
        MOUSE_BUTTON button;
        unsigned short position[2];
    } mouse_event;
} ESC_CODE_Data;

typedef enum {
    CSI_UNKNOWN,
    CSI_C_GRAPHICAL_RENDITION,
    CSI_C_CURSOR_POSITION,

    CSI_REQUEST_STATUS,
    CSI_STATUS_REPORT,

    CSI_FOCUS_REPORTED,
    CSI_P_CURSOR_EVENT,
} CSI_Type;

typedef enum {
    CSI_P_UNKNOWN,
    CSI_P_CURSOR_STYLE,
} CSI_P_Type;

typedef enum {
    OSC_UNKNOWN,
    OSC_C_TERMINAL_BG,
} OSC_Type;

typedef enum {
    ESC_CODE_Unknown,
    ESC_CODE_CSI,
    ESC_CODE_CSI_PRIVATE,
    ESC_CODE_FE,
    ESC_CODE_DCS,
    ESC_CODE_OSC,
} EscapeCode_Type;

typedef struct {
    EscapeCode_Type type;
    int subtype;
    int specify;
    ESC_CODE_Data data;
} EscapeCode;

void print_escape_code(FILE* file, EscapeCode code);
EscapeCode extract_escape_code(char *buffer, int *index);


const char* get_type_repr(EscapeCode_Type type);
const char* get_subtype_repr(EscapeCode_Type type, int subtype);
const char* get_specify_repr(EscapeCode_Type type, int subtype, int specify);
const char* get_metadata_repr(EscapeCode_Type type, int subtype, int specify, ESC_CODE_Data data);
#endif

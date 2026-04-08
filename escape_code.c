#include "escape_code.h"
#include <stdio.h>

#ifdef DEBUG
#include "debug.h"
#endif

const char* get_type_repr(EscapeCode_Type type) {
    switch (type) {
        case ESC_CODE_Unknown:
            return "Unknown";
        case ESC_CODE_CSI:
            return "CSI";
        case ESC_CODE_CSI_PRIVATE:
            return "CSI_PRIVATE";
        case ESC_CODE_DCS:
            return "DCS";
        case ESC_CODE_OSC:
            return "OSC";
        case ESC_CODE_FE:
            return "FE";
    }
    return "Unreachable";
}

const char* get_subtype_repr(EscapeCode_Type type, int subtype) {
    switch (type) {
        case ESC_CODE_Unknown:
            return "Unknown";
        case ESC_CODE_CSI:
            switch (subtype) {
                case CSI_C_GRAPHICAL_RENDITION: return "CHANGE_GRAPHICAL_RENDITION";
                case CSI_C_CURSOR_POSITION: return "CHANGE_CURSOR_POSITION";
                case CSI_REQUEST_STATUS: return "REQUEST_STATUS";
                case CSI_STATUS_REPORT: return "STATUS_REPORT";
                case CSI_FOCUS_REPORTED: return "FOCUS_REPORTED";
                case CSI_P_CURSOR_EVENT: return "CURSOR_EVENT";
            }
            return "Unreachable";
        case ESC_CODE_CSI_PRIVATE:
            return "Unknown";
        case ESC_CODE_DCS:
            return "Unknown";
        case ESC_CODE_OSC:
            switch (subtype) {
                case OSC_C_TERMINAL_BG: return "CHANGE_TERMINAL_BACKGROUND";
            }
            return "Unknown";
        case ESC_CODE_FE:
            return "Unknown";
    }
    return "Unreachable";
}

char specify_buffer[256];
int specify_buffer_i = 0;

#define STR(txt) txt, strlen(txt) + 1

const char* get_specify_repr(EscapeCode_Type type, int subtype, int specify) {
    specify_buffer_i = 0;
    switch (type) {
        case ESC_CODE_Unknown:
            return "Unknown";
        case ESC_CODE_CSI:
            switch (subtype) {
                case CSI_FOCUS_REPORTED:
                    switch (specify) {
                        case FOCUS_IN:  return "FOCUS_IN";
                        case FOCUS_OUT:  return "FOCUS_OUT";
                    }
                    return "Unreachable";
                case CSI_STATUS_REPORT:
                    switch (specify) {
                        case STATUS_REPORT_CURSOR_POSITION: return "CURSOR_POSITION";
                    }
                    return "Unreachable";
                case CSI_REQUEST_STATUS:
                    switch (specify) {
                        case REQUEST_CURSOR_POSITION: return "CURSOR_POSITION";
                    }
                    return "Unreachable";
                case CSI_P_CURSOR_EVENT:
                    switch (specify) {
                        case MOUSE_PRESS:   return "MOUSE_PRESS";
                        case MOUSE_RELEASE:   return "MOUSE_RELEASE";
                        case MOUSE_DRAG:   return "MOUSE_DRAG";
                        case MOUSE_WHEEL_UP:  return "MOUSE_WHEEL_UP";
                        case MOUSE_WHEEL_DOWN:  return "MOUSE_WHEEL_DOWN";
                    }
                    return "Unreachable";
                case CSI_C_GRAPHICAL_RENDITION:
                    // LOG("NUMBER: "); LOGN(specify); NL;
                    if (specify == GRAPHICAL_RENDITION_NO_STYLE)
                        return "NO_STYLE";

                    memcpy(specify_buffer, STR("Unreachable"));
                    if (specify & GRAPHICAL_RENDITION_BG_COLOR) {
                        memcpy(specify_buffer, STR("BG_COLOR"));
                        specify_buffer_i = 8;
                    }
                    if (specify & GRAPHICAL_RENDITION_FG_COLOR) {
                        if (specify_buffer_i != 0) {
                            memcpy(specify_buffer + specify_buffer_i, STR(" & FG_COLOR"));
                            specify_buffer_i += 11;
                        } else {
                            memcpy(specify_buffer + specify_buffer_i, STR("FG_COLOR"));
                            specify_buffer_i = 8;
                        }
                    }
                    if (specify & GRAPHICAL_RENDITION_STYLE_BOLD) {
                        if (specify_buffer_i != 0) {
                            memcpy(specify_buffer + specify_buffer_i, STR(" & STYLE_BOLD"));
                            specify_buffer_i += 13;
                        } else {
                            memcpy(specify_buffer + specify_buffer_i, STR("STYLE_BOLD"));
                            specify_buffer_i = 10;
                        }
                    }
                    if (specify & GRAPHICAL_RENDITION_STYLE_UNDERLINE) {
                        if (specify_buffer_i != 0) {
                            memcpy(specify_buffer + specify_buffer_i, STR(" & STYLE_UNDERLINE"));
                            specify_buffer_i += 18;
                        } else {
                            memcpy(specify_buffer + specify_buffer_i, STR("STYLE_UNDERLINE"));
                            specify_buffer_i = 15;
                        }
                    }
                    if (specify & GRAPHICAL_RENDITION_STYLE_BLINK) {
                        if (specify_buffer_i != 0) {
                            memcpy(specify_buffer + specify_buffer_i, STR(" & STYLE_BLINK"));
                            specify_buffer_i += 14;
                        } else {
                            memcpy(specify_buffer + specify_buffer_i, STR("STYLE_BLINK"));
                            specify_buffer_i = 11;
                        }
                    }
                    if (specify & GRAPHICAL_RENDITION_STYLE_INVERSE) {
                        if (specify_buffer_i != 0) {
                            memcpy(specify_buffer + specify_buffer_i, STR(" & STYLE_INVERSE"));
                            specify_buffer_i += 16;
                        } else {
                            memcpy(specify_buffer + specify_buffer_i, STR("STYLE_INVERSE"));
                            specify_buffer_i = 13;
                        }
                    }
                    if (specify & GRAPHICAL_RENDITION_STYLE_INVISIBLE) {
                        if (specify_buffer_i != 0) {
                            memcpy(specify_buffer + specify_buffer_i, STR(" & STYLE_INVISIBLE"));
                            specify_buffer_i += 18;
                        } else {
                            memcpy(specify_buffer + specify_buffer_i, STR("STYLE_INVISIBLE"));
                            specify_buffer_i = 15;
                        }
                    }
                    if (specify & GRAPHICAL_RENDITION_STYLE_ITALIC) {
                        if (specify_buffer_i != 0) {
                            memcpy(specify_buffer + specify_buffer_i, STR(" & STYLE_ITALIC"));
                            specify_buffer_i += 15;
                        } else {
                            memcpy(specify_buffer + specify_buffer_i, STR("STYLE_ITALIC"));
                            specify_buffer_i = 12;
                        }
                    }
                    if (specify & GRAPHICAL_RENDITION_STYLE_STRIKE_THROUGHT) {
                        if (specify_buffer_i != 0) {
                            memcpy(specify_buffer + specify_buffer_i, STR(" & STYLE_STRIKE_THROUGHT"));
                            specify_buffer_i += 24;
                        } else {
                            memcpy(specify_buffer + specify_buffer_i, STR("STYLE_STRIKE_THROUGHT"));
                            specify_buffer_i = 21;
                        }
                    }
                    return specify_buffer;
                case CSI_C_CURSOR_POSITION:
                    return "";
            }
            return "Unreachable";
        case ESC_CODE_CSI_PRIVATE:
            return "Unknown";
        case ESC_CODE_DCS:
            return "Unknown";
        case ESC_CODE_OSC:
            return "Unknown";
        case ESC_CODE_FE:
            return "Unknown";
    }
    return "Unreachable";
}

char metadata_buffer[256];
int metada_buffer_i = 0;

const char* get_metadata_repr(EscapeCode_Type type, int subtype, int specify, ESC_CODE_Data data) {
    switch (type) {
        case ESC_CODE_CSI:
        switch (subtype) {
                case CSI_C_GRAPHICAL_RENDITION:
                    snprintf(metadata_buffer, 255, "BG: (%d, %d, %d) | FG (%d, %d, %d)",
                             data.color[0], data.color[1], data.color[2],
                             data.color[3], data.color[4], data.color[5]);
                    return metadata_buffer;
                case CSI_C_CURSOR_POSITION:
                    snprintf(metadata_buffer, 255, "POSITION: (%d, %d)",
                            data.position[0], data.position[1]);
                    return metadata_buffer;
                case CSI_STATUS_REPORT:
                    switch (specify) {
                        case STATUS_REPORT_CURSOR_POSITION:
                            snprintf(metadata_buffer, 255, "(%d, %d)",
                                     data.position[0], data.position[1]);
                            return metadata_buffer;
                    }
                case CSI_P_CURSOR_EVENT:
                    switch (data.mouse_event.button) {
                        case MOUSE_BTN_LEFT: snprintf(metadata_buffer, 255, "<LEFT>(%d, %d)", data.mouse_event.position[0], data.mouse_event.position[1]); break;
                        case MOUSE_BTN_RIGHT: snprintf(metadata_buffer, 255, "<RIGHT>(%d, %d)", data.mouse_event.position[0], data.mouse_event.position[1]); break;
                        case MOUSE_BTN_MIDDLE: if (specify == MOUSE_WHEEL_DOWN || specify == MOUSE_WHEEL_UP) return ""; snprintf(metadata_buffer, 255, "<MIDDLE>(%d, %d)", data.mouse_event.position[0], data.mouse_event.position[1]); break;
                        case MOUSE_BTN_UNDEF: return "UNDEFINED";
                        default:    return "Unreachable";
                    }
                    return metadata_buffer;
                case CSI_FOCUS_REPORTED:
                case CSI_REQUEST_STATUS:
                    return "\b";
            }
    }
    return "Unreachable";
}

unsigned short get_number(char *buffer, short step) {
    unsigned short n = 0;
    for (unsigned short i = 0; i < step; i++) {
        n *= 10;
        n += buffer[i] - '0';
    }
    return n;
}

#define SAVE_BF_LEN 1024
char save_buffer[SAVE_BF_LEN];
int save_buffer_i = 0;

EscapeCode extract_escape_code(char *buffer, int *index) {
    EscapeCode code = {
        .type = ESC_CODE_Unknown,
        .specify = 0,
    };
    if (buffer[*index] != 27)
        return code;
    memset(save_buffer, 0, SAVE_BF_LEN);
    save_buffer_i = 0;
    *index = *index + 1;

    switch (buffer[*index]) {
        case '@':
        case 'H':
        case '#':
        case '%':
        case '\x20':
            *index = *index + 1;
        case '6':
        case '7':
        case '8':
        case '9':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'M':
        case 'N':
        case 'O':
        case 'V':
        case 'W':
        case 'Z':
        case 'c':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case '>':
        case '=':
        case '|':
        case '}':
        case '~':
            code.type = ESC_CODE_FE;
            *index = *index + 1;
            return code;
    }

    if (buffer[*index] == '[') {
        code.type = ESC_CODE_CSI;
        *index = *index + 1;
        if (buffer[*index] == '?') {
            code.type = ESC_CODE_CSI_PRIVATE;
            *index = *index + 1;
        } else if (buffer[*index] == '>') {
            code.type = ESC_CODE_CSI_PRIVATE;
            *index = *index + 1;
        }
    } else if (buffer[*index] == 'P') {
        code.type = ESC_CODE_DCS;
    } else if (buffer[*index] == ']') {
        code.type = ESC_CODE_OSC;
        *index = *index + 1;
    }

    char ch = buffer[*index];
    int N = 0;
    int custom_flag = 0;
    while (ch != '\0') {
        ch = buffer[*index];
        save_buffer[save_buffer_i] = ch;
        save_buffer_i++;
        if (code.type == ESC_CODE_OSC) {
            if (ch == 7 || (custom_flag == 1 && ch == '\\'))  {
                // NL; LOG("OSC: "); LOGr(save_buffer, save_buffer_i); NL;
                for (int i = 0; i < save_buffer_i; i++) {
                    if (save_buffer[i] == 7 || (save_buffer[i] == '\\' && custom_flag == 1) || save_buffer[i] == ';') {
                        unsigned short n = get_number(save_buffer + N, i - N);
                        switch (n) {
                            case 11: code.subtype = OSC_C_TERMINAL_BG; return code;
                        }
                        break;
                    }
                }
                return code;
            }
            if (ch == 27)
                custom_flag = 1;
            else 
                custom_flag = 0;
        } else if (code.type == ESC_CODE_CSI) {
            if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
                switch (ch) {
                    case 'I':
                        code.subtype = CSI_FOCUS_REPORTED;
                        code.specify = FOCUS_IN;
                        return code;
                    case 'O':
                        code.subtype = CSI_FOCUS_REPORTED;
                        code.specify = FOCUS_OUT;
                        return code;
                    case 'R':
                        code.subtype = CSI_STATUS_REPORT;
                        code.specify = STATUS_REPORT_CURSOR_POSITION;
#ifdef DEBUG
                    LOG("REPORT: "); LOG(save_buffer); NL;
#endif
                        for (int i = 0; i < save_buffer_i; i++) {
                            if (save_buffer[i] == ';' || save_buffer[i] == 'R') {
                                unsigned short n = get_number(save_buffer + N, i - N);
                                if (custom_flag == 0) {
                                    code.data.size[0] = n;
                                    custom_flag = 1;
                                } else {
                                    code.data.size[1] = n;
                                    return code;
                                }
                                N = i + 1;
                            }
                        }
                        return code;
                    case 'n':
                        code.subtype = CSI_REQUEST_STATUS; 
#ifdef DEBUG
                        LOG("REQUEST: "); LOG(save_buffer); NL;
#endif
                        unsigned short n = get_number(save_buffer, save_buffer_i - 1);
                        switch (n) {
                            case 6: code.specify = REQUEST_CURSOR_POSITION; break;
                        }
                        return code;
                    case 'H':
                        code.subtype = CSI_C_CURSOR_POSITION;
#ifdef DEBUG
                        LOG("CURSOR: "); LOG(save_buffer); NL;
#endif
                        for (int i = 0; i < save_buffer_i; i++) {
                            if (save_buffer[i] == ';' || save_buffer[i] == 'H') {
                                unsigned short n = get_number(save_buffer + N, i - N);
                                if (custom_flag == 0) {
                                    code.data.position[0] = n;
                                    custom_flag = 1;
                                } else {
                                    code.data.position[1] = n;
                                    return code;
                                }
                                N = i + 1;
                            }
                        }
                        return code;
                    case 'M':
                    case 'm':
                        if (save_buffer[0] == '<') {
                            code.subtype = CSI_P_CURSOR_EVENT;
#ifdef DEBUG
                            LOG("CURSOR BUFFER: "); LOG(save_buffer); NL;
#endif
                            N = 1;
                            for (int i = 1; i < save_buffer_i; i++) {
                                if (save_buffer[i] == ';' || save_buffer[i] == 'm' || save_buffer[i] == 'M') {
                                    unsigned short n = get_number(save_buffer + N, i - N);
#ifdef DEBUG
                                    LOG("NUMBER OF CURSOR: "); LOGN(n); NL;
#endif
                                    switch (custom_flag) {
                                        case 0:
                                            switch (n) {
                                                case 0: code.data.mouse_event.button = MOUSE_BTN_LEFT; break;
                                                case 1: code.data.mouse_event.button = MOUSE_BTN_MIDDLE; break;
                                                case 2: code.data.mouse_event.button = MOUSE_BTN_RIGHT; break;
                                                case 32: code.data.mouse_event.button = MOUSE_BTN_UNDEF; code.specify = MOUSE_DRAG; return code;
                                                case 64: code.data.mouse_event.button = MOUSE_BTN_MIDDLE; code.specify = MOUSE_WHEEL_UP; return code;
                                                case 65: code.data.mouse_event.button = MOUSE_BTN_MIDDLE; code.specify = MOUSE_WHEEL_DOWN; return code;
                                                default: code.data.mouse_event.button = 500; break;
                                            }
                                            custom_flag = 1;
                                            break;
                                        case 1: code.data.mouse_event.position[0] = n; custom_flag = 2; break;
                                        case 2: code.data.mouse_event.position[1] = n; custom_flag = 3; break;
                                    }
                                    N = i + 1;
                                }
                            }
                            if (ch == 'm') code.specify = MOUSE_RELEASE;
                            else if (ch == 'M') code.specify = MOUSE_PRESS;
                            return code;
                        }
                        code.subtype = CSI_C_GRAPHICAL_RENDITION;
                        for (int i = 0; i < save_buffer_i; i++) {
                            if (save_buffer[i] == ';' || save_buffer[i] == 'm') {
                                unsigned short n = get_number(save_buffer + N, i - N);
                                switch (custom_flag) {
                                    case 2: custom_flag = 3; code.data.color[0] = n; break;
                                    case 3: custom_flag = 4; code.data.color[1] = n; break;
                                    case 4: custom_flag = 0; code.data.color[2] = n; break;
                                    case 7: custom_flag = 8; code.data.color[0] = n; break;
                                    case 8: custom_flag = 9; code.data.color[1] = n; break;
                                    case 9: custom_flag = 0; code.data.color[2] = n; break;
                                }
                                if (custom_flag > 1) {
                                    N = i + 1;
                                    continue;
                                }
                                switch (n) {
                                    case 0:
                                        code.specify = GRAPHICAL_RENDITION_NO_STYLE;
                                        break;
                                    case 1:
                                        code.specify |= GRAPHICAL_RENDITION_STYLE_BOLD;
                                        break;
                                    case 2:
                                        if (custom_flag == 1)
                                            custom_flag = 2;
                                        if (custom_flag == 6)
                                            custom_flag = 7;
                                        break;
                                    case 22: break;
                                    case 3:
                                        code.specify |= GRAPHICAL_RENDITION_STYLE_ITALIC;
                                        break;
                                    case 23: break;
                                    case 4:
                                        code.specify |= GRAPHICAL_RENDITION_STYLE_UNDERLINE;
                                        break;
                                    case 24: break;
                                    case 5:
                                        code.specify |= GRAPHICAL_RENDITION_STYLE_BLINK;
                                        break;
                                    case 25: break;
                                    case 7:
                                        code.specify |= GRAPHICAL_RENDITION_STYLE_INVERSE;
                                        break;
                                    case 27: break;
                                    case 8:
                                        code.specify |= GRAPHICAL_RENDITION_STYLE_INVISIBLE;
                                        break;
                                    case 28: break;
                                    case 9:
                                        code.specify |= GRAPHICAL_RENDITION_STYLE_STRIKE_THROUGHT;
                                        break;
                                    case 29: break;
                                    case 30: code.specify |= GRAPHICAL_RENDITION_BG_COLOR; break;
                                    case 31: code.specify |= GRAPHICAL_RENDITION_BG_COLOR; code.data.color[0] = 255; break;
                                    case 32: code.specify |= GRAPHICAL_RENDITION_BG_COLOR; code.data.color[1] = 255; break;
                                    case 33: code.specify |= GRAPHICAL_RENDITION_BG_COLOR; code.data.color[0] = 255; code.data.color[1] = 255; break;
                                    case 34: code.specify |= GRAPHICAL_RENDITION_BG_COLOR; code.data.color[2] = 255; break;
                                    case 35: code.specify |= GRAPHICAL_RENDITION_BG_COLOR; code.data.color[0] = 255; code.data.color[2] = 255; break;
                                    case 36: code.specify |= GRAPHICAL_RENDITION_BG_COLOR; code.data.color[1] = 255; code.data.color[2] = 255; break;
                                    case 37: code.specify |= GRAPHICAL_RENDITION_BG_COLOR; code.data.color[0] = 255; code.data.color[1] = 255; code.data.color[2] = 255; break;
                                    case 38: code.specify |= GRAPHICAL_RENDITION_BG_COLOR; custom_flag = 1; break;
                                    case 40: code.specify |= GRAPHICAL_RENDITION_FG_COLOR; break;
                                    case 41: code.specify |= GRAPHICAL_RENDITION_FG_COLOR; code.data.color[3] = 255; break;
                                    case 42: code.specify |= GRAPHICAL_RENDITION_FG_COLOR; code.data.color[4] = 255; break;
                                    case 43: code.specify |= GRAPHICAL_RENDITION_FG_COLOR; code.data.color[3] = 255; code.data.color[4] = 255; break;
                                    case 44: code.specify |= GRAPHICAL_RENDITION_FG_COLOR; code.data.color[5] = 255; break;
                                    case 45: code.specify |= GRAPHICAL_RENDITION_FG_COLOR; code.data.color[3] = 255; code.data.color[5] = 255; break;
                                    case 46: code.specify |= GRAPHICAL_RENDITION_FG_COLOR; code.data.color[4] = 255; code.data.color[5] = 255; break;
                                    case 47: code.specify |= GRAPHICAL_RENDITION_FG_COLOR; code.data.color[3] = 255; code.data.color[4] = 255; code.data.color[5] = 255; break;
                                    case 48: code.specify |= GRAPHICAL_RENDITION_FG_COLOR; custom_flag = 6; break;
                                    case 49: break;
                                }
                                N = i + 1;
                            }
                        }
                        // NL; LOG("Buffer["); LOGc(save_buffer[0]); LOG("]: "); LOG(save_buffer); LOG(" .. ");
                        // LOG_ESC(code); NL;
                        break;
                    default:
#ifdef DEBUG
                        LOG("  Buffer: "); LOG(save_buffer); NL;
#endif
                        break;
                }
                return code;
            }
            switch (ch) {
                case '@':
                case ']':
                case '^':
                case '`':
                case '{':
                case '|':
                case '}':
                case '~':
                    return code;
            }
        } else if (code.type == ESC_CODE_CSI_PRIVATE) {
            if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
                switch (ch) {
                    case 'c':
                        code.subtype = CSI_P_CURSOR_STYLE;
                        for (int i = 0; i < save_buffer_i; i++) {
                            if (save_buffer[i] == 'c' || save_buffer[i] == ';') {
                                // unsigned short n = get_number(save_buffer + N, i - N);
                            }
                        }

                        break;
                }
                return code;
            }
            switch (ch) {
                case '@':
                case ']':
                case '^':
                case '`':
                case '{':
                case '|':
                case '}':
                case '~':
                    return code;
            }
        } else if (code.type == ESC_CODE_DCS) {
            if (ch == '\\') {
                return code;
            }
        } else if (code.type == ESC_CODE_OSC) {
            if (ch == '\\') {
                return code;
            }
        }
        *index = *index + 1;
    }

    return code;
}

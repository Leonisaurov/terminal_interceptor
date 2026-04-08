#include "debug.h"
int logFile = -1;

void closeLog() {
    if (logFile == -1) {
        return;
    }
    close(logFile);
}

void init() {
    if (logFile != -1) {
        return;
    }
    logFile = open("log.txt", O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    atexit(closeLog);
}

void LOGrr(const char* txt, unsigned int n) {
    init();
    write(logFile, txt, n);
}

void LOGc(char ch) {
    switch (ch) {
        case 27: LOGrr(STR("<ESC>")); return;
        case 13: case 10: LOGrr(STR("\\n")); return;
        case 7: LOGrr(STR("<BELL>")); return;
        default:
            if (ch < 30) {
                LOGrr(STR("<"));
                LOGN(ch);
                LOGrr(STR(">"));
                return;
            }
    }
    LOGrr(&ch, 1);
}

#define STR(txt) txt, strlen(txt)

void LOGr(const char* txt, unsigned int n) {
    for (unsigned int i = 0; i < n; i++) {
        LOGc(txt[i]);
    }
}

void LOG(const char* txt) {
    LOGr(txt, strlen(txt));
}

#ifdef LEO_ISA_STR_LIB
void LOGS(const String str) {
    LOGr(str->content, str->length);
}
#endif

void LOGN(long long number) {
    char buffer[19];
    LOGr(buffer, snprintf(buffer, 19, "%lld", number));
}

void LOGp(void* pointer) {
    char buffer[20];
    LOGr(buffer, snprintf(buffer, 20, "%p", pointer));
}

#ifdef SHEREDOM_HASHMAP_H_INCLUDED
unsigned int indentation = 0;
int hashmap_iterator(void* const context, struct hashmap_element_s *element) {
    void (*logFunction)(void* const key, void* const content) = context;
    for (unsigned int i = 0; i < indentation; i++)
        LOG("\t");

    void* const key = (void*const) element->key;
    void* const content = (void*const)element->data;
    logFunction(key, content); NL;
    return 1;
}

void LOGhashmap(struct hashmap_s *hashmap, void (*logFunction)(void* const key, void* const content)) {
    for (unsigned int i = 0; i < indentation; i++)
        LOG("\t");
    LOG("{"); NL;

    indentation += 1;
    hashmap_iterate_pairs(hashmap, hashmap_iterator, logFunction);

    for (unsigned int i = 0; i < indentation; i++)
        LOG("\t");
    LOG("}"); NL;
}
#endif

void TODO() {
    LOG("Not handled yet!"); exit(-1);
}

#ifdef KEYEXPAND_ESC_CODES
void LOG_ESC(EscapeCode code) {
    const char* code_type = get_type_repr(code.type);
    const char* code_subtype = get_subtype_repr(code.type, code.subtype);
    const char* code_specify = get_specify_repr(code.type, code.subtype, code.specify);
    const char* code_metadata = get_metadata_repr(code.type, code.subtype, code.specify, code.data);

    LOG("ESCAPE_CODE["); LOG(code_type); LOG("]<"); LOG(code_subtype); LOG(">: "); LOG(code_specify); LOG(" "); LOG(code_metadata);
}
#endif

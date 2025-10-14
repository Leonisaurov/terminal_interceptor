#ifndef KEYEXPAND_DEBUG
#define KEYEXPAND_DEBUG

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>
#include <asm-generic/fcntl.h>

#include "string.h"

#define NL LOG("\n")
#define STR(txt) txt, strlen(txt)

void LOGr(const char* txt, unsigned int n);
void LOG(const char* txt);
void LOGN(long long number);
void LOGp(void* pointer);

#ifdef LEO_ISA_STR_LIB
#define STRL(str) str->content, str->length
void LOGS(String str);
#endif

#ifdef SHEREDOM_HASHMAP_H_INCLUDED
#include "hashmap.h"
void LOGhashmap(struct hashmap_s *hashmap, void (*logFunction)(void* key, void* content));
#endif

void TODO();

#endif

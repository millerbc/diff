#ifndef util_h
#define util_h

#include <stdio.h>
#define BUFLEN 256

char* yesorno(int condition);
FILE* openfile(const char* filename, const char* openflags);

void printleft(const char* left);
void printleftmarked(const char* left);
void printleftonly(const char* left);
void printright(const char* right);
void printrightmarked(const char* right);
void printboth(const char* left_right);
void print_normalleft(const char* left);
void print_normalright(const char* right);

void printline(void);

#endif /* util_h */

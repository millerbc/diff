#ifndef diff_
#define diff_h

void function_calls(para* p, para* q, const char* filename1, const char* filename2);
void version(void);
void brief(para* p, para* q, const char* filename1, const char* filename2);
void identical(para* p, para* q, const char* filename1, const char* filename2);
void print_sidebyside(para* p, para* q);
void print_leftcolumn(para* p, para* q);
void print_context(para* p, para* q);
void print_unified(para*p, para* q);
char* diff_last_modified(const char* filename);


#endif /* diff.h */

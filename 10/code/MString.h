extern int get_filename(char *source, char *destination);
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#define MAX_LINE 256

extern char *strrev(char *str);
extern int CmpSuffix(char *filename, char *suffix);
extern int split_line(char *line, char **character);
extern int is_annotation(char *line,FILE *file);
extern int find_string(char *string, char **source);
extern void squeeze(char *s, char c);
extern void Jacktokenizer(FILE *, FILE *);
extern void get_spilt(char s[], char d[], int n);
extern void compilation(FILE *filein, FILE *fileout);
extern void init_fgets_buff();
int fgets_buff(char *line, int size, FILE *file);
void unfgets_buff(char *line, int size);
void free_buffer();


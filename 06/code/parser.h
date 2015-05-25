#define A_COMMAND 0
#define C_COMMAND 1
#define L_COMMAND 2
#define ANNOTATION 3
#define UNKNOW 4

typedef int COMMAND_TYPE;

#define MAX_DEST_LEN 5
#define MAX_COMP_LEN 5
#define MAX_JUMP_LEN 5
#define MAX_CODE_LEN 16
#define MAX_LEN 100

COMMAND_TYPE commandType(char string[]);
void symbol(char p[],char s[]);
void dest(char p[],char s[]);
void comp(char p[],char s[]);
void jump(char p[],char s[]);
void code_dest(char p[],char code[]);
void code_jump(char p[],char code[]);
void code_comp(char p[],char code[]);





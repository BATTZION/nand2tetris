#include <stdio.h>
#include "parser.h"
#include <string.h>
#define MAX_ARG 50
#define MAX_LINE 256
#define MAX_FILE 10
char *Arithmetic[]={"add","sub","neg","eq","gt","lt","and","or","not",NULL};
char *Push[]={"push",NULL};
char *Pop[]={"pop",NULL};
char *Label[]={"label",NULL};
char *Goto[]={"goto",NULL};
char *If[]={"if-goto",NULL};
char *Function[]={"function",NULL};
char *Return[]={"return",NULL};
char *Call[]={"call",NULL};
char *Annotation[]={"//",NULL};

char filename[25];

void get_spilt(char source[], char dest[], int n);
int have_string(char s[],char *d[]);
COMMAND_TYPE GetCommandType(char s[]);
void GetArg(char source[], char dest[], COMMAND_TYPE type, int n);
void GetFilename(char source[]);
char *strrev(char *str);
void get_spilt(char s[], char d[], int n)
{
	char temp[MAX_ARG];
	int i=0,j=0;
	while(n>0){
		j=0;
		memset(temp,0,sizeof(temp));
		while(s[i]==' ')
		  i++;
		while(s[i]!=' ' && s[i]!='\r' && s[i]!='\n')
		  temp[j++]=s[i++];
		temp[j]=0;
		n--;
	}
	strcpy(d,temp);
}
int have_string(char s[], char *d[])
{
	int i=0,flag=0;
	while(d[i] != NULL){
		if(strcmp(d[i],s)==0){
			flag=1;
			break;
		}
		i++;
	}
	return flag;
}
COMMAND_TYPE GetCommandType(char s[])
{
	int i=0;
	char head[MAX_ARG];
	memset(head,0,sizeof(head));
	get_spilt(s,head,1);
	if(have_string(head,Arithmetic))
	  return C_ARITHMETIC;                         //finished
	if(have_string(head,Push)) 
	  return C_PUSH;                               //finished
	if(have_string(head,Pop))
	  return C_POP;                                 //finished
	if(have_string(head,Label))  
	  return C_LABEL;
	if(have_string(head,Goto))
	  return C_GOTO;
	if(have_string(head,If))
	  return C_IF;
	if(have_string(head,Function))
	  return C_FUNCTION;
	if(have_string(head,Return))
	  return C_RETURN;
	if(have_string(head,Call))
	  return C_CALL;
	if(have_string(head,Annotation))
	  return C_ANNOTATION;                      //finished
	else
	  return C_UNKNOW;                           //finished

}
void GetArg(char s[], char d[], COMMAND_TYPE type, int n)
{
	switch(type){
		case C_ARITHMETIC :
			get_spilt(s,d,1);
			break;
		case C_RETURN :
			get_spilt(s,d,1);
			break;
		default:
			get_spilt(s,d,n+1);
			break;
	}
}
void GetFilename(char source[])
{
	char *temp,*f;
	int len;
	f = filename;
	if ( (temp = strstr(source, "/")) == NULL){
		temp = source;
		while (*temp != '.')
		  *f++ = *temp++;
		*f = 0;
	}
	else {
		temp = source;
		len = strlen(source);
		while (temp[len] != '/' )
		  len--;
		len++;
		while ( temp[len] != '.')
		  *f++ = temp[len++];
		*f++ = temp[len];
		*f = 0;
	}
}
void parser(FILE *file, FILE *file1)
{
	COMMAND_TYPE TYPE;
	char line[MAX_ARG];
	char arg1[MAX_ARG];
	char arg2[MAX_ARG];
	memset(line,0,sizeof(line));
	memset(arg1,0,sizeof(arg1));
	memset(arg2,0,sizeof(arg2));

	while(fgets(line,MAX_LINE,file)!=NULL){
		TYPE=GetCommandType(line);
		switch (TYPE) {
			case C_ARITHMETIC:
				GetArg(line,arg1,TYPE,1);
				CodeArithmetic(arg1,file1);
				memset(arg1,0,sizeof(arg1));
				break;
			case C_POP:
			case C_PUSH:
				GetArg(line,arg1,TYPE,1);
			    GetArg(line,arg2,TYPE,2);
	            CodePushPop(TYPE,arg1,arg2,file1);
	            memset(arg1,0,sizeof(arg1));
 	            memset(arg2,0,sizeof(arg2));
				break;
			case C_FUNCTION:
				GetArg(line,arg1,TYPE,1);
				GetArg(line,arg2,TYPE,2);
				CodeFunction(arg1,arg2,file1);
				memset(arg1,0,sizeof(arg1));
				memset(arg2,0,sizeof(arg2));
				break;
			case C_GOTO:
				GetArg(line,arg1,TYPE,1);
				CodeGoto(arg1,file1);
				memset(arg1,0,sizeof(arg1));
				break;
			case C_IF:
				GetArg(line,arg1,TYPE,1);
				CodeIf(arg1,file1);
				memset(arg1,0,sizeof(arg1));
				break;
			case C_CALL:
				GetArg(line,arg1,TYPE,1);
				GetArg(line,arg2,TYPE,2);
				CodeCall(arg1,arg2,file1);
		        memset(arg1,0,sizeof(arg1));
				memset(arg2,0,sizeof(arg2));
				break;
			case C_RETURN:
				CodeReturn(file1);
				break;
			case C_LABEL:
				GetArg(line,arg1,TYPE,1);
				CodeLabel(arg1,file1);
				memset(arg1,0,sizeof(arg1));
				break;
			default:
				break;
		}
	}
}
int main(int argc, char *argv[])
{
	FILE *file[MAX_FILE];
	char line[MAX_LINE];
	int i=0;
	if(argc != 3){
		while (i < argc -2 ) {
			if((file[i]=fopen(argv[i+1],"r"))==NULL){
				printf("error in open file %d \n", i+1);
				return 1;
			}
			i++;
		}
		if((file[argc -2] = fopen(argv[argc - 1], "a+")) == NULL){
			printf("error in open file %d \n",argc -1);
			return 1;
		}
		CodeInit(file[argc - 2]);
		i = 0;
		while (i < argc - 1){
			GetFilename(argv[i + 1]);
			parser(file[i++],file[argc - 2]);
		}
		i = 0;
		while (i < argc - 1)
		  fclose(file[i++]);
	}
	else{
		if((file[0]=fopen(argv[1],"r"))==NULL){
			printf("error in open file \n");
			return 1;
		}
		if((file[1]=fopen(argv[2],"a+"))==NULL){
			printf("error in open file1\n");
			return 1;
		}
		parser(file[0],file[1]);
		fclose(file[0]);
		fclose(file[1]);
	}
	return 0;
}

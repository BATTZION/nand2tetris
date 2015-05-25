#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include <string.h>
#include <malloc.h>
#include "hashtable.h"

char *strrev(char *str)  
{  
      char *p1, *p2;  
  
      if (! str || ! *str)  
            return str;  
      for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2) {  
            *p1 ^= *p2;  
            *p2 ^= *p1;  
            *p1 ^= *p2;  
      }  
      return str;  
}
void translate(int num,char s[])
{
	int i=0;
	while(num){
		*(s+i)=num % 2 + '0';
		num /=2;
		i++;
	}
	while(i<MAX_CODE_LEN-1)
	s[i++]='0';
	s[i]=0;
	strrev(s);
}
int is_number(char s[])
{
	int i=0,flage=1;
	while(s[i]!='\0' && s[i]!='\r' && s[i]!='\n' && s[i]!='/')
	  if(s[i]<'0'||s[i]>'9'){
        flage=0;
		break;
	  }
	  else
		i++;
	return flage;
}
COMMAND_TYPE commandType(char string[])
{
	int i=0;
	char *p = string;
	while(p[i]==' ')
	  i++;
	if(p[i]=='@')
	  return A_COMMAND;
	else if(p[i]=='(')
	  return L_COMMAND;
	else if(p[i]=='/' && p[i+1]=='/')
	  return ANNOTATION;
	else if((p[i]<='Z' && p[i]>='A') || (p[i]<='9' && p[i]>='0'))
	  return C_COMMAND;
	else 
	  return UNKNOW;
}
void symbol(char p[],char s[])
{
	int i=0,num=0,j=0;
	while(p[i]==' ')
	  i++;
	if(p[i]=='@'){
		while(p[i]==' ')
		  i++;
		while(p[i]!='\n'&& p[i]!='\r' &&p[i]!= '/')
		  if(p[i]!=' '&&p[i]!='@')
			s[j++]=p[i++];
		  else
			i++;
		s[j]=0;
		i=0;
		num=atoi(s);
		if(is_number(s)==0)
		  return;
		else
		  translate(num,s);
	}
	else{
		while(p[i]!=')')
		  if(p[i]!=' ' && p[i]!='(')
			s[j++]=p[i++];
		  else
			i++;
		s[j]=0;
	}
	return;
}
void dest(char p[],char s[])
{
	int i=0,j=0;
	char *tmp=strstr(p,"=");
	if(tmp==NULL){
		strcpy(s,"null");
	}
	else{
		while(p[i]==' ')
		  i++;
		while( p+i<tmp && p[i]!=' ')
		  s[j++]=p[i++];
		s[j]=0;
	}
	return;
}
void comp(char p[],char s[])
{
	int i=0,j=0;
	char *tmp =strstr(p,"=");
	if(tmp==NULL){
		while(p[i]==' ')
		  i++;
		while(p[i]!=';'){
		   if(p[i]!=' ')
			 s[j++]=p[i++];
		   else
			 i++;
		}
		s[j]='\0';
	}
	else{
		tmp++;
		while(tmp[i] != ';' && tmp[i] != '\n' && tmp[i] != '\r' && tmp[i] != '/')
		  if(tmp[i]!=' ')
			s[j++]=tmp[i++];
		  else
			i++;
		s[j]='\0';
	}
	return;

}
void jump(char p[],char s[])
{
	int i=1,j=0;
	char *tmp=strchr(p,';');
	if(tmp!=NULL){
	  while(tmp[i]!='\n' && tmp[i]!= '/' && tmp[i]!='\r')
		if(tmp[i]!=' ')
		  s[j++]=tmp[i++];
		else
		  i++;
	  s[j]='\0';
	}
	else
	  strcpy(s,"null");
	return;
	
}
int parser(char in_path[], char out_path[], HashTable *list)
{
	char s[MAX_LEN];
	char code_s[MAX_CODE_LEN+1];
	COMMAND_TYPE type;
	FILE *file=NULL,*file1=NULL;
	file=fopen(in_path,"r");
	file1=fopen(out_path,"a+");
	if(file==NULL||file1==NULL){
		printf("open fail");
		return -1;
	}
	memset(s, '\0', sizeof(s)-1);
	memset(code_s,'\0',sizeof(code_s)-1);
	while(fgets(s,MAX_LEN,file)!=NULL){
		type=commandType(s);
		switch(type){
			case A_COMMAND:
				code_a_command(s,code_s,list);
				fputs(code_s,file1);
				break;
			case C_COMMAND:
				code_c_command(s,code_s);
				fputs(code_s,file1);
				break;
			case L_COMMAND:
				break;
			case ANNOTATION:
				break;
			default:
				break;
		}
	memset(s,'\0',sizeof(s)-1);
	memset(code_s,'\0',sizeof(code_s)-1);
	}
	fclose(file);
	fclose(file1);
	return 1;
}
int  built_symbol_list(char path[], HashTable *list)
{
	int real_line=0;
	FILE *file=NULL;
	char symbol_of_s[5];
	if((file=fopen(path,"r"))==NULL)
	  return -1;
	char s[MAX_LEN],address[MAX_CODE_LEN];
	COMMAND_TYPE type;
	memset(s, '\0', sizeof(s)-1);
	memset(address, '\0', sizeof(address)-1);
	while(fgets(s,MAX_LEN,file)!=NULL){
			type=commandType(s);
		switch(type){
			case A_COMMAND:
				real_line++;
				break;
			case C_COMMAND:
				real_line++;
				break;
			case L_COMMAND:
				   symbol(s,symbol_of_s);
				   if(hash_exists(list,symbol_of_s)!=EXISTS)
					   hash_add(list,symbol_of_s,real_line);
				   break;
			case ANNOTATION:
				break;
			default:
				break;
		}
	memset(s, '\0', sizeof(address)-1);
	memset(address, '\0', sizeof(address)-1);
	memset(symbol_of_s,'\0',sizeof(symbol_of_s)-1);
	}
}
void init_symbol_table(HashTable *list)
{
	hash_add(list,"SP",0);
	hash_add(list,"LCL",1);
	hash_add(list,"ARG",2);
	hash_add(list,"THIS",3);
	hash_add(list,"THAT",4);
	//regeister
    hash_add(list,"R0",0);
    hash_add(list,"R1",1);
	hash_add(list,"R2",2);
    hash_add(list,"R3",3);
    hash_add(list,"R4",4);
    hash_add(list,"R5",5);
    hash_add(list,"R6",6);
    hash_add(list,"R7",7);
    hash_add(list,"R8",8);
    hash_add(list,"R9",9);
    hash_add(list,"R10",10);
    hash_add(list,"R11",11);
    hash_add(list,"R12",12);
    hash_add(list,"R13",13);
    hash_add(list,"R14",14);
    hash_add(list,"R15",15);
    hash_add(list,"SCREEN",0x4000);
	hash_add(list,"KBD",0x6000);
}
int main(int argc,char *argv[])
{
   	HashTable *list = create_hashtable(80000, char*,int);
	init_symbol_table(list);
    if(list==NULL){
	  printf("error!");
	  return -1;
	}
	built_symbol_list(argv[1],list);
	if(parser(argv[1], argv[2],list)==-1)
	  printf("error");
	hash_free(list);
	return  0;
}


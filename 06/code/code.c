#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "parser.h"
#include "hashtable.h"

char *DEST[]={"null","M","D","MD","A","AM","AD","AMD"};
char *CODE_DEST[]={"000","001","010","011","100","101","110","111"};
char *JUMP[]={"null","JGT","JEQ","JGE","JLT","JNE","JLE","JMP"};
char *CODE_JUMP[]={"000","001","010","011","100","101","110","111"};
char *COMP[]={"0","1","-1","D","A","!D","!A","-D","-A","D+1","A+1","D-1","A-1","D+A","D-A","A-D","D&A","D|A","M","!M","-M","M+1","M-1","D+M","D-M","M-D","D&M","D|M"};
char *CODE_COMP[]={"0101010","0111111","0111010","0001100","0110000","0001101","0110001","0001111","0110011","0011111","0110111","0001110","0110010","0000010","0010011","0000111","0000000","010101","1110000","1110001","1110011","1110111","1110010","1000010","1010011","1000111","1000000","1010101"};

void code_dest(char p[],char code[])
{
	int i=0;
	while(strcmp(p,*(DEST+i))!=0)
	  i++;
	strcat(code,*(CODE_DEST+i));
     
}
void code_jump(char p[],char code[])
{
	int i=0;
	while(strcmp(p,*(JUMP+i))!=0)
	  i++;
	strcat(code,*(CODE_JUMP+i));
}
void code_comp(char p[],char code[])
{
	int i=0;
	while(strcmp(p,*(COMP+i))!=0)
	  i++;
	strcat(code,*(CODE_COMP+i));

}
void code_a_command(char source[], char code[], HashTable *list)
{
	static reg=16;
	int number;
	char address[MAX_CODE_LEN];
	char symbol_of_source[5];
	strcat(code,"0");
	symbol(source,symbol_of_source);
	if(is_number(symbol_of_source))
	  strcat(code,symbol_of_source);
	else{
	  if(hash_find(list,symbol_of_source,&number)==SUCCESS)
		translate(number,address);
	  else{
		number=reg++;
		hash_add(list,symbol_of_source,number);
		translate(number,address);
	  }
	  strcat(code,address);
	}
	code[MAX_CODE_LEN]='\n';
	code[MAX_CODE_LEN+1]='\0';
    
}
void code_c_command(char source[], char code[])
{
	strcat(code,"111");
	char comp_of_source[5];
	comp(source,comp_of_source);
	char dest_of_source[5];
	dest(source,dest_of_source);
	char jump_of_source[5];
	jump(source,jump_of_source);
	code_comp(comp_of_source,code);
	code_dest(dest_of_source,code);
	code_jump(jump_of_source,code);
	code[MAX_CODE_LEN]='\n';
	code[MAX_CODE_LEN+1]='\0';
}


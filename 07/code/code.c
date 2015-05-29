#include <stdio.h>
#include <string.h>
#include "parser.h"


#define MAX_ARG 50
#define SP          "@SP\n"
#define ADDSP       "@SP\nM=M+1\n"
#define DECSP       "@SP\nM=M-1\n"
#define DEA         "D=A\n"
#define AEM         "A=M\n"
#define MED         "M=D\n"
#define ADEAaddD    "AD=D+A\n"
#define DEAaddD     "D=D+A\n"
#define R13         "@R13\n"
#define DEM         "D=M\n"
#define POPTOD      "@SP\nM=M-1\n@SP\nA=M\nD=M\n"
#define POPTOA      "@SP\nM=M-1\n@SP\nA=M\nA=M\n"
#define PUSHFROMD   "@SP\nA=M\nM=D\n@SP\nM=M+1\n"
#define GETSP       "@SP\nA=M\n"
char *JUMPS[]={"D;JEQ\n","D;JGT\n","D;JLT\n"};
extern char *Arithmetic[];
char *operation[]={"D=D+A\n","D=A-D\n","D=-D\n","D=A-D\n","D=A-D\n","D=A-D\n","D=A&D\n","D=A|D\n","D=!D\n",NULL};
void CodeArithmetic(char operator[], FILE *file);
void CodePushPop(COMMAND_TYPE type, char segment[], char index[], FILE *file);
void CodePop(char segment[], char index[], FILE *file);
void CodePush(char segment[], char index[], FILE *file);
void GetSegment(char asmcode[], char segment[]);
char *strrev(char *str);
void translate(int num, char s[]);
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
void translate(int num,char s[])       //change number to string           
{
	int i=0;
	while(num){
		*(s+i)=num % 10 + '0';
		num /=10;
		i++;
	}
	strrev(s);
}
void CodeArithmetic(char operator[], FILE * file)
{
	int static counter1=1;
	int static counter2=2;
	int op=0;
	char number[MAX_ARG];
	char label1[MAX_ARG];
	char label2[MAX_ARG];
	memset(number,0,sizeof(number));
	memset(label2,0,sizeof(number));
	memset(label1,0,sizeof(number));
	while(Arithmetic[op]!=NULL){
		if(!strcmp(Arithmetic[op],operator))
		  break;
		else
		  op++;
	}
	switch(op){
		case 0:   //add
		case 1:   //sub
		case 6:   //and
		case 7:   //or
			fputs(POPTOD,file);             //pop D
			fputs(POPTOA,file);             //pop A
			fputs(operation[op],file);      //operation
			fputs(PUSHFROMD,file);          //push D
			break;
		case 3:  //eq
 		case 4:  //qt
		case 5:  //lt
			fputs(POPTOD,file);          //POP D
			fputs(POPTOA,file);          //POP A
			fputs(operation[op],file);   //D=A-D

			strcat(label1,"LABEL");
			translate(counter1,number);
			strcat(label1,number);
			fprintf(file,"@%s\n",label1);
			fputs(JUMPS[op-3],file);                       //D;JEQ or D;JGT or D;JLT
			memset(number,0,sizeof(number));
            //@SP
			//A=M
			//M=0
			fputs("@SP\nA=M\nM=0\n",file);
			//@LABEL2
			//0;JMP
			//(LABEL1)
			strcat(label2,"LABEL");
			translate(counter2,number);
			strcat(label2,number);
			fprintf(file,"@%s\n",label2);
			memset(number,0,sizeof(number));
			fputs("0;JMP\n",file);      //0;JMP
			fprintf(file,"(%s)\n",label1);  //(LABEL1)
			//@SP
			//A=M
			//M=-1
			fputs("@SP\nA=M\nM=-1\n",file);
			//(LABEL2)
			//@SP
			//M=M+1
			fprintf(file,"(%s)\n",label2);
			fputs("@SP\nM=M+1\n",file);
			counter1+=2;
			counter2+=2;
			break;
	    case 2:
		case 8:
			fputs(POPTOD,file);    //POP D
			fputs(operation[op],file);
			fputs(PUSHFROMD,file);
			break;
		default:
			break;
  	}



}
void GetSegment(char asmcode[], char segment[])
{
	if(!strcmp(segment,"local")){
		strcat(asmcode,"@LCL\n");
		return;
	}
	if(!strcmp(segment,"argument")){
		strcat(asmcode,"@ARG\n");
		return;
	}
	if(!strcmp(segment,"this")){
		strcat(asmcode,"@THIS\n");
		return;
	}
	if(!strcmp(segment,"that")){
		strcat(asmcode,"@THAT\n");
		return;
	}
	if(!strcmp(segment,"temp")){
		strcat(asmcode,"@5\n");
		return;
	}
	if(!strcmp(segment,"pointer")){
		strcat(asmcode,"@3\n");
		return;
	}

}
void CodePushPop(COMMAND_TYPE type, char segment[], char index[], FILE *file)
{
	if(type==C_PUSH)
		CodePush(segment,index,file);
	else
	  CodePop(segment,index,file);
}
void CodePush(char segment[], char index[], FILE *file)
{
	char asmcode[MAX_ARG];
	memset(asmcode,0,sizeof(asmcode));
	if(strcmp(segment,"constant")==0){         //segment is constant
		strcat(asmcode,"@");         
	    strcat(asmcode,index);
	    strcat(asmcode,"\n");  

	    fputs(asmcode,file);              //@number
	    fputs(DEA,file);                  //D=A

	    fputs(PUSHFROMD,file);
	}
	else if(strcmp(segment,"pointer")==0 ||strcmp(segment,"temp")==0){
	    //@index
		//D=A
		strcat(asmcode,"@");
		strcat(asmcode,index);
		strcat(asmcode,"\n");
		fputs(asmcode,file);
		fputs(DEA,file);
		memset(asmcode,0,sizeof(asmcode));
		//@segment
		//AD=A+D
		//D=M
		GetSegment(asmcode,segment);
		fputs(asmcode,file);
		fputs(ADEAaddD,file);
		fputs(DEM,file);
	    // PUSH D
		fputs(PUSHFROMD,file);

	}
	else if(strcmp(segment,"static")==0){
		//@static.index
		//D=M;
		strcat(asmcode,"@");
		strcat(asmcode,"static.");
		strcat(asmcode,index);
		strcat(asmcode,"\n");
		fputs(asmcode,file);
		memset(asmcode,0,sizeof(asmcode));
		fputs(DEM,file);
		//POSH D
		fputs(PUSHFROMD,file);
	}
	else{
		//@index
		//D=A
		strcat(asmcode,"@");
		strcat(asmcode,index);
		strcat(asmcode,"\n");
		fputs(asmcode,file);
		fputs(DEA,file);
		memset(asmcode,0,sizeof(asmcode));
		//@segement
		//A=M
		//AD=D+A
		//D=M
		GetSegment(asmcode,segment);
		fputs(asmcode,file);
		fputs(AEM,file);
		fputs(ADEAaddD,file);
		fputs(DEM,file);
		//PUSH D
		fputs(PUSHFROMD,file);

	}


}
void CodePop(char segment[], char index[], FILE *file)
{
	char asmcode[MAX_ARG];
	memset(asmcode,0,sizeof(asmcode));
	if(strcmp(segment,"static")==0){
		//POP D
		fputs(POPTOD,file);
		//@static.index
		//D=M;
		strcat(asmcode,"@");
		strcat(asmcode,"static.");
		strcat(asmcode,index);
		strcat(asmcode,"\n");
		fputs(asmcode,file);
		memset(asmcode,0,sizeof(asmcode));
		fputs(MED,file);
	}
	else{
		//@SP
		//M=M-1
		fputs(DECSP,file);
		//@index
		//D=A
		strcat(asmcode,"@");
		strcat(asmcode,index);
		strcat(asmcode,"\n");
		fputs(asmcode,file);
		fputs(DEA,file);
		memset(asmcode,0,sizeof(asmcode));
		//@segment
		//A=M
		//AD=D+A
		GetSegment(asmcode,segment);
		fputs(asmcode,file);
		memset(asmcode,0,sizeof(asmcode));
		if(strcmp(segment,"temp")!=0 && strcmp(segment,"pointer")!=0)
		  fputs(AEM,file);
		fputs(ADEAaddD,file);
		//@R13
		//M=D
		fputs(R13,file);
		fputs(MED,file);
		//@SP
		//A=M
		//D=M
		fputs(SP,file);
		fputs(AEM,file);
		fputs(DEM,file);
		//R13
		//A=M
		//M=D
		fputs(R13,file);
		fputs(AEM,file);
		fputs(MED,file);
	}
}

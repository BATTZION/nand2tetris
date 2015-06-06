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
#define ADEM        "AD=M\n"
#define ADEAaddD    "AD=D+A\n"
#define DEAaddD     "D=D+A\n"
#define R13         "@R13\n"
#define DEM         "D=M\n"
#define POPTOD      "@SP\nM=M-1\n@SP\nA=M\nD=M\n"
#define POPTOA      "@SP\nM=M-1\n@SP\nA=M\nA=M\n"
#define PUSHFROMD   "@SP\nA=M\nM=D\n@SP\nM=M+1\n"
#define GETSP       "@SP\nA=M\n"
#define JMP         "0;JMP\n"
#define JNE         "D;JNE\n"

static int counter = 1;

char *JUMPS[]={"D;JEQ\n","D;JGT\n","D;JLT\n"};
extern char *Arithmetic[];
extern char filename[];

char *operation[]={"D=D+A\n","D=A-D\n","D=-D\n","D=A-D\n","D=A-D\n","D=A-D\n","D=A&D\n","D=A|D\n","D=!D\n",NULL};
void CodeArithmetic(char operator[], FILE *file);
void CodePushPop(COMMAND_TYPE type, char segment[], char index[], FILE *file);
void CodePop(char segment[], char index[], FILE *file);
void CodePush(char segment[], char index[], FILE *file);
void GetSegment(char asmcode[], char segment[]);
void CodeLabel(char label[], FILE *file);
void translate(int num, char s[]);
void CodeGoto(char destination[], FILE *file);
void CodeIf(char destination[], FILE *file);
void CodeCall(char function[], char argnum[], FILE *file);
void CodeReturn(FILE *file);
void CodeFunction(char function[], char number[], FILE *file);
void CodeInit(FILE *file);
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
			translate(counter,number);
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
			translate(counter+1,number);
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
			counter += 2;
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
		fprintf(file,"@%s\n",index);           //@number
	    fputs(DEA,file);                       //D=A
	    fputs(PUSHFROMD,file);
	}
	else if(strcmp(segment,"pointer")==0 ||strcmp(segment,"temp")==0){
	    //@index
		//D=A
		fprintf(file,"@%s\n",index);
		fputs(DEA,file);
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
		strcat(asmcode,filename);
		strcat(asmcode,index);
		strcat(asmcode,"\n");
		fputs(asmcode,file);
		memset(asmcode,0,sizeof(asmcode));
		fputs(DEM,file);
		//POSH D
		fputs(PUSHFROMD,file);
	}
	else{
		GetSegment(asmcode,segment);
		if(strcmp(index, "0")){
			//@index
			//D=A
			fprintf(file,"@%s\n",index);
			fputs(DEA,file);
			//@segement
			//A=M
			//AD=D+A
			//D=M
			fputs(asmcode,file);
			fputs(AEM,file);
			fputs(ADEAaddD,file);
			fputs(DEM,file);
			//PUSH D
			fputs(PUSHFROMD,file);
			}
		else{
			fputs(asmcode,file);
			fputs(ADEM,file);
			fputs(DEM,file);
			fputs(PUSHFROMD,file);
		}
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
		strcat(asmcode,filename);
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
		fprintf(file,"@%s\n",index);
		fputs(DEA,file);
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

void CodeLabel(char label[], FILE *file)
{
	// (label)
	fprintf(file,"(%s)\n",label);
}

void CodeGoto(char destination[], FILE *file)
{
	//@destination
	fprintf(file,"@%s\n",destination);
    //0;JMP
	fputs(JMP,file);
}

void CodeIf(char destination[], FILE *file)
{
	// POP D
	fputs(POPTOD, file);
	//@destination
	fprintf(file,"@%s\n",destination);
	//D;JNE
	fputs(JNE,file);
}

void CodeCall(char function[], char argnum[], FILE *file)
{
	char call_label[MAX_ARG];
	char number[MAX_ARG];
	int n = 0;
	memset(call_label,0,sizeof(call_label));
	memset(number,0,sizeof(number));
	//push return-address
	translate(counter,number);
	counter++;
	strcat(call_label,"LABEL");
	strcat(call_label,number);
	memset(number,0,sizeof(number));
      	//@CallLabel-counter
	fprintf(file,"@%s\n",call_label);
	    //D=A
    fputs(DEA,file);
	    // PUSH D
	fputs(PUSHFROMD,file);
	//push LCL
	fputs("@R1\nD=M\n",file);
	fputs(PUSHFROMD,file);
	//push ARG
	fputs("@R2\nD=M\n",file);
	fputs(PUSHFROMD,file);
    //PUSH THIS
	fputs("@R3\nD=M\n",file);
	fputs(PUSHFROMD,file);
	//push THAT
	fputs("@R4\nD=M\n",file);
	fputs(PUSHFROMD,file);
	
	//ARG = SP - n -5
	
	n = atoi(argnum) + 5;
	fprintf(file,"@%d\nD=A\n",n);
	fputs("@R0\nA=M\nAD=A-D\n",file);
	fputs("@R2\nM=D\n",file);
	
	//LCL = SP
	fputs("@R0\nD=M\n",file);
	fputs("@R1\nM=D\n",file);
	//goto if
	fprintf(file,"@%s\n",function);
	fputs(JMP,file);
    //(return-address)
	fprintf(file,"(%s)\n",call_label);
}
void CodeInit(FILE *file)
{
	//@256
	//D=A
	//@R0
	//M=D
	fputs("@256\nD=A\n@R0\nM=D\n", file);
	//call sys.init
	CodeCall("Sys.init","0",file);
}
void CodeFunction(char function[], char number[], FILE *file)
{
	//(function)
	fprintf(file, "(%s)\n", function);
	int n = atoi(number);
	while ( n-- > 0 )
	  CodePush("constant", "0", file);
}
void CodeReturn(FILE *file)
{
	//FRAME = LCL
	fputs("@R1\nD=M\n",file);
	fputs("@R13\nM=D\n",file);
	//RET = * (FRAME - 5)
	fputs("@5\nA=D-A\nD=M\n",file);
	fputs("@R14\nM=D\n",file);
	//*ARG = pop()
	fputs(DECSP,file);
	fputs("@ARG\nAD=M\n",file);
	fputs("@R15\nM=D\n@SP\nA=M\nD=M\n",file);
	fputs("@R15\nA=M\nM=D\n",file);
	//SP = ARG + 1
	fputs("@R2\nD=M\n",file);
	fputs("@R0\nM=D+1\n",file);
	//THAT = *(FRAME- 1)
	fputs("@R13\nD=M\nD=D-1\n",file);
	fputs("@R13\nM=D\nA=D\nD=M\n",file);
	fputs("@R4\nM=D\n",file);
	//THIS = *(FRAME -2)
	fputs("@R13\nD=M\nD=D-1\n",file);
	fputs("@R13\nM=D\nA=D\nD=M\n",file);
	fputs("@R3\nM=D\n",file);
	//ARG = *(FRAME - 3)
	fputs("@R13\nD=M\nD=D-1\n",file);
	fputs("@R13\nM=D\nA=D\nD=M\n",file);
	fputs("@R2\nM=D\n",file);
	//LCL = *(FRAME - 4)
	fputs("@R13\nD=M\nD=D-1\n",file);
	fputs("@R13\nM=D\nA=D\nD=M\n",file);
	fputs("@R1\nM=D\n",file);
	//goto RET
	fputs("@R14\nA=M\n",file);
	fputs(JMP,file);
}

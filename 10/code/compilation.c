#include <stdio.h>
#include "MString.h"


char *class_statement[] = {"constructor","function","method","field",NULL};
char *statements[] = {"do","let","while","if","return",NULL};
char *operator[] = {"+","-","*","/","&lt;","&gt;","&amp;","=",NULL};
char *unaryop[] = {"~","-",NULL};
int is_class_statement(char *line);
void compile_class(FILE *filein, FILE *fileout);
void compile_subroutine(FILE *filein, FILE *fileout, int key);
void compile_parameter_list(FILE *filein, FILE *fileout);
void compile_statements(FILE *filein, FILE *fileout);
void compile_vardec(FILE *filein, FILE *fileout);
void compile_do(FILE *filein, FILE *fileout);
void compile_while(FILE *filein, FILE *fileout);
void compile_let(FILE *filein, FILE *fileout);
void compile_expression(FILE *filein, FILE *fileout);
void compile_term(FILE *filein, FILE *fileout);
void compile_return(FILE *filein, FILE *fileout);
void compile_expressionlist(FILE *filein, FILE *fileout);
void compile_class_vardec(FILE *filein, FILE *fileout);
void compile_if(FILE *filein, FILE *fileout);

int is_first = 1;             //whether is the first term in a expression 

void compilation(FILE *source, FILE *destination)
{
	init_fgets_buff();
	compile_class(source,destination);
	free_buffer();
}

void compile_class(FILE *filein, FILE *fileout)
{
	char line[MAX_LINE];
	char character[MAX_LINE];
	int key, i =0;
	//get <tokens>
	fgets_buff(line,MAX_LINE,filein);
	memset(line,0,sizeof(line));
	memset(character,0,sizeof(character));
    //<class>
	fprintf(fileout,"<class>\n");
	// 'class' classname '{'
	for (i = 0; i< 3; i++){
		fgets_buff(line,MAX_LINE,filein);
		fputs(line,fileout);
		memset(line,0,sizeof(line));
	}
	while (fgets_buff(line,MAX_LINE,filein)){
		get_spilt(line,character,2);
		key = is_class_statement(character);
		switch(key){
			case 0:
				if(strcmp(character,"</tokens>\n"))
				  fputs(line,fileout);
				break;
			case 1:
			case 2:
			case 3:
				compile_subroutine(filein,fileout,key);
				break;
			case 4:
				unfgets_buff(line,MAX_LINE);
				compile_class_vardec(filein,fileout);
				break;
			default:
				break;
	}
	if ( strcmp(character,"}") == 0)
	  break;
	memset(line,0,sizeof(line));
	memset(character,0,sizeof(character));
	}
	fputs("</class>",fileout);

}
int is_class_statement(char *line)
{
	return find_string(line,class_statement);
}
int is_statements(char *line)
{
	return find_string(line,statements);
}
void compile_subroutine(FILE *filein, FILE *fileout, int key)
{
	int i = 0;
	char line[MAX_LINE];
	memset(line,0,sizeof(line));
	fprintf(fileout,"<subroutineDec>\n");
    fprintf(fileout,"<keyword> %s </keyword>\n", class_statement[key - 1]);
	// type + functionname + ( 
	while ( i < 3){
		fgets_buff(line,MAX_LINE,filein);
		fputs(line,fileout);
		memset(line,0,sizeof(line));
		i++;
	}
	compile_parameter_list(filein,fileout);

	fputs("<subroutineBody>\n",fileout);
	fgets_buff(line,MAX_LINE,filein);
	fputs(line,fileout);

	compile_vardec(filein, fileout);
	compile_statements(filein,fileout);
	
	fputs("</subroutineBody>\n",fileout);
    fprintf(fileout,"</subroutineDec>\n");

}
void compile_parameter_list(FILE *filein, FILE *fileout)
{
	char line[MAX_LINE];
	char character[MAX_LINE];
	memset(line,0,sizeof(line));
	memset(character,0,sizeof(character));
	fputs("<parameterList>\n",fileout);
	while (fgets_buff(line,MAX_LINE,filein)){
		get_spilt(line,character,2);
	    if (strcmp(character,")") == 0)
		  break;
		fputs(line,fileout);
		memset(character,0,sizeof(character));
		memset(line,0,sizeof(line));
	}
	fputs("</parameterList>\n",fileout);
	fputs("<symbol> ) </symbol>\n",fileout);
}
void compile_statements(FILE *filein, FILE *fileout)
{

	char line[MAX_LINE];
	char character[MAX_LINE];
	int key = 0;
	memset(line,0,sizeof(line));
	memset(character,0,sizeof(character));
	fputs("<statements>\n",fileout);
	while (fgets_buff(line,MAX_LINE,filein)){
		get_spilt(line,character,2);
		key = is_statements(character);
		if ( strcmp(character,"}") == 0)
		  break;
		switch(key){
			case 0:
				fputs(line,fileout);
				break;
			case 1:
				compile_do(filein, fileout);
				break;
			case 2:
				compile_let(filein, fileout);
				break;
			case 3:
				compile_while(filein,fileout);
				break;
			case 4:
				compile_if(filein,fileout);
				break;
			case 5:
				compile_return(filein,fileout);
				break;
			default:
				break;
		}
		memset(line,0,sizeof(line));
		memset(character,0,sizeof(character));
	}
	fputs("</statements>\n",fileout);
	fputs("<symbol> } </symbol>\n",fileout);
}
void compile_vardec(FILE *filein, FILE *fileout)
{
	char line[MAX_LINE];
	char character[MAX_LINE];
	memset(line,0,sizeof(line));
	memset(character,0,sizeof(character));
	while (fgets_buff(line,MAX_LINE,filein)){
		get_spilt(line,character,2);
		if(strcmp ( character, "var" ) ==0 ){
			fputs("<varDec>\n",fileout);
			fputs(line,fileout);
			memset(line,0,sizeof(line));
			memset(character,0,sizeof(character));
			while (fgets_buff(line,MAX_LINE,filein)) {
				fputs(line,fileout);
				get_spilt(line,character,2);
				if (strcmp(character,";") ==0)
				  break;
				memset(line,0,sizeof(line));
				memset(character,0,sizeof(character));
			}
			fputs("</varDec>\n",fileout);
		}
		else
		  break;
	}
	unfgets_buff(line,MAX_LINE);
}
void compile_let(FILE *filein, FILE *fileout)
{
	char line[MAX_LINE];
	char character[MAX_LINE];
	memset(line,0,sizeof(line));
	memset(character,0,sizeof(character));
	fputs("<letStatement>\n",fileout);
	fputs("<keyword> let </keyword>\n",fileout);
	while (fgets_buff(line,MAX_LINE,filein)){
		get_spilt(line,character,2);
		fputs(line,fileout);
		if (strcmp(character, "=") == 0 || strcmp(character, "[") == 0){
			compile_expression(filein, fileout);
		}
		if (strcmp(character, ";") == 0)
		  break;
		memset(line,0,sizeof(line));
		memset(character,0,sizeof(character));
	}
	fputs("</letStatement>\n",fileout);

}
void compile_expression(FILE *filein, FILE *fileout)
{
	char line[MAX_LINE];
	char character[MAX_LINE];
	memset(line, 0, sizeof(line));
	memset(character, 0, sizeof(character));
	fputs("<expression>\n",fileout);
	is_first = 1;
	while (fgets_buff(line,MAX_LINE,filein)){
		get_spilt(line,character,2);
        if (strcmp(character,";") == 0 || strcmp(character, ")") == 0 ){
			unfgets_buff(line,MAX_LINE);
			break;
		}
		if (find_string(character,unaryop)  && is_first  ){
			unfgets_buff(line, MAX_LINE);
			compile_term(filein,fileout);
			is_first = 0;
			continue;
		}
		if (strcmp(character,"]") == 0 || strcmp(character, ",") == 0 ){
			fputs("</expression>\n",fileout);
			fputs(line,fileout);
			return;
		}
		if (find_string(character,operator)){
			fputs(line,fileout);
			compile_term(filein,fileout);
		}
		else{
			unfgets_buff(line,MAX_LINE);
			compile_term(filein,fileout);
			is_first = 0;
		}
	}
	fputs("</expression>\n",fileout);
}
void compile_term(FILE *filein, FILE *fileout)
{
	int flag = 0;
	int lpren = 0;    //whether have lpren ?
  	char line[MAX_LINE];
	memset(line, 0, sizeof(line));
    char character[MAX_LINE];
	memset(character, 0, sizeof(character));
	fputs("<term>\n",fileout);
	while((fgets_buff(line,MAX_LINE,filein))){
		get_spilt(line,character,2);
		
		if (strcmp(character,";") == 0  || strcmp(character,"]") == 0 || find_string (character, operator)){
			unfgets_buff(line,MAX_LINE);
			break;
		}
	    if (find_string ( character, unaryop) && is_first){
			fputs(line,fileout);
			compile_term(filein,fileout);
			fputs("</term>\n",fileout);
			return;
		}

		if (strcmp(character, ",") == 0 ) {
			fputs("</term>\n",fileout);
			unfgets_buff(line,MAX_LINE);
		    return;
		}
		if (strcmp(character, ")") ==0) {
			if (lpren == 1){
				fputs(line,fileout);
				fputs("</term>\n",fileout);
				lpren ==0;
				return;
			}
			else{
				fputs("</term>\n",fileout);
				unfgets_buff(line,MAX_LINE);
				return;
			}
		}

		if (strcmp(character, ".") == 0)
		  flag = 1;
		fputs(line,fileout);

		if (strcmp(character, "(") == 0){
			lpren = 1;
			if (flag == 1)
			  compile_expressionlist(filein,fileout);
			else 
			  compile_expression(filein,fileout);
		}
		if (strcmp(character, "[") == 0)
		  compile_expression(filein,fileout);

		memset(line,0,sizeof(line));
		memset(character,0,sizeof(character));
	}
	fputs("</term>\n",fileout);
}
void compile_do(FILE *filein, FILE *fileout)
{
	char line[MAX_LINE];
	memset(line, 0 ,sizeof(line));
	char character[MAX_LINE];
	memset(character, 0, sizeof(character));
	fputs("<doStatement>\n", fileout);
	fputs("<keyword> do </keyword>\n",fileout);
	while (fgets_buff(line, MAX_LINE, filein)){
		get_spilt(line, character, 2);
		fputs(line,fileout);
		if (strcmp(character, "(") == 0){
		  compile_expressionlist(filein, fileout);
		}
		if (strcmp(character, ";") == 0)
		  break;
	}
	fputs("</doStatement>\n", fileout);
}
void compile_return(FILE *filein, FILE *fileout)
{
	char line[MAX_LINE];
	memset(line, 0 ,sizeof(line));
	char character[MAX_LINE];
	memset(character, 0, sizeof(character));
	fputs("<returnStatement>\n", fileout);
	fputs("<keyword> return </keyword>\n",fileout);
	while (fgets_buff(line, MAX_LINE, filein)){
		get_spilt(line, character, 2);
		if (strcmp(character, ";") == 0){
			fputs(line,fileout);
			break;
		}
		else{
			unfgets_buff(line,MAX_LINE);
			compile_expression(filein,fileout);
		}
		memset(line,0,sizeof(line));
		memset(character,0,sizeof(character));
	}
	fputs("</returnStatement>\n", fileout);
}
void compile_while(FILE *filein, FILE *fileout)
{
	char line[MAX_LINE];
	char character[MAX_LINE];
	memset(line,0,sizeof(line));
	memset(character,0,sizeof(character));
	
	fputs("<whileStatement>\n",fileout);
	fputs("<keyword> while </keyword>\n",fileout);
    
	// compile lparen
	fgets_buff(line,MAX_LINE,filein);
	fputs(line,fileout);
	memset(line,0,sizeof(line));
	// compile expression
	compile_expression(filein,fileout); 
	// compile rparen
	fgets_buff(line,MAX_LINE,filein);
	fputs(line,fileout);
	memset(line,0,sizeof(line));
    // compile "{"
	fgets_buff(line,MAX_LINE,filein);
	fputs(line,fileout);
	memset(line,0,sizeof(line));
	//compile statement
	compile_statements(filein,fileout);

	fputs("</whileStatement>\n",fileout);

}
void compile_expressionlist(FILE *filein, FILE *fileout)
{
	char line[MAX_LINE];
	memset(line, 0, sizeof(line));
	char character[MAX_LINE];
	memset(character, 0, sizeof(character));
	fputs("<expressionList>\n",fileout);
	while(fgets_buff(line,MAX_LINE,filein)){
		get_spilt(line,character,2);
		if (strcmp(character,")") == 0){
			fputs("</expressionList>\n",fileout);
			fputs(line,fileout);
			return;
		}
		else{
			unfgets_buff(line,MAX_LINE);
			compile_expression(filein,fileout);
		}
		memset(line,0,sizeof(line));
		memset(character,0,sizeof(character));
	}
	fputs("</expressionList>\n",fileout);
}
void compile_class_vardec(FILE *filein, FILE *fileout)
{
	char line[MAX_LINE];
	char character[MAX_LINE];
	memset(line,0,sizeof(line));
	memset(character,0,sizeof(character));
	while (fgets_buff(line,MAX_LINE,filein)){
		get_spilt(line,character,2);
		if(strcmp ( character, "field" ) ==0 ){
			fputs("<classVarDec>\n",fileout);
			fputs(line,fileout);
			memset(line,0,sizeof(line));
			memset(character,0,sizeof(character));
			while (fgets_buff(line,MAX_LINE,filein)) {
				fputs(line,fileout);
				get_spilt(line,character,2);
				if (strcmp(character,";") ==0)
				  break;
				memset(line,0,sizeof(line));
				memset(character,0,sizeof(character));
			}
			fputs("</classVarDec>\n",fileout);
		}
		else
		  break;
	}
	unfgets_buff(line,MAX_LINE);
}
void compile_if(FILE *filein, FILE *fileout)
{
	char line[MAX_LINE];
	char character[MAX_LINE];
	memset(line,0,sizeof(line));
	memset(character,0,sizeof(character));
	
	fputs("<ifStatement>\n",fileout);
	fputs("<keyword> if </keyword>\n",fileout);
    
	// compile lparen
	fgets_buff(line,MAX_LINE,filein);
	fputs(line,fileout);
	memset(line,0,sizeof(line));
	// compile expression
	compile_expression(filein,fileout); 
	// compile rparen
	fgets_buff(line,MAX_LINE,filein);
	fputs(line,fileout);
	memset(line,0,sizeof(line));
    // compile "{"
	fgets_buff(line,MAX_LINE,filein);
	fputs(line,fileout);
	memset(line,0,sizeof(line));
	//compile statement
	compile_statements(filein,fileout);

	fputs("</ifStatement>\n",fileout);

}


#include <stdio.h>
#include <dirent.h>
#include <malloc.h>
#include <string.h>

#define MAXLEN_FILENAME 100
#define MAX_LINE        256
#define JACKSUFFIX      "jack"
#define KEYWORD         0
#define SYMBOL          1
#define IDENTIFIER      2
#define INT_CONST       3
#define STRING_CONST    4

typedef int TYPE;
char symbol[] = {';', '(', ')', ',', '=', '.', '<', '>', '{', '}', '[', ']', '+', '-', '*', '/', '&', '|',0};
char *keyword[] = {"class","method","int","function","boolean","constructor","var","void","char","static","field","let","do","if","else","while","return","ture","false","null","this",NULL};
char *character[50];     //存放每一行分割的字符串1
TYPE token_type(char *line);
void print_token(char *line, TYPE type, FILE *file); 
void init_character()
{
	int i = 0;
	while (i < 50){
	  character[i] = (char*)malloc(100*sizeof(char));
	  memset(character[i],0,sizeof(character[i]));
	  i++;
	}
}
void memset_character(int num_split)
{
	while (--num_split > -1)
	  memset(character[num_split],0,sizeof(character[num_split]));
}
void Jacktokenizer(FILE *source, FILE *destination)
{
	int num_split = 0, i=0;
	int Type;
	char line[MAX_LINE];
	memset(line,0,sizeof(line));
	fprintf(destination,"<tokens>\n");
	while(fgets(line,MAX_LINE,source)){
		if (!is_annotation(line,source)){
			num_split = split_line(line,character,symbol);
			for (i = 0; i< num_split; i++){
				Type = token_type(character[i]);
				switch (Type){
					case KEYWORD :
						print_token(character[i],KEYWORD,destination);
						break;
					case IDENTIFIER :
						print_token(character[i],IDENTIFIER,destination);
						break;
					case SYMBOL:
						print_token(character[i],SYMBOL,destination);
						break;
					case INT_CONST:
						print_token(character[i],INT_CONST,destination);
						break;
					case STRING_CONST:
						print_token(character[i],STRING_CONST,destination);
						break;
					default:
						break;
				}
			}
			memset_character(num_split);
			memset(line,0,sizeof(line));
		}
	}
	fprintf(destination,"</tokens>\n");

}

TYPE token_type(char *line)
{
	if (find_string(line,keyword))
	  return KEYWORD;
	else if (not_in_arrary(line[0],symbol)==0)
	  return SYMBOL;
	else if (line[0] == '"')
	  return STRING_CONST;
	else if (is_digital(line))
	  return INT_CONST;
	else 
	  return IDENTIFIER;
}
void print_token(char *line, TYPE type, FILE *file)
{
	char temp[MAX_LINE];
	memset(temp,0,sizeof(temp));
	switch(type){
		case KEYWORD:
			fprintf(file,"<keyword> %s </keyword>\n",line);
			break;
		case SYMBOL:
			fprintf(file,"<symbol> %s </symbol>\n",line);
			break;
		case IDENTIFIER:
			fprintf(file,"<identifier> %s </identifier>\n",line);
			break;
		case INT_CONST:
			fprintf(file,"<integerConstant> %s </integerConstant>\n",line);
			break;
		case STRING_CONST:
			strcpy(temp,line);
			squeeze(temp,'"');
			fprintf(file,"<stringConstant> %s </stringConstant>\n",temp);
			break;
	}

}


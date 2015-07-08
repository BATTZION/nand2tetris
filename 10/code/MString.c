/**
 *  一些字符串处理函数 .
 *  some string Manipulation
 *
 */
#include <string.h>
#include <stdio.h>
#include <malloc.h>

#define MAXLEN_STRING 255
#define MAX_LINE 256
#define BUFF_LEN 5


/**
 * 带有缓冲区的fgets_buff;
 * 支持 unfgets_buff(), 将读取的输入重新返回到缓冲区;
 * 注意:对不同文件读,公用相同缓冲区
 */
static int sp = 0;
static char * buffer[BUFF_LEN];
extern void init_fgets_buff()
{
	while (sp < BUFF_LEN){
	  buffer[sp] = (char *)malloc(MAX_LINE * sizeof(char));
	  memset(buffer[sp],0,sizeof(buffer[sp]));
	  sp++;
	}
	sp = -1;
}
char *fgets_buff(char *line, int size, FILE *file)
{
	if (sp < 0)
	  return fgets(line,size,file);
	else{
	  strncpy(line,buffer[sp],size);
	  memset(buffer[sp],0,sizeof(buffer[sp]));
	  sp--;
	  return line;
	}
}
void unfgets_buff(char *line, int size)
{
	int len;
	len = strlen(line);
	strncpy(buffer[++sp],line,len);
	buffer[sp][len] = 0;
	memset(line,0,sizeof(line));
}
void free_buffer()
{
	int i = 0;
	while(i < BUFF_LEN)
	{
		free(buffer[i]);
		buffer[i++] = NULL;
	}
}

/*
 * 字符串翻转函数
 */

extern char *strrev(char *str)  
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
extern int get_filename(char *source, char *destination)
{
	int i, len = 0, j = 0;
	len = strlen(source);
	i = len + 1;
	while (source[--i] == ' ')
	  ;
	while (source[i--] != '.')
	  ;
	while (i >= 0 && source[i] != '/')
	  destination[j++] = source[i--];
	destination[j] = '\0';
	strrev(destination);
	return 0;
}
extern int CmpSuffix(char *filename, char *suffix)
{
	int i, len =0;
	len = strlen(filename);
	i = len;
	while (filename[i] != '.' && filename[i] != '/' && i > 0)
	  i--;
	if (filename[i] != '.')
	  return 999;        // no suffix
	else 
	  return strcmp((filename+i+1),suffix);
}
int not_in_arrary(char c, char *arrary)
{
	int i = 0, len = strlen(arrary);
	while (i < len){
		if (arrary[i++] == c)
		  return 0;
	}
	return 1;
}
extern int split_line(char *line, char **character,char *symbol)  //以 
{
	int i = 0, j = 0, k = 0, flag = 0;
	while (line[i] != '\r' && line[i] != '\n' && !(line[i] =='\t' && line[i+1] =='\r' && !(line[i] == '/' && line[i+1] == '/'))){
		while (line[i] == ' ' || line[i] == '\t')
		  i++;
		if (not_in_arrary(line[i],symbol) == 0){
			if (line[i] == '<'){
				strcpy(character[j],"&lt;");
				j++;
				i++;
			}
			else if (line[i] == '>'){
				strcpy(character[j],"&gt;");
				j++;
				i++;
			}
			else if (line[i] == '&'){
				strcpy(character[j],"&amp;");
				j++;
				i++;
			}	
			else {
				character[j][k++] = line[i++];
				character[j++][k] = 0;
				k = 0;
			}
			continue;
		}
		while (not_in_arrary(line[i],symbol) && line[i] != ' '){
			if (line[i] == '"'){
				character[j][k++] = line[i++];
				while (line[i] != '"')
				  character[j][k++] = line[i++];
				character[j][k++] = line[i++];
				break;
			}
			else
			  character[j][k++] = line[i++];
		}
		character[j][k] = 0;
		j++;
		k = 0;
	}
	return j;
}
int is_annotation(char * line, FILE *file)
{
	int i = -1;
	char annotation[MAX_LINE];
	memset(annotation,0,sizeof(annotation));
	while (line[++i] == ' ')
	  ;
	if (line[i] == '/' && line[i+1] == '/')
	  return 1;
	else if (line[i] == '/' && line[i+1] == '*'){
		while (fgets(annotation,MAX_LINE,file)){
			if (strstr(annotation,"*/"))
			  break;
			memset(annotation,0,sizeof(annotation));
		}
		return 1;
	}
	else
	  return 0;
}
int find_string(char *string, char **source)
{
	int i=0;
	while (source[i])
	  if (strcmp(string, source[i++]) == 0)
		return i;
	return 0;
	
}
int is_digital(char *line)
{
	int i = 0, len = strlen(line);
	while (i < len){
		if ( line[i] >= '0' && line[i] <= '9')
		  i++;
		else
		  return 0;
	}
	return 1;
}
/*将字符串s中出现的字符c删除*/
void squeeze(char *s, char c)
{
	int i,j;
	for (i = 0, j = 0; s[i] != '\0'; i++)
	{
		if (s[i] != c)
		{
			s[j++] = s[i];
		}
	}
	s[j] = '\0';
}
void get_spilt(char s[], char d[], int n)
{
	char temp[MAX_LINE];
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



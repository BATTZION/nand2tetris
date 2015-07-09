/**
 *   argv[1] : 输入目标文件
 *   argv[2] : 输出字元集文件
 *   argv[3] : 输出语法树文件
 *   注:输出文件均为 xml 格式
 */

#include <stdio.h>
#include "MString.h"

int main(int argc, char *argv[])
{
	FILE *file1,*file2,*file3;
	init_character();
	file1 = fopen(argv[1],"r+");
	file2 = fopen(argv[2],"a+");
	file3 = fopen(argv[3],"a+");
	Jacktokenizer(file1,file2);
	fclose(file1);
	fclose(file2);
	file2 = fopen(argv[2],"r+");
	compilation(file2,file3);
	return 0;
}

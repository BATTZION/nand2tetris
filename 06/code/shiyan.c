#include "hashtable.h"
#include <stdio.h>
int main()
{
	char *tmp;
	HashTable *list=create_hashtable(100,char*,char*);
	hash_add(list,"value","key");
	hash_find(list,"values",&tmp);
	printf("%s\n",tmp);
}

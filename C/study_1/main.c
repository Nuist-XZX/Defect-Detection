/* 
函数学习2026/4/11

内容：函数的声明和定义

Note: 	如果函数定义在main函数或者其他使用函数之前，则可以不声明；
		如果位于main函数之后，则需要在main函数之前进行声明。
		
		1. 告诉编译器有一个函数叫什么，参数是什么，返回类型是什么。
		   但是具体是不是存在，函数声明决定不了。
		2. 函数的声明一般出现在函数的使用之前。要满足先声明后使用。
		3. 函数的声明一般要放在头文件中的（.h）
		
		导入静态库：pragma comment(lib, "name.lib")
 */

#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include "sub.h"

//函数声明
int ADD(int x, int y);

//main函数其实是有三个参数，包括int argc、 char* argv[]、 char* envp[]
int main(){
	
	int a,b = 0;
	scanf("%d %d", &a, &b);
	//加法函数
	int sum = ADD(a, b); 
	printf("%d\n", sum);
	//减法函数，声明放在头文件中，减法函数定义放在.c文件中
	int subtraction = sub(a, b);
	printf("%d\n", subtraction);
	
	return 0;
}

//函数定义
int ADD(int x, int y){
	return x+y;
}
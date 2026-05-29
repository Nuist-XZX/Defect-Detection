/*
	函数学习2026/4/13

	内容：函数递归 与 迭代
	
	Note:
		递归的两个必要条件：
		1、存在限制条件，当满足这个限制条件的时候，递归便不再继续。
		2、每次递归调用之后越来越接近这个限制条件
	
	笔记：
		1、%d是打印或输入有符号整数，但是%u是打印或输入无符号整数
		2、1/10=0  1%10=1
		3、内存包括：栈、堆、静态。栈中包括局部变量、函数的形参，每一次函数的调用都会在栈区申请空间，如果空间耗尽，
		  则会出现stack overflow（栈溢出）,所以函数递归需要终止（限制）条件，不能死递归。
		4、char strdata;->这是定义了一个单个字符变量，不是字符串也不是数组，所以不能作为指针传入函数，
		  它只能存储一个字符，例如 strdata = 'A';不能存储字符串或多个字符。
		5、char strdata[];->这是不完整的数组定义，编译会报错,数组定义时必须指定大小，
		  或者通过初始化列表让编译器自动推导大小。
		6、数组传入函数时必须使用指针，例如：int strlength_1(char *x)，当然也可以int strlength_1(char x[])
		  其中，*x指向的是数组中存储的第一个字符，x存放的是第一个字符的地址。
		7、error: stray '\302' in program 52 | <U+00A0> <U+00A0> <U+00A0> <U+00A0> return 1+strlength_2(x+1);
		   //<U+5982><U+679C><U+4E0D><U+662F><U+7A7A><U+FF0C><U+81F3><U+5C11><U+957F><U+5EA6><U+4E3A>1<U+FF0C><U+6240><U+4EE5><U+201C>1+<U+201D>
		   代码段有中文/全角空格或者不可见特殊字符
		8、递归很有可能出现栈溢出的现象，即使他不是死循环递归。如果出现栈溢出，考虑迭代；或者使用static的静态变量替代局部变量
*/

#include<stdio.h>
#include "opdef.h"

int main(){
	/* 	
		1、接受一个整型值（无符号），按照顺序打印它的每一位。
		例如：
		输入：1234，输出 1 2 3 4
	*/
	// unsigned int a = 0;
	// scanf("%u",&a); //%d是打印或输入有符号整数，但是%u是打印或输入无符号整数
	// opdef_1(a);
	// opdef_2(a);
	// opdef_3(a);
	
	/* 
		2、编写函数不允许创建临时变量，求字符串的长度
	*/
	// char strdata[50];
	// scanf("%s",&strdata);
	// int num_str, num_str2 = 0;
	// num_str = strlength_1(strdata);
	// num_str2 = strlength_2(strdata);
	// printf("%d\n",num_str);
	// printf("%d\n",num_str2);
	
	/* 
		3、n的阶乘 
	*/
	// int n = 0;
	// scanf("%d",&n);
	//方法一
	// int i = 1;
	// int nnm = 1;
	// for(i = 1; i <= n; i++){
		// nnm = nnm * i;
	// }
	// printf("%d\n",nnm);
	
	//方法二
	// for(i = 1; i <= n; i++){
		// static int nnm = 1;
		// nnm = nnm * i;
		// printf("%d\n",nnm);
	// }
	
	//方法三 递归
	// int nc = 0;
	// nc = fac(n);
	// printf("%d\n",nc);
	
	/* 
		4、求第n个斐波那契数 
	*/
	//1 1 2 3 5 8 13 21 34 55 ....前两个数相加等于第三个数
	//方法一：递归,一旦数据很大，计算会非常很慢，效率低下，出现栈溢出
	int q = 0;
	scanf("%d",&q);
	int nu = 0;
	nu = fib_1(q);
	printf("%d\n",nu);
	
	//方法二：迭代
	int nu1 = 0;
	nu1 = fib_2(q);
	printf("%d\n",nu1);
	
	return 0;
}
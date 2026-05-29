/*
	函数学习2026/4

	内容：函数
	
	Note:
		a 和 b是实参，当实参传递给形参时，形参是实参的一种临时拷贝，修改形参不会改变实参
	
	笔记：

*/

#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>

int compare_num(int x, int y) {

	return ((x < y) ? y : x); 

}

void change_num(int *x, int *y) {
	int c;
	c = *x; //a
	*x = *y;//a = b
	*y = c;//b = a
}

void sushu(int x) {
	//用2-i-1之间的数去试除i
	int i;
	int flag = 1;//假设1为素数
	for (i = 2;i <= x-1; i++)
	{
		if (x % i == 0) {
			flag = 0;
			printf("%d 不是素数\n", x);
			break;
		}
	}
	if (flag == 1)
		printf("%d 是素数\n", x);
}


int main() {

	int a;
	int b;
	printf("请输入两个数字：\n");
	scanf("%d %d", &a, &b);

	int m = compare_num(a, b);
	printf("最大值是：%d\n", m);
	// a 和 b是实参，当实参传递给形参时，形参是实参的一种临时拷贝，修改形参不会改变实参
	//change_num(a, b);
	change_num(&a, &b);
	printf("a = %d, b = %d\n", a, b);

	sushu(a);

	printf("%d",43);//43

	//函数链式访问
	printf("%d", printf("%d", printf("%d", 43)));//4321

	return 0;
}
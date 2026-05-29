#include<stdio.h>
#include<string.h>
//方法一
void opdef_1(unsigned int x){
	
	int Q = (int)(x/1000);
	int B = (int)((x - (Q * 1000))/100);
	int S = (int)((x - (Q * 1000) - (B * 100))/10);
	int G = (int)((x - (Q * 1000) - (B * 100) - (S * 10)));
	printf("%d\n %d\n %d\n %d\n", Q, B, S, G);
}

//方法二
void opdef_2(unsigned int x){
	
	while(x){ // 当x存在时，即不为0，当x = 1时，x / 10 = 0.
		printf("%d\n", x%10);
		x /= 10;
	}
}

//方法三:递归  按顺序打印 例如1234
void opdef_3(unsigned int x){
	if (x > 9){ // 判断x至少是两位数
		opdef_3(x/10);//123 12 1
	}
	printf("%d\n", x%10);
}

// if x = 1234, opdef_3(123)->if x = 123, opdef(12)->if x = 12, opdef(1)->if x = 1, printf(1%10)
//          4                           3                     2                    1  note:1%10  = 1

//求字符串长度
//方法一
//sizeof是返回字节大小，strlen是返回字符长度
int strlength_1(char *x){
	int count = 0; //临时变量
	while(*x != '\0'){
		//*x指向的是数组中存储的第一个字符，只要判断每一个字符是否以\0结尾就行
		count++;
		x++; // 地址+1
	}
	return count;
}

//方法二：递归
int strlength_2(char *x)
{
	if(*x == '\0')
		return 0;
	else
		return 1 + strlength_2(x + 1);//如果不是空，至少长度为1，所以“1+”
}

// strlength_2("abc")->1+strlength_2("bc")->1+strlength_2("c")->1+strlength_2("")
//          1 +            1 +                     1 +              0


//n的阶乘
int fac(int x){
	if(x <= 1)
		return 1;
	else
		return (x * fac(x - 1));
}

//求第n个斐波那契数
//方法一：递归，但是递归会出现严重的溢出，计算次数会非常大
int fib_1(int n){
	if (n <= 2)
		return 1;
	else
		return fib_1(n - 1) + fib_1(n - 2);
}

//方法二：迭代
int fib_2(int n){
	int a,b = 1;
	int c = 0;
	while(n>=3){
		c = a + b;	
		a = b;
		b = c;
		n--;
	} 
	return c;
}

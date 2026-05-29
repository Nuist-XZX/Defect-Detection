/* 
    结构体学习：2026/05/19
    
    内容：枚举
		1、
			enum Day//星期
			{
				Mon,
				Tues,
				Wed,
				Thur,
				Fri,
				Sat,
				Sun
			};
			enum Sex//性别
			{
				MALE,
				FEMALE,
				SECRET
			};
			enum Color//颜色
			{
				RED,
				GREEN,
				BLUE
			};
		
	Note:
	
*/

#include<stdio.h>

// 结构体
struct Stu{
	int a;
	char b;
};

// 枚举声明
enum Day{
	Mon,//0
	Tues,//1
	Wed,//2
	Thur,//3
	Fri,//4
	Sat,//5
	Sun//6
}; // {}中的内容叫枚举常量，是可能的取值，使用枚举类型时是利用这些可能取值进行赋值
//这些可能取值都是有值的，默认从0开始，一次递增1，当然在定义的时候也可以赋初值
// enum Day1{
	// Mon = 1,//1
	// Tues,//2
	// Wed,//3
	// Thur,//4
	// Fri,//5
	// Sat,//6
	// Sun//7
// };


int main(){
	
	// 枚举使用
	enum Day d = Fri;
	printf("%d\n",Mon);	//0
	printf("%d\n",Tues);//1
	printf("%d\n",Wed);	//2
	
	return 0;
}
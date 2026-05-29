/* 
    结构体学习：2026/05/19
    
    内容：结构体
		1、结构体内存对齐
			1. 第一个成员在与结构体变量偏移量为0的地址处。
			2. 其他成员变量要对齐到某个数字（对齐数）的整数倍的地址处。
				对齐数 = 编译器默认的一个对齐数 与 该成员大小 的较小值。即，二者之间的最小值
				VS中默认的值为8
			3. 结构体总大小为最大对齐数（每个成员变量都有一个对齐数）的整数倍。
			4. 如果嵌套了结构体的情况，嵌套的结构体对齐到自己的最大对齐数的整数倍处，
				结构体的整体大小就是所有最大对齐数（含嵌套结构体的对齐数）的整数倍
		
		2、为什么存在内存对齐
			1. 平台原因(移植原因)：
				不是所有的硬件平台都能访问任意地址上的任意数据的；某些硬件平台只能在某些地址处取某些特
				定类型的数据，否则抛出硬件异常。
			2. 性能原因：
				数据结构(尤其是栈)应该尽可能地在自然边界上对齐。
				原因在于，为了访问未对齐的内存，处理器需要作两次内存访问；而对齐的内存访问仅需要一次访
				问。
			总体来说：
				结构体的内存对齐是拿空间来换取时间的做法。
				如果们既要满足对齐，又要节省空间，那么让占用空间小的成员尽量集中在一起。
				
		3、修改默认对齐数：#pragma pack(8)//设置默认对齐数为
		
		
	
	Note:

*/
 
#include<stdio.h>
#include<stddef.h> // offsetof宏

struct S1{
	char c1;	// 1字节
	int i;		// 4字节
	char c2;	// 1字节
};

struct S2{
	char c1;	// 1字节
	char c2;	// 1字节
	int i;		// 4字节
};

struct S3{
	double c1;	// 8字节
	char c2;	// 1字节
	int i;		// 4字节
};

struct S4{
	char c1;		// 1字节
	struct S3 s4;	// 16字节
	double d;		// 8字节
};

// 修改默认对齐数
#pragma pack(4) // 设置默认对齐数是4
struct S5{
	int i;		// 0-3
				// 4-7
	double d;	// 8-15
};
#pragma pack() 	// 取消设置的默认对齐数，还原为默认

struct S6{
	// 默认对齐数是8
	int i;		// 0-3
				// 4-7
	double d;	// 8-15
};

int main(){
	
	printf("%d\n",sizeof(struct S1));	// 12字节
	printf("%d\n",sizeof(struct S2));	// 
	printf("%d\n",sizeof(struct S3));	// 
	printf("%d\n",sizeof(struct S4));	// 
	printf("%d\n",sizeof(struct S5));	// 12字节
	printf("%d\n",sizeof(struct S6));	// 16字节
	
	struct S1 s1;
	struct S2 s2;
	struct S3 s3;
	// offsetof返回一个结构体成员在这个类型创建的变量中的偏移量
	// offsetof(type, member-designator)该宏返回类型为 size_t 的值，表示 type 中成员的偏移量。
	// 偏移量是成员与变量之间差几个字节
	printf("%d\n",offsetof(struct S1,c1));	//0
	printf("%d\n",offsetof(struct S1,i));	//4
	printf("%d\n",offsetof(struct S1,c2));	//8
	// 结构体变量 0  1 2 3     4 5 6 7                    8                       9 10 11
	// 			c1 (1-3)浪费  i(4-7共4字节)               c2                      (9-11)浪费
	//		偏移 0            4(4(int是4字节)的整数倍)      8(1(char是1字节)的整数倍)  0-11共12字节(是最大对齐数4的整数倍)
	
	printf("%d\n",sizeof(double));	// 8字节
	printf("%d\n",sizeof(float));	// 4字节
	
	printf("%d\n",offsetof(struct S4,c1));	//0
	printf("%d\n",offsetof(struct S4,s4));	//8
	printf("%d\n",offsetof(struct S4,d));	//24
	// 结构体变量 0  1-7  8-22 23    24-31
	//			c1 浪费  s4   浪费   d(对齐数是8，24是8的倍数)    0-31共32字节，struct S4的最大对齐数是8，struct S3最大对齐数是8，32是两个对齐数的倍数
	//					(s4该结构体最大对齐数是8,结构体大小是16字节,8是8的倍数)
	
	return 0;
}
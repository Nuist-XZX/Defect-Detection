/* 
    结构体学习：2026/05/19
    
    内容：结构体
		1、结构的基础知识：
			结构是一些值的结合，这些值称为成员变量(member-list)。
			结构的每个成员可以是不同类型的变量。
			
		2、结构体声明：
			struct tag{			// 自定义的结构体标签(名字)tag
					member-list;// 成员列表
			}variable-list;		// 变量列表variable-list
			
			例如：
			struct Stu{
				// 学生相关属性
				int age;
				char name[20];
			}; // 这边的结构体就是一个类型，与int float相似，只是一个数据类型，后续可以使用这个数据类型创建变量
			
			例如：
			struct Stu{
				// 学生相关属性
				int age;
				char name[20];
			}s1,s2; // 创建了两个s1和s2变量，与int a;创建变量相同
			
		3、结构体类型创建的变量也分为全局变量和局部变量
		
		4、特殊的结构体声明：匿名结构体类型
			struct
			{
				int a;
				char b;
				float c;
			}x;
			struct
			{
				int a;
				char b;
				float c;
			}a[20], *p;
		
		5、匿名结构体类型只能使用一次
		
		6、结构体自引用
		
		7、结构体变量的定义和初始化
		
		8、结构体传参
			
	Note:

*/

#include<stdio.h>

// 声明一个学生信息的结构体

// 写法一
struct Stu_1{
	// 学生相关属性
	int age;
	char name[20];
};

// 写法二
struct Stu{
	int age;
	char name[20];
}s1,s2; // s1 s2是struct Stu类型的变量，并且这边的s1和s2是全局变量，也可以在main内部创建局部变量

// 匿名结构体类型
struct{
	int age;
	char name[20];
}s4;// 匿名结构体类型只能使用一次，如果你在声明结构体时创建了一个对象，那么后续就无法再使用该结构体创建对象

struct{
	int a;
	char b;
	float c;
}x;

struct{
	int a;
	char b;
	float c;
}a[20], *p;

// 结构体自引用
// 像链表
struct Node{
	int data;
	// struct Node next; // 这种写法是错误的，无限套娃内存会爆炸
	struct Node* next; // 正确写法，相当于一个内存拆成两个部分，一个部分是数据域(存放数据)，另一个部分是指针域(用来存放下一个数据所在内存的地址)
};

/* 
typedef struct Node{
	int data;
	struct Node* next;
}* linklist;

等价于
typedef struct Node{
	int data;
	struct Node* next;
};
typedef struct Node* linklist; // 对struct Node*类型重命名位linklist

*/

// 匿名结构体与自引用结合
// 错误写法，
// typedef struct{
	// int data;
	// Node* next;
// }Node; // 将匿名结构体重命名位Node

// 正确写法
typedef struct Node{
	int data;
	struct Node* next;
}Node;

//结构体变量的定义和初始化
struct point_1{
	int math;
	int english;
}s5; // 定义一个s5的结构体变量

struct point{
	int math;
	int english;
}s6 = {10,20}; // 结构体变量s6初始化

struct point_2{
	int math;
	int english;
	struct Stu s;
};

// 结构体传参
struct S{
	int data[100];
	int num;
};

void print1(struct S x){
	int i  = 0;
	for(i=0;i<3;i++){
		printf("%d ",x.data[i]);
	}
	printf("%d\n",x.num);
}

// const锁住，变成常变量，以防改变test值
void print2(const struct S* x){
	int i  = 0;
	for(i=0;i<3;i++){
		printf("%d ",x->data[i]);
	}
	printf("%d\n",x->num);
}

int main(){
	
	struct Stu s3; // 使用声明的结构体类型创建一个局部变量s3
	// p = &x;	// 报错，原因：虽然x变量和*p的结构体成员一样，但是在编译器看来，依然是两个不同的结构体类型
	
	printf("%d\n",sizeof(struct Node));//16
	
	struct point s7; // 结构体变量定义
	struct point s8 = {20,50}; // 结构体变量定义的同时初始化
	struct point_2 s9 = {10,1,{22,"zhangsan"}};
	
	struct S test = {{10,2,3},10};
	print1(test);	// 传值调用-占用时间和空间
	print2(&test);	// 传址调用-使用空间小
	
	return 0;
}
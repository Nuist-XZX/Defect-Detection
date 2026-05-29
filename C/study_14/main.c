/* 
    指针学习：2026/05/13-2026/05/15
    
    内容：指针
		1、字符指针：是指针，char*表示指向字符类型的指针；
		2、指针数组：是数组，用来存放指针的数组。例如 int* arr[5]，他的每个元素是int*的指针；
		3、数组指针：是指针，与char*、int*相似，表示指向数组类型的指针；
		4、数组参数、指针参数
		5、函数指针：是指针，表示指向函数的指针；
		6、函数指针数组：把函数的指针放在数组中，就是函数指针数组。即存放函数指针的数组
		7、指向函数指针数组的指针：是指针，表示指向函数指针数组的指针。
		8、回调函数：
			回调函数就是一个通过函数指针调用的函数。如果你把函数的指针（地址）作为参数传递给另一个
			函数，当这个指针被用来调用其所指向的函数时，我们就说这是回调函数。回调函数不是由该函数
			的实现方直接调用，而是在特定的事件或条件发生时由另外的一方调用的，用于对该事件或条件进行响应

	Note:
		1、 在C语言中,下标运算符a[b]等价于*(a + b);
		
		2、 []的优先级要高于*;
		
		3、 一个内存包括多个内存单元，每个内存单元大小是1个字节，包含一个由32/64bit 0/1组成的地址。
		   当32/64位机器时，每个内存包含2^32/2^64根地址线/内存单元。
		   
		4、&数组名表示的是数组的地址，而不是数组首元素的地址。
		
		5、int (*p)[5]:
			p的类型是int (*)[5];
			p是指向一个整型数组的，数组5个元素，每个元素是int类型 int [5];
			p+1 -> 跳过一个5个int类型元素的数组。
			
		6、int arr[10]:
			int (*p)[10] = &arr; // 这里&arr的类型是int (*)[10]，所以才能赋给指针p
			&arr+1 -> 跳过40个字节/10个int类型元素的数组
			
		7、&arr的类型是int (*)[10]
		  arr的类型是int*
		  数组的类型决定了它+/-1时是+/-多少个字节。
		  
		8、&函数名取出的就是函数的地址；函数名也是函数的地址。
		  对于函数来说，&函数名和函数名都是函数的地址。
		  
		9、void* = 万能指针,可以指向任何类型的数据：int、float、char、结构体等等。
		  但是因为void*指针是无具体类型的指针，所以不能直接进行解引用操作，也不能进行+/-整数操作。
		  因此，在解引用之前需要对指针进行强制类型转换。例如：void* a;强制类型转换(int*)a;解引用*(int*)a
		  
		10、	qsort使用快速排序的思想实现的一个排序方式，可以排序任意类型的数据
			void qsort(void *base, size_t nitems, size_t size, int (*compar)(const void* p1, const void* p2));
			base: 指向待排序数组的第一个元素的指针。
			nitems: 数组中的元素数量。
			size: 数组中每个元素的大小（以字节为单位）。
			compar: 比较函数的指针（函数指针），该函数用于比较两个元素。比较函数应当返回一个整数，表示比较结果：
					小于零：表示第一个元素小于第二个元素。	-> *p1 < *p2
					等于零：表示两个元素相等。			-> *p1 = *p2
					大于零：表示第一个元素大于第二个元素。 -> *p1 > *p2
		
		11、结构体指针：
			struct Student s;	//定义结构体变量
			struct Student *p = &s;	定义结构体指针
			s.name等价于p->name
		
		

*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int DIV(int a, int b);
int ADD(int a, int b);
int SUB(int a, int b);
int MUL(int a, int b);
void calc(int (*x)(int, int));

void print1(int* x,int r,int c);
void print2(int x[3][5],int r,int c);
void print3(int (*x)[5],int r,int c);

void exchange(int* x, int* arr_10);
int cmp_int(const void* p1, const void* p2);
int cmp_struct_by_name(const void* p1, const void* p2); // 按名字比较结构体大小

// 模拟qsort实现冒泡排序
int int_cmp(const void * p1, const void * p2);
void _swap(void *p1, void * p2, int size);
void bubble(void *base, int count , int size, int(*cmp )(const void *, const void *));

typedef void (* pf_t)(int); // 把void (*)(int)类型重命名为pf_t

struct stu{
	char name[20];
	int age;
};

int main(){
	system("CHCP 65001");
	/* 
		1、字符指针
	*/
	char ch = 'w';
	printf("%c\n",ch);
	char *pc = &ch;
	*pc = 'q';
	printf("%c\n",ch);

	// 加入const修饰常变量
	const char* p = "sadaf";// 把字符串首字符s的地址赋值给p，而不是把字符串sadaf赋值给p
	// *p = 'c';
	// 注意无论是否加上const关键字，*p这里不能被重新赋值，"sadaf" 是字符串字面量，编译后会被放在程序的只读数据段（RODATA），这块内存只能读，不能写，操作系统会保护它p 只是一个指针，存的是这个只读内存的首地址
	printf("%s\n",p);// %s打印字符串，知道看到\0结束；%c打印单个字符
	
	char str1[] = "hello bit.";
	char str2[] = "hello bit.";
	const char *str3 = "hello bit.";
	const char *str4 = "hello bit.";
	if(str1 == str2)//这边也是比较的首字符地址，只是这两个字符是存储在两个不同的数组中，所以首字符地址不同
		printf("str1 and str2 are same\n");
	else
		printf("str1 and str2 are not same\n");//字符串内容比较要用strcmp

	if(str3 == str4)//比较首字符的地址，因为两个指针都指向同一个字符串，所以首字符地址相同
		printf("str3 and str4 are same\n");//首地址相同
	else
		printf("str3 and str4 are not same\n");
	
	/* 
		2、指针数组
	*/
	int arr1[] = {1,2,3,4};
	int arr2[] = {2,3,4,5};
	int arr3[] = {3,4,5,6};
	
	int* arr[] = {arr1,arr2,arr3}; // 这里的arr1是数组名，数组名相当于arr1的首元素arr1[0]=1的地址
	printf("%p\n",arr[0]);//AC15DFF870
	printf("%p\n",arr[0]+1);//AC15DFF874
	
	for(int i = 0; i<3;i++){
		for(int j = 0;j<4;j++){
			// printf("%d ",*(arr[i] + j));
			printf("%d ",arr[i][j]); // arr[i][j] = (arr[i])[j]等价于a[b] = *(a + b) = *(arr[i] + j)
		}
		printf("\n");
	}
	
	// 两种写法的指针数组
    int* arr_a[] = {arr1, arr2, arr3};
    int* arr_b[] = {&arr1[0], &arr2[0], &arr3[0]};
    
    // 验证地址值完全相同
    printf("arr_a[0] = %p\n", (void*)arr_a[0]);
    printf("arr_b[0] = %p\n", (void*)arr_b[0]);
    printf("&arr1[0] = %p\n", (void*)&arr1[0]);
    
    // 验证通过两种方式访问元素的结果相同
    printf("arr_a[0][1] = %d\n", arr_a[0][1]); // 输出2
    printf("arr_b[0][1] = %d\n", arr_b[0][1]); // 输出2
	
	char **arr4[5];//二级字符指针的数组,每个元素中存放的是字符的二级指针
	
	/* 
		3、数组指针
	*/
	int *p1[10];	//p1是指针数组，数组中存放的是int*类型的指针
	int (*p2)[10];	//p2是数组指针，p2指向的是一个数组，该数组有10个元素，每个元素是int类型
	
	/* 
		3.1 数组名
		数组名通常表示的是数组首元素的地址，但是有2个意外：
		1、sizeof(数组名)，这里的数组名表示整个数组，计算的是整个数组的大小；
		2、&数组名，这里的数组名表示的依然是整个数组，所以&数组名取出的是整个数组的地址。
		  即：&数组名表示的是数组的地址，而不是数组首元素的地址。
	*/
	int a = 10;
	int arr5[10] = {1,2,3,4,5,6,7,8,9,10};
	// p5的类型是int* [100]
	// p3的类型是int*
	// p4的类型是int (*)[10]
	int* p5[100] = {&a,&arr1[0]};	//指针数组，是数组，存放的内容是地址，可以是数组的地址也可以是整型的地址也可以是字符的地址,具体看指针数组的类型。
	int* p3 = arr5;
	// p4可以理解为：p4指向一个一维数组，指向的数组是10个元素，每个元素是int类型。
	int (*p4)[10] = &arr5;			//数组指针，是指针，指向数组类型，只可以存放数组的地址。
	// 整型指针是用来存放整型的地址；
	// 字符指针是用来存放字符的地址；
	// 数组指针是用来存放数组的地址(&数组名)；
	
	printf("%p\n", arr5);		//0098F8C8
	printf("%p\n", arr5+1);		//0098F8CC -> 0098F8C8 + 4(4字节)

	printf("%p\n", &arr5[0]);	//0098F8C8
	printf("%p\n", &arr5[0]+1);	//0098F8CC -> 0098F8C8 + 4(4字节)
	
	printf("%p\n", &arr5);		//0098F8C8	0x0098F8F0-0x0098F8C8=0x28=2*16^1+8*16^0=40
	printf("%p\n", &arr5+1);	//0098F8F0 -> 0098F8C8 + 40(40字节)

	int sz = sizeof(arr5);
	printf("%d\n",sz);			//40字节
	
	/* 
		3.2 数组指针的使用
	*/
	// int (*p6)[] = &arr5; // 注意,[]中的数组数据长度不能省略,否则报错
	// 用p4/p3打印数组中的数据
	// 方法一：很变扭，不建议这样做
	int sza = sizeof(arr5)/sizeof(arr5[0]);
	for(int i = 0; i < sza; i++){
		//p4是指向数组arr5地址,*p4就相当于数组名,数组名又相当于数组首元素的地址,
		//所以*p4本质上是数组首元素的地址。
		// printf("%d\n",(*p4)[i]);
		printf("%d\n",*(*p4 + i));
	}
	// 方法二：推荐
	for(int i = 0; i < sza; i++){
		printf("%d\n",p3[i]);
		// printf("%d\n",*(p3 + i));
	}
	
	// 二维数组的使用
	int arr6[3][5] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
	// 注意：二维数组的首元素地址是他的第一行的地址，即二维数组的首元素是他的第一行
	print1((int*)arr6,3,5);
	printf("\n");
	print3(arr6,3,5);
	printf("\n");
	print2(arr6,3,5);
	printf("\n");

	/* 
		4、数组参数、指针参数
		
		Note:
			1、数组传参，形参可以写成数组也可以写成指针
			
		4.1 一维数组传参
		void test(int arr[])	//可行
		{}
		void test(int arr[10])	//可行
		{}
		void test(int *arr)		//可行
		{}
		void test2(int *arr[20])//可行
		{}
		void test2(int **arr)	//可行
		{}
		int main()
		{
			int arr[10] = {0};
			int* arr2[20] = {0}; // 20个元素，每个元素是一个int*类型
			test(arr);
			test2(arr2);
		}
		
		4.2 二维数组传参
		void test(int arr[3][5])	//可行
		{}
		void test(int arr[][])		//不可行，二维数组至少要写列数，不可全部省略
		{}
		void test(int arr[][5])		//可行
		{}
		//总结：二维数组传参，函数形参的设计只能省略第一个[]的数字。因为对一个二维数组，可以不知道有多少行，但是必须知道一行多少元素。
		void test(int *arr)			//不可行，这是一维数组的形式
		{}
		void test(int* arr[5])		//不可行，形参中的arr是指针数组，表示的是包含5个int*类型元素的数组，每个元素都是一个指针
		{}
		void test(int (*arr)[5])	//可行
		{}
		void test(int **arr)		//不可行
		{}
		void test2(int *arr)		//可行
		{}
		int main()
		{
			int arr[3][5] = {0};
			test(arr);
			test2((int*)arr);
		}
		
		4.3 一级指针传参
		void print(int *p, int sz)
		{
			int i = 0;
			for(i=0; i<sz; i++)
			{
				printf("%d\n", *(p+i));
			}
		}
		int main()
		{
			int arr[10] = {1,2,3,4,5,6,7,8,9};
			int *p = arr;
			int sz = sizeof(arr)/sizeof(arr[0]);
			//一级指针p，传给函数
			print(p, sz);
			return 0;
		}
		
		4.4 二级指针传参
		void test(int** ptr)
		{
			printf("num = %d\n", **ptr);
		}
		int main()
		{
			int n = 10;
			int*p = &n;
			int **pp = &p;
			test(pp);
			test(&p);
			return 0;
		}
	*/
	
	/* 
		5、函数指针
	*/
	printf("%p\n", print1);		//0x004513B1
	printf("%p\n", &print1);	//0x004513B1
	// 函数地址赋值-函数的指针写法
	// void是函数的类型，也可以是int等；(int*,int,int)是函数的形参类型，根据函数形参来写。
	void (*pr)(int*, int, int) = &print1;
	printf("%p\n",pr);			//0x004513B1
	//函数指针的调用
	(*pr)((int*)arr6,3,5); // 方式一：标准写法
	// pr((int*)arr6,3,5); // 方式二
	printf("\n");
	
	// 函数指针调用的另一种方式
	calc(ADD);	// 将函数地址传入calc
	
	/*
	(*(void (*)())0)();
	 
		void (*)() 是一个函数的指针类型；
		(void (*)())0)是将0的int强制类型转换成void (*)()的函数指针类型，所有0由int变成了一个函数地址；
		*(void (*)())0是对这个函数地址解引用，找到这个函数；
		(*(void (*)())0)()因为void (*)()函数指针类型中函数是没有任何参数的，所以解引用后的函数也没有传入任何参数。		
		
		注意：以上代码是一次函数调用，调用的是0作为地址的函数。
	
	void (*signal(int , void(*)(int)))(int);
	pf_t signal(int, pf_t);//重命名void (*)(int)后的书写方式
	
		signal是函数名；
		(int , void(*)(int))是signal的参数类型；
		signal(int , void(*)(int))返回的也是函数指针类型；
		
		注意：以上代码是一个函数声明，声明的signal函数的第一个参数类型是int，
			第二个参数类型函数指针，该函数指针指向的函数参数是int，返回类型是void；
			signal函数的返回类型也是一个函数指针，该函数指针指向的函数参数是int，返回类型是void。
	*/
	
	/* 
		6、函数指针数组
	*/	
	// 例如现在有4个函数：ADD SUB MUL DIV
	int (*pf)(int, int) = &ADD; // 函数指针，pf是函数指针
	int (*arr7[4])(int, int) = {ADD, SUB, MUL, DIV}; // arr7就是函数指针数组
	
	// 函数指针数组的调用
	for(int i = 0; i<4; i++){
		int ret1 = arr7[i](4,3);
		printf("%d\n",ret1);
	}
		
	// 利用函数指针数组实现一个计算器
	int x, y;
	int input = 1;
	int ret = 0;
	int(*arr8[5])(int x, int y) = {0, ADD, SUB, MUL, DIV}; //转移表
	// while (input){
		// printf( "*************************\n" );
		// printf( " 1:add           2:sub \n" );
		// printf( " 3:mul           4:div \n" );
		// printf( "*************************\n" );
		// printf( "请选择：" );
		// scanf( "%d", &input);
		// if ((input <= 4 && input >= 1)){
			// printf( "输入操作数：" );
			// scanf( "%d %d", &x, &y);
			// ret = (*arr8[input])(x, y);
		// }
		// else{
			// printf( "输入有误\n" );
		// }
		// printf( "ret = %d\n", ret);
	// }
	
	/* 
		7、指向函数指针数组的指针 - 不常用
	*/		
	// 指针数组和数组指针
	int arr9[10] = {0};
	int* p5_1[100] = {&arr9[0]};			// 指针数组
	int* p3_1 = arr9;						// 指针 
	int (*p4_1)[10] = &arr9;				// 数组指针
	
	// 函数指针、函数指针数组、指向函数指针数组的指针
	// int ADD(int x, int y);				// 假设有个函数
	int (*p7)(int x, int y) = ADD;			// 函数指针
	int (*p8[10])(int, int) = {ADD};		// 函数指针数组
	int (*(*p9)[10])(int, int) = &p8;		// 指向函数指针数组的指针
	
	/* 
		8、回调函数
	*/
	// 冒泡排序
	int arr10[] = {5,3,8,4,2,12,4,13,6,7};
	for(int i = 9; i > 0; i--){
		exchange(arr10,&arr10[i]);
	}
	for(int i = 0; i < 10; i++){
		printf("%d ",arr10[i]);
	}
	printf("\n");
	
	// qsort排序整型
	qsort(arr10, 10, sizeof(arr10[0]), cmp_int);
	for(int i = 0; i < 10; i++){
		printf("%d ",arr10[i]);
	}
	printf("\n");
	
	// qsort排序结构体数据
	// 定义一个结构体数组
	struct stu s[] = {{"zhangsan",20},{"lisi",15},{"wanger",18}};
	int szs = sizeof(s)/sizeof(s[0]);
	qsort(s, szs, sizeof(s[0]), cmp_struct_by_name);
	
	printf("@@@@@@@@@@@@@@@@@@@@@@\n");
	bubble(arr10, 10, sizeof(arr10[0]), int_cmp);
	for(int i = 0; i < 10; i++){
		printf("%d ",arr10[i]);
	}
	printf("\n");
	

	
	return 0;
}

int ADD(int x, int y){
	return x+y;
}

int SUB(int x, int y){
	return x-y;
}

int MUL(int a, int b){
	return a*b;
}

int DIV(int a, int b){
	return a / b;
}

//calc接受函数地址的指针
void calc(int (*x)(int, int)){
	int a = 3;
	int b = 1;
	int c = x(a,b);
	printf("%d\n",c);	//4
}

void print1(int* x,int r,int c){
	for(int i = 0;i<r;i++){
		for(int j = 0;j<c;j++){
			printf("%d ",x[i * c + j]);
		}
		printf("\n");
	}
	
	return ;
}

void print2(int x[3][5],int r,int c){
	for(int i = 0;i<r;i++){
		for(int j = 0;j<c;j++){
			// printf("%d ",x[i * c + j]);
			printf("%d ",x[i][j]);
		}
		printf("\n");
	}
	
	return ;
}

void print3(int (*x)[5],int r,int c){
	for(int i = 0;i<r;i++){
		for(int j = 0;j<c;j++){
		/* 			
			这边x相当于二维数组第一行(一维数组)的地址,
			x+i表示第i行一维数组的地址,
			*(x+i)表示对x+i这一行解引用，相当于拿到这一行的数组名，也就是这一行首元素的地址,
			*(x+i) + j表示这一行第j个元素的地址,
			*(*(x+i) + j)表示这一行第j个元素的值。
			x+i和*(x+i)：前者指向第i行整行地址,后者指向第i行首元素地址,数值相同,长度不同
		*/
			printf("%d ",*(*(x+i) + j));
			// printf("%d ",x[i][j]);
		}
		printf("\n");
	}
	
	return ;
}


// 冒泡排序
void exchange(int* x, int* arr_10){
	if((x+1) <= arr_10){
		if(((*x) > *(x+1))){
			int a = *x;
			*x = *(x+1);
			*(x+1) = a;
			exchange(x+1,arr_10);
		}
		else{
			exchange(x+1,arr_10);
		}
	}
}

int cmp_int(const void* p1, const void* p2){
	// void* = 万能指针,但是void*指针不能直接进行解引用操作
	// if(*(int*)p1 > *(int*)p2)
		// return 1;
	// else if(*(int*)p1 == *(int*)p2)
		// return 0;
	// else
		// return -1;
	
	// 上述if-else判断语句相当于下列return语句。
	// return (*(int*)p1 - *(int*)p2); // 升序
	return (*(int*)p2 - *(int*)p1); // 降序
}

int cmp_struct_by_name(const void* p1, const void* p2){
	// 注意：这边的*p1指的是s[0]，即数组的第0位元素。而第0位元素是结构体，所以*p1就是一个指向第0个元素的结构体指针
	// 即：*p1表示的是{"zhangsan",20}，p1就是指针，可以直接p1->name，但由于是void*类型，所以需要先强制类型转换	
	//(*(struct stu*)p1).name等价于((struct stu*)p1)->name
	return strcmp(((struct stu*)p1)->name,((struct stu*)p2)->name); // 按名字排序
	/* 
		strcmp：
		= 0 → str1 == str2 两个字符串完全一样
		< 0 → str1 < str2 第一个字符串更小
		> 0 → str1 > str2 第一个字符串更大
	*/
	// return (((struct stu*)p1)->age - ((struct stu*)p2)->age); // 按年龄排序
}

int int_cmp(const void * p1, const void * p2){
	return (*( int *)p1 - *(int *) p2);
}

void _swap(void *p1, void * p2, int size){
	int i = 0;
	for (i = 0; i< size; i++){
		char tmp = *((char *)p1 + i);
		*(( char *)p1 + i) = *((char *) p2 + i);
		*(( char *)p2 + i) = tmp;
	}
}

void bubble(void* base, int count , int size, int(*cmp )(const void *, const void *)){
	int i = 0;
	int j = 0;
	for (i = 0; i< count - 1; i++)//轮次
	{
		for (j = 0; j<count-i-1; j++){
			// base 是void*类型，无法直接+/-整数实现地址前进后退
			// char刚好是1个字节，将base强制转换为char*类型，如果转换为int类型或者其他类型，可能+/-1跳过的是4个字节或者更多
			if (cmp ((char *) base + j*size , (char *)base + (j + 1)*size) > 0){
				_swap(( char *)base + j*size, (char *)base + (j + 1)*size, size);
			}
		}
	}
}
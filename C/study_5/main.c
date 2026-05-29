/*
	数组学习2026/04/15

	内容：一维数组
		1、定义：数组是一组相同类型元素的集合
		  创建方式：
				type_t   arr_name   [const_n];
				//type_t 是指数组的元素类型
				//const_n 是一个常量表达式，用来指定数组的大 -- 必须是常量。
				  在C99之前，数组的大小（即const_n）必须是常量或者常量表达式；
				  在C99之后，数组的大小可以是变量，为了支持变长数组，例如：int arr[n];n为变量。
			
	Note:
		1、数组在创建的时候如果想不指定数组的确定的大小就得初始化。

*/

#include<stdio.h>

int main(){

	int arr_int1[10];
	char arr_int2[5];
	double arr_int3[2+3] = {1000.0, 2.0, 3.4, 7.0, 50.0}; //完全初始化
	double arr_int_1[2+3] = {1000.0, 2.0}; 				 //不完全初始化
	char as[] = {'a', 's'};
	char arr[] = "asdasd";

	int arr1[10] = {1,2,3};
	int arr2[] = {1,2,3,4}; // 数组在创建的时候如果想不指定数组的确定的大小就得初始化。
	int arr3[5] = {1,2,3,4,5};
	char arr4[3] = {'a', 98, 'c'};
	char arr5[] = {'a','b','c'};  // a b c 
	char arr6[] = "abcdef"; 		  // a b c d e f \0

	char arr7[5] = {'a','b','c'}; // 初始化内容  a b c 0 0 
	char arr8[5] = "abc"; 		  // 初始化内容  a b c \0 0
	
	//求数组长度
	int sz = sizeof(arr) / sizeof(arr[0]);
	printf("%d\n",sz);
	
	/* 	
	// 下列代码只能在支持C99标准的编译器上使用
	int n = 19;
	scanf("%d",&n);
	int cc[n]; 
	*/
	
	
	return 0;
}
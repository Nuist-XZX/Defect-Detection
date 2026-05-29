/* 
    数组学习：2026/04/16
    
    内容：数组作为函数参数 -- 冒泡排序、数组名

    Note:
		1、C语言中指针地址+1表示加1个元素，而非加1个字节，即例如：指针+1 = 向后移动1个int的长度，
		  所以y + 1 = 地址 + 4字节 = 下一个元素地址。
		  
		2、数组传参传的是首元素的地址。
		
		3、数组名确实可以表示首元素的地址，例如int arr[10]，arr是数组名。但是有两个意外：
			（1）sizeof(数组名)，这里的数组名表示整个数组，计算的是整个数组的大小，单位是字节，sizeof(arr)= 10*4= 40字节；
			（2）&(数组名)，这里的数组名表示整个整数，取出的是整个数组的地址，整个数组的地址和数组首元素地址一样，但有区别，
				arr + 1 与 &arr[0] + 1 一样，但是与 &arr + 1 不同，区别在于 &arr + 1 是加了一整个数组的大小，
				而 arr + 1 与 &arr[0] + 1 只加了数组中一个元素的大小。
		
		4、二维数组的数组名也表示数组首元素的地址。
		  例如：int arr[3][4];arr表示的是第一行一维数组的地址，即第一行的地址，而不是第0行第0列的元素地址。
		  1 2 3 4  --> arr表示的是1 2 3 4这一整行的地址。
		  2 3 4 5  --> arr + 1 表示的是第二行一维数组的地址，即2 3 4 5 这一行的地址。
		  3 4 5 6  --> 每行的地址相差16个字节，每行的一维数组为 4*4 = 16 字节。
		  
		  计算二维数组行和列数 arr[3][4] 数组大小为4*12=48字节
		  sizeof(arr) / sizeof(arr[0])       行数  --> 数组总大小 / 第一行数组的大小           --> 48 / (4*4) = 3
		  sizeof(arr[0]) / sizeof(arr[0][0]) 列数  --> 第一行数组的大小 / 第一行第一个元素的大小 --> (4*4) / 4  = 4
		
		
*/

#include<stdio.h>

void exchange(int *x, int *y, int *p);

int main(){

	// 冒泡排序：重复遍历数组，相邻两个元素比较，大的往后挪，像气泡一样慢慢 “冒” 到数组末尾。
	int arr[] = {5,3,8,4,2,12,4,13,6,7};
	int arr_len = sizeof(arr) / sizeof(arr[0]);
	// printf("&arr[arr_len]: %p\n",&arr[arr_len]);

	int j = 0;// j用来判断最后一位数组，冒泡排序每一轮都会将最后一位变为前几位中最大值。
	for(j = arr_len; j > 0; j--){
		exchange(&arr[0],&arr[1],&arr[j]); //&arr[0]和arr都是代表首元素地址；但是&arr代表的是整个数组的地址（但显示也是显示第一个元素的地址，如果需要用&arr进行地址运算，那么就会运算一整个数组，而非单个数组元素）
		// exchange(arr,arr + 1,arr + j);//因为我是y<p，所以不要arr+j-1，如果y<=p则可以
	}
 
	int i = 0;
	for(i=0;i<arr_len;i++){
		printf("%d ", arr[i]);
	}

	return 0;
}

// 交换函数--递归
void exchange(int *x, int *y, int *p){
	// printf("%p\n",p);
	// 可以y<=p，但是如果y=p那么就会多进行一轮exchange(y,y+1,p)，不需要，而且可能造成数组越界崩溃
	if(y<p){
		if(*x > *y){
			int m;
			m = *x;
			*x = *y;
			*y = m;
			printf("%d %d\n",*x,*y);
			exchange(y,y+1,p);//C语言中指针+1表示加一个元素，即指针+1 = 向后移动1个int的长度，所以y + 1 = 地址 + 4字节 = 下一个元素地址
		}
		else{
			exchange(y,y+1,p);
		}
	}
	// 如果y地址超过数组最后一位地址
	else{
		return;
	}
}

/* 
// 豆包
#include<stdio.h>

// 交换两个指针指向的值
void swap(int *x, int *y) {
    int temp = *x;
    *x = *y;
    *y = temp;
}

// 冒泡排序递归版：从头开始比较到尾
void bubble(int *start, int *end) {
    // 只剩一个元素，结束
    if (start >= end) {
        return;
    }

    // 相邻比较一轮，把最大的放到最后
    for (int *p = start; p < end; p++) {
        if (*p > *(p + 1)) {
            swap(p, p + 1);
        }
    }

    // 最后一个已经排好，递归前面
    bubble(start, end - 1);
}

int main() {
    int arr[] = {5, 3, 8, 4, 2};
    int len = sizeof(arr) / sizeof(arr[0]);

    // 传入：数组起点、最后一个元素地址
    bubble(arr, arr + len - 1);

    // 输出
    for (int i = 0; i < len; i++) {
        printf("%d ", arr[i]);
    }

    return 0;
}
 */

/* 
//视频
#include <stdio.h>

void bubble_sort(int arr[], int sz)
{
	int i = 0;
	for(i=0; i<sz-1; i++){
		int j = 0;
		for(j=0; j<sz-i-1; j++){ // 固定最后一位
			if(arr[j] > arr[j+1]){
				int tmp = arr[j];
				arr[j] = arr[j+1];
				arr[j+1] = tmp;
			}
		}
	}
}

int main(){
	int arr[] = {3,1,7,5,8,9,0,2,4,6};
	int sz = sizeof(arr)/sizeof(arr[0]);
	bubble_sort(arr,sz);
	for(i=0; i<sizeof(arr)/sizeof(arr[0]); i++){
		printf("%d ", arr[i]);
	}
	
	return 0;
}
 */
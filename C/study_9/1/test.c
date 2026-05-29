/* 
    数组学习：2026/04/21
    
    内容：三子棋测试

    Note:
		1、当二维数组传入函数时，如果函数中没有其他参数，即void chessboard(int *x)，那么可以直接将数组传入，
		例如chessboard(arr)或者chessboard(&arr[0][0]);
		2、当二维数组传入函数时，如果函数中有其他参数，例如void chessboard(int *x, int row, int col)，
		则传参时需要将arr强制类型转换，例如chessboard((int*)arr,4,4)或者chessboard(&arr[0][0], 4, 4);
	
	原因：
		二维数组在内存中的存放是：
			arr[0][0] arr[0][1] arr[0][2] arr[0][3]
			arr[1][0] arr[1][1] arr[1][2] arr[1][3]
			...
		arr的真实类型是int(*)[4]，意思是指向“包含4个int的一维数组”的指针，不是普通的int*；
		而我的函数中存在其他参数，即void chessboard(int *x, int row, int col)，所以它需要的是int *的普通整型指针；然而：
			arr 的类型 = int (*)[4]
			函数要的类型 = int *
		所以两个类型不一样 → 编译器报错！bug：expected 'int *' but argument is of type 'int (*)[4]'。
		因为二维数组名 arr 不是普通指针，它是数组指针，类型和 int* 不兼容，所以传过去会报错。
		但如果是一维数组，例如int arr1[5]，那么此时arr1的类型是int *，与函数参数int *x是同一类型，所以编译器不会报错。
		
		另外：C语言规定，函数形参里写二维数组，编译器会把它自动解释成 int (*)[4]。所以当函数中无其他参数时void chessboard(int *x)，编译器不会报错。

*/
#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>

void menu();
void chessboard(int *x, int row, int col);
int estimate(int *x,int value,int row, int col);

int a,m,n = 0;
int player = 1;//玩家1、2互换。1^3=2,2^3=1

int main(){
	//system("chcp 65001");
	
	do{
		menu();
		scanf("%d",&a);
		if(a == 1){
			int arr[4][4] = {0};//数组传入函数时，如果是单个元素传入，则使用&取地址，即&arr[0][0];如果是数组名传入，则直接arr
			chessboard((int*)arr,4,4);
			break;
		}
		else if(a == 2){
			printf("退出游戏\n");
			break;
		}
		else{
			printf("请重新输入\n");
		}
	}while((a != 1)&&(a != 2));
	
}

void menu(){
	printf("**********   选择菜单  **********\n");
	printf("**********  1、进入游戏 **********\n");
	printf("**********  2、退出游戏 **********\n");
}

void chessboard(int *x, int row, int col){
	//数组传入是地址，所以要使用第几行第几列的元素值，直接x[i*col + j]，第i行第j列

	// printf("创建棋盘\n");
	
	int i,j = 0;
	for(i = 0; i < row; i++){
		for(j = 0; j < col; j++){
			printf("%d ",x[i * col + j]);
		}
		printf("\n");
	}
/* 	
	while(1){
		//玩家1和2轮流，默认玩家1开始
		printf("请玩家 %d 输入：",player);
		scanf("%d%d",&m,&n); //sacanf不能写中文提示
		if(((m>0)&&(m<=4))&&((n>0)&&(n<=4))&&(x[(m-1)*col + (n-1)] == 0)){
			// printf("第%d行第%d列\n",m,n);
			x[(m-1)*col + (n-1)] = player;
			
			for(i = 0; i < row; i++){
				for(j = 0; j < col; j++){
					printf("%d ",x[i * col + j]);
				}
				printf("\n");
			}
			
			//判断是否胜利
			if(estimate(x,player,4,4) == 1){
				printf("玩家 %d 胜利！\n",player);
				break;
			}

			//交换对手
			player ^= 3; //异或操作

		}
		else{
			printf("请重新输入\n");
			
		}
	}
	 */
	
	//这边不需要使用dowhile，而且不适合在循环中使用递归，这边递归也只是重复打印了一次数组，并且再进入一次dowhile循环
	//输入棋盘地址，m行n列
	
    //玩家1和2轮流，默认玩家1开始
	printf("请玩家 %d 输入：",player);
	scanf("%d%d",&m,&n); //sacanf不能写中文提示
    if(((m>0)&&(m<=4))&&((n>0)&&(n<=4))&&(x[(m-1)*col + (n-1)] == 0)){
        // printf("第%d行第%d列\n",m,n);
        x[(m-1)*col + (n-1)] = player;
        
        //判断是否胜利
        if(estimate(x,player,4,4) == 1){
            printf("玩家 %d 胜利！\n",player);
             //这边如果直接break，是终止不掉我的循环，因为使用了递归，
             //所以只会跳出最内层的do-while循环，但无法退出整个递归调用；
             //所以删除递归中的dowhile循环语句。
        }
        else{
            //否则交换对手
            player ^= 3; //异或操作
            chessboard(x,4,4);
        }
    }
    else{
        printf("请重新输入\n");
        chessboard(x,4,4);
        
    }
	/* 
	//输入棋盘地址，m行n列
	do{
		//玩家1和2轮流，默认玩家1开始
		printf("请玩家 %d 输入：",player);
		scanf("%d%d",&m,&n); //sacanf不能写中文提示
		if(((m>0)&&(m<=4))&&((n>0)&&(n<=4))&&(x[(m-1)*col + (n-1)] == 0)){
			// printf("第%d行第%d列\n",m,n);
			x[(m-1)*col + (n-1)] = player;
			
			//判断是否胜利
			if(estimate(x,player,4,4) == 1){
				printf("玩家 %d 胜利！\n",player);
				break 1; //这边如果直接break，是终止不掉我的循环，因为使用了递归，所以只会跳出最内层的do-while循环，但无法退出整个递归调用；如果一定要使用递归，则让递归函数返回某个状态值，例如ruturn 1。
			}
			else{
				//否则交换对手
				player ^= 3; //异或操作
				chessboard(x,4,4);
			}
		}
		else{
			printf("请重新输入\n");
			
		}
	}while((m<0)||(m>4)||(n<0)||(n>4)||(x[(m-1)*col + (n-1)] != 0));
 */
}
 
//判断是否三个棋子相连
int estimate(int *x,int value,int row, int col){
	// 现在的x相当于是个一维数组
	int p,q = 0; //p行q列
	
	// 先判断所有行和斜线
	for(p = 0; p < row; p++){
		for(q = 0; q < col; q++){
			//判断行
			if(((x[p * col + q]) == value) && (q < 2) && ((x[p * col + (q + 1)]) == value) && ((x[p * col + (q + 2)]) == value)){
				return 1;
			}
			//判断右斜线
			else if((p < 2) && (q < 2) && ((x[p * col + q]) == value) && ((x[(p + 1) * col + (q + 1)]) == value) && ((x[(p + 2) * col + (q + 2)]) == value)){
				return 1;
			}
			//判断左斜线
			else if((p < 2) && (q > 1) && ((x[p * col + q]) == value) && ((x[(p + 1) * col + (q - 1)]) == value) && ((x[(p + 2) * col + (q- 2)]) == value)){
				return 1;
			}
		}
	}
	
	// 再判断所有列
	for(q = 0; q < col; q++){
		for(p = 0; p < row; p++){
			if(((x[p * col + q]) == value) && (p < 2) && ((x[(p + 1) * col + q]) == value) && ((x[(p + 2) * col + q]) == value)){
				return 1;
			}
		}
	}
	
}

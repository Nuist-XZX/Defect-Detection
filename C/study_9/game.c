/*
	Note:
		1、如果是字符比较，即比较两个''括起来的字符是否相同，则直接使用 ==；
		  如果是字符串比较，即比较两个""括起来的字符串是否相同，则使用strcmp(s1,s2) == 0；为0相同，非0不同
		2、#define定义的是一个常量，常量不能被赋值，当player ^= 3时会报错，等号左边必须是变量，所以应该int player；
		3、rand()函数用法： stdlib.h库  time()为time.h库
			如果直接调用rand()，那么每次随机生成的结果都是一样；
			如果用srand((unsigned int)time(NULL))在主函数开头设置随机种子（利用时间），那每次随机结果不同
			其中：sand整个程序只使用一次，不放在循环中；time(NULL)获取当前系统时间(秒级)，保证种子唯一
			用法：rand()默认随机范围太大，在0~32767之间，因此使用时需要固定区间数，如下：
				生成[0,n)的整数			int num = rand() % n;
				生成[a,b]的整数			int num = rand() % (b - a + 1) + a;
				生成随机浮点数				(double)rand() / RAND_MAX
		4、余数的范围x%n(x / n):0<=余数<除数 --> 0~n
*/

#include<stdio.h>
#include<stdlib.h>
#include "game.h"

void menu(){
	printf("**********     选择菜单     **********\n");
	printf("**********  1、玩家 VS 玩家  **********\n");
	printf("**********  2、玩家 VS 电脑  **********\n");
	printf("**********    3、退出游戏    **********\n");
}

// 玩家vs玩家
void chessboard(char *x, int row, int col){
	//数组传入是地址，所以要使用第几行第几列的元素值，直接x[i*col + j]，第i行第j列
    adaptive_circulation(x,row,col);    
    int m,n = 0;
    char symbol; // 玩家棋子
    int player = 1; //玩家1、2互换。1^3=2,2^3=1
	int count = 0;

 	while(1){
		//玩家1和2轮流，默认玩家1开始
		printf("请玩家 %d 输入：",player);
		scanf("%d%d",&m,&n); //sacanf不能写中文提示
		if(((m>0)&&(m<=row))&&((n>0)&&(n<=col))&&(x[(m-1)*col + (n-1)] == ' ')){
			if(player == 1){
                symbol = '*';
                x[(m-1)*col + (n-1)] = symbol;
            }
            else{
                symbol = '#';
                x[(m-1)*col + (n-1)] = symbol;
            }
			count++;
			
			adaptive_circulation(x,row,col);
			
			//判断是否胜利
			if(count != (row * col)){
				if(estimate(x,symbol,row,col) == 1){
					printf("玩家 %d 胜利！\n",player);
					break;
				}	
			}
			else{
				printf("平局\n");
				break;
			}

			//交换对手
			player ^= 3; //异或操作

		}
		else{
			printf("请重新输入\n");
			
		}
	}
}

// 玩家vs人机
void chessboard_computer(char *x, int row, int col){
	//数组传入是地址，所以要使用第几行第几列的元素值，直接x[i*col + j]，第i行第j列
    adaptive_circulation(x,row,col);    
    int m,n = 0;
    char symbol; // 玩家棋子
    int player = 1; //玩家1、2互换。1^3=2,2^3=1
	int count = 0;

 	while(1){
		//玩家开始
		if(player == 1){
			printf("请玩家 %d 输入：",player);
			scanf("%d%d",&m,&n); //sacanf不能写中文提示
			if(((m>0)&&(m<=row))&&((n>0)&&(n<=col))&&(x[(m-1)*col + (n-1)] == ' ')){
				symbol = '*';
                x[(m-1)*col + (n-1)] = symbol;
				count++;
				
				// 打印棋盘
				adaptive_circulation(x,row,col);
				
				//判断是否胜利
				if(count != (row * col)){
					if(estimate(x,symbol,row,col) == 1){
						printf("玩家 %d 胜利！\n",player);
						break;
					}
					else{
						player ^= 3;
						symbol = '#';
						continue;
					}
				}
				else{
					printf("平局\n");
					break;
				}
			}
			else{
				printf("请重新输入\n");
			}
		}
		else{
			printf("电脑输入：\n");
			// 人机
			usual_computer(x,symbol,row,col);
			count++;
			
			// 打印棋盘
			adaptive_circulation(x,row,col);
			
			//判断是否胜利
			if(count != (row * col)){
				if(estimate(x,symbol,row,col) == 1){
					printf("电脑胜利！\n");
					break;
				}
				else{
					player ^= 3;
					symbol = '*';
					continue;
				}
			}
			else{
				printf("平局\n");
				break;
			}
		}
	}
}

//判断是否三个棋子相连 -- 自适应棋盘
int estimate(char *x, char symbol, int row, int col){
	// 现在的x相当于是个一维数组
	int p,q = 0; //p行q列
	
	// 先判断所有行和斜线
	for(p = 0; p < row; p++){
		for(q = 0; q < col; q++){
			//判断行
			if(((x[p * col + q]) == symbol) && (q < (col - 2)) && ((x[p * col + (q + 1)]) == symbol) && ((x[p * col + (q + 2)]) == symbol)){
				return 1;
			}
			//判断右斜线
			else if((p < (row - 2)) && (q < (col - 2)) && ((x[p * col + q]) == symbol) && ((x[(p + 1) * col + (q + 1)]) == symbol) && ((x[(p + 2) * col + (q + 2)]) == symbol)){
				return 1;
			}
			//判断左斜线
			else if((p < (row - 2)) && (q > 1) && ((x[p * col + q]) == symbol) && ((x[(p + 1) * col + (q - 1)]) == symbol) && ((x[(p + 2) * col + (q- 2)]) == symbol)){
				return 1;
			}
		}
	}
	
	// 再判断所有列
	for(q = 0; q < col; q++){
		for(p = 0; p < row; p++){
			if(((x[p * col + q]) == symbol) && (p < (row - 2)) && ((x[(p + 1) * col + q]) == symbol) && ((x[(p + 2) * col + q]) == symbol)){
				return 1;
			}
		}
	}	
}

void game(int x){
	char arr[ROW][COL];//如何传入空白数组--即空格
    
    //初始化数组每个元素值为空格
    int i,j = 0;
	for(i = 0; i < ROW; i++){
		for(j = 0; j < COL; j++){
			arr[i][j] = ' ';
		}
	}
    
	// 开始下棋
	if(x == 1)
		// 玩家vs玩家
		chessboard((char*)arr,ROW,COL);
	else
		// 玩家vs人机
		chessboard_computer((char*)arr,ROW,COL);

}

// 固定棋盘
void circulation(char *x, int row, int col){
    int i = 0;
	for(i = 0; i < row; i++){
		printf(" %c | %c | %c | %c\n",x[i * col + 0],x[i * col + 1],x[i * col + 2],x[i * col + 3]); // 打印字符串用%s(""括起)，打印字符用%c(''括起)
		if(i<row-1)
            printf("___|___|___|___\n");
        else
            printf("   |   |   |   \n");
	}
}

// 根据数组长度自适应调整棋盘分隔符
void adaptive_circulation(char *x, int row, int col){
	int i,j = 0;
	for(i = 0; i < row; i++){
		for(j = 0; j < col; j++){
			if(j < (col -1)){
				printf(" %c |",x[i * col + j]);
			}
			else{
				printf(" %c \n",x[i * col + j]);
			}
			
		}
		
		if(i<row-1){
			for(j = 0; j < col; j++){
				if(j < (col -1)){
					printf("---|");
				}
				else{
					printf("---\n");
				}
			}
		}   
	}
}

// 普通人机
void usual_computer(char *x, char symbol, int row, int col){
	int m,n = 0;
	
	while(1){
		m = rand() % row; // 0~row-1
		n = rand() % col;
		if(x[m * col + n] == ' '){
			x[m * col + n] = symbol;
			break;
		}
	}
}

/* //高级人机
void advance_computer(char *x, char symbol, int row, int col){
	int m,n = 0;
	
	while(1){
		m = rand() % row; // 0~row-1
		n = rand() % col;
		if(x[m * col + n] == ' '){
			x[m * col + n] = symbol;
			break;
		}
	}
}
 */
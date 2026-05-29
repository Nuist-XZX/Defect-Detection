/* 
    数组学习：2026/04/21
    
    内容：三子棋测试

    Note:
		1、switch-case中的break只会跳出switch，而不会跳出循环，所以循环还会继续；
		  而if-else中的break则会跳出循环，结束整个循环。
*/

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include "game.h"

int main(){
	system("chcp 65001");//中文显示
	srand((unsigned int)time(NULL));
	
	int a = 0;
	do{
		menu();
		scanf("%d",&a);
		switch(a){
			case 1:	{		
				game(a);
				break; //switch-case中的break只会跳出switch，而不会跳出循环
			}
			case 2:{
				game(a);
				break;
			}
			case 3:{
				printf("退出游戏\n");
				break;
			}
			default:
				printf("请重新输入\n");
		};
		
	}while(a!=3);//只有选2才会跳出循环
	
}


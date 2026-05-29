/* 
    结构体学习：2026/05/09
    
    内容：结构体
		struct 结构标签{
			
			成员列表;
		}变量列表;//变量列表可有可无
		struct tag{
			member-list;
		}variable-lis;
		
	Note:

*/

#include<stdio.h>
#include<stdlib.h>

struct Point
{
	int x;
	int y;
}p1; //声明类型的同时定义变量p1
struct Point p2; //定义结构体变量p2

//结构声明
//定义一个结构体类型
struct peo{
	char name[20];
	char tele[12];
	char sex[5]; // 女 男 保密 -- 一个汉字占两个字符
	int high;
};
//定义一个结构体变量
struct peo p3;//这边是全局变量，也可以创建局部变量

//结构体嵌套
struct st{
	struct peo p;
	float a;
};

void print1(struct peo* ps){
	printf("%s %s %s %d\n",ps->name,ps->tele,ps->sex,ps->high);
}
void print2(struct peo ps){
	printf("%s %s %s %d\n",ps.name,ps.tele,ps.sex,ps.high);
}

int main(){
	system("chcp 65001");
	struct peo p4 = {"张","1211121","男",12};
	//注意，只有结构体指针变量才能用p4->name等，结构体变量只可以p4.name等
	printf("%s %s %s %d\n",p4.name,p4.tele,p4.sex,p4.high); 
	struct st s = {{"张","1211121","男",12},3.2};
	printf("%s %s %s %d %f\n",s.p.name,s.p.tele,s.p.sex,s.p.high,s.a); 
	print1(&p4);
	print2(p4);

	return 0;
}
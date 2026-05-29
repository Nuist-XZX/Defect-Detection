/*
    C++学习 - 2026/05/29

    内容：函数重载
        1、 定义：用同一个函数名定义不同的函数叫做函数重载。
		
		2、 当函数名和不同的参数搭配时,函数的含义不同
			
		3、 函数重载条件:
			同一个作用域、参数个数不同、参数类型不同、参数顺序不同
			
    Node:
        1、
*/

#include<iostream>

using namespace std;

// 在命名空间中重载,也可以在类中重载
namespace MyFuncReload{
	void func(){
		cout << "void func()" << endl;
	};
	
	void func(int x){
		cout << "void func(int x) - x = " << x << endl;
	};
	
	void func(int x, int y){
		cout << "void func(int x, int y) - x = " << x << " y = " << y <<endl;
	};
};

int main(int argc, char* argv[]){
	MyFuncReload::func();
	MyFuncReload::func(10);
	MyFuncReload::func(10,20);
	
	return 0;
}
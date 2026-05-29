/* 
    C++命名空间学习：2026/05/15
    
    内容：命名空间-namespace

	Note: 

*/

#include<iostream> //
#include "test.h"	//自定义命名空间

using namespace std; // 使用一个名为std的命名空间
using namespace abab;

// 自定义命名空间
namespace Myspace{
	int a = 10;
	void func(){
		cout << "hello\n" << endl;
	};
}

int main(){
	
	cout << "helloworld!\n" << endl;
	// std::cout << "helloworld!\n" << std::endl; //如果不适用using namespace std，则需要加上std::cout
	Myspace::func();
	cout << "Myspace a: " << Myspace::a << "\n" <<endl;
	cout << c << endl;
	func(11);
	
	// 命名空间名字别名 - 给命名空间取名字
	namespace othername = Myspace;
	cout << "Myspace a: " << othername::a << "\n" <<endl;
	
	return 0;
}
/* 
    C++学习 - 2026/05/20
    
    内容：引用
        1、数据类型 &别名 = 原名
            作用：给变量取别名
    
    Node:
        1、int &b = a,其本质上都是修饰的同一块内存，有点像指针取地址
        
        2、引用必须初始化,即：
                int &b = a;
            而非：
                int &b;
        3、引用一旦初始化后就不能更改，例如：
            int &b = a;
            b = c; // 这里只是更改了赋值，并不表示改名字
*/

#include<iostream>

using namespace std;

int main(){
    
    int a = 10;
    int &b = a; // 给变量a取了一个别名叫做b
    b = 12; // 对别名重新赋值，等价于修改原变量，因为他们修饰的是同一个内存
    
    cout << a << endl;
    
    return 0;
}
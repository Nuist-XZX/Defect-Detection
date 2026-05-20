/* 
    C++学习 - 2026/05/20
    
    内容：类和对象
        1、
    
    Node:
        1、
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
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

        4、可以利用引用来进行函数传参，可以代替指针传值的方式，即可以用形参代替实参。
            如果是值传递，那么形参只是实参的一个临时拷贝。

        5、引用做函数的返回值
            1. 不要反悔局部变量的引用
            2. 函数的调用可以作为左值

        6、常量引用：用来修饰形参，防止误操作，即使用const进行修饰
*/

#include <iostream>

using namespace std;

void exchange(int *x, int *y);
void exchange_1(int &x, int &y);
// int& test01();
int &test02();
void show(const int &x);

int main()
{

    int a = 10;
    int &b = a; // 给变量a取了一个别名叫做b
    b = 12;     // 对别名重新赋值，等价于修改原变量，因为他们修饰的是同一个内存

    int c = 32;
    int &d = c; // 等价于 int* const d = &c
    // exchange(&a,&c);
    exchange_1(a, c);

    cout << a << c << endl; // a = 32  c = 12

    // int& test1 = test01();
    // cout << test1 << endl;  // 第一次10
    // cout << test1 << endl;   // 第二次数据错误，不再是10，因为第一次结束后，局部变量已经被内存释放掉
    int &test2 = test02();
    cout << test2 << endl; // 10
    cout << test2 << endl; // 10
    cout << test2 << endl; // 10 因为是静态变量，只有在程序结束后才会释放静态变量

    // 函数调用可以作为左值
    test02() = 1000;
    cout << test2 << endl; // 1000
    cout << test2 << endl; // 1000

    // const 修饰
    int q = 10;
    const int &q_1 = q; // const 修饰了 ，所以之后无法对q内存进行修改
    // q_1 = 12; // 会报错 error: assignment of read-only reference 'q_1'  即q变为只读，不可更改
    cout << q << endl;
    show(c);
    cout << c << endl;

    return 0;
}

// 指针的方式
void exchange(int *x, int *y)
{
    int c = *x;
    *x = *y;
    *y = c;
}

// 引用的方式
void exchange_1(int &x, int &y)
{
    int c = x;
    x = y;
    y = c;
}

// 引用作为函数的返回值
// 1 不要返回局部变量的引用
// int& test01(){

// int as = 10; // 局部变量，存放在栈区

// return as;
// } // 64位会报错 86位第二次打印会错误，因为第一次打印编译器会保留，第二次时as的内存已经释放，因为是局部变量，用完就消失

// 函数调用作为左值
int &test02()
{

    static int as = 10; // 静态变量，存放在静态区，静态区的数据在程序结束后系统释放。

    return as; // 返回值as的类型是int&,返回的是一个as的引用，等价于return (int&)as，等价于int& b = as,即调用者拿到的是一个变量的别名
}

// const 修饰引用变成常变量
void show(const int &x)
{
    // x = 1000; // 因为已经用const进行修饰，所以这边无法赋值，会报错
    cout << x << endl;
}

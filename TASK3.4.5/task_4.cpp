#ifndef DUAL_H
#define DUAL_H
#include <cmath>
#include <iostream>
#include <string>
using namespace std;
class Dual
{
public:
    double val;  //数值部
    double der;  //导数部分
    //用初始化列表初始化成员对象
    Dual(double value = 0.0, double derivative = 0.0):val(value), der(derivative){};

    //运算符重载
    //Dual与Dual
    Dual operator+(const Dual& fun) const
    {
        return Dual(val + fun.val, der + fun.der);
    }
    Dual operator-(const Dual& fun) const
    {
        return Dual(val - fun.val, der - fun.der);
    }
    Dual operator*(const Dual& fun) const
    {
        return Dual(val * fun.val, der * fun.val + val * fun.der);
    }
    Dual operator/(const Dual& fun) const
    {
        return Dual(val / fun.val, (der * fun.val + val * fun.der) / pow(fun.val, 2));
    }

    //Dual与常数
    Dual operator+(double num) const
    {
        return Dual(val + num, der);
    }
    Dual operator-(double num) const
    {
        return Dual(val - num, der);
    }
    Dual operator*(double num) const
    {
        return Dual(val * num, der);
    }
    Dual operator/(double num) const
    {
        return Dual(val / num, der);
    }

    //符号
    Dual operator-()
    {
        return Dual(-val, -der);
    }
};

//全局函数实现常见数学函数
//函数重载

//===基本初等函数===
Dual sin(const Dual& x)
{
    return Dual(sin(x.val), cos(x.val) * x.der);
}
Dual cos(const Dual& x)
{
    return Dual(cos(x.val), -sin(x.val) * x.der);
}
Dual tan(const Dual& x)
{
    if(cos(x.val) == 0)
    {
        throw runtime_error("除数不能为0！");
    }
    return Dual(tan(x.val), x.der / pow( cos(x.val), 2));
}
Dual cot(const Dual& x)
{
    if(sin(x.val) == 0)
    {
        throw runtime_error("除数不能为0！");
    }
    return Dual(1 / tan(x.val), -x.der / pow( sin(x.val), 2));
}
Dual sec(const Dual& x)
{
    if(cos(x.val) == 0)
    {
        throw runtime_error("除数不能为0！");
    }
    return Dual(1 / cos(x.val), tan(x.val) * x.der / cos(x.val));   
}
Dual csc(const Dual& x)
{
    if(sin(x.val) == 0)
    {
        throw runtime_error("除数不能为0！");
    }
    return Dual(1 / sin(x.val), -x.der * cos(x.val) / pow( sin(x.val), 2));
}

// //===反三角函数===
Dual asin(const Dual& x)
{
    if(pow(x.val, 2) == 1)
    {
        throw runtime_error("除数不能为0！");
    }
    return Dual(asin(x.val), x.der / sqrt(1 - pow(x.val, 2)));
}
Dual acos(const Dual& x)
{
    if(pow(x.val, 2) == 1)
    {
        throw runtime_error("除数不能为0！");
    }
    return Dual(acos(x.val), -x.der / sqrt(1 - pow(x.val, 2)));
}
Dual atan(const Dual& x)
{
    return Dual(atan(x.val), x.der / (1 + pow(x.val, 2)));
}
Dual acot(const Dual& x)
{
    return Dual(atan(1 / x.val), -x.der / (1 + pow(x.val, 2)));
}

// //===双曲函数===
Dual sinh(const Dual& x)
{
    return Dual(sinh(x.val), x.der * cosh(x.val));
}
Dual cosh(const Dual& x)
{
    return Dual(cosh(x.val), x.der * sinh(x.val));
}
Dual tanh(const Dual& x)
{
    if(cosh(x.val) == 0)
    {
        throw runtime_error("除数不能为0！");
    }
    return Dual(tanh(x.val), x.der / pow( cosh(x.val), 2));
}
Dual coth(const Dual& x)
{
    return Dual(1 / tanh(x.val), -x.der / pow(sinh(x.val), 2));
}

// //===指数与对数函数===
Dual exp(const Dual& x)
{
    return Dual(exp(x.val), exp(x.val) * x.der);
}
Dual log(const Dual& x)
{
    if(x.val <= 0)
    {
        throw runtime_error("指数的自变量要大于0！");
    }
    return Dual(log(x.val), x.der / x.val);
}
Dual log10(const Dual& x)
{
    if(x.val <= 0)
    {
        throw runtime_error("指数的自变量要大于0！");
    }
    return Dual(log10(x.val), x.der / (x.val * log(10)));
}

// //===幂函数==
Dual pow(const Dual& x, double num)
{
    if(x.val < 0)
    {
        throw runtime_error("指数的自变量要大于0！");
    }
    return Dual(pow(x.val, num), x.der * num * pow(x.val, num - 1));
}
Dual pow(const Dual& x, const Dual& num)
{
    if(x.val < 0)
    {
        throw runtime_error("要求底数大于0！");
    }
    return Dual(pow(x.val, num.val), pow(x.val, num.val) * (num.der * log(x.val)) + num.val * x.der / x.val);
}

// //===常见复合算术函数===
Dual sqrt(const Dual& x)
{
    if(x.val < 0)
    {
        throw runtime_error("开平方数要大于0！");
    }
    return Dual(sqrt(x.val), x.der / (2 * sqrt(x.der)));
}
Dual abs(const Dual& x)
{
    return Dual(x.val, (x.val > 0) ? x.der : (x.val < 0) ? -x.der : 0);
}
Dual floor(const Dual& x)
{
    return Dual(floor(x.val), 0);
}
Dual ceil(const Dual& x)
{
    return Dual(ceil(x.val), 0);
}
Dual erf(const Dual& x)
{
    return Dual(erf(x.val), x.der * (2 / sqrt(M_PI)) * exp(-pow(x.val, 2)));
}

// //===自定义函数===
Dual sigmoid(const Dual& x)
{
    return Dual(1 / (1 + exp(-x.val)), x.der * exp(-x.val) / pow(1 + exp(-x.val), 2));
}

//测试代码
void function1()
{
    Dual x(3.0, 1.0);  // x = 3
    //返回值赋值给y
    Dual y = sin(x) + x * x;  
    cout << "f(x) = " << y.val << endl;
    cout << "f'(x) = " << y.der << endl;
    //验证一下
    cout << "f(x) = " << (sin(3) + 3 * 3) << endl;
    cout << "f'(x) = " << (cos(3) + 2 * 3) << endl;
}

void function2()
{
    Dual x(1.0, 1.0);
    Dual y = sigmoid(x);  //Sigmoid
    cout << "f(x) = " << y.val << endl;
    cout << "f'(x) = " << y.der << endl;
}

int main()
{
    function1();
    function2();
    system("pause");
    return 0;
}



#endif
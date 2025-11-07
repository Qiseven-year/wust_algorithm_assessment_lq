#include <iostream>
#include <string>
using namespace std;

//类模板使成员变量多样化
template<class T>
class Person
{
private:
    T element;
    //表示定义状态
    bool isset = false;
public:
    void setElement(const T& value)
    {
        element = value;
        isset = true;
    }
    //获取值，未定义则抛出异常
    T getElement()
    {
        if(!isset)
        {
            try
            {
                throw "错误！未被定义！";
            }
            catch(const char* e)
            {
                cout << "e：" << e << endl;
            }
            throw;
        }
        cout << "值element为：" << element << endl;
        return element;
    }
    //定义常函数，获取定义状态
    bool isSet()
    {
        return isset;
    }
    //清除定义状态，不清除内存
    void clearSet()
    {
        isset = false;
    }
};

int main()
{
    //调试
    //正常运行
    int num = 23;
    Person<int> p;
    p.setElement(num);
    p.getElement();
    
    // //抛出错误
    // Person<string> pp;
    // pp.getElement();
    
    //清除状态
    p.clearSet();
    cout << p.isSet() << endl;

    system("pause");
    return 0;
}
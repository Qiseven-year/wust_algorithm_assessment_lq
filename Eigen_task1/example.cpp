#include <iostream>
#include <Eigen/Dense>
using namespace std;
using namespace Eigen;
int main()
{
    //创建矩阵
    Matrix2d A;
    A << 1, 2,
         3, 4;
    //矩阵向量乘法
    Vector2d b(1.0,1.0);
    Vector2d result = A * b;
    cout << result << endl;
    return 0;

}
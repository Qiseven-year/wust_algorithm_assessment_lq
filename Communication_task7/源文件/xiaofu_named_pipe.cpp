//命名管道服务器端
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <chrono>
#include <unistd.h>
#include <sys/stat.h>
using namespace std;

struct shared_Inerface
{
    char date[256];
    int id;
    int64_t timestamp;
};

int main()
{
    //创建命名管道
    const char* pipe_name = "/tmp/xiaofu_pipe";
    //删除可能存在的旧管道
    unlink(pipe_name);
    if (mkfifo(pipe_name, 0666) == -1)
    {
        cout << "错误：无法创建命名管道" << endl;
        return 1;
    }

    cout << "小符同学 - 命名管道服务器启动" << endl;
    cout << "等待客户端连接..." << endl;
    
    int message_count = 0;
    while(message_count < 5)
    {
        //打开管道进行写入,会阻塞直到客户端连接
        ofstream pipe(pipe_name, ios::binary);
        if (!pipe)
        {
            cerr << "无法打开管道" <<endl;
            sleep(1);
            continue;
        }

        //准备数据
        shared_Inerface date;
        string message = "来自小符的管道消息" + to_string(message_count);
        strncpy(date.date, message.c_str(), sizeof(date.date) - 1);
        date.date[sizeof(date.date) - 1] = '\0';
        date.id = message_count;
        date.timestamp =chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
        //写入数据
        pipe.write(reinterpret_cast<const char*>(&date), sizeof(date));
        pipe.close();
        cout << "发送" << date.date
             << "ID" << date.id
             << "时间戳" << date.timestamp << endl;

        //等待2秒
        sleep(2);
        message_count++;
    }

    cout <<"服务器完成，删除管道" << endl;
    //删除管道
    unlink(pipe_name);

    return 0;
}
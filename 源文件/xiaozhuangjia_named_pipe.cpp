//命名管道客户端
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <chrono>
#include <unistd.h>
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

    cout << "小装甲同学 - 命名管道客户端启动" << endl;
    cout << "等待服务器连接" << endl;
    
    int messages_received = 0;
    while(messages_received < 5)
    {
        //打开管道进行读取
        ifstream pipe(pipe_name, ios::binary);
        if (!pipe)
        {
            cout << "等待管道数据" << endl;
            //等待1s再试
            sleep(1);
            continue;
        }

        //读取数据
        shared_Inerface date;
        pipe.read(reinterpret_cast<char*>(&date), sizeof(date));

        if (pipe.gcount() == sizeof(date))
        {
            //计算时间差
            auto current_time =chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
            int64_t time_diff = current_time - date.timestamp;
            cout << "发送" << date.date
                 << "ID" << date.id
                 << "时间戳" << date.timestamp
                 << "延迟" << time_diff << "ms" << endl;
            messages_received++;
        }
        else
        {
            cout << "读取数据不完整，继续等待... " << endl;
        }
        
        pipe.close();
    }

    cout << "收到所有消息，客户端退出" << endl;

    return 0;
}
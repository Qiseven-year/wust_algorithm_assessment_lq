//共享内存客户端
#include <iostream>
#include <string>
#include <chrono>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <cstring>
using namespace std;

struct shared_Inerface
{
    char date[256];
    int id;
    int64_t timestamp;
};

int main()
{
    cout << "小装甲同学 - 共享内存服务器启动" << endl;

    //创建共享内存
    key_t key = ftok("shmfile", 65);
    int shmid = shmget(key, sizeof(shared_Inerface), 0666);

    if (shmid == -1)
    {
        cerr << "错误：无法获取共享内存，请先启动服务器！" << endl;
        return 1;
    }

    cout << "成功连接到共享内存，等待数据..." << endl;

    //附加共享内存
    shared_Inerface* shared_date = (shared_Inerface*) shmat(shmid, NULL, 0);

    if (shared_date == (shared_Inerface*) - 1)
    {
        cerr << "错误：无法附加共享内存" << endl;
        return 1;
    }

    cout << "成功连接到共享内存，等待数据..." << endl;

    
    int las_id = -1;
    int timeout_count = 0;
    //30s超时
    const int MAX_TIMEOUT = 6;

    while(timeout_count < MAX_TIMEOUT)
    {
        if(shared_date->id > las_id)
        {
            //计算时间差
            auto current_time =chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
            int64_t time_diff = current_time - shared_date->timestamp;
            cout << "发送" << shared_date->date
                 << "ID" << shared_date->id
                 << "时间戳" << shared_date->timestamp
                 << "延迟" << time_diff << "ms" << endl;
            las_id = shared_date->id;
            timeout_count++;
        }

        //等待100ms
        usleep(100000);
    }

    cout << "收到所有消息，客户端退出" << endl;
    //分离共享内存
    shmdt(shared_date);

    return 0;
} 
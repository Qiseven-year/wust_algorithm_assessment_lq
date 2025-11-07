//共享内存服务器端
#include <iostream>
#include <string>
#include <cstring>
#include <chrono>
#include <sys/ipc.h>
#include <sys/shm.h>
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
    //创建共享内存
    key_t key = ftok("shmfile", 65);
    int shmid = shmget(key, sizeof(shared_Inerface), 0666 | IPC_CREAT);
    if (shmid == -1)
    {
        cout << "错误，无法创建共享内存" << endl;
        return 1;
    }


    //附加共享内存
    shared_Inerface* shared_date = (shared_Inerface*) shmat(shmid, NULL, 0);

    if (shared_date == (shared_Inerface*) - 1)
    {
        cout << "错误，无法附加共享内存" << endl;
        return 1;
    }

    cout << "小符同学 - 共享内存服务器启动" << endl;
    
    int message_count = 0;
    while (message_count <= 5)
    {
        //准备数据
        string message = "来自小符的消息" + to_string(message_count);
        strncpy(shared_date->date, message.c_str(), sizeof(shared_date->date) - 1);
        shared_date->date[sizeof(shared_date->date) - 1] = '0';
        shared_date->id = message_count;
        shared_date->timestamp =chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
        cout << "发送" << shared_date->date
             << "ID" << shared_date->id
             << "时间戳" << shared_date->timestamp << endl;
        //等待4秒
        sleep(4);  
        message_count++;
    }

    cout << "服务器完成消息发送，等待10s后退出" << endl;
    //等待10s让客户端读取数据
    sleep(10);

    //分离共享内存
    shmdt(shared_date);
    //删除共享内存
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}

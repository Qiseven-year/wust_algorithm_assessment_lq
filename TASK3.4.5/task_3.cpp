#include <iostream>
#include <string>
#include <unordered_map>
#include <yaml-cpp/yaml.h>
#include <thread>
#include <chrono>
#include <mutex>
using namespace std;

class YamlManager
{
private:
    static YamlManager* instance;
    static mutex mtx;
    string configFile;
    YAML::Node config;
    unordered_map<string, YAML::Node> cache;
    YamlManager() = default;

public:
    //删除拷贝构造和赋值
    YamlManager(const YamlManager&) = delete;
    YamlManager& operator = (const YamlManager&) = delete;

    //获取单例实例
    static YamlManager& getInstance()
    {
        lock_guard<mutex> lock(mtx);
        if (!instance)
        {
            instance = new YamlManager();
        }
        return *instance;
    }

    //初始化配置文件路径
    void init(const string& filename)
    {
        configFile = filename;
        reload();
    }
    
    //动态加载配置(核心函数)
    template<typename T>
    T get(const string& key, const T& defaultValue = T())
    {
        lock_guard<mutex> lock(mtx);
        try
        {
            //重新加载配置文件获取最新值
            reload();
            
            //解析键路径(支持嵌套配置)
            vector<string> keys;
            string temp;
            for (char c : key)
            {
                if (c == '.')
                {
                    if (!temp.empty())
                    {
                        keys.push_back(temp);
                        temp.clear();
                    }
                }
                else
                {
                    temp += c;
                }
            }
            if (!temp.empty()) keys.push_back(temp);

            //遍历获取值
            YAML::Node node = config;
            for (const auto& k : keys)
            {
                if(node[k])
                {
                    node = node[k];
                }
                else
                {
                    cout << "Key is not found:" << key << ", using dedault:" << defaultValue << endl;
                    return defaultValue;
                }
            }
            return node.as<T>();
        }
        catch(const exception& e)
        {
            cout << "Error reading key" << key << ":" << e.what() << ", using default:" << defaultValue << endl;
            return defaultValue;
        }   
    }

private:
    void reload()
    {
        try
        {
            config = YAML::LoadFile(configFile);
        }
        catch(const exception& e)
        {
            //文件读取失败时使用缓存，程序不会崩溃
            cout << "Config file reload failed:" << e.what() << ", using cached config" << endl;
        }
    }
};

//静态成员初始化
YamlManager* YamlManager::instance = nullptr;
mutex YamlManager::mtx;

//全局访问宏（简化调用）
#define YAML_GET(key, defaultValue) YamlManager::getInstance().get(key, defaultValue)

//示例使用类
class ServiceA
{
public:
    void run()
    {
        while(true)
        {
            int timeout = YAML_GET("service.timeout", 30);
            string host = YAML_GET("service.host", string("localhost"));
            bool debug = YAML_GET("service.debug", false);

            cout << "[ServiceA] timeout =" << timeout << ", host = " << host << ", debug = " << debug << endl;
            this_thread::sleep_for(chrono::seconds(2));
        }
    }
};

class ServiceB
{
public:
    void run()
    {
        while (true)
        {
            double threshold = YAML_GET("alerts.threshold", 0.8);
            int interval = YAML_GET("alerts.interval", 60);
            cout << "[ServiceB] threshold = " << threshold << ", interval = " << interval << endl;
            this_thread::sleep_for(chrono::seconds(3));
        }
    }
};

int main()
{
    //初始化配置管理器
    YamlManager::getInstance().init("config.yaml");

    //创建服务实例
    ServiceA serviceA;
    ServiceB serviceB;

    cout << "Starting service with dynamic YAML configuration..." << endl;
    cout << "Modify config.yaml to see changes in real-time!" << endl;

    //在不同线程中运行服务（模拟独立类调用）
    thread threadA([&]() {serviceA.run(); });
    thread threadB([&]() {serviceB.run(); });

    //主线程也使用配置
    while (true)
    {
        string appName = YAML_GET("app.name", string("MyApp"));
        string version = YAML_GET("app.version", string("1.0.0"));

        cout << "[Main]" << appName << "v" << version << endl;
        this_thread::sleep_for(chrono::seconds(5));
    }

    threadA.join();
    threadB.join();


    return 0;
}
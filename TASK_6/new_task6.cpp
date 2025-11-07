#include "ThreadPool.h"
#include "fake_camera.hpp"
#include "fake_infer.hpp"
#include "signal.hpp"

#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <vector>

class Vision {
public:
    Vision() {
        pool_ = std::make_unique<ThreadPool>(std::thread::hardware_concurrency());
        camera_ =
            FakeCamera::createUnique(std::bind(&Vision::cameraCallback, this, std::placeholders::_1)
            );
        infer_ =
            FakeInfer::createUnique(std::bind(&Vision::inferCallback, this, std::placeholders::_1));

        //启动顺序处理线程
        process_thread_ = std::thread(&Vision::processResults, this);
    }

    ~Vision() {
        stop();
    }

    void stop() {
        std::cout << "Vision destroyed" << std::endl;
        run_flag_ = false;

        //通知处理线程退出
        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            cv_.notify_all();
        }

        if (process_thread_.joinable()) {
            process_thread_.join();
        }

        if (camera_) {
            camera_->stop();
            camera_.reset();
        }
        if (pool_) {
            pool_->waitUntilEmpty();
            pool_.reset();
        }
    }

    void start() {
        run_flag_ = true;
        camera_->start();
    }

    void cameraCallback(Image& image) {
        camera_count_.fetch_add(1, std::memory_order_relaxed);

        printStatus();

        if (pool_ && run_flag_) {
            //并行推理
            pool_->enqueue([this, frame = std::move(image)]() mutable {
                if (!run_flag_)
                    return;
                if (infer_)
                    infer_->infer(frame);
                });
        }
    }

    void inferCallback(const InferResult& result) {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        //将推理结果放入优先队列
        result_queue_.push(result);
        cv_.notify_one(); //通知处理线程有新数据
    }

    void processResults() {
        std::chrono::steady_clock::time_point last_processed_timestamp;
        bool is_first_result = true;

        while (run_flag_) {
            std::unique_lock<std::mutex> lock(queue_mutex_);

            //等待队列不为空
            cv_.wait_for(lock, std::chrono::milliseconds(100), [this]() {
                return !result_queue_.empty() || !run_flag_;
                });

            if (!run_flag_) break;
            if (result_queue_.empty()) continue;

            //获取当前队列中时间戳最小的结果
            const InferResult& next_result = result_queue_.top();

            //如果是第一个结果，或者时间戳符合顺序，就处理它
            if (is_first_result || next_result.timestamp > last_processed_timestamp) {
                InferResult result = next_result;
                result_queue_.pop();
                lock.unlock(); //解锁后再处理，避免长时间持有锁

                track(result);

                if (is_first_result) {
                    is_first_result = false;
                }
                last_processed_timestamp = result.timestamp;
            }
            else {
                //如果时间戳不符合顺序，等待更多结果
                lock.unlock();
                std::this_thread::yield();
            }
        }
    }

    void track(const InferResult& result) {
        infer_count_.fetch_add(1, std::memory_order_relaxed);
        static std::chrono::steady_clock::time_point last_timestamp =
            std::chrono::steady_clock::now();

        if (result.timestamp <= last_timestamp) {
            error_count_.fetch_add(1, std::memory_order_relaxed);
            return;
        }
        last_timestamp = result.timestamp;
        track_count_.fetch_add(1, std::memory_order_relaxed);
    }

    void printStatus() {
        using namespace std::chrono;
        auto now = steady_clock::now();
        if (duration_cast<seconds>(now - last_print_time_) >= seconds(1)) {
            int cam = camera_count_.exchange(0);
            int inf = infer_count_.exchange(0);
            int err = error_count_.exchange(0);
            int track = track_count_.exchange(0);

            std::cout << "[Status] cameraCallback/s: " << cam << " | inferCallback/s: " << inf
                << " | track/s: " << track << " | Error/s: " << err << std::endl;
            last_print_time_ = now;
        }
    }

private:
    std::unique_ptr<FakeInfer> infer_;
    std::unique_ptr<FakeCamera> camera_;
    std::unique_ptr<ThreadPool> pool_;

    //用于顺序处理的优先队列
    struct CompareTimestamp {
        bool operator()(const InferResult& a, const InferResult& b) {
            return a.timestamp > b.timestamp; //最小堆，时间戳小的在顶部
        }
    };
    std::priority_queue<InferResult, std::vector<InferResult>, CompareTimestamp> result_queue_;
    std::mutex queue_mutex_;
    std::condition_variable cv_;
    std::thread process_thread_;

    std::atomic<int> camera_count_{ 0 };
    std::atomic<int> infer_count_{ 0 };
    std::atomic<int> error_count_{ 0 };
    std::atomic<int> track_count_{ 0 };
    std::chrono::steady_clock::time_point last_print_time_{ std::chrono::steady_clock::now() };
    bool run_flag_ = false;
};

int main() {
    try {
        Vision v;
        v.start();

        SignalHandler sig;
        sig.start([&] { v.stop(); });

        while (!sig.shouldExit()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
        std::cout << "Exiting program..." << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << "Caught exception in main: " << e.what() << "\n";
        return -1;
    }
    catch (...) {
        std::cerr << "Unknown exception caught in main!\n";
        return -1;
    }
    return 0;

}

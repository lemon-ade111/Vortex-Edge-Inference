#include <opencv2/opencv.hpp>
#include <onnxruntime_cxx_api.h>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <queue>
#include <condition_variable>

std::mutex mtx;
std::condition_variable cv_producer, cv_consumer;
std::queue<cv::Mat> frame_queue;
const int MAX_QUEUE_SIZE = 5;
bool finished = false;

Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "AIEngine");
Ort::SessionOptions session_options;

void setup_session() {
    session_options.SetIntraOpNumThreads(4);
    session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);
}

void producer(const std::string& path) {
    cv::Mat img = cv::imread(path);
    for (int i = 0; i < 20; ++i) {
        {
            std::unique_lock<std::mutex> lock(mtx);
            // 如果队列满了，Producer 就在这儿睡觉，等 Consumer 喊它
            cv_producer.wait(lock, [] { return frame_queue.size() < MAX_QUEUE_SIZE; });

            frame_queue.push(img.clone());
            std::cout << "[Producer] 采集进度: " << i << " (队列当前长度: " << frame_queue.size() << ")" << std::endl;

            cv_consumer.notify_one(); // 唤醒一个正在睡觉的 Consumer
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); // 模拟高速快门
    }
    {
        std::lock_guard<std::mutex> lock(mtx);
        finished = true;
    }
    cv_consumer.notify_all(); // 全部干完，唤醒所有 Consumer 准备下班
}

void consumer(int id, const std::string& model_path) {
    Ort::Session session(env, model_path.c_str(), session_options);
    Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);

    while (true) {
        cv::Mat frame;
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv_consumer.wait(lock, [] { return !frame_queue.empty() || finished; });
            if (frame_queue.empty() && finished) break;
            frame = std::move(frame_queue.front());
            frame_queue.pop();
        }
        cv_producer.notify_one();

        cv::Mat blob;
        cv::resize(frame, blob, cv::Size(224, 224));
        blob.convertTo(blob, CV_32F, 1.0 / 255.0);

        std::cout << "[Consumer " << id << "] AI 正在识别该帧..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

int main() {
    setup_session();
    std::thread p(producer, "../test.jpg");
    std::vector<std::thread> consumers;
    std::string model_path = "../model.onnx";
    for (int i = 0; i < 3; ++i) {
        consumers.emplace_back(consumer, i, model_path);
    }

    p.join();
    for (auto& t : consumers) t.join();
    std::cout << "所有 AI 任务处理完毕，系统安全退出。" << std::endl;
    return 0;
}
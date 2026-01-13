#include <vector>
#include <thread>
#include <chrono>
#include <random>
#include <functional>
#include "../src/skiplist.h"


// 匿名命名空间, 将常量限制在当前文件作用域内
namespace {
    constexpr int NUM_THREADS = 4;
    constexpr int TEST_COUNT = 100000;
    skip_list::SkipList<int, std::string> test_skip_list(18);   // 最大高度18
}


// 生成随机数（线程私有）
int getRandomNumber(std::mt19937& gen) {
    static thread_local std::uniform_int_distribution<int> dist(0, TEST_COUNT - 1);
    return dist(gen);
}


// 插入测试
void testInsertNode(int tid) {
    // 每个线程一个随机数引擎（关键点）
    std::mt19937 gen(
        static_cast<unsigned>(
            std::chrono::high_resolution_clock::now()
                .time_since_epoch().count() + tid
        )
    );

    int count_each_thread = TEST_COUNT / NUM_THREADS;
    for (int i = 0; i < count_each_thread; ++i) {
        test_skip_list.insertNode(getRandomNumber(gen), "rain");
    }
}


// 查找测试
void testGetNode(int tid) {
    std::mt19937 gen(
        static_cast<unsigned>(
            std::chrono::high_resolution_clock::now()
                .time_since_epoch().count() + tid + 1000
        )
    );

    int count_each_thread = TEST_COUNT / NUM_THREADS;
    for (int i = 0; i < count_each_thread; ++i) {
        test_skip_list.searchNode(getRandomNumber(gen));
    }
}


int main() {
    std::vector<std::thread> threads;
    threads.reserve(NUM_THREADS);

    // ================= 插入测试 =================
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back(testInsertNode, i);
    }

    for (auto& t : threads) {
        t.join();
    }

    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;

    std::cout << "Insert elapsed: " << elapsed.count() << " s\n";
    std::cout << "SkipList size: " << test_skip_list.size() << "\n";

    threads.clear();

    // ================= 查找测试 =================
    start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back(testGetNode, i);
    }

    for (auto& t : threads) {
        t.join();
    }

    finish = std::chrono::high_resolution_clock::now();
    elapsed = finish - start;

    std::cout << "Search elapsed: " << elapsed.count() << " s\n";

    return 0;
}

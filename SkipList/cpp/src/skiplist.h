#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <random>
#include <fstream>
#include <string>


namespace kv_node {
    // 跳表中的节点类
    template<typename K, typename V>
    class Node {
    public:
        K key;
        V value;
        int height; // 节点高度
        // height 表示这个节点"有多高"
        // 它会出现在 Level 0 到 Level (height - 1) 的所有层中

        /*
         * forward[i] 表示:
         * 在跳表的第 i 层级中, 当前节点指向的下一个节点指针。 
         * 约定:
         * 1. i = 0 是最底层(完整有序链表层)
         * 2. 当前节点存在于 Level 0 ~ Level (height - 1)
         * 3. forward.size() == height
         * 4. 若 forward[i] == nullptr，表示该层已经到达表尾
         * 举例:
         * forward[3]表示该节点在Level 3上时指向的下一个节点
        */
        std::vector<std::shared_ptr<Node<K, V>>> forward;   // 把它理解为next指针数组


        Node(const K& k, const V& v, int h)
            : key(k), value(v), height(h), forward(h, nullptr) {}

        const K& getKey() const { return key; }
        const V& getValue() const { return value; }
        void setValue(const V& val) { value = val; }
    };

    template<typename K, typename V>
    using NodeVec = std::vector<std::shared_ptr<Node<K, V>>>;

} // namespace kv_node


namespace skip_list {
    const std::string delimiter = ":";
    const std::string STORE_FILE = "store/dumpFile.txt";

    // 实现跳表类
    template<typename K, typename V>
    class SkipList {
    private:
        int max_height;                             // 跳表的最大存储高度
        int current_height;                         // 跳表当前已存储的高度, 最小为1
        std::shared_ptr<kv_node::Node<K,V>> head;   // 跳表虚拟头节点
        int node_count;                             // 跳表中节点数量
        mutable std::shared_mutex rw_mutex;         // 读写锁


    private:
        bool isValidString(const std::string& str) const {
            return !str.empty() && str.find(delimiter) != std::string::npos;
        }

        
        void getKeyValueFromString(const std::string& str, std::string& key, std::string& value) const {
            if (!isValidString(str)) return;

            key = str.substr(0, str.find(delimiter));
            value = str.substr(str.find(delimiter) + 1);
        }


    public:
        SkipList(int max_h)
            : max_height(max_h), current_height(1), node_count(0),
              head(std::make_shared<kv_node::Node<K,V>>(K(), V(), max_h)) {}


        // 返回跳表节点数量
        int size() const {
            std::shared_lock<std::shared_mutex> lock(rw_mutex);
            return node_count;
        }


        // 多线程安全随机高度生成
        int getRandomHeight() {
            thread_local std::mt19937 rng(std::random_device{}());
            std::bernoulli_distribution coin(0.5);
            int h = 1;
            while (coin(rng) && h < max_height) ++h;
            return h;
        }


        // 查找节点方法, 多线程安全, 可并发查找
        bool searchNode(const K& key) const {
            std::shared_lock<std::shared_mutex> lock(rw_mutex); // 共享锁允许多线程读
            auto current = head;

            // 从最高层级开始查找
            for (int level = current_height - 1; level >= 0; --level) {
                // 把forward[level]理解成next指针 
                // node->forward[level]->getKey() < key, 不断逼近key
                while (current->forward[level] && current->forward[level]->getKey() < key) {
                    current = current->forward[level];
                }
            }

            bool result = false;

            // 第 0 层有所有节点
            current = current->forward[0];
            if (current && current->getKey() == key) {
                std::cout << "Find key: " << key << ", value: " << current->getValue() << "\n";
                result = true;
            }
            else {
                std::cout << "No key: " << key << " in skip list\n";
            }

            return result;
        }


        // 插入节点方法, 返回0表示插入成功, 返回1表示跳表中已有该节点
        int insertNode(const K& key, const V& value) {
            std::unique_lock<std::shared_mutex> lock(rw_mutex);     // 独占锁保证写安全

            // 插入节点后, 需要更新forward数组的节点存放在在数组update里, 下标对应跳表中的索引
            kv_node::NodeVec<K,V> update(max_height);
            auto node = head;       // 遍历节点

            // 从当前最高层级开始存储所有需要更新forward数组的节点
            for (int i = current_height - 1; i >= 0; --i) {
                while (node->forward[i] && node->forward[i]->getKey() < key) {
                    node = node->forward[i];
                }
                update[i] = node;
            }

            // 第0层的节点
            node = node->forward[0];
            if (node && node->getKey() == key) {
                std::cout << "key exists\n";
                return 1; // 已存在
            }

            int random_h = getRandomHeight();
            if (random_h > current_height) {
                for (int i = current_height; i < random_h; ++i) {
                    update[i] = head;
                }
                current_height = random_h;
            }

            auto new_node = std::make_shared<kv_node::Node<K,V>>(key, value, random_h);
            // 从下到上更新update数组中的节点
            for (int i = 0; i < random_h; ++i) {
                new_node->forward[i] = update[i]->forward[i];
                update[i]->forward[i] = new_node;
            }
            ++node_count;
            
            std::cout << "Insert key: " << key << "\n";
            // 插入节点成功
            return 0;
        }


        // 删除节点方法
        void deleteNode(const K& key) {
            std::unique_lock<std::shared_mutex> lock(rw_mutex);      // 独占锁保证写安全

            // 删除节点后, 需要更新forward数组的节点存放在在数组update里, 下标对应跳表中的索引
            kv_node::NodeVec<K, V> update(max_height);
            auto node = head;

            // 从当前最高层级开始存储所有需要更新forward数组的节点
            for (int i = current_height - 1; i >= 0; --i) {
                while (node->forward[i] && node->forward[i]->getKey() < key) {
                    node = node->forward[i];
                }
                update[i] = node;
            }

            // 第0层的节点
            node = node->forward[0];
            // 跳表中存在节点key
            if (node && node->getKey() == key) {
                // 从下到上更新update数组中的节点
                for (int i = current_height - 1; i >= 0; --i) {
                    // 第 i 层没有待删除结点, 直接退出
                    if (update[i]->forward[i] != node) break;
                    update[i]->forward[i] = node->forward[i];
                }
                
                // 删除节点后可能会减小跳表高度, 需要在这里更新跳表, 删掉没有节点的层
                while (current_height > 1 && head->forward[current_height - 1] == nullptr) {
                    --current_height;
                }

                --node_count;

                std::cout << "Successfully delete key: " << key << std::endl;
            }
        }


        // 从最高层开始展示跳表
        void displaySkipList() const {
            std::shared_lock<std::shared_mutex> lock(rw_mutex);

            std::cout << "\n========================= Skip List =========================\n";
            for (int i = current_height - 1; i >= 0; --i) {
                auto node = head->forward[i];
                std::cout << "Level " << i << ": ";
                while (node) {
                    std::cout << node->getKey() << ":" << node->getValue();
                    node = node->forward[i];
                    if (node) std::cout << " -> ";
                }
                std::cout << "\n";
            }
        }


        void loadFile() {
            std::ifstream file_reader(STORE_FILE);
            if (!file_reader.is_open()) return;

            std::cout << "\n==================== load file ====================" << std::endl;
            
            std::string line;
            // 读取一行
            while (getline(file_reader, line)) {
                std::string key, value;
                getKeyValueFromString(line, key, value);
                if (key.empty() || value.empty()) continue;
                std::cout << key << delimiter << value << std::endl; 
                insertNode(stoi(key), value);
            }

            file_reader.close();
        }


        // 加独占锁的写文件方法
        void dumpFile() {
            std::unique_lock<std::shared_mutex> lock(rw_mutex);
            std::ofstream file_writer(STORE_FILE, std::ios::out | std::ios::binary);
            if (!file_writer.is_open()) {
                std::cerr << "Failed to open file for writing: " << STORE_FILE << std::endl;
                return;
            }
            
            std::cout << "\n==================== dump fil e====================" << std::endl;
            
            //只遍历跳表的第0层级
            auto node = this->head->forward[0];
            while (node) {
                file_writer << node->getKey() << delimiter << node->getValue() << std::endl;
                std::cout << node->getKey() << delimiter << node->getValue() << std::endl;
                node = node->forward[0];
            }
            file_writer.flush();
            file_writer.close();
        }
    };
} // namespace skip_list

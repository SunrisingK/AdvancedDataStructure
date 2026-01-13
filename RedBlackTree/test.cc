#include "RedBlackTree.h"

// 测试示例
int main(int argc, char const* argv[]) {
    RedBlackTree<int> rbt;

    // 插入测试
    int values[] = {10, 20, 30, 15, 25, 5, 35};
    for (int v : values) {
        rbt.insert(v);
        std::cout << "插入 " << v << " 后: ";
        rbt.print();
    }

    // 查找测试
    int key = 20;
    auto node = rbt.search(key);
    if (node != nullptr) {
        std::cout << "查找 " << key << "：存在(" << (node->color == RED ? "红" : "黑") << ")" << std::endl;
    }

    // 删除测试
    rbt.remove(20);
    std::cout << "删除 20 后: ";
    rbt.print();

    rbt.remove(10);
    std::cout << "删除 10 后: ";
    rbt.print();

    return 0;
}
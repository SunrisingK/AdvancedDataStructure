# SkipList Project

一个基于 C++23 的 **跳表实现**，支持多线程读写锁保护操作，并带有压测示例和文件存储功能。  

该项目在 Windows + VSCode + g++ 环境下开发，支持中文内容存储和显示。

---

## 项目目录
├── src/
│ ├── main.cc # 主程序示例
│ └── skiplist.h # 跳表实现头文件
├── test/
│ └── stress_test.cc # 跳表压测程序
└── store/
└── dumpFile.txt # 跳表数据文件（读写）

---

## 功能特性

- 支持插入、查找、删除操作
- 支持多线程安全操作（`std::shared_mutex` 读写锁）
- 支持随机高度生成节点
- 支持跳表存储到文件和从文件加载
- 支持中文内容（UTF-8 编码）

---

## 编译和运行（VSCode + g++）

### 1. 配置 VSCode Tasks 和 Launch
- **tasks.json**：  
  使用 `-std=c++23` 编译，指定 `-fexec-charset=UTF-8` 支持中文字符串。

- **launch.json**：  
  设置环境变量 `CHCP=65001` 保证 UTF-8 输出。

### 2. 一键编译和运行

在 VSCode 中， 按 F5 运行调试对应代码。

### 3. 文件存储

程序会将跳表数据保存到 `store/dumpFile.txt`：

```text
1:是徒为静养
3:而不用克己工夫也
...
```

可通过 `loadFile()` 方法加载数据。

---

## 示例代码

```cpp
#include "./skiplist.h"
using namespace skip_list;

int main() {
    SkipList<int, std::string> skipList(10);

    skipList.insertNode(1, "是徒为静养");
    skipList.insertNode(3, "而不用克己工夫也");
    skipList.insertNode(7, "如此");

    skipList.displaySkipList();

    skipList.dumpFile();   // 保存到 store/dumpFile.txt

    return 0;
}
```

运行后终端输出（中文正常显示）：

```
Insert key: 1
Insert key: 3
Insert key: 7

========================= Skip List =========================
Level 0: 1:是徒为静养 -> 3:而不用克己工夫也 -> 7:如此
```

---

## 注意事项

* 确保 VSCode 集成终端使用  **UTF-8** （`CHCP 65001`）
* 编译时使用 `-fexec-charset=UTF-8` 支持中文
* Windows 下中文显示问题主要与终端编码相关

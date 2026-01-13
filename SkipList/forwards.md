# 跳表的多层前向指针数组详解

## 基本概念

**多层前向指针数组**是跳表的核心结构，每个节点包含一个指针数组，数组的每个元素指向该节点在**不同层级**的下一个节点。

```java
class Node<K, V> {
    K key;
    V value;
    List<Node<K, V>> forwards;  // 多层前向指针数组
    int level;                  // 节点实际拥有的层数
}
```

## 数组结构详解

### 1. 数组索引与层级关系

```
forwards[0] → 第0层（最底层，完整链表）的下一个节点
forwards[1] → 第1层（第一级索引）的下一个节点  
forwards[2] → 第2层（第二级索引）的下一个节点
...
forwards[level] → 该节点最高层的下一个节点
```

### 2. 实际示例

考虑以下跳表：

```
Level 3: head → ───────────────────────→ 50 → tail
Level 2: head → ──────────→ 30 → ───────→ 50 → tail  
Level 1: head → ──→ 10 → ──→ 30 → ──→ 40 → 50 → tail
Level 0: head → 5 → 10 → 20 → 30 → 40 → 50 → 60 → tail
```

**节点30的forwards数组**：

```java
节点30:
- level = 2  (节点出现在3层：0,1,2)
- forwards[0] = 节点40  // 第0层指向40
- forwards[1] = 节点40  // 第1层指向40  
- forwards[2] = 节点50  // 第2层指向50
- forwards[3] = null    // 第3层不存在
```

**节点10的forwards数组**：

```java
节点10:
- level = 1  (节点出现在2层：0,1)
- forwards[0] = 节点20  // 第0层指向20
- forwards[1] = 节点30  // 第1层指向30
- forwards[2] = null    // 第2层不存在
- forwards[3] = null    // 第3层不存在
```

## 数组在操作中的作用

### 1. 查找操作中的使用

```java
public V search(K key) {
    Node<K, V> current = header;
  
    for (int i = skipListLevel; i >= 0; i--) {
        // 关键：使用forwards[i]在当前层向右移动
        while (current.forwards.get(i) != null && 
               current.forwards.get(i).key.compareTo(key) < 0) {
            current = current.forwards.get(i);  // 跳到当前层的下一个节点
        }
    }
  
    current = current.forwards.get(0);  // 最终在第0层检查
    // ... 检查是否找到
}
```

**查找过程可视化**（查找40）：

```
Level 3: current=header → forwards[3]=50 (50>40, 下降)
         ↓
Level 2: current=header → forwards[2]=30 (30<40, 移动到30)
         current=30 → forwards[2]=50 (50>40, 下降)
         ↓  
Level 1: current=30 → forwards[1]=40 (40=40? 不，40>=40，下降)
         ↓
Level 0: current=30 → forwards[0]=40 (40=40, 找到!)
```

### 2. 插入操作中的指针更新

```java
// 创建新节点（假设层数=2）
Node<K, V> newNode = new Node(key, value, 2);

// 更新各层指针
for (int i = 0; i <= 2; i++) {
    // 新节点指向原后继
    newNode.forwards.set(i, updateList.get(i).forwards.get(i));
    // 前驱节点指向新节点
    updateList.get(i).forwards.set(i, newNode);
}
```

**插入35的指针变化**：

```
插入前：
Level 2: 30 → 50
Level 1: 30 → 40 → 50  
Level 0: 30 → 40 → 50

插入35（层数=2）：
Level 2: 30.forwards[2] = 35, 35.forwards[2] = 50
Level 1: 30.forwards[1] = 35, 35.forwards[1] = 40  
Level 0: 30.forwards[0] = 35, 35.forwards[0] = 40
```

### 3. 内存布局示意

```
节点30内存布局：
+---------+-----+-----+-----+-----+
| key:30  | val | f[0]| f[1]| f[2]|
+---------+-----+-----+-----+-----+
              |     |     |
              v     v     v
            节点40 节点40 节点50

节点10内存布局：  
+---------+-----+-----+-----+-----+
| key:10  | val | f[0]| f[1]| null|
+---------+-----+-----+-----+-----+
              |     |
              v     v
            节点20 节点30
```

## 数组大小的确定

### 1. 随机层数生成

```java
private int generateNodeLevel() {
    int level = 0;
    // 通常使用概率p=1/2
    while (Math.random() < 0.5 && level < MAX_LEVEL) {
        level++;
    }
    return level;
}
```

### 2. 层数分布概率

- 层数0: 50% 的节点
- 层数1: 25% 的节点
- 层数2: 12.5% 的节点
- 层数3: 6.25% 的节点
- ...

## 性能优势

### 1. 搜索加速原理

```
没有索引：5 → 10 → 20 → 30 → 40 → 50 → 60 （需要6步）

有索引：
Level 1: 5 → 20 → 40 → 60
Level 0: 5 → 10 → 20 → 30 → 40 → 50 → 60

查找50：L1:5→20→40→60(60>50)→L0:40→50（找到！只需4步）
```

### 2. 空间-时间权衡

- **空间开销**：每个节点平均需要 1/(1-p) 个指针（p=1/2时约2个指针）
- **时间收益**：搜索复杂度从 O(n) 降到 O(log n)

## 实际实现考虑

### 1. 数组预分配

```java
class Node<K, V> {
    private final List<Node<K, V>> forwards;
  
    public Node(K key, V value, int level) {
        this.key = key;
        this.value = value;
        this.forwards = new ArrayList<>(level + 1);
        // 预分配并初始化为null
        for (int i = 0; i <= level; i++) {
            forwards.add(null);
        }
    }
}
```

### 2. 空指针处理

高层指针可能为null，表示该层没有后续节点：

```java
while (current.forwards.get(i) != null &&  // 检查指针非空
       current.forwards.get(i).key.compareTo(key) < 0) {
    current = current.forwards.get(i);
}
```

**总结**：多层前向指针数组是跳表的"骨架"，通过在不同层级建立索引，实现了从最高层开始的快速"跳跃式"搜索，是跳表高效性能的关键所在。

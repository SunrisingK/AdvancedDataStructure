# 跳表（Skip List）数据结构详解

## 什么是跳表？

跳表是一种**概率性的有序数据结构**，通过建立多级索引来实现快速查找，可以看作是**带有多级索引的链表**。

### 基本结构

```
Level 3: head → ───────────────────────→ 50 → tail
Level 2: head → ──────────→ 30 → ───────→ 50 → tail  
Level 1: head → ──→ 10 → ──→ 30 → ──→ 40 → 50 → tail
Level 0: head → 5 → 10 → 20 → 30 → 40 → 50 → 60 → tail
```

## 核心特性

### 1. 跳表节点结构

```java
class Node<K, V> {
    K key;
    V value;
    List<Node<K, V>> forwards;  // 多层前向指针数组
    int level;  // 节点实际层数
}
```

### 2. 跳表关键参数

- **最大层数 (MAX_LEVEL)**：限制索引的最大高度
- **当前层数 (skipListLevel)**：跳表实际使用的最高层数
- **头节点 (header)**：各层的起始点

## 操作过程详解

### 1. 查找操作

**过程**：

```java
public V search(K key) {
    Node<K, V> current = header;
  
    // 从最高层开始向下搜索
    for (int i = skipListLevel; i >= 0; i--) {
        // 在当前层向右移动，直到下一个节点大于等于目标key
        while (current.forwards.get(i) != null && 
               current.forwards.get(i).key.compareTo(key) < 0) {
            current = current.forwards.get(i);
        }
    }
  
    // 到达第0层，检查下一个节点
    current = current.forwards.get(0);
    if (current != null && current.key.equals(key)) {
        return current.value;
    }
    return null;
}
```

**示例**：查找 40

```
Level 3: head → 50 (50>40, 下降)
         ↓
Level 2: head → 30 → 50 (30<40, 移动到30; 50>40, 下降)  
         ↓
Level 1: head → 30 → 40 (30<40, 移动到30; 40=40, 找到!)
```

**时间复杂度**：平均 O(log n)，最坏 O(n)

### 2. 插入操作

**详细步骤**：

#### 步骤1：查找插入位置并记录路径

```java
List<Node<K, V>> updateList = new ArrayList<>(MAX_LEVEL + 1);
Node<K, V> current = header;

for (int i = skipListLevel; i >= 0; i--) {
    while (current.forwards.get(i) != null && 
           current.forwards.get(i).key.compareTo(key) < 0) {
        current = current.forwards.get(i);
    }
    updateList.set(i, current);  // 记录每层的前驱节点
}
```

#### 步骤2：生成随机层数

```java
int randomLevel = generateNodeLevel();
// 通常使用概率算法：每层有 1/2 概率继续向上
```

#### 步骤3：更新跳表层数（如果需要）

```java
if (randomLevel > skipListLevel) {
    for (int i = skipListLevel + 1; i <= randomLevel; i++) {
        updateList.set(i, header);  // 新层的前驱都是header
    }
    skipListLevel = randomLevel;
}
```

#### 步骤4：创建新节点并更新指针

```java
Node<K, V> newNode = new Node(key, value, randomLevel);

for (int i = 0; i <= randomLevel; i++) {
    // 新节点指向原后继
    newNode.forwards.set(i, updateList.get(i).forwards.get(i));
    // 前驱节点指向新节点
    updateList.get(i).forwards.set(i, newNode);
}
```

**示例**：插入 35

```
查找路径记录：
Level 2: updateList[2] = 30
Level 1: updateList[1] = 30  
Level 0: updateList[0] = 30

假设随机层数=2，插入后：
Level 2: 30 → 35 → 50
Level 1: 30 → 35 → 40 → 50
Level 0: 30 → 35 → 40 → 50
```

### 3. 删除操作

**过程**：

```java
public boolean delete(K key) {
    List<Node<K, V>> updateList = new ArrayList<>(MAX_LEVEL + 1);
    Node<K, V> current = header;
  
    // 查找要删除的节点并记录路径
    for (int i = skipListLevel; i >= 0; i--) {
        while (current.forwards.get(i) != null && 
               current.forwards.get(i).key.compareTo(key) < 0) {
            current = current.forwards.get(i);
        }
        updateList.set(i, current);
    }
  
    current = current.forwards.get(0);
    if (current != null && current.key.equals(key)) {
        // 更新各层指针
        for (int i = 0; i <= current.level; i++) {
            updateList.get(i).forwards.set(i, current.forwards.get(i));
        }
    
        // 更新跳表层数
        while (skipListLevel > 0 && header.forwards.get(skipListLevel) == null) {
            skipListLevel--;
        }
        return true;
    }
    return false;
}
```

### 4. 更新操作

更新 = 查找 + 修改值（如果key存在）

## 时间复杂度分析

| 操作 | 平均时间复杂度 | 最坏时间复杂度 |
| ---- | -------------- | -------------- |
| 查找 | O(log n)       | O(n)           |
| 插入 | O(log n)       | O(n)           |
| 删除 | O(log n)       | O(n)           |
| 更新 | O(log n)       | O(n)           |

## 优势与劣势

### 优势

1. **实现相对简单**：比平衡树更容易实现
2. **支持范围查询**：可以高效进行区间查找
3. **并发友好**：可以使用细粒度锁
4. **内存使用灵活**：相比B+树更灵活

### 劣势

1. **空间开销**：需要存储多级指针
2. **性能不稳定**：由于随机性，性能有波动
3. **缓存不友好**：节点在内存中不连续

## 实际应用

- **Redis有序集合**：使用跳表实现
- **LevelDB/RocksDB**：内存中的MemTable
- **Lucene**：倒排索引的跳表实现

跳表通过"空间换时间"的策略，以相对简单的实现获得了接近平衡树的性能，是工程实践中常用的高效数据结构。

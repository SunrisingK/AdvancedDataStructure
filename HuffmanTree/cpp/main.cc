#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <unordered_map>
using namespace std;

// Huffman树节点
struct HuffmanNode {
    char ch;           // 字符
    int freq;          // 频率
    HuffmanNode* left; // 左孩子
    HuffmanNode* right;// 右孩子
    
    HuffmanNode(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
    HuffmanNode(int f) : ch('\0'), freq(f), left(nullptr), right(nullptr) {}
};

// 用于优先队列的比较函数
struct Compare {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->freq > b->freq; // 最小堆
    }
};

class HuffmanTree {
private:
    HuffmanNode* root;
    unordered_map<char, string> huffmanCodes;
    
    // 构建Huffman编码
    void buildCodes(HuffmanNode* node, string code) {
        if (!node) return;
        
        // 叶子节点，存储编码
        if (!node->left && !node->right) {
            huffmanCodes[node->ch] = code;
            return;
        }
        
        buildCodes(node->left, code + "0");
        buildCodes(node->right, code + "1");
    }
    
    // 删除树
    void deleteTree(HuffmanNode* node) {
        if (!node) return;
        deleteTree(node->left);
        deleteTree(node->right);
        delete node;
    }

public:
    HuffmanTree() : root(nullptr) {}
    
    ~HuffmanTree() {
        deleteTree(root);
    }
    
    // 构建Huffman树
    void buildTree(const unordered_map<char, int>& freqMap) {
        // 最小堆优先队列
        priority_queue<HuffmanNode*, vector<HuffmanNode*>, Compare> pq;
        
        // 为每个字符创建节点并加入优先队列
        for (const auto& pair : freqMap) {
            pq.push(new HuffmanNode(pair.first, pair.second));
        }
        
        // 构建Huffman树
        while (pq.size() > 1) {
            // 取出频率最小的两个节点
            HuffmanNode* left = pq.top(); pq.pop();
            HuffmanNode* right = pq.top(); pq.pop();
            
            // 创建新节点，频率为两个子节点频率之和
            HuffmanNode* parent = new HuffmanNode(left->freq + right->freq);
            parent->left = left;
            parent->right = right;
            
            pq.push(parent);
        }
        
        root = pq.top();
        pq.pop();
        
        // 构建Huffman编码
        buildCodes(root, "");
    }
    
    // 获取Huffman编码
    unordered_map<char, string> getCodes() const {
        return huffmanCodes;
    }
    
    // 打印Huffman编码
    void printCodes() const {
        cout << "Huffman Codes:" << endl;
        for (const auto& pair : huffmanCodes) {
            cout << pair.first << ": " << pair.second << endl;
        }
    }
    
    // 编码字符串
    string encode(const string& text) {
        string encoded = "";
        for (char ch : text) {
            encoded += huffmanCodes.at(ch);
        }
        return encoded;
    }
    
    // 解码
    string decode(const string& encoded) {
        string decoded = "";
        HuffmanNode* current = root;
        
        for (char bit : encoded) {
            if (bit == '0') {
                current = current->left;
            } else {
                current = current->right;
            }
            
            // 到达叶子节点
            if (!current->left && !current->right) {
                decoded += current->ch;
                current = root;
            }
        }
        
        return decoded;
    }
};

// 计算字符频率
unordered_map<char, int> calculateFrequency(const string& text) {
    unordered_map<char, int> freqMap;
    for (char ch : text) {
        freqMap[ch]++;
    }
    return freqMap;
}

int main() {
    string text = "hello world";
    
    // 计算字符频率
    auto freqMap = calculateFrequency(text);
    
    // 构建Huffman树
    HuffmanTree huffmanTree;
    huffmanTree.buildTree(freqMap);
    
    // 打印编码
    huffmanTree.printCodes();
    
    // 编码
    string encoded = huffmanTree.encode(text);
    cout << "\nEncoded: " << encoded << endl;
    
    // 解码
    string decoded = huffmanTree.decode(encoded);
    cout << "Decoded: " << decoded << endl;
    
    return 0;
}
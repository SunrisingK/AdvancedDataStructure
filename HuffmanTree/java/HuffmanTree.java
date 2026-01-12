import java.util.*;

// Huffman树节点
class HuffmanNode implements Comparable<HuffmanNode> {
    char ch;
    int freq;
    HuffmanNode left;
    HuffmanNode right;
    
    public HuffmanNode(char ch, int freq) {
        this.ch = ch;
        this.freq = freq;
        this.left = null;
        this.right = null;
    }
    
    public HuffmanNode(int freq) {
        this.ch = '\0';
        this.freq = freq;
        this.left = null;
        this.right = null;
    }
    
    @Override
    public int compareTo(HuffmanNode other) {
        return this.freq - other.freq;
    }
    
    public boolean isLeaf() {
        return left == null && right == null;
    }
}

public class HuffmanTree {
    private HuffmanNode root;
    private Map<Character, String> huffmanCodes;
    
    public HuffmanTree() {
        this.root = null;
        this.huffmanCodes = new HashMap<>();
    }
    
    // 构建Huffman树
    public void buildTree(Map<Character, Integer> freqMap) {
        // 使用优先队列（最小堆）
        PriorityQueue<HuffmanNode> pq = new PriorityQueue<>();
        
        // 为每个字符创建节点并加入优先队列
        for (Map.Entry<Character, Integer> entry : freqMap.entrySet()) {
            pq.offer(new HuffmanNode(entry.getKey(), entry.getValue()));
        }
        
        // 构建Huffman树
        while (pq.size() > 1) {
            // 取出频率最小的两个节点
            HuffmanNode left = pq.poll();
            HuffmanNode right = pq.poll();
            
            // 创建新节点，频率为两个子节点频率之和
            HuffmanNode parent = new HuffmanNode(left.freq + right.freq);
            parent.left = left;
            parent.right = right;
            
            pq.offer(parent);
        }
        
        root = pq.poll();
        
        // 构建Huffman编码
        buildCodes(root, "");
    }
    
    // 递归构建Huffman编码
    private void buildCodes(HuffmanNode node, String code) {
        if (node == null) return;
        
        // 叶子节点，存储编码
        if (node.isLeaf()) {
            huffmanCodes.put(node.ch, code);
            return;
        }
        
        buildCodes(node.left, code + "0");
        buildCodes(node.right, code + "1");
    }
    
    // 获取Huffman编码
    public Map<Character, String> getCodes() {
        return huffmanCodes;
    }
    
    // 打印Huffman编码
    public void printCodes() {
        System.out.println("Huffman Codes:");
        for (Map.Entry<Character, String> entry : huffmanCodes.entrySet()) {
            System.out.println(entry.getKey() + ": " + entry.getValue());
        }
    }
    
    // 编码字符串
    public String encode(String text) {
        StringBuilder encoded = new StringBuilder();
        for (char ch : text.toCharArray()) {
            encoded.append(huffmanCodes.get(ch));
        }
        return encoded.toString();
    }
    
    // 解码
    public String decode(String encoded) {
        StringBuilder decoded = new StringBuilder();
        HuffmanNode current = root;
        
        for (char bit : encoded.toCharArray()) {
            if (bit == '0') {
                current = current.left;
            } else {
                current = current.right;
            }
            
            // 到达叶子节点
            if (current.isLeaf()) {
                decoded.append(current.ch);
                current = root;
            }
        }
        
        return decoded.toString();
    }
    
    // 计算字符频率
    public static Map<Character, Integer> calculateFrequency(String text) {
        Map<Character, Integer> freqMap = new HashMap<>();
        for (char ch : text.toCharArray()) {
            freqMap.put(ch, freqMap.getOrDefault(ch, 0) + 1);
        }
        return freqMap;
    }
    
    public static void main(String[] args) {
        String text = "hello world";
        
        // 计算字符频率
        Map<Character, Integer> freqMap = calculateFrequency(text);
        
        // 构建Huffman树
        HuffmanTree huffmanTree = new HuffmanTree();
        huffmanTree.buildTree(freqMap);
        
        // 打印编码
        huffmanTree.printCodes();
        
        // 编码
        String encoded = huffmanTree.encode(text);
        System.out.println("\nEncoded: " + encoded);
        
        // 解码
        String decoded = huffmanTree.decode(encoded);
        System.out.println("Decoded: " + decoded);
        
        // 计算压缩率
        double originalSize = text.length() * 8.0; // 假设每个字符8位
        double compressedSize = encoded.length();
        double compressionRatio = (1 - compressedSize / originalSize) * 100;
        System.out.printf("Compression ratio: %.2f%%\n", compressionRatio);
    }
}
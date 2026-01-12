import java.util.NoSuchElementException;

// 红黑树节点颜色
enum Color {
    RED, BLACK
}

// 红黑树节点类
class Node {
    int data;
    Color color;
    Node left, right, parent;

    // 构造函数(默认红色, 符合插入规则)
    public Node(int data) {
        this.data = data;
        this.color = Color.RED;
        this.left = null;
        this.right = null;
        this.parent = null;
    }
}

public class RedBlackTree {
    private Node root;
    private Node nil;  // 哨兵节点(替代null, 简化边界处理)

    // 构造函数初始化
    public RedBlackTree() {
        nil = new Node(0);
        nil.color = Color.BLACK;  // 哨兵节点为黑色
        nil.left = nil;
        nil.right = nil;
        nil.parent = nil;
        root = nil;  // 初始根节点指向哨兵
    }

    // 左旋转
    private void leftRotate(Node x) {
        Node y = x.right;
        x.right = y.left;

        if (y.left != nil) {
            y.left.parent = x;
        }
        y.parent = x.parent;

        if (x.parent == nil) {
            root = y;
        } 
        else if (x == x.parent.left) {
            x.parent.left = y;
        } 
        else {
            x.parent.right = y;
        }
        y.left = x;
        x.parent = y;
    }

    // 右旋转
    private void rightRotate(Node y) {
        Node x = y.left;
        y.left = x.right;

        if (x.right != nil) {
            x.right.parent = y;
        }
        x.parent = y.parent;

        if (y.parent == nil) {
            root = x;
        } 
        else if (y == y.parent.right) {
            y.parent.right = x;
        } 
        else {
            y.parent.left = x;
        }
        x.right = y;
        y.parent = x;
    }

    // 插入后修复红黑树性质
    private void insertFixup(Node z) {
        while (z.parent.color == Color.RED) {  // 父节点为红色才可能违反规则
            if (z.parent == z.parent.parent.left) {
                Node y = z.parent.parent.right;  // 叔节点

                if (y.color == Color.RED) {
                    // 情况1: 叔节点为红(父、叔变黑, 祖父变红, 继续向上检查)
                    z.parent.color = Color.BLACK;
                    y.color = Color.BLACK;
                    z.parent.parent.color = Color.RED;
                    z = z.parent.parent;
                } 
                else {
                    if (z == z.parent.right) {
                        // 情况2: 叔节点为黑, 且z是右孩子(转为情况3)
                        z = z.parent;
                        leftRotate(z);
                    }
                    // 情况3: 叔节点为黑, 且z是左孩子(旋转+变色)
                    z.parent.color = Color.BLACK;
                    z.parent.parent.color = Color.RED;
                    rightRotate(z.parent.parent);
                }
            } 
            else {
                // 对称情况(父节点是右孩子)
                Node y = z.parent.parent.left;  // 叔节点

                if (y.color == Color.RED) {
                    z.parent.color = Color.BLACK;
                    y.color = Color.BLACK;
                    z.parent.parent.color = Color.RED;
                    z = z.parent.parent;
                } else {
                    if (z == z.parent.left) {
                        z = z.parent;
                        rightRotate(z);
                    }
                    z.parent.color = Color.BLACK;
                    z.parent.parent.color = Color.RED;
                    leftRotate(z.parent.parent);
                }
            }
        }
        root.color = Color.BLACK;  // 确保根节点为黑
    }

    // 插入操作
    public void insert(int data) {
        Node z = new Node(data);
        Node y = nil;
        Node x = root;

        // 找到插入位置
        while (x != nil) {
            y = x;
            if (z.data < x.data) {
                x = x.left;
            } 
            else {
                x = x.right;
            }
        }
        z.parent = y;

        if (y == nil) {
            root = z;  // 树为空, z成为根
        } 
        else if (z.data < y.data) {
            y.left = z;
        } 
        else {
            y.right = z;
        }

        z.left = nil;
        z.right = nil;
        z.color = Color.RED;  // 新节点默认红色

        insertFixup(z);  // 修复红黑树性质
    }

    // 查找最小值节点
    private Node minimum(Node node) {
        while (node.left != nil) {
            node = node.left;
        }
        return node;
    }

    // 替换子节点(用于删除操作)
    private void transplant(Node u, Node v) {
        if (u.parent == nil) {
            root = v;
        } 
        else if (u == u.parent.left) {
            u.parent.left = v;
        } 
        else {
            u.parent.right = v;
        }
        v.parent = u.parent;
    }

    // 删除后修复红黑树性质
    private void deleteFixup(Node x) {
        while (x != root && x.color == Color.BLACK) {
            if (x == x.parent.left) {
                Node w = x.parent.right;  // 兄弟节点

                if (w.color == Color.RED) {
                    // 情况1: 兄弟为红(转为兄弟为黑的情况)
                    w.color = Color.BLACK;
                    x.parent.color = Color.RED;
                    leftRotate(x.parent);
                    w = x.parent.right;
                }

                if (w.left.color == Color.BLACK && w.right.color == Color.BLACK) {
                    // 情况2: 兄弟为黑, 且两子均黑(兄弟变红, x上移)
                    w.color = Color.RED;
                    x = x.parent;
                } 
                else {
                    if (w.right.color == Color.BLACK) {
                        // 情况3: 兄弟为黑, 左子红右子黑(转为情况4)
                        w.left.color = Color.BLACK;
                        w.color = Color.RED;
                        rightRotate(w);
                        w = x.parent.right;
                    }
                    // 情况4: 兄弟为黑, 右子红(旋转+变色)
                    w.color = x.parent.color;
                    x.parent.color = Color.BLACK;
                    w.right.color = Color.BLACK;
                    leftRotate(x.parent);
                    x = root;  // 退出循环
                }
            } 
            else {
                // 对称情况(x是右孩子)
                Node w = x.parent.left;

                if (w.color == Color.RED) {
                    w.color = Color.BLACK;
                    x.parent.color = Color.RED;
                    rightRotate(x.parent);
                    w = x.parent.left;
                }

                if (w.right.color == Color.BLACK && w.left.color == Color.BLACK) {
                    w.color = Color.RED;
                    x = x.parent;
                } 
                else {
                    if (w.left.color == Color.BLACK) {
                        w.right.color = Color.BLACK;
                        w.color = Color.RED;
                        leftRotate(w);
                        w = x.parent.left;
                    }
                    w.color = x.parent.color;
                    x.parent.color = Color.BLACK;
                    w.left.color = Color.BLACK;
                    rightRotate(x.parent);
                    x = root;
                }
            }
        }
        x.color = Color.BLACK;  // 确保x为黑
    }

    // 删除操作
    public void delete(int data) {
        Node z = search(data);
        if (z == nil) {
            throw new NoSuchElementException("值 " + data + " 不存在于树中");
        }

        Node y = z;
        Node x;
        Color yOriginalColor = y.color;

        if (z.left == nil) {
            x = z.right;
            transplant(z, z.right);
        } 
        else if (z.right == nil) {
            x = z.left;
            transplant(z, z.left);
        } 
        else {
            y = minimum(z.right);  // 找后继节点
            yOriginalColor = y.color;
            x = y.right;

            if (y.parent == z) {
                x.parent = y;
            } 
            else {
                transplant(y, y.right);
                y.right = z.right;
                y.right.parent = y;
            }
            transplant(z, y);
            y.left = z.left;
            y.left.parent = y;
            y.color = z.color;
        }

        if (yOriginalColor == Color.BLACK) {
            deleteFixup(x);  // 只有删除黑节点才需要修复
        }
    }

    // 查找操作
    public Node search(int data) {
        Node current = root;
        while (current != nil && current.data != data) {
            if (data < current.data) {
                current = current.left;
            } 
            else {
                current = current.right;
            }
        }
        return current;  // 找到返回节点, 否则返回nil
    }

    // 中序遍历(用于打印树结构)
    private void inorder(Node node) {
        if (node != nil) {
            inorder(node.left);
            System.out.print(node.data + "(" + (node.color == Color.RED ? "R" : "B") + ") ");
            inorder(node.right);
        }
    }

    // 打印树
    public void print() {
        System.out.print("红黑树(中序遍历, R=红, B=黑): ");
        inorder(root);
        System.out.println();
    }

    // 测试
    public static void main(String[] args) {
        RedBlackTree rbt = new RedBlackTree();

        // 插入测试
        int[] values = {10, 20, 30, 15, 25, 5, 35};
        for (int v : values) {
            rbt.insert(v);
            System.out.print("插入 " + v + " 后: ");
            rbt.print();
        }

        // 查找测试
        int key = 20;
        Node node = rbt.search(key);
        if (node != rbt.nil) {
            System.out.println("查找 " + key + ": 存在(" + (node.color == Color.RED ? "红" : "黑") + ")");
        }

        // 删除测试
        rbt.delete(20);
        System.out.print("删除 20 后: ");
        rbt.print();

        rbt.delete(10);
        System.out.print("删除 10 后: ");
        rbt.print();
    }
}

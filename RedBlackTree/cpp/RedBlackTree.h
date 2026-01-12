#include <iostream>
#include <cassert>

enum Color { RED, BLACK };

template <typename T>
struct Node {
    T data;
    Color color;
    Node *left, *right, *parent;

    // 构造函数(默认红色，符合插入规则)
    Node(const T& val) : data(val), color(RED), left(nullptr), right(nullptr), parent(nullptr) {}
};

template <typename T>
class RedBlackTree {
private:
    Node<T>* root;
    Node<T>* nil;  // 哨兵节点(所有叶节点的替代，简化边界处理)

    // 左旋转
    void leftRotate(Node<T>* x) {
        Node<T>* y = x->right;
        x->right = y->left;

        if (y->left != nil) {
            y->left->parent = x;
        }
        y->parent = x->parent;

        if (x->parent == nil) {
            root = y;
        } 
        else if (x == x->parent->left) {
            x->parent->left = y;
        } 
        else {
            x->parent->right = y;
        }
        y->left = x;
        x->parent = y;
    }

    // 右旋转
    void rightRotate(Node<T>* y) {
        Node<T>* x = y->left;
        y->left = x->right;

        if (x->right != nil) {
            x->right->parent = y;
        }
        x->parent = y->parent;

        if (y->parent == nil) {
            root = x;
        } 
        else if (y == y->parent->right) {
            y->parent->right = x;
        } 
        else {
            y->parent->left = x;
        }
        x->right = y;
        y->parent = x;
    }

    // 插入后修复红黑树性质
    void insertFixup(Node<T>* z) {
        while (z->parent->color == RED) {  // 父节点为红才可能违反规则
            if (z->parent == z->parent->parent->left) {
                Node<T>* y = z->parent->parent->right;  // 叔节点

                if (y->color == RED) {
                    // 情况1：叔节点为红(父、叔变黑, 祖父变红, 继续向上检查)
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                } 
                else {
                    if (z == z->parent->right) {
                        // 情况2：叔节点为黑，且z是右孩子(转为情况3)
                        z = z->parent;
                        leftRotate(z);
                    }
                    // 情况3：叔节点为黑，且z是左孩子(旋转 + 变色)
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    rightRotate(z->parent->parent);
                }
            } 
            else {
                // 对称情况(父节点是右孩子)
                Node<T>* y = z->parent->parent->left;  // 叔节点

                if (y->color == RED) {
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                } 
                else {
                    if (z == z->parent->left) {
                        z = z->parent;
                        rightRotate(z);
                    }
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    leftRotate(z->parent->parent);
                }
            }
        }
        root->color = BLACK;  // 确保根节点为黑
    }

    // 查找最小值节点
    Node<T>* minimum(Node<T>* node) {
        while (node->left != nil) {
            node = node->left;
        }
        return node;
    }

    // 替换子节点(用于删除操作)
    void transplant(Node<T>* u, Node<T>* v) {
        if (u->parent == nil) {
            root = v;
        } 
        else if (u == u->parent->left) {
            u->parent->left = v;
        } 
        else {
            u->parent->right = v;
        }
        v->parent = u->parent;
    }

    // 删除后修复红黑树性质
    void deleteFixup(Node<T>* x) {
        while (x != root && x->color == BLACK) {
            if (x == x->parent->left) {
                Node<T>* w = x->parent->right;  // 兄弟节点

                if (w->color == RED) {
                    // 情况1：兄弟为红(转为兄弟为黑的情况)
                    w->color = BLACK;
                    x->parent->color = RED;
                    leftRotate(x->parent);
                    w = x->parent->right;
                }

                if (w->left->color == BLACK && w->right->color == BLACK) {
                    // 情况2：兄弟为黑，且两子均黑(兄弟变红，x上移)
                    w->color = RED;
                    x = x->parent;
                } 
                else {
                    if (w->right->color == BLACK) {
                        // 情况3：兄弟为黑，左子红右子黑(转为情况4)
                        w->left->color = BLACK;
                        w->color = RED;
                        rightRotate(w);
                        w = x->parent->right;
                    }
                    // 情况4：兄弟为黑，右子红(旋转+变色)
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    w->right->color = BLACK;
                    leftRotate(x->parent);
                    x = root;  // 退出循环
                }
            } 
            else {
                // 对称情况(x是右孩子)
                Node<T>* w = x->parent->left;

                if (w->color == RED) {
                    w->color = BLACK;
                    x->parent->color = RED;
                    rightRotate(x->parent);
                    w = x->parent->left;
                }

                if (w->right->color == BLACK && w->left->color == BLACK) {
                    w->color = RED;
                    x = x->parent;
                } 
                else {
                    if (w->left->color == BLACK) {
                        w->right->color = BLACK;
                        w->color = RED;
                        leftRotate(w);
                        w = x->parent->left;
                    }
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    w->left->color = BLACK;
                    rightRotate(x->parent);
                    x = root;
                }
            }
        }
        x->color = BLACK;  // 确保x为黑
    }

    // 递归删除节点(析构时使用)
    void destroy(Node<T>* node) {
        if (node != nil) {
            destroy(node->left);
            destroy(node->right);
            delete node;
        }
    }

    // 中序遍历(用于打印)
    void inorder(Node<T>* node) const {
        if (node != nil) {
            inorder(node->left);
            std::cout << node->data << "(" << (node->color == RED ? "R" : "B") << ") ";
            inorder(node->right);
        }
    }

public:
    // 构造函数(初始化哨兵节点和根节点)
    RedBlackTree() {
        nil = new Node<T>(T());
        nil->color = BLACK;  // 哨兵节点为黑
        nil->left = nil->right = nil->parent = nil;  // 哨兵自循环
        root = nil;
    }

    // 析构函数
    ~RedBlackTree() {
        destroy(root);
        delete nil;
    }

    // 插入操作
    void insert(const T& val) {
        Node<T>* z = new Node<T>(val);
        Node<T>* y = nil;
        Node<T>* x = root;

        // 找到插入位置
        while (x != nil) {
            y = x;
            if (z->data < x->data) {
                x = x->left;
            } 
            else {
                x = x->right;
            }
        }
        z->parent = y;

        if (y == nil) {
            root = z;  // 树为空，z成为根
        } 
        else if (z->data < y->data) {
            y->left = z;
        } 
        else {
            y->right = z;
        }

        z->left = nil;
        z->right = nil;
        z->color = RED;  // 新节点默认红色

        insertFixup(z);  // 修复红黑树性质
    }

    // 删除操作
    void remove(const T& val) {
        Node<T>* z = search(val);
        if (z == nil) {
            std::cout << "值 " << val << " 不存在于树中" << std::endl;
            return;
        }

        Node<T>* y = z;
        Node<T>* x;
        Color yOriginalColor = y->color;

        if (z->left == nil) {
            x = z->right;
            transplant(z, z->right);
        } 
        else if (z->right == nil) {
            x = z->left;
            transplant(z, z->left);
        } 
        else {
            y = minimum(z->right);  // 找后继节点
            yOriginalColor = y->color;
            x = y->right;

            if (y->parent == z) {
                x->parent = y;
            } 
            else {
                transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }

        delete z;  // 释放被删除节点的内存

        if (yOriginalColor == BLACK) {
            deleteFixup(x);  // 只有删除黑节点才需要修复
        }
    }

    // 查找操作
    Node<T>* search(const T& val) const {
        Node<T>* current = root;
        while (current != nil && current->data != val) {
            if (val < current->data) {
                current = current->left;
            } 
            else {
                current = current->right;
            }
        }
        return current;  // 找到返回节点，否则返回nil
    }

    // 打印树（中序遍历，按值升序）
    void print() const {
        std::cout << "红黑树(中序遍历, R=红, B=黑): ";
        inorder(root);
        std::cout << std::endl;
    }
};


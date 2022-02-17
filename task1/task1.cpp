#include <cstddef>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <random>

template<typename T>
class Set {
private:
    size_t size_ = 0;

    class TreeNode;

public:
    class iterator;

    Set() : size_(0) {
        delete root_;
        root_ = nullptr;
    }

    template<typename Iterator>
    Set(Iterator first, Iterator last) : size_(0) {
        delete root_;
        root_ = nullptr;
        for (auto it = first; it != last; ++it) {
            insert(*it);
        }
    }

    Set(std::initializer_list<T> elems) : size_(0) {
        delete root_;
        root_ = nullptr;
        for (const auto& elem: elems) {
            insert(elem);
        }
    }

    Set(const Set<T>& other) {
        *this = other;
    }

    Set<T>& operator=(const Set<T>& other) {
        if (this == &other) {
            return *this;
        }
        delete root_;
        root_ = nullptr;
        size_ = other.size_;
        if (other.root_ == nullptr) {
            return *this;
        }
        root_ = new TreeNode(other.root_->value);
        RecursiveCopy(root_, other.root_);
        return *this;
    }

    ~Set() {
        delete root_;
    }

    size_t size() const {
        return size_;
    }

    bool empty() const {
        return size_ == 0;
    }

    void insert(const T& elem) {
        if (root_ == nullptr) {
            ++size_;
            root_ = new TreeNode(elem);
            return;
        }
        auto v = root_;
        while (v->value < elem || elem < v->value) {
            if (elem < v->value) {
                if (v->left == nullptr) {
                    ++size_;
                    v->left = new TreeNode(elem);
                    v->left->parent = v;
                }
                v = v->left;
            } else {
                if (v->right == nullptr) {
                    ++size_;
                    v->right = new TreeNode(elem);
                    v->right->parent = v;
                }
                v = v->right;
            }
        }
        RenewUp(v);
    }

    void erase(const T& elem) {
        if (root_ == nullptr) {
            return;
        }
        auto v = root_;
        while (v != nullptr && (v->value < elem || elem < v->value)) {
            if (elem == v->value) {
                break;
            }
            if (elem < v->value) {
                v = v->left;
            } else {
                v = v->right;
            }
        }
        if (v == nullptr) {
            return;
        }
        --size_;
        RenewUp(RecursiveErase(v));
    }

    iterator begin() const {
        auto v = root_;
        while (v != nullptr && v->left != nullptr) {
            v = v->left;
        }
        return iterator(this, v);
    }

    iterator end() const {
        return iterator(this, nullptr);
    }

    iterator find(const T& elem) const {
        auto v = root_;
        while (v != nullptr && (v->value < elem || elem < v->value)) {
            if (elem < v->value) {
                v = v->left;
            } else {
                v = v->right;
            }
        }
        return iterator(this, v);
    }

    iterator lower_bound(const T& elem) const {
        if (root_ == nullptr) {
            return iterator(this, nullptr);
        }
        auto v = root_;
        while (v->value < elem || elem < v->value) {
            if (v->value < elem) {
                if (v->right == nullptr) {
                    break;
                }
                v = v->right;
            } else {
                if (v->left == nullptr) {
                    break;
                }
                v = v->left;
            }
        }
        while (v != nullptr && v->value < elem) {
            v = v->parent;
        }
        return iterator(this, v);
    }

    class iterator {
    public:
        const Set* owner;

        TreeNode* node_ref;

        iterator& operator=(const iterator& it) {
            owner = it.owner;
            node_ref = it.node_ref;
            return *this;
        }

        iterator() : owner(nullptr), node_ref(nullptr) {

        }

        explicit iterator(const Set* const owner, TreeNode* node_ref) : owner(owner), node_ref(node_ref) {

        }

        iterator operator++(int) {
            auto to_ret = *this;
            if (node_ref->right != nullptr) {
                node_ref = node_ref->right;
                while (node_ref->left != nullptr) {
                    node_ref = node_ref->left;
                }
            } else {
                while (node_ref->parent != nullptr && node_ref->parent->right == node_ref) {
                    node_ref = node_ref->parent;
                }
                node_ref = node_ref->parent;
            }
            return to_ret;
        }

        iterator& operator++() {
            if (node_ref->right != nullptr) {
                node_ref = node_ref->right;
                while (node_ref->left != nullptr) {
                    node_ref = node_ref->left;
                }
            } else {
                while (node_ref->parent != nullptr && node_ref->parent->right == node_ref) {
                    node_ref = node_ref->parent;
                }
                node_ref = node_ref->parent;
            }
            return *this;
        }

        iterator operator--(int) {
            auto to_ret = *this;
            if (node_ref == nullptr) {
                node_ref = owner->root_;
                while (node_ref->right != nullptr) {
                    node_ref = node_ref->right;
                }
            } else if (node_ref->left != nullptr) {
                node_ref = node_ref->left;
                while (node_ref->right != nullptr) {
                    node_ref = node_ref->right;
                }
            } else {
                while (node_ref->parent != nullptr && node_ref->parent->left == node_ref) {
                    node_ref = node_ref->parent;
                }
                node_ref = node_ref->parent;
            }
            return to_ret;
        }

        iterator& operator--() {
            if (node_ref == nullptr) {
                node_ref = owner->root_;
                while (node_ref->right != nullptr) {
                    node_ref = node_ref->right;
                }
            } else if (node_ref->left != nullptr) {
                node_ref = node_ref->left;
                while (node_ref->right != nullptr) {
                    node_ref = node_ref->right;
                }
            } else {
                while (node_ref->parent != nullptr && node_ref->parent->left == node_ref) {
                    node_ref = node_ref->parent;
                }
                node_ref = node_ref->parent;
            }
            return *this;
        }

        const T& operator*() const {
            return node_ref->value;
        }

        const T* operator->() const {
            return &(node_ref->value);
        }

        bool operator==(const iterator& it) {
            return node_ref == it.node_ref;
        }

        bool operator!=(const iterator& it) {
            return node_ref != it.node_ref;
        }
    };

private:
    void RecursiveCopy(TreeNode* to, TreeNode* from) {
        to->value = from->value;
        to->height = from->height;
        if (from->left != nullptr) {
            to->left = new TreeNode(from->left->value);
            to->left->parent = to;
            RecursiveCopy(to->left, from->left);
        }
        if (from->right != nullptr) {
            to->right = new TreeNode(from->right->value);
            to->right->parent = to;
            RecursiveCopy(to->right, from->right);
        }
    }

    void RenewUp(TreeNode* v) {
        if (v == nullptr) {
            root_ = nullptr;
            return;
        }
        while (v->parent != nullptr) {
            v->UpdateHeight();
            v = v->Rotate();
            v = v->parent;
        }
        v->UpdateHeight();
        v = v->Rotate();
        root_ = v;
    }

    TreeNode* RecursiveErase(TreeNode* v) {
        if (v->left == nullptr && v->right == nullptr) {
            if (v->parent == nullptr) {
                delete v;
                return nullptr;
            }
            auto parent = v->parent;
            if (v == parent->left) {
                parent->left = nullptr;
            } else {
                parent->right = nullptr;
            }
            delete v;
            return parent;
        }
        if (GetHeight(v->left) >= GetHeight(v->right)) {
            auto next_v = v->left;
            while (next_v->right != nullptr) {
                next_v = next_v->right;
            }
            std::swap(v->value, next_v->value);
            return RecursiveErase(next_v);
        }
        auto next_v = v->right;
        while (next_v->left != nullptr) {
            next_v = next_v->left;
        }
        std::swap(v->value, next_v->value);
        return RecursiveErase(next_v);
    }

    static size_t GetHeight(const TreeNode* node_ref) {
        if (node_ref == nullptr) {
            return static_cast<size_t>(0);
        }
        return node_ref->height;
    }

    class TreeNode {
    public:
        T value;
        TreeNode* left = nullptr;
        TreeNode* right = nullptr;
        TreeNode* parent = nullptr;
        size_t height = 0;

        TreeNode(const T& value) : value(value), height(1) {

        }

        void UpdateHeight() {
            height = std::max(GetHeight(left), GetHeight(right)) + 1;
        }

        ~TreeNode() {
            delete left;
            delete right;
        }

        TreeNode* SmallLeftRotate() {
            auto B = right;
            if (parent != nullptr) {
                if (this == parent->left) {
                    parent->left = B;
                } else {
                    parent->right = B;
                }
            }
            B->parent = parent;
            parent = B;
            auto C = B->left;
            if (C != nullptr) {
                C->parent = this;
            }
            B->left = this;
            this->right = C;
            this->parent = B;
            UpdateHeight();
            B->UpdateHeight();
            return B;
        }

        TreeNode* BigLeftRotate() {
            auto B = right;
            auto C = B->left;
            auto M = C->left;
            auto N = C->right;
            if (parent != nullptr) {
                if (this == parent->left) {
                    parent->left = C;
                } else {
                    parent->right = C;
                }
            }
            C->parent = parent;
            right = M;
            if (M != nullptr) {
                M->parent = this;
            }
            B->left = N;
            if (N != nullptr) {
                N->parent = B;
            }
            parent = C;
            B->parent = C;
            C->left = this;
            C->right = B;
            UpdateHeight();
            B->UpdateHeight();
            C->UpdateHeight();
            return C;
        }

        TreeNode* SmallRightRotate() {
            auto B = left;
            if (parent != nullptr) {
                if (this == parent->left) {
                    parent->left = B;
                } else {
                    parent->right = B;
                }
            }
            B->parent = parent;
            parent = B;
            auto C = B->right;
            if (C != nullptr) {
                C->parent = this;
            }
            B->right = this;
            this->left = C;
            this->parent = B;
            UpdateHeight();
            B->UpdateHeight();
            return B;
        }

        TreeNode* BigRightRotate() {
            auto B = left;
            auto C = B->right;
            auto M = C->left;
            auto N = C->right;
            if (parent != nullptr) {
                if (this == parent->left) {
                    parent->left = C;
                } else {
                    parent->right = C;
                }
            }
            C->parent = parent;
            B->right = M;
            if (M != nullptr) {
                M->parent = B;
            }
            left = N;
            if (N != nullptr) {
                N->parent = this;
            }
            parent = C;
            B->parent = C;
            C->left = B;
            C->right = this;
            UpdateHeight();
            B->UpdateHeight();
            C->UpdateHeight();
            return C;
        }

        TreeNode* Rotate() {
            if (abs(static_cast<int>(GetHeight(left)) - static_cast<int>(GetHeight(right))) <= 1) {
                return this;
            }
            if (GetHeight(left) < GetHeight(right)) {
                if (GetHeight(right->left) <= GetHeight(right->right)) {
                    return SmallLeftRotate();
                }
                return BigLeftRotate();
            } else {
                if (GetHeight(left->right) <= GetHeight(left->left)) {
                    return SmallRightRotate();
                }
                return BigRightRotate();
            }
        }
    };

    TreeNode* root_ = nullptr;
};

#include <algorithm>
#include <iostream>
#include <numeric>

template<typename T>
class Set {
private:
    class TreeNode {
    public:
        T value;
        TreeNode* left = nullptr;
        TreeNode* right = nullptr;
        TreeNode* parent = nullptr;
        size_t height = INITIAL_HEIGHT;

        TreeNode(const T& value) : value(value), height(1) {

        }

        void UpdateHeight() {
            height = std::max(GetHeight(left), GetHeight(right)) + 1;
        }

        ~TreeNode() {
            delete left;
            delete right;
        }

        TreeNode* SmallLeftRotation() {
            TreeNode* old_right = right;
            if (parent != nullptr) {
                if (this == parent->left) {
                    parent->left = old_right;
                } else {
                    parent->right = old_right;
                }
            }
            old_right->parent = parent;
            parent = old_right;
            TreeNode* old_right_left = old_right->left;
            if (old_right_left != nullptr) {
                old_right_left->parent = this;
            }
            old_right->left = this;
            this->right = old_right_left;
            this->parent = old_right;
            UpdateHeight();
            old_right->UpdateHeight();
            return old_right;
        }

        void ChangeRightLeft(TreeNode*& old_right, TreeNode*& old_right_left) {
            right = right->left->left;
            if (old_right_left->left != nullptr) {
                old_right_left->left->parent = this;
            }
            old_right->left = old_right_left->right;
            if (old_right_left->right != nullptr) {
                old_right_left->right->parent = old_right;
            }
        }

        void ChangeRight(TreeNode*& old_right, TreeNode*& old_right_left) {
            parent = old_right_left;
            old_right->parent = old_right_left;
            old_right_left->left = this;
            old_right_left->right = old_right;
            UpdateHeight();
            old_right->UpdateHeight();
            old_right_left->UpdateHeight();
        }

        TreeNode* BigLeftRotation() {
            if (parent != nullptr) {
                if (this == parent->left) {
                    parent->left = right->left;
                } else {
                    parent->right = right->left;
                }
            }
            right->left->parent = parent;
            TreeNode* old_right = right;
            TreeNode* old_right_left = right->left;
            ChangeRightLeft(old_right, old_right_left);
            ChangeRight(old_right, old_right_left);
            return old_right_left;
        }

        TreeNode* SmallRightRotation() {
            TreeNode* old_left = left;
            if (parent != nullptr) {
                if (this == parent->left) {
                    parent->left = old_left;
                } else {
                    parent->right = old_left;
                }
            }
            old_left->parent = parent;
            parent = old_left;
            TreeNode* old_left_right = old_left->right;
            if (old_left_right != nullptr) {
                old_left_right->parent = this;
            }
            old_left->right = this;
            this->left = old_left_right;
            this->parent = old_left;
            UpdateHeight();
            old_left->UpdateHeight();
            return old_left;
        }

        void ChangeLeftRight(TreeNode*& old_left_right) {
            left->right = old_left_right->left;
            if (old_left_right->left != nullptr) {
                old_left_right->left->parent = left;
            }
            left = old_left_right->right;
            if (old_left_right->right != nullptr) {
                old_left_right->right->parent = this;
            }
        }

        void ChangeLeft(TreeNode*& old_left, TreeNode*& old_left_right) {
            parent = old_left_right;
            old_left->parent = old_left_right;
            old_left_right->left = old_left;
            old_left_right->right = this;
            UpdateHeight();
            old_left->UpdateHeight();
            old_left_right->UpdateHeight();
        }

        TreeNode* BigRightRotation() {
            if (parent != nullptr) {
                if (this == parent->left) {
                    parent->left = left->right;
                } else {
                    parent->right = left->right;
                }
            }
            left->right->parent = parent;
            TreeNode* old_left = left;
            TreeNode* old_left_right = left->right;
            ChangeLeftRight(old_left_right);
            ChangeLeft(old_left, old_left_right);
            return old_left_right;
        }

        TreeNode* Rotate() {
            size_t height_left = GetHeight(left);
            size_t height_right = GetHeight(right);
            if (height_left == height_right || height_left + 1 == height_right || height_left == height_right + 1) {
                return this;
            }
            if (GetHeight(left) < GetHeight(right)) {
                if (GetHeight(right->left) <= GetHeight(right->right)) {
                    return SmallLeftRotation();
                }
                return BigLeftRotation();
            } else {
                if (GetHeight(left->right) <= GetHeight(left->left)) {
                    return SmallRightRotation();
                }
                return BigRightRotation();
            }
        }
    };

public:
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
            iterator to_ret = *this;
            operator++();
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
            iterator to_ret = *this;
            operator--();
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

    Set() : size_(INITIAL_SIZE) {
        root_ = nullptr;
    }

    template<typename Iterator>
    Set(Iterator first, Iterator last) : size_(INITIAL_SIZE) {
        root_ = nullptr;
        for (Iterator it = first; it != last; ++it) {
            insert(*it);
        }
    }

    Set(std::initializer_list<T> elems) : size_(INITIAL_SIZE) {
        root_ = nullptr;
        for (const T& elem: elems) {
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
        return size_ == INITIAL_SIZE;
    }

    void insert(const T& elem) {
        if (root_ == nullptr) {
            ++size_;
            root_ = new TreeNode(elem);
            return;
        }
        TreeNode* v = root_;
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
        TreeNode* v = root_;
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
        TreeNode* v = root_;
        while (v != nullptr && v->left != nullptr) {
            v = v->left;
        }
        return iterator(this, v);
    }

    iterator end() const {
        return iterator(this, nullptr);
    }

    iterator find(const T& elem) const {
        TreeNode* v = root_;
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
        TreeNode* v = root_;
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

private:
    static constexpr size_t INITIAL_SIZE = 0;
    static constexpr size_t INITIAL_HEIGHT = 0;

    size_t size_ = INITIAL_SIZE;

    /**
     * Copies all tree from @param from to @param to.
     * Firstly copies root's data, then copies left child, then right child.
     */
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

    /**
     * Makes rotates on the path to root
     */
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

    /**
     * Deep erase
     */
    TreeNode* RecursiveErase(TreeNode* v) {
        if (v->left == nullptr && v->right == nullptr) {
            if (v->parent == nullptr) {
                delete v;
                return nullptr;
            }
            TreeNode* parent = v->parent;
            if (v == parent->left) {
                parent->left = nullptr;
            } else {
                parent->right = nullptr;
            }
            delete v;
            return parent;
        }
        if (GetHeight(v->left) >= GetHeight(v->right)) {
            TreeNode* next_v = v->left;
            while (next_v->right != nullptr) {
                next_v = next_v->right;
            }
            std::swap(v->value, next_v->value);
            return RecursiveErase(next_v);
        }
        TreeNode* next_v = v->right;
        while (next_v->left != nullptr) {
            next_v = next_v->left;
        }
        std::swap(v->value, next_v->value);
        return RecursiveErase(next_v);
    }

    /**
     * if @param node_pointer is nullptr, return 0, else return the height if the tree
     */
    static size_t GetHeight(const TreeNode* node_pointer) {
        if (node_pointer == nullptr) {
            return INITIAL_HEIGHT;
        }
        return node_pointer->height;
    }

    TreeNode* root_ = nullptr;
};

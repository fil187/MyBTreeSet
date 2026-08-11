#include <cstddef>
#include <concepts>
#include <stdexcept>

enum Color { Black, Red };

/**
 * @brief An entry in a set.
 * 
 * @tparam T Must be totally ordered.
 * 
 * @invariant If `parent` then `parent->left == this` or `parent->right == this`. 
 * @invariant If `left` then `left->parent == this`.
 * @invariant If `right` then `right->parent == this`.
 */
template <std::totally_ordered T>
struct Entry {

    T value;
    Color color;
    Entry<T>* parent;
    Entry<T>* left;
    Entry<T>* right;

    /** 
     * @brief Constructs an entry.
     * 
     * @post `color == red`
     * @post The created entry has no children.
     */
    Entry(const T& value, Entry<T>* parent): value(value), color(Red), parent(parent), left(nullptr), right(nullptr) {}
};

/**
 * @brief A mutable red-black binary tree set.
 * 
 * @tparam T Must be totally ordered.
 * 
 * @invariant `root` is black.
 * @invariant `root == nullptr` iff `size == 0`.
 * @invariant `size` equals the number of nodes reachable from root.
 * @invariant A red node does not have a red child.
 * @invariant Every path from the root to each leaf has the same amount of black nodes.
 * @invariant All leaves are Black.
 * @invariant `entry->parent == nullptr` iff entry is the root.
 * @invariant Each value is contained in the set at most once.
 */
template <std::totally_ordered T>
class MyBTreeSet {

private:
    
    Entry<T>* root;
    size_t size;

public:
    
    /**
     * @brief Constructs an empty set.
     */
    MyBTreeSet(): root(nullptr), size(0) {}

    /**
     * @note Copying is disabled because the container owns dynamically 
     *       allocated storage and does not implement deep-copy semantics.
     */
    MyBTreeSet(const MyBTreeSet<T>&) = delete;

    /**
     * @note Copying is disabled because the container owns dynamically 
     *       allocated storage and does not implement deep-copy semantics.
     */
    MyBTreeSet<T>& operator=(const MyBTreeSet<T>&) = delete;

    /**
     * Constructs a set containing copies of the elements in `source`.
     * @brief Move Constructor.
     * 
     * @post All allocated memory owned by `source` are now owned by this set.
     * @post `source` no longer ownes the allocated memory.
     */
    MyBTreeSet(MyBTreeSet<T>&& source) noexcept : root(source.root), size(source.size) {
        source.root = nullptr;
        source.size = 0;
    }

    /**
     * Replaces the contents of this set with those of `source`.
     * @brief Move Assignment Operator.
     * 
     * @post All allocated memory owned by `source` are now owned by this set.
     * @post `source` no longer ownes the allocated memory.
     */
    MyBTreeSet<T>& operator=(MyBTreeSet<T>&& source) noexcept {
        delete_entry(root);
        root = source.root;
        size = source.size;
        source.root = nullptr;
        source.size = 0;
        return *this;
    }

    /**
     * @return The number of elements currently stored in the set.
     */
    size_t length() const {
        return size;
    }

    /**
     * @par Complexity
     *      O(log n)
     * 
     * @return true if the value was successfully inserted in the set, otherwise false.
     * @throw std::bad_alloc if the allocation fails.
     * 
     * @post The size of this set is increased by 1 iff the function returns true.
     * @post The value is present in the set.
     * @post All values initially present in the set are still present.
     */
    bool insert(const T& value) {
        
        if (size == 0) {
            root = new Entry<T>(value, nullptr);
            root->color = Black;
            size += 1;
            return true;
        }
        
        Entry<T>* entry = find_entry(value);

        if (entry->value == value)
            return false;
        
        Entry<T>* new_entry = new Entry(value, entry);

        if (entry->value > value && entry->left == nullptr) 
            entry->left = new_entry;
        else if (entry->value < value && entry->right == nullptr)
            entry->right = new_entry;
        else 
            return false;

        size += 1;
        fix_insert(new_entry);
        return true;
    }

    /**
     * @par Complexity
     *      O(log n)
     * 
     * @return true if the value is present in the set, otherwise false.
     */
    bool contains(const T& value) const {
        if (size == 0)
            return false;
        else
            return find_entry(value)->value == value;
    }

    /**
     * @par Complexity
     *      O(log n)
     * 
     * @return true if the value was successfully removed, otherwise false.
     * 
     * @post The size of this set is decreased by 1 iff the function returns true.
     * @post `value` is not in the set.
     * @post All values different from `value` initially present in the set are still present.
     */
    bool remove(const T& value) {

        if (size == 0)
            return false;

        Entry<T>* entry = find_entry(value);
        if (entry->value == value) {
            if (entry->left != nullptr && entry->right != nullptr) {
                
                Entry<T>* tail = entry->right;
                while (tail->left != nullptr)
                    tail = tail->left;
                bool is_left_child = tail == tail->parent->left;
                Entry<T>* replacement = tail->right;
                Entry<T>* parent = (tail->parent != entry) ? tail->parent : tail;
                if (tail->parent != entry) {
                    tail->parent->left = tail->right;
                    if (tail->right != nullptr)
                        tail->right->parent = tail->parent;
                    tail->parent = entry->parent;
                    tail->left = entry->left;
                    entry->left->parent = tail;
                    tail->right = entry->right;
                    entry->right->parent = tail;
                } else {
                    tail->parent = entry->parent;
                    tail->left = entry->left;
                    entry->left->parent = tail;
                }
                
                Color removed_color = tail->color;
                tail->color = entry->color;
                if (entry == root)
                    root = tail;
                else if (entry == entry->parent->left)
                    entry->parent->left = tail;
                else 
                    entry->parent->right = tail;

                if (removed_color == Black)
                    fix_remove(replacement, parent, is_left_child);
                

            } else if (entry->left != nullptr) {
                
                bool is_left_child = true;
                if (entry == root) {
                    root = entry->left;
                    root->parent = nullptr;
                } else if (entry == entry->parent->left) {
                    entry->parent->left = entry->left;
                    entry->left->parent = entry->parent;
                } else {
                    is_left_child = false;
                    entry->parent->right = entry->left;
                    entry->left->parent = entry->parent;
                }
                if (entry->color == Black)
                    fix_remove(entry->left, entry->left->parent, is_left_child);
            } else if (entry->right != nullptr) {
                
                bool is_left_child = true;
                if (entry == root) {
                    root = entry->right;
                    root->parent = nullptr;
                } else if (entry == entry->parent->left) {
                    entry->parent->left = entry->right;
                    entry->right->parent = entry->parent;
                } else {
                    is_left_child = false;
                    entry->parent->right = entry->right;
                    entry->right->parent = entry->parent;
                }
                if (entry->color == Black)
                    fix_remove(entry->right, entry->right->parent, is_left_child);
            } else {
                
                if (entry == root) {
                    root = nullptr;
                } else if (entry == entry->parent->left) {
                    entry->parent->left = nullptr;
                    if (entry->color == Black)
                        fix_remove(nullptr, entry->parent, true);
                } else {
                    entry->parent->right = nullptr;
                    if (entry->color == Black)
                        fix_remove(nullptr, entry->parent, false);
                }
            }
            delete entry;
            size -= 1;    
            return true;    
        } else {
            return false;
        }
    }

    /**
     * @par Complexity
     *      O(n)
     */
    ~MyBTreeSet() {
        delete_entry(root);
    }

private:

    /** 
     * @par Complexity
     *      O(log n)
     * 
     * @return The entry with value `value` if it exists,
     *         the last non-null entry otherwise.
     * 
     * @pre The tree is not empty.
     * @post The returned entry is not null.
     */
    Entry<T>* find_entry(const T& value) const {
        
        Entry<T>* node = root;
        while (node->value != value)
            if (node->value > value && node->left != nullptr)
                node = node->left;
            else if (node->value < value && node->right != nullptr)
                node = node->right;
            else
                break;
            
        return node;
    }

    /**
     * @par Complexity
     *      O(log(n))
     * 
     * @param entry The node added to the tree.
     * 
     * @pre `size > 1`
     */
    void fix_insert(Entry<T>* entry) {
        
        while (entry != root && entry->parent->color == Red) {

            if (entry->parent == entry->parent->parent->left) {                
                Entry<T>* uncle = entry->parent->parent->right;
                if (uncle != nullptr && uncle->color == Red) {
                    entry->parent->color = Black;
                    uncle->color = Black;
                    entry->parent->parent->color = Red;
                    entry = entry->parent->parent;
                } else {
                    if (entry == entry->parent->right) {
                        entry = entry->parent;
                        rotate_left(entry);
                    }
                    entry->parent->color = Black;
                    entry->parent->parent->color = Red;
                    rotate_right(entry->parent->parent);
                }
            } else {
                Entry<T>* uncle = entry->parent->parent->left;
                if (uncle != nullptr && uncle->color == Red) {
                    entry->parent->color = Black;
                    uncle->color = Black;
                    entry->parent->parent->color = Red;
                    entry = entry->parent->parent;
                } else {
                    if (entry == entry->parent->left) {
                        entry = entry->parent;
                        rotate_right(entry);
                    }
                    entry->parent->color = Black;
                    entry->parent->parent->color = Red;
                    rotate_left(entry->parent->parent);
                }
            }
        }
        root->color = Black;
    }

    /**
     * @par Complexity
     *      O(log n)
     * 
     * @param replacement The node that replaces the physically removed node in the tree,
     *                    `replacement == nullptr` iff the deleted node is a leaf.
     * @param parent The parent of the replacement node,
     *               `parent == nullptr` iff the deleted node was the root.
     * @param left_child true if the physically removed node was the left child, false otherwise.
     * 
     * @pre `size > 1`
     * @pre The removed entry was black.
     */
    void fix_remove(Entry<T>* replacement, Entry<T>* parent, bool left_child) {

        auto is_black = [] (Entry<T>* entry) -> bool {
            return entry == nullptr || entry->color == Black;
        };

        if (replacement == nullptr && parent == nullptr)
            return;
        if (parent == nullptr) {
            replacement->color = Black;
            return;
        }
        
        Entry<T>* node = replacement;
        Entry<T>* node_parent = parent;
        Entry<T>* sibling = left_child ? node_parent->right : node_parent->left;

        while (node != root && is_black(node)) {
            if (left_child) {
                if (!is_black(sibling)) {
                    node_parent->color = Red;
                    sibling->color = Black;
                    rotate_left(node_parent);
                    node_parent = node ? node->parent : node_parent;
                    left_child = node_parent->left == node;
                    sibling = left_child ? node_parent->right : node_parent->left;
                } else if (is_black(sibling ? sibling->left : nullptr) 
                    && is_black(sibling ? sibling->right : nullptr))
                {
                    if (sibling)
                        sibling->color = Red;
                    if (node_parent->color == Red) {
                        node_parent->color = Black;
                        break;
                    } else {
                        node = node_parent;
                        node_parent = node_parent->parent;
                        left_child = node_parent && node == node_parent->left;
                        sibling = node_parent ? (left_child ? node_parent->right : node_parent->left) : nullptr;
                    }
                } else if (!is_black(sibling ? sibling->left : nullptr)
                    && is_black(sibling ? sibling->right : nullptr)) 
                {
                    sibling->color = Red;
                    sibling->left->color = Black;
                    rotate_right(sibling);
                    sibling = left_child ? node_parent->right : node_parent->left;
                } else if (!is_black(sibling ? sibling->right : nullptr)) {
                    sibling->color = node_parent->color;
                    node_parent->color = Black;
                    sibling->right->color = Black;
                    rotate_left(node_parent);
                    break;
                }
            } else {
                if (!is_black(sibling)) {
                    node_parent->color = Red;
                    sibling->color = Black;
                    rotate_right(node_parent);
                    node_parent = node ? node->parent : node_parent;
                    left_child = node_parent->left == node;
                    sibling = left_child ? node_parent->right : node_parent->left;
                } else if (is_black(sibling ? sibling->left : nullptr) 
                    && is_black(sibling ? sibling->right : nullptr))
                {
                    if (sibling)
                        sibling->color = Red;
                    if (node_parent->color == Red) {
                        node_parent->color = Black;
                        break;
                    } else {
                        node = node_parent;
                        node_parent = node_parent->parent;
                        left_child = node_parent != nullptr && node == node_parent->left;
                        sibling = node_parent ? (left_child ? node_parent->right : node_parent->left) : nullptr;
                    }
                } else if (!is_black(sibling ? sibling->right : nullptr)
                    && is_black(sibling ? sibling->left : nullptr)) 
                {
                    sibling->color = Red;
                    sibling->right->color = Black;
                    rotate_left(sibling);
                    sibling = left_child ? node_parent->right : node_parent->left;
                } else if (!is_black(sibling ? sibling->left : nullptr)) {
                    sibling->color = node_parent->color;
                    node_parent->color = Black;
                    sibling->left->color = Black;
                    rotate_right(node_parent);
                    break;
                }
            }
        }

        if (node)
            node->color = Black;
    }

    /**
     * @param node The root of the subtree to rotate.
     * 
     * @pre `node != nullptr` and `node->right != nullptr`.
     * @post `entry->left == node`
     * @post `entry` is the child of the grandparent.
     * @post `this.root == entry` iff `node` was the root.
     */
    void rotate_left(Entry<T>* node) {

        Entry<T>* entry = node->right;
        
        node->right = entry->left;
        if (entry->left != nullptr)
            entry->left->parent = node;
        
        entry->parent = node->parent;
        if (node->parent == nullptr)
            root = entry;
        else if (node == node->parent->left)
            node->parent->left = entry;
        else
            node->parent->right = entry;
        
        entry->left = node;
        node->parent = entry;
    }

    /**
     * @param node The root of the subtree to rotate.
     * 
     * @pre `node != nullptr` and `node->left != nullptr`.
     * @post `entry->right == node` 
     * @post `entry` is the child of the grandparent.
     * @post `this.root == entry` iff `node` was the root.
     */
    void rotate_right(Entry<T>* node) {

        Entry<T>* entry = node->left;
        
        node->left = entry->right;
        if (entry->right != nullptr)
            entry->right->parent = node;
        
        entry->parent = node->parent;
        if (node->parent == nullptr)
            root = entry;
        else if (node == node->parent->left)
            node->parent->left = entry;
        else
            node->parent->right = entry;
        
        entry->right = node;
        node->parent = entry;
    }

    /**
     * @param entry The root of the subtree to be deleted.
     * @post All nodes from the subtree with root `entry` are deleted.
     */
    void delete_entry(Entry<T>* entry) {
        
        if (entry != nullptr) {
            if (entry->left)
                delete_entry(entry->left);
            if (entry->right)
                delete_entry(entry->right);
            delete entry;
        }
    }

};
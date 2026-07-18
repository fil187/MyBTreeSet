#include <cstddef>
#include <concepts>
#include <stdexcept>

enum Color { Black, Red };

template <std::totally_ordered T>
struct Entry {

    /**
     * @invariant an entry forms a bidirectional 1 to 1 relationship with another entry (parent <-> child)
     *            the parent must have this entry as either it's left or right child
     */
    T value;
    Color color;
    Entry<T>* parent;
    Entry<T>* left;
    Entry<T>* right;

    /** 
     * @param value -> value to be stored by the Entry
     * @param parent -> the parent of this Entry, parent is null only for root
     * @pre created entry is Red and has no child
     */
    Entry(const T& value, Entry<T>* parent): value(value), color(Red), parent(parent), left(nullptr), right(nullptr) {}
};

template <std::totally_ordered T>
class MyBTreeSet {

private:
    
    /**
     * @invariant the root is always black
     * @invariant a Red node does not have a Red child
     * @invariant every path from the root to each leaf has the same ammount of Black nodes
     * @invariant all leaves are Black
     * @invariant entry->parent == nullptr then entry is the root
     */
    Entry<T>* root;
    size_t size;

public:
    
    /**
     * default constructor
     * @post root is null and the size is 0
     */
    MyBTreeSet(): root(nullptr), size(0) {}

    size_t length() const {
        return size;
    }

    /**
     * @returns True if the value was succesfully inserted in the set,
     * @returns False if the value was not succesfully inserted (was already present in the set)
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

    bool contains(const T& value) const {
        if (size == 0)
            return false;
        else
            return find_entry(value)->value == value;
    }

    /**
     * @returns True if the value was removed from the tree,
     * @returns False if the value was not removed from the tree (value was not present)
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
     * calls delet_entry -> deletes all child node and current node
     * @post all entrys are deleted
     */
    ~MyBTreeSet() {
        delete_entry(root);
    }

private:

    /** 
     * @param value -> the value to search for in the tree
     * @pre it is guaranteed that the tree is not empty
     * @post returned Entry is not NULL
     * @returns Entry corresponding with value if it exist
     * @returns last not null entry if value is not present in the tree
     */
    Entry<T>* find_entry(const T& value) const {
        
        Entry<T>* node = root;
        while (node->value != value)
            if (node->value > value && node->left != nullptr)
                node = node->left;
            else if (node->right != nullptr)
                node = node->right;
            else
                break;
            
        return node;
    }

    /**
     * @param entry -> the node added to the tree
     * @pre size of the tree is greater than 1
     * @post tree respects the red black tree rules
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
     * @param replacement -> the node that replaces the physically removed node in the tree,
     *                       is null if deleted node is a leaf
     * @param parent -> the parent of the replacement node,
     *                  is null if deleted node was root
     * @param left_child -> true if the physically removed node was left child otherwise false
     * @pre size > 1
     * @pre the removed entry was Black
     * @post tree respects the red black tree rules
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
     * @param node -> root of the subtree to rotate
     * @pre node and node.right are not null
     * @post node is the left child of entry, 
     *       entry is the child of the grandparent, 
     *       if node was the root then this.root = entry
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
     * @param node -> root of the subtree to rotate
     * @pre node and node.left are not null
     * @post node is the right child of entry, 
     *       entry is the child of the grandparent, 
     *       if node was the root then this.root = entry
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
     * @param entry -> the root of the sub-tree to be deleted
     * @post deletes all node from the sub-tree with root entry
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
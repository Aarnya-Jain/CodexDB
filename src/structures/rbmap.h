#ifndef RBMAP_H
#define RBMAP_H

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

template<typename Key, typename T, typename Compare = std::less<Key>>
class RBMap {
    enum Color { RED, BLACK };

    struct Node {
        pair<Key, T> kv;
        Color color;
        Node *left, *right, *parent;
        Node(const Key &k, const T &v, Color c = RED)
            : kv(k, v), color(c), left(nullptr), right(nullptr), parent(nullptr) {}
    };

    Node *root;
    size_t cnt;
    Compare comp;

public:
    RBMap() : root(nullptr), cnt(0), comp(Compare()) {}
    ~RBMap() { clear(); }

    struct iterator {
        Node *n;
        iterator(Node *p = nullptr) : n(p) {}

        struct proxy {
            const Key &first;
            T &second;
            proxy(const Key &f, T &s) : first(f), second(s) {}
        };

        proxy operator*() const { return proxy(n->kv.first, n->kv.second); }
        proxy operator->() const { return proxy(n->kv.first, n->kv.second); }

        bool operator==(const iterator &o) const { return n == o.n; }
        bool operator!=(const iterator &o) const { return n != o.n; }

        iterator& operator++() {
            if (!n) return *this;
            if (n->right) {
                n = n->right;
                while (n->left) n = n->left;
            } else {
                Node *p = n->parent;
                while (p && n == p->right) {
                    n = p;
                    p = p->parent;
                }
                n = p;
            }
            return *this;
        }
    };

    struct const_iterator {
        const Node *n;
        const_iterator(const Node *p = nullptr) : n(p) {}

        struct proxy {
            const Key &first;
            const T &second;
            proxy(const Key &f, const T &s) : first(f), second(s) {}
        };

        proxy operator*() const { return proxy(n->kv.first, n->kv.second); }
        proxy operator->() const { return proxy(n->kv.first, n->kv.second); }

        bool operator==(const const_iterator &o) const { return n == o.n; }
        bool operator!=(const const_iterator &o) const { return n != o.n; }

        const_iterator& operator++() {
            if (!n) return *this;
            if (n->right) {
                n = n->right;
                while (n->left) n = n->left;
            } else {
                const Node *p = n->parent;
                while (p && n == p->right) {
                    n = p;
                    p = p->parent;
                }
                n = p;
            }
            return *this;
        }
    };

    bool empty() const { return cnt == 0; }
    size_t size() const { return cnt; }
    void clear() { clear_rec(root); root = nullptr; cnt = 0; }

    void insert(const Key &key, const T &value) {
        Node* z = bstInsert(key, value);
        if (z->color == RED || z == root) {
            insert_fixup(z);
        }
    }

    T& operator[](const Key &k) {
        Node *x = root, *p = nullptr;
        while (x) {
            p = x;
            if (!comp(k, x->kv.first) && !comp(x->kv.first, k)) {
                return x->kv.second;
            }
            if (comp(k, x->kv.first)) x = x->left;
            else x = x->right;
        }
        Node *z = new Node(k, T(), RED);
        z->parent = p;
        if (!p) root = z;
        else if (comp(z->kv.first, p->kv.first)) p->left = z;
        else p->right = z;
        ++cnt;
        insert_fixup(z);
        return z->kv.second;
    }

    size_t count(const Key &k) const {
        return findNode(k) ? 1 : 0;
    }

    iterator find(const Key &k) {
        Node *n = findNode(k);
        return iterator(n);
    }

    const_iterator find(const Key &k) const {
        const Node *n = findNode(k);
        return const_iterator(n);
    }

    iterator lower_bound(const Key &k) {
        Node *x = root, *res = nullptr;
        while (x) {
            if (!comp(x->kv.first, k)) { res = x; x = x->left; }
            else x = x->right;
        }
        return iterator(res);
    }

    iterator upper_bound(const Key &k) {
        Node *x = root, *res = nullptr;
        while (x) {
            if (comp(k, x->kv.first)) { res = x; x = x->left; }
            else x = x->right;
        }
        return iterator(res);
    }

    iterator begin() {
        Node *x = root;
        if (!x) return iterator(nullptr);
        while (x->left) x = x->left;
        return iterator(x);
    }

    iterator end() { return iterator(nullptr); }

    const_iterator begin() const {
        const Node *x = root;
        if (!x) return const_iterator(nullptr);
        while (x->left) x = x->left;
        return const_iterator(x);
    }

    const_iterator end() const { return const_iterator(nullptr); }

    void erase(const Key &k) {
        Node *z = findNode(k);
        if (!z) return;

        Node *y = z;
        Node *x;
        Color y_original_color = y->color;

        if (!z->left) {
            x = z->right;
            transplant(z, z->right);
        } else if (!z->right) {
            x = z->left;
            transplant(z, z->left);
        } else {
            y = minimum(z->right);
            y_original_color = y->color;
            x = y->right;
            if (y->parent == z) {
                if (x) x->parent = y;
            } else {
                transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }
        delete z;
        cnt--;
        if (y_original_color == BLACK) {
            delete_fixup(x);
        }
    }

private:
    Node* findNode(const Key &k) const {
        Node *x = root;
        while (x) {
            if (!comp(k, x->kv.first) && !comp(x->kv.first, k)) return x;
            if (comp(k, x->kv.first)) x = x->left;
            else x = x->right;
        }
        return nullptr;
    }

    Node* bstInsert(const Key &key, const T &value) {
        Node* y = nullptr;
        Node* x = root;

        while (x != nullptr) {
            y = x;
            if (comp(key, x->kv.first))
                x = x->left;
            else if (comp(x->kv.first, key))
                x = x->right;
            else {
                x->kv.second = value; // Update existing value
                return x;
            }
        }

        Node* z = new Node(key, value);
        z->parent = y;

        if (!y)
            root = z;
        else if (comp(key, y->kv.first))
            y->left = z;
        else
            y->right = z;

        return z;
    }

    Node* minimum(Node* n) {
        while (n->left) n = n->left;
        return n;
    }

    void transplant(Node* u, Node* v) {
        if (!u->parent) root = v;
        else if (u == u->parent->left) u->parent->left = v;
        else u->parent->right = v;
        if (v) v->parent = u->parent;
    }

    void clear_rec(Node *n) {
        if (!n) return;
        clear_rec(n->left);
        clear_rec(n->right);
        delete n;
    }

    void rotate_left(Node *x) {
        Node *y = x->right;
        x->right = y->left;
        if (y->left) y->left->parent = x;
        y->parent = x->parent;
        if (!x->parent) root = y;
        else if (x == x->parent->left) x->parent->left = y;
        else x->parent->right = y;
        y->left = x;
        x->parent = y;
    }

    void rotate_right(Node *x) {
        Node *y = x->left;
        x->left = y->right;
        if (y->right) y->right->parent = x;
        y->parent = x->parent;
        if (!x->parent) root = y;
        else if (x == x->parent->right) x->parent->right = y;
        else x->parent->left = y;
        y->right = x;
        x->parent = y;
    }

    void insert_fixup(Node *z) {
        while (z->parent && z->parent->color == RED) {
            Node *gp = z->parent->parent;
            if (!gp) break;
            if (z->parent == gp->left) {
                Node *y = gp->right;
                if (y && y->color == RED) {
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    gp->color = RED;
                    z = gp;
                } else {
                    if (z == z->parent->right) {
                        z = z->parent;
                        rotate_left(z);
                    }
                    z->parent->color = BLACK;
                    gp->color = RED;
                    rotate_right(gp);
                }
            } else {
                Node *y = gp->left;
                if (y && y->color == RED) {
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    gp->color = RED;
                    z = gp;
                } else {
                    if (z == z->parent->left) {
                        z = z->parent;
                        rotate_right(z);
                    }
                    z->parent->color = BLACK;
                    gp->color = RED;
                    rotate_left(gp);
                }
            }
        }
        if (root) root->color = BLACK;
    }

    void delete_fixup(Node* x) {
        while (x != root && (!x || x->color == BLACK)) {
            if (x == x->parent->left) {
                Node* w = x->parent->right;
                if (w->color == RED) {
                    w->color = BLACK;
                    x->parent->color = RED;
                    rotate_left(x->parent);
                    w = x->parent->right;
                }
                if ((!w->left || w->left->color == BLACK) && (!w->right || w->right->color == BLACK)) {
                    w->color = RED;
                    x = x->parent;
                } else {
                    if (!w->right || w->right->color == BLACK) {
                        if (w->left) w->left->color = BLACK;
                        w->color = RED;
                        rotate_right(w);
                        w = x->parent->right;
                    }
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    if (w->right) w->right->color = BLACK;
                    rotate_left(x->parent);
                    x = root;
                }
            } else {
                Node* w = x->parent->left;
                if (w->color == RED) {
                    w->color = BLACK;
                    x->parent->color = RED;
                    rotate_right(x->parent);
                    w = x->parent->left;
                }
                if ((!w->right || w->right->color == BLACK) && (!w->left || w->left->color == BLACK)) {
                    w->color = RED;
                    x = x->parent;
                } else {
                    if (!w->left || w->left->color == BLACK) {
                        if (w->right) w->right->color = BLACK;
                        w->color = RED;
                        rotate_left(w);
                        w = x->parent->left;
                    }
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    if (w->left) w->left->color = BLACK;
                    rotate_right(x->parent);
                    x = root;
                }
            }
        }
        if (x) x->color = BLACK;
    }
};

#endif

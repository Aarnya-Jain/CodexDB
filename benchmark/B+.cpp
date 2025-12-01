#include <bits/stdc++.h>
using namespace std;

template <typename Key, typename T, typename Compare = std::less<Key>>
class mapp
{
    static const int MAX_KEYS = 4;

    struct Node
    {
        bool is_leaf;
        vector<Key> keys;
        Node *parent;
        vector<Node *> children;
        vector<T> values;
        Node *next;

        Node(bool leaf = false) : is_leaf(leaf), parent(nullptr), next(nullptr) {}
    };

    Node *root;
    size_t node_count;

public:
    struct iterator
    {
        Node *leaf;
        int idx;
        iterator(Node *l = nullptr, int i = 0) : leaf(l), idx(i) {}

        struct reference
        {
            const Key &first;
            typename vector<T>::reference second;
            reference(const Key &f, typename vector<T>::reference s) : first(f), second(s) {}
        };

        reference operator*() const
        {
            return reference(leaf->keys[idx], leaf->values[idx]);
        }

        bool operator==(const iterator &o) const { return leaf == o.leaf && idx == o.idx; }
        bool operator!=(const iterator &o) const { return !(*this == o); }

        iterator &operator++()
        {
            if (!leaf)
                return *this;
            idx++;
            if (idx >= (int)leaf->keys.size())
            {
                leaf = leaf->next;
                idx = 0;
            }
            return *this;
        }
    };

    using const_iterator = iterator;

    mapp() : root(nullptr), node_count(0) {}
    ~mapp() { clear(); }

    mapp(const mapp &other) : root(nullptr), node_count(0)
    {
        if (other.root)
            clone_from(other);
        node_count = other.node_count;
    }

    mapp &operator=(const mapp &other)
    {
        if (this == &other)
            return *this;
        clear();
        if (other.root)
        {
            clone_from(other);
            node_count = other.node_count;
        }
        else
        {
            root = nullptr;
            node_count = 0;
        }
        return *this;
    }

    mapp(mapp &&other) noexcept : root(other.root), node_count(other.node_count)
    {
        other.root = nullptr;
        other.node_count = 0;
    }

    mapp &operator=(mapp &&other) noexcept
    {
        if (this == &other)
            return *this;
        clear();
        root = other.root;
        node_count = other.node_count;
        other.root = nullptr;
        other.node_count = 0;
        return *this;
    }

    bool empty() const { return node_count == 0; }
    size_t size() const { return node_count; }

    void clear()
    {
        clear_rec(root);
        root = nullptr;
        node_count = 0;
    }

    typename vector<T>::reference operator[](const Key &k)
    {
        if (!root)
        {
            root = new Node(true);
            root->keys.push_back(k);
            root->values.push_back(T());
            node_count = 1;
            return root->values[0];
        }

        Node *leaf = find_leaf(k);
        if (!leaf)
        {

            root = new Node(true);
            root->keys.push_back(k);
            root->values.push_back(T());
            node_count = 1;
            return root->values[0];
        }

        int pos = lower_bound_index(leaf->keys, k);

        if (pos < (int)leaf->keys.size() && !Compare()(k, leaf->keys[pos]) && !Compare()(leaf->keys[pos], k))
        {
            return leaf->values[pos];
        }

        leaf->keys.insert(leaf->keys.begin() + pos, k);
        leaf->values.insert(leaf->values.begin() + pos, T());
        node_count++;

        if ((int)leaf->keys.size() > MAX_KEYS)
        {
            split_leaf(leaf);
            leaf = find_leaf(k);
            if (!leaf)
            {

                cerr << "ERROR: Could not find leaf after split for key" << endl;
                abort();
            }
            pos = lower_bound_index(leaf->keys, k);
        }

        return leaf->values[pos];
    }

    size_t count(const Key &k) const
    {
        if (!root)
            return 0;
        Node *lf = find_leaf_const(k);
        if (!lf)
            return 0;
        int pos = lower_bound_index(lf->keys, k);
        if (pos < (int)lf->keys.size() && !Compare()(k, lf->keys[pos]) && !Compare()(lf->keys[pos], k))
            return 1;
        return 0;
    }

    iterator find(const Key &k)
    {
        if (!root)
            return iterator(nullptr, 0);
        Node *lf = find_leaf(k);
        if (!lf)
            return iterator(nullptr, 0);
        int pos = lower_bound_index(lf->keys, k);
        if (pos < (int)lf->keys.size() && !Compare()(k, lf->keys[pos]) && !Compare()(lf->keys[pos], k))
            return iterator(lf, pos);
        return iterator(nullptr, 0);
    }

    const_iterator find(const Key &k) const
    {
        if (!root)
            return const_iterator(nullptr, 0);
        Node *lf = find_leaf_const(k);
        if (!lf)
            return const_iterator(nullptr, 0);
        int pos = lower_bound_index(lf->keys, k);
        if (pos < (int)lf->keys.size() && !Compare()(k, lf->keys[pos]) && !Compare()(lf->keys[pos], k))
            return const_iterator(lf, pos);
        return const_iterator(nullptr, 0);
    }

    iterator lower_bound(const Key &k)
    {
        if (!root)
            return iterator(nullptr, 0);
        Node *lf = find_leaf(k);
        if (!lf)
            return iterator(nullptr, 0);
        int pos = lower_bound_index(lf->keys, k);
        if (pos < (int)lf->keys.size())
            return iterator(lf, pos);
        if (lf->next)
            return iterator(lf->next, 0);
        return iterator(nullptr, 0);
    }

    iterator upper_bound(const Key &k)
    {
        if (!root)
            return iterator(nullptr, 0);
        Node *lf = find_leaf(k);
        if (!lf)
            return iterator(nullptr, 0);
        int pos = upper_bound_index(lf->keys, k);
        if (pos < (int)lf->keys.size())
            return iterator(lf, pos);
        if (lf->next)
            return iterator(lf->next, 0);
        return iterator(nullptr, 0);
    }

    iterator begin()
    {
        Node *n = root;
        if (!n)
            return iterator(nullptr, 0);
        while (!n->is_leaf)
        {
            if (n->children.empty())
                return iterator(nullptr, 0);
            n = n->children.front();
        }
        if (n->keys.empty())
            return iterator(nullptr, 0);
        return iterator(n, 0);
    }
    iterator end() { return iterator(nullptr, 0); }

    const_iterator begin() const
    {
        Node *n = root;
        if (!n)
            return const_iterator(nullptr, 0);
        while (!n->is_leaf)
        {
            if (n->children.empty())
                return const_iterator(nullptr, 0);
            n = n->children.front();
        }
        if (n->keys.empty())
            return const_iterator(nullptr, 0);
        return const_iterator(n, 0);
    }
    const_iterator end() const { return const_iterator(nullptr, 0); }

private:
    static int lower_bound_index(const vector<Key> &vec, const Key &k)
    {
        int l = 0, r = (int)vec.size();
        while (l < r)
        {
            int m = (l + r) >> 1;
            if (Compare()(vec[m], k))
                l = m + 1;
            else
                r = m;
        }
        return l;
    }

    static int upper_bound_index(const vector<Key> &vec, const Key &k)
    {
        int l = 0, r = (int)vec.size();
        while (l < r)
        {
            int m = (l + r) >> 1;
            if (!Compare()(k, vec[m]))
                l = m + 1;
            else
                r = m;
        }
        return l;
    }

    Node *find_leaf(const Key &k)
    {
        Node *n = root;
        if (!n)
            return nullptr;
        while (!n->is_leaf)
        {
            if (n->children.empty())
            {
                cerr << "ERROR: Internal node has no children! Keys: " << n->keys.size() << endl;
                cerr << "This should never happen - internal nodes must have children" << endl;
                abort();
            }
            int pos = lower_bound_index(n->keys, k);

            if (pos >= (int)n->children.size())
            {
                pos = (int)n->children.size() - 1;
            }
            n = n->children[pos];
            if (!n)
            {
                cerr << "ERROR: Null child pointer at position " << pos << endl;
                abort();
            }
        }
        return n;
    }

    Node *find_leaf_const(const Key &k) const
    {
        Node *n = root;
        if (!n)
            return nullptr;
        while (!n->is_leaf)
        {
            if (n->children.empty())
                return nullptr;
            int pos = lower_bound_index(n->keys, k);
            if (pos >= (int)n->children.size())
            {
                pos = (int)n->children.size() - 1;
            }
            n = n->children[pos];
            if (!n)
                return nullptr;
        }
        return n;
    }

    void split_leaf(Node *leaf)
    {
        int total = (int)leaf->keys.size();
        int mid = total / 2;

        Node *new_leaf = new Node(true);
        new_leaf->keys.assign(leaf->keys.begin() + mid, leaf->keys.end());
        new_leaf->values.assign(leaf->values.begin() + mid, leaf->values.end());
        leaf->keys.erase(leaf->keys.begin() + mid, leaf->keys.end());
        leaf->values.erase(leaf->values.begin() + mid, leaf->values.end());

        new_leaf->next = leaf->next;
        leaf->next = new_leaf;
        new_leaf->parent = leaf->parent;

        Key promote = new_leaf->keys.front();
        insert_into_parent(leaf, promote, new_leaf);
    }

    void split_internal(Node *node)
    {
        int total = (int)node->keys.size();
        int mid = total / 2;
        Key promote = node->keys[mid];

        Node *new_internal = new Node(false);

        new_internal->keys.assign(node->keys.begin() + mid + 1, node->keys.end());

        new_internal->children.assign(node->children.begin() + mid + 1, node->children.end());

        node->keys.erase(node->keys.begin() + mid, node->keys.end());

        node->children.erase(node->children.begin() + mid + 1, node->children.end());

        if ((int)node->keys.size() + 1 != (int)node->children.size())
        {
            cerr << "ERROR: Invariant violated in original node after split!" << endl;
            cerr << "Keys: " << node->keys.size() << ", Children: " << node->children.size() << endl;
            abort();
        }
        if ((int)new_internal->keys.size() + 1 != (int)new_internal->children.size())
        {
            cerr << "ERROR: Invariant violated in new node after split!" << endl;
            cerr << "Keys: " << new_internal->keys.size() << ", Children: " << new_internal->children.size() << endl;
            abort();
        }

        for (Node *c : new_internal->children)
            if (c)
                c->parent = new_internal;
        new_internal->parent = node->parent;

        insert_into_parent(node, promote, new_internal);
    }

    void insert_into_parent(Node *left_node, const Key &promoteKey, Node *right_node)
    {
        if (left_node->parent == nullptr)
        {
            Node *new_root = new Node(false);
            new_root->keys.push_back(promoteKey);
            new_root->children.push_back(left_node);
            new_root->children.push_back(right_node);
            left_node->parent = new_root;
            right_node->parent = new_root;
            root = new_root;
            return;
        }

        Node *parent = left_node->parent;
        int insert_pos = 0;
        while (insert_pos < (int)parent->children.size() && parent->children[insert_pos] != left_node)
        {
            insert_pos++;
        }

        if (insert_pos >= (int)parent->children.size())
        {
            cerr << "ERROR: Could not find child in parent during split!" << endl;
            return;
        }

        parent->keys.insert(parent->keys.begin() + insert_pos, promoteKey);
        parent->children.insert(parent->children.begin() + insert_pos + 1, right_node);
        right_node->parent = parent;

        if ((int)parent->keys.size() > MAX_KEYS)
        {
            split_internal(parent);
        }
    }

    void clear_rec(Node *n)
    {
        if (!n)
            return;
        if (!n->is_leaf)
        {
            for (Node *c : n->children)
                clear_rec(c);
        }
        delete n;
    }

    void clone_from(const mapp &other)
    {
        Node *prev_leaf = nullptr;
        root = clone_subtree(other.root, nullptr, prev_leaf);
    }

    Node *clone_subtree(Node *src, Node *parent, Node *&prev_leaf)
    {
        if (!src)
            return nullptr;
        Node *node = new Node(src->is_leaf);
        node->keys = src->keys;
        node->parent = parent;
        if (src->is_leaf)
        {
            node->values = src->values;
            if (prev_leaf)
                prev_leaf->next = node;
            prev_leaf = node;
        }
        else
        {
            node->children.reserve(src->children.size());
            for (Node *child : src->children)
            {
                Node *cloned_child = clone_subtree(child, node, prev_leaf);
                node->children.push_back(cloned_child);
            }
        }
        return node;
    }
};

bool isNumber(const string &s)
{
    if (s.empty())
        return false;
    for (char c : s)
        if (!isdigit((unsigned char)c))
            return false;
    return true;
}

struct Row
{
    mapp<string, string> data;
};

class Table
{
public:
    mapp<string, bool> columns;
    deque<Row> rows;
    mapp<string, mapp<int, vector<Row *>>> indexNum;
    mapp<string, mapp<string, vector<Row *>>> indexStr;

    void addColumn(const string &col)
    {
        if (!columns.count(col))
        {
            columns[col] = true;
            for (auto &r : rows)
                r.data[col] = "";
        }
    }

    void insertRow(const mapp<string, string> &input)
    {

        for (auto it = input.begin(); it != input.end(); ++it)
        {
            string colInInput = (*it).first;
            if (!columns.count(colInInput))
                addColumn(colInInput);
        }

        rows.push_back(Row());
        Row *rPtr = &rows.back();

        for (auto it = columns.begin(); it != columns.end(); ++it)
        {
            string col = (*it).first;
            auto found = input.find(col);
            if (found != input.end())
            {
                rPtr->data[col] = (*found).second;
            }
            else
            {
                rPtr->data[col] = "";
            }
        }

        for (auto it = rPtr->data.begin(); it != rPtr->data.end(); ++it)
        {
            string col = (*it).first;
            string val = (*it).second;

            if (!val.empty())
            {
                if (isNumber(val))
                {
                    int num = stoi(val);
                    indexNum[col][num].push_back(rPtr);
                }
                else
                {
                    indexStr[col][val].push_back(rPtr);
                }
            }
        }
    }

    vector<Row *> selectEqual(const string &col, const string &val)
    {
        if (!columns.count(col))
            return {};
        if (isNumber(val))
        {
            int x = stoi(val);
            auto it = indexNum[col].find(x);
            if (it != indexNum[col].end())
                return (*it).second;
            return {};
        }
        else
        {
            auto it = indexStr[col].find(val);
            if (it != indexStr[col].end())
                return (*it).second;
            return {};
        }
    }

    vector<Row *> selectGreater(const string &col, const string &val)
    {
        vector<Row *> out;
        if (!columns.count(col))
            return out;

        if (isNumber(val))
        {
            int x = stoi(val);
            auto start_it = indexNum[col].upper_bound(x);
            auto end_it = indexNum[col].end();
            for (auto it = start_it; it != end_it; ++it)
            {
                out.insert(out.end(), (*it).second.begin(), (*it).second.end());
            }
        }
        else
        {
            auto start_it = indexStr[col].upper_bound(val);
            auto end_it = indexStr[col].end();
            for (auto it = start_it; it != end_it; ++it)
            {
                out.insert(out.end(), (*it).second.begin(), (*it).second.end());
            }
        }
        return out;
    }

    vector<Row *> selectLess(const string &col, const string &val)
    {
        vector<Row *> out;
        if (!columns.count(col))
            return out;

        if (isNumber(val))
        {
            int x = stoi(val);
            auto start_it = indexNum[col].begin();
            auto end_it = indexNum[col].lower_bound(x);
            for (auto it = start_it; it != end_it; ++it)
            {
                out.insert(out.end(), (*it).second.begin(), (*it).second.end());
            }
        }
        else
        {
            auto start_it = indexStr[col].begin();
            auto end_it = indexStr[col].lower_bound(val);
            for (auto it = start_it; it != end_it; ++it)
            {
                out.insert(out.end(), (*it).second.begin(), (*it).second.end());
            }
        }
        return out;
    }

    vector<Row *> selectBetween(const string &col, const string &low, const string &high)
    {
        vector<Row *> out;
        if (!columns.count(col))
            return out;

        if (isNumber(low) && isNumber(high))
        {
            int L = stoi(low), H = stoi(high);
            auto start_it = indexNum[col].lower_bound(L);
            auto end_it = indexNum[col].upper_bound(H);
            for (auto it = start_it; it != end_it; ++it)
            {
                out.insert(out.end(), (*it).second.begin(), (*it).second.end());
            }
        }
        else
        {
            auto start_it = indexStr[col].lower_bound(low);
            auto end_it = indexStr[col].upper_bound(high);
            for (auto it = start_it; it != end_it; ++it)
            {
                out.insert(out.end(), (*it).second.begin(), (*it).second.end());
            }
        }
        return out;
    }

    void printTable()
    {
        for (auto it = columns.begin(); it != columns.end(); ++it)
            cout << (*it).first << "\t";
        cout << "\n";

        for (auto &rowRef : rows)
        {
            for (auto it = columns.begin(); it != columns.end(); ++it)
            {
                auto found = rowRef.data.find((*it).first);
                if (found != rowRef.data.end())
                {
                    cout << (*found).second << "\t";
                }
                else
                {
                    cout << "" << "\t";
                }
            }
            cout << "\n";
        }
    }
};

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    Table t;

    cout << "Starting program..." << endl;

    t.addColumn("id");
    t.addColumn("name");
    t.addColumn("age");
    t.addColumn("city");
    t.addColumn("salary");

    cout << "Columns added. Starting insertion..." << endl;

    for (int i = 1; i <= 100000; i++)
    {
        mapp<string, string> r;
        r["id"] = to_string(100 + i);
        r["name"] = "Person" + to_string(i);
        r["age"] = to_string(20 + (i % 10));
        r["city"] = (i % 3 == 0) ? "Delhi" : (i % 3 == 1) ? "Mumbai"
                                                          : "Pune";
        r["salary"] = to_string(50000 + (i * 1000));
        t.insertRow(r);

        if (i % 10 == 0)
        {
            cout << "Inserted " << i << " rows..." << endl;
        }
    }

    cout << "\n===== Insertion complete. Total rows: " << t.rows.size() << " =====\n"
         << endl;

    cout << "===== WHERE age = 25 =====\n";
    auto results = t.selectEqual("age", "25");
    cout << "Found " << results.size() << " rows:\n";
    for (auto rPtr : results)
    {
        auto nameIt = rPtr->data.find("name");
        auto ageIt = rPtr->data.find("age");
        if (nameIt != rPtr->data.end() && ageIt != rPtr->data.end())
        {
            cout << (*nameIt).second << " (age: " << (*ageIt).second << ")\n";
        }
    }

    cout << "\n===== WHERE salary > 100000 =====\n";
    results = t.selectGreater("salary", "100000");
    cout << "Found " << results.size() << " rows (showing first 10):\n";
    for (int i = 0; i < min(10, (int)results.size()); i++)
    {
        auto nameIt = results[i]->data.find("name");
        auto salIt = results[i]->data.find("salary");
        if (nameIt != results[i]->data.end() && salIt != results[i]->data.end())
        {
            cout << (*nameIt).second << " - $" << (*salIt).second << "\n";
        }
    }

    cout << "\n===== WHERE age BETWEEN 22 AND 25 =====\n";
    results = t.selectBetween("age", "22", "25");
    cout << "Found " << results.size() << " rows (showing first 10):\n";
    for (int i = 0; i < min(10, (int)results.size()); i++)
    {
        auto nameIt = results[i]->data.find("name");
        auto ageIt = results[i]->data.find("age");
        if (nameIt != results[i]->data.end() && ageIt != results[i]->data.end())
        {
            cout << (*nameIt).second << " (age: " << (*ageIt).second << ")\n";
        }
    }

    cout << "\n===== WHERE city = Delhi =====\n";
    results = t.selectEqual("city", "Delhi");
    cout << "Found " << results.size() << " rows (showing first 5):\n";
    for (int i = 0; i < min(5, (int)results.size()); i++)
    {
        auto nameIt = results[i]->data.find("name");
        auto cityIt = results[i]->data.find("city");
        if (nameIt != results[i]->data.end() && cityIt != results[i]->data.end())
        {
            cout << (*nameIt).second << " (" << (*cityIt).second << ")\n";
        }
    }

    cout << "\nProgram completed successfully!" << endl;

    return 0;
}
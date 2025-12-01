#include <bits/stdc++.h>
using namespace std;

template <typename Key, typename T, typename Compare = std::less<Key>>
class mapp
{
    struct Node
    {
        Key key;
        T value;
        Node *left = nullptr;
        Node *right = nullptr;
        Node *parent = nullptr;
        Node *next = nullptr;
        int height = 1;
        Node(const Key &k, const T &v = T()) : key(k), value(v) {}
    };

    Node *root = nullptr;
    size_t node_count = 0;

public:
    struct iterator
    {
        Node *node;
        int idx;
        iterator(Node *n = nullptr, int i = 0) : node(n), idx(i) {}

        struct reference
        {
            Key first;
            T &second;
            reference(const Key &f, T &s) : first(f), second(s) {}
        };

        reference operator*() const
        {
            return reference(node->key, node->value);
        }

        bool operator==(const iterator &o) const { return node == o.node; }
        bool operator!=(const iterator &o) const { return node != o.node; }

        iterator &operator++()
        {
            if (!node)
                return *this;
            node = node->next;
            idx = 0;
            return *this;
        }
    };

    using const_iterator = iterator;

    mapp() = default;
    ~mapp() { clear(); }

    bool empty() const { return node_count == 0; }
    size_t size() const { return node_count; }

    void clear()
    {
        clear_rec(root);
        root = nullptr;
        node_count = 0;
    }

    T &operator[](const Key &k)
    {
        Node *created = nullptr;
        root = insert_rec(root, k, created);
        if (root)
            root->parent = nullptr;

        if (created)
        {

            Node *n = find_node(root, k);

            Node *pred = nullptr;
            Node *cur = root;
            while (cur)
            {
                if (Compare()(cur->key, k))
                {
                    pred = cur;
                    cur = cur->right;
                }
                else
                {
                    cur = cur->left;
                }
            }
            Node *succ = nullptr;
            cur = root;
            while (cur)
            {
                if (Compare()(k, cur->key))
                {
                    succ = cur;
                    cur = cur->left;
                }
                else
                {
                    cur = cur->right;
                }
            }

            if (pred)
                pred->next = n;
            n->next = succ;
            node_count++;
            return n->value;
        }
        else
        {
            Node *n = find_node(root, k);
            return n->value;
        }
    }

    size_t count(const Key &k) const
    {
        return find_node_const(root, k) ? 1 : 0;
    }

    iterator find(const Key &k)
    {
        Node *n = find_node(root, k);
        if (!n)
            return iterator(nullptr, 0);
        return iterator(n, 0);
    }

    const_iterator find(const Key &k) const
    {
        Node *n = find_node_const(root, k);
        if (!n)
            return const_iterator(nullptr, 0);
        return const_iterator(n, 0);
    }

    iterator lower_bound(const Key &k)
    {
        Node *n = lower_bound_node(root, k);
        if (!n)
            return iterator(nullptr, 0);
        return iterator(n, 0);
    }

    iterator upper_bound(const Key &k)
    {
        Node *n = upper_bound_node(root, k);
        if (!n)
            return iterator(nullptr, 0);
        return iterator(n, 0);
    }

    iterator begin()
    {
        Node *n = root;
        if (!n)
            return iterator(nullptr, 0);
        while (n->left)
            n = n->left;
        return iterator(n, 0);
    }
    iterator end() { return iterator(nullptr, 0); }

    const_iterator begin() const
    {
        Node *n = root;
        if (!n)
            return const_iterator(nullptr, 0);
        while (n->left)
            n = n->left;
        return const_iterator(n, 0);
    }
    const_iterator end() const { return const_iterator(nullptr, 0); }

private:
    void clear_rec(Node *n)
    {
        if (!n)
            return;
        clear_rec(n->left);
        clear_rec(n->right);
        delete n;
    }

    int height(Node *n) const { return n ? n->height : 0; }
    void update_height(Node *n)
    {
        if (n)
            n->height = 1 + max(height(n->left), height(n->right));
    }
    int balance_factor(Node *n) const { return n ? height(n->left) - height(n->right) : 0; }

    Node *rotate_right(Node *y)
    {
        Node *x = y->left;
        Node *T2 = x->right;

        x->right = y;
        y->left = T2;

        x->parent = y->parent;
        y->parent = x;
        if (T2)
            T2->parent = y;

        update_height(y);
        update_height(x);
        return x;
    }

    Node *rotate_left(Node *x)
    {
        Node *y = x->right;
        Node *T2 = y->left;

        y->left = x;
        x->right = T2;

        y->parent = x->parent;
        x->parent = y;
        if (T2)
            T2->parent = x;

        update_height(x);
        update_height(y);
        return y;
    }

    Node *rebalance(Node *node)
    {
        update_height(node);
        int bf = balance_factor(node);

        if (bf > 1)
        {
            if (balance_factor(node->left) < 0)
            {
                node->left = rotate_left(node->left);
                if (node->left)
                    node->left->parent = node;
            }
            Node *newroot = rotate_right(node);
            return newroot;
        }
        if (bf < -1)
        {
            if (balance_factor(node->right) > 0)
            {
                node->right = rotate_right(node->right);
                if (node->right)
                    node->right->parent = node;
            }
            Node *newroot = rotate_left(node);
            return newroot;
        }
        return node;
    }

    Node *insert_rec(Node *node, const Key &k, Node *&created_out)
    {
        if (!node)
        {
            created_out = new Node(k, T());
            return created_out;
        }

        if (Compare()(k, node->key))
        {
            node->left = insert_rec(node->left, k, created_out);
            if (node->left)
                node->left->parent = node;
        }
        else if (Compare()(node->key, k))
        {
            node->right = insert_rec(node->right, k, created_out);
            if (node->right)
                node->right->parent = node;
        }
        else
        {

            created_out = nullptr;
            return node;
        }

        Node *newroot = rebalance(node);
        if (newroot->left)
            newroot->left->parent = newroot;
        if (newroot->right)
            newroot->right->parent = newroot;
        return newroot;
    }

    Node *find_node(Node *n, const Key &k)
    {
        while (n)
        {
            if (Compare()(k, n->key))
                n = n->left;
            else if (Compare()(n->key, k))
                n = n->right;
            else
                return n;
        }
        return nullptr;
    }

    Node *find_node_const(Node *n, const Key &k) const
    {
        while (n)
        {
            if (Compare()(k, n->key))
                n = n->left;
            else if (Compare()(n->key, k))
                n = n->right;
            else
                return n;
        }
        return nullptr;
    }

    Node *lower_bound_node(Node *n, const Key &k)
    {
        Node *res = nullptr;
        while (n)
        {
            if (!Compare()(n->key, k))
            {
                res = n;
                n = n->left;
            }
            else
                n = n->right;
        }
        return res;
    }

    Node *upper_bound_node(Node *n, const Key &k)
    {
        Node *res = nullptr;
        while (n)
        {
            if (Compare()(k, n->key))
            {
                res = n;
                n = n->left;
            }
            else
                n = n->right;
        }
        return res;
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
            for (auto it = indexNum[col].upper_bound(x); it != indexNum[col].end(); ++it)
                out.insert(out.end(), (*it).second.begin(), (*it).second.end());
        }
        else
        {
            for (auto it = indexStr[col].upper_bound(val); it != indexStr[col].end(); ++it)
                out.insert(out.end(), (*it).second.begin(), (*it).second.end());
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
            for (auto it = indexNum[col].begin(); it != indexNum[col].lower_bound(x); ++it)
                out.insert(out.end(), (*it).second.begin(), (*it).second.end());
        }
        else
        {
            for (auto it = indexStr[col].begin(); it != indexStr[col].lower_bound(val); ++it)
                out.insert(out.end(), (*it).second.begin(), (*it).second.end());
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
            for (auto it = indexNum[col].lower_bound(L); it != indexNum[col].upper_bound(H); ++it)
                out.insert(out.end(), (*it).second.begin(), (*it).second.end());
        }
        else
        {
            for (auto it = indexStr[col].lower_bound(low); it != indexStr[col].upper_bound(high); ++it)
                out.insert(out.end(), (*it).second.begin(), (*it).second.end());
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
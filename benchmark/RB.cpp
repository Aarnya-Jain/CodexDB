
#include <bits/stdc++.h>
using namespace std;

template <typename Key, typename T, typename Compare = std::less<Key>>
class mapp
{
    enum Color
    {
        RED,
        BLACK
    };

    struct Node
    {
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
    mapp() : root(nullptr), cnt(0), comp(Compare()) {}
    ~mapp() { clear(); }

    struct iterator
    {
        Node *n;

        iterator(Node *p = nullptr) : n(p) {}

        struct proxy
        {
            const Key &first;
            T &second;
            proxy(const Key &f, T &s) : first(f), second(s) {}
        };

        proxy operator*() const { return proxy(n->kv.first, n->kv.second); }
        proxy operator->() const { return proxy(n->kv.first, n->kv.second); }

        bool operator==(const iterator &o) const { return n == o.n; }
        bool operator!=(const iterator &o) const { return n != o.n; }

        iterator &operator++()
        {
            if (!n)
                return *this;

            if (n->right)
            {
                n = n->right;
                while (n->left)
                    n = n->left;
            }
            else
            {
                Node *p = n->parent;
                while (p && n == p->right)
                {
                    n = p;
                    p = p->parent;
                }
                n = p;
            }
            return *this;
        }
    };

    struct const_iterator
    {
        const Node *n;

        const_iterator(const Node *p = nullptr) : n(p) {}

        struct proxy
        {
            const Key &first;
            const T &second;
            proxy(const Key &f, const T &s) : first(f), second(s) {}
        };

        proxy operator*() const { return proxy(n->kv.first, n->kv.second); }
        proxy operator->() const { return proxy(n->kv.first, n->kv.second); }

        bool operator==(const const_iterator &o) const { return n == o.n; }
        bool operator!=(const const_iterator &o) const { return n != o.n; }

        const_iterator &operator++()
        {
            if (!n)
                return *this;

            if (n->right)
            {
                n = n->right;
                while (n->left)
                    n = n->left;
            }
            else
            {
                const Node *p = n->parent;
                while (p && n == p->right)
                {
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

    void clear()
    {
        clear_rec(root);
        root = nullptr;
        cnt = 0;
    }

    T &operator[](const Key &k)
    {
        Node *x = root, *p = nullptr;
        while (x)
        {
            p = x;
            if (!comp(k, x->kv.first) && !comp(x->kv.first, k))
            {
                return x->kv.second;
            }
            if (comp(k, x->kv.first))
                x = x->left;
            else
                x = x->right;
        }
        Node *z = new Node(k, T(), RED);
        z->parent = p;
        if (!p)
            root = z;
        else if (comp(z->kv.first, p->kv.first))
            p->left = z;
        else
            p->right = z;
        ++cnt;
        insert_fixup(z);
        return z->kv.second;
    }

    size_t count(const Key &k) const
    {
        return findNode(k) ? 1 : 0;
    }

    iterator find(const Key &k)
    {
        Node *n = findNode(k);
        return iterator(n);
    }
    const_iterator find(const Key &k) const
    {
        const Node *n = findNode(k);
        return const_iterator(n);
    }

    iterator lower_bound(const Key &k)
    {
        Node *x = root, *res = nullptr;
        while (x)
        {
            if (!comp(x->kv.first, k))
            {
                res = x;
                x = x->left;
            }
            else
                x = x->right;
        }
        return iterator(res);
    }

    iterator upper_bound(const Key &k)
    {
        Node *x = root, *res = nullptr;
        while (x)
        {
            if (comp(k, x->kv.first))
            {
                res = x;
                x = x->left;
            }
            else
                x = x->right;
        }
        return iterator(res);
    }

    iterator begin()
    {
        Node *x = root;
        if (!x)
            return iterator(nullptr);
        while (x->left)
            x = x->left;
        return iterator(x);
    }
    iterator end() { return iterator(nullptr); }

    const_iterator begin() const
    {
        const Node *x = root;
        if (!x)
            return const_iterator(nullptr);
        while (x->left)
            x = x->left;
        return const_iterator(x);
    }
    const_iterator end() const { return const_iterator(nullptr); }

private:
    Node *findNode(const Key &k) const
    {
        Node *x = root;
        while (x)
        {
            if (!comp(k, x->kv.first) && !comp(x->kv.first, k))
                return x;
            if (comp(k, x->kv.first))
                x = x->left;
            else
                x = x->right;
        }
        return nullptr;
    }

    void clear_rec(Node *n)
    {
        if (!n)
            return;
        clear_rec(n->left);
        clear_rec(n->right);
        delete n;
    }

    void rotate_left(Node *x)
    {
        Node *y = x->right;
        x->right = y->left;
        if (y->left)
            y->left->parent = x;
        y->parent = x->parent;
        if (!x->parent)
            root = y;
        else if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;
        y->left = x;
        x->parent = y;
    }

    void rotate_right(Node *x)
    {
        Node *y = x->left;
        x->left = y->right;
        if (y->right)
            y->right->parent = x;
        y->parent = x->parent;
        if (!x->parent)
            root = y;
        else if (x == x->parent->right)
            x->parent->right = y;
        else
            x->parent->left = y;
        y->right = x;
        x->parent = y;
    }

    void insert_fixup(Node *z)
    {
        while (z->parent && z->parent->color == RED)
        {
            Node *gp = z->parent->parent;
            if (!gp)
                break;
            if (z->parent == gp->left)
            {
                Node *y = gp->right;
                if (y && y->color == RED)
                {
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    gp->color = RED;
                    z = gp;
                }
                else
                {
                    if (z == z->parent->right)
                    {
                        z = z->parent;
                        rotate_left(z);
                    }
                    z->parent->color = BLACK;
                    gp->color = RED;
                    rotate_right(gp);
                }
            }
            else
            {
                Node *y = gp->left;
                if (y && y->color == RED)
                {
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    gp->color = RED;
                    z = gp;
                }
                else
                {
                    if (z == z->parent->left)
                    {
                        z = z->parent;
                        rotate_right(z);
                    }
                    z->parent->color = BLACK;
                    gp->color = RED;
                    rotate_left(gp);
                }
            }
        }
        if (root)
            root->color = BLACK;
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
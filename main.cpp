#include <iostream> 
#include <vector>
#include <fstream>

using std::vector;
using std::ofstream;
using std::ifstream;

using std::cout;
using std::endl;

template<typename K, typename V>
class Node {
public:
    Node() {}
    Node(K k, V v, int level) : key(k), value(v), node_level(level), forward(level + 1) {}
    ~Node() {}

    K get_key() const { return key; }
    V get_value() const { return value; }
    void set_value(V v) { value = v; }

    vector<Node<K, V>*> forward;
    int node_level;
private:
    K key;
    V value;
};


template<typename K, typename V>
class SkipList {
public:
    SkipList(int);                      // 构造函数
    ~SkipList();                        // 析构函数
    int get_random_level();             // 生成节点的随机层级
    Node<K, V>* create_node(K, V, int); // 节点创建
    int insert_element(K, V);           // 插入节点
    void display_list();                // 展示节点
    bool search_element(K);             // 搜索节点
    void delete_element(K);             // 删除节点
    void dump_file();                   // 持久化数据到文件
    void load_file();                   // 从文件加载数据
    void clear(Node<K, V>*);            // 递归删除节点
    int size();                         // 跳表中的节点个数
private:
    int _max_level;              // 跳表允许的最大层数
    int _skip_list_level;        // 跳表当前的层数(当前最高层是第几层)
    Node<K, V>* _header;         // 跳表的头节点
    int _element_count;          // 跳表中组织的所有节点的数量
    ofstream _file_writer;       // 文件写入器
    ifstream _file_reader;       // 文件读取器
};


template<typename K, typename V>
SkipList<K, V>::SkipList(int max_level) : _max_level(max_level), _skip_list_level(0), _element_count(0) {
    K k{};  // 默认键
    V v{};  // 默认值
    // 创建头节点，并初始化键值为默认值
    _header = new Node<K, V>(k, v, _max_level);
};

template<typename K, typename V>
int SkipList<K, V>::get_random_level() {
    int level = 1;       // 初始化层级：每个节点至少出现在第一层
    // 随机层级增加：使用 rand() % 2 实现抛硬币效果，决定是否升层（连续奇数连续升层）
    while (rand() % 2) {
        level++;
    }
    level = std::min(level, _max_level);   // 层级限制：确保节点层级不超过最大值 _max_level
    return level;         // 返回层级：返回确定的层级值，决定节点插入的层
};

template<typename K, typename V>
Node<K, V>* SkipList<K, V>::create_node(const K k, const V v, int level) {
    Node<K, V>* n = new Node<K, V>(k, v, level);       // 实例化新节点，并为其分配指定的键、值和层级
    return n; // 返回新创建的节点
}

template<typename K, typename V>
bool SkipList<K, V>::search_element(K key) {
    Node<K, V>* current = _header;                           // 定义一个指针 current，初始化为跳表的头节点 _header

    for (int i = _skip_list_level; i >= 0; i--) {            // 从跳表的最高层开始搜索
        while (current->forward[i] && current->forward[i]->get_key() < key) {         // 遍历当前层级，直到下一个节点的键值大于或等于待查找的键值
            current = current->forward[i];          // 移动到当前层的下一个节点
        }
        // 当前节点的下一个节点的键值大于待查找的键值时，进行下沉到下一层
        // 下沉操作通过循环的 i-- 实现
    }
    // 检查当前层（最底层）的下一个节点的键值是否为待查找的键值
    current = current->forward[0];
    if (current && current->get_key() == key) {
        // 如果找到匹配的键值，返回 true
        // cout << "Found key: " << key << ", value: " << current->get_value() << endl;
        return true;
    }
    // 如果没有找到匹配的键值，返回 false
    // cout << "Not Found Key: " << key << endl;
    return false;
}

template <typename K, typename V>
int SkipList<K, V>::insert_element(K key, V value) {
    Node<K, V>* current = _header;
    // 用于在各层更新指针的数组
    vector<Node<K, V>*> update(_max_level + 1, 0);     // 用于记录每层中待更新指针的节点

    // 从最高层向下搜索插入位置
    for (int i = _skip_list_level; i >= 0; i--) {
        // 寻找当前层中最接近且小于 key 的节点
        while (current->forward[i] != NULL && current->forward[i]->get_key() < key) {
            current = current->forward[i]; // 移动到下一节点
        }
        // 保存每层中该节点，以便后续插入时更新指针
        update[i] = current;
    }

    // 移动到最底层的下一节点，准备插入操作
    current = current->forward[0];
    // 检查待插入的节点的键是否已存在
    if (current != NULL && current->get_key() == key) {
        return 1;         // 键已存在，取消插入
    }
    // 检查待插入的节点是否已存在于跳表中
    if (current == NULL || current->get_key() != key) {
        // 通过随机函数决定新节点的层级高度
        int random_level = get_random_level();
        // 如果新节点的层级超出了跳表的当前最高层级
        if (random_level > _skip_list_level) {
            // 对所有新的更高层级，将头节点设置为它们的前驱节点
            for (int i = _skip_list_level + 1; i <= random_level; i++) {
                update[i] = _header;
            }
            // 更新跳表的当前最高层级为新节点的层级
            _skip_list_level = random_level;
        }

        Node<K, V>* inserted_node = create_node(key, value, random_level);
        // 在各层插入新节点，同时更新前驱节点的forward指针
        for (int i = 0; i <= random_level; i++) {
            // 新节点指向当前节点的下一个节点
            inserted_node->forward[i] = update[i]->forward[i];
            // 当前节点的下一个节点更新为新节点
            update[i]->forward[i] = inserted_node;
        }
        _element_count++;
    }
    return 0;
}



int main() {
    int N;
    int M;
    SkipList<int, int>* skip_list = new SkipList<int, int>(16);
    std::cin >> N >> M;
    for (int i = 0; i < N; i++) {
        int key;
        int value;
        std::cin >> key >> value;
        if (skip_list->insert_element(key, value) == 0) {
            std::cout << "Insert Success" << std::endl;
        }
        else {
            std::cout << "Insert Failed" << std::endl;
        }
    }

    // 搜索
    for (int i = 0; i < M; i++) {
        int key;
        std::cin >> key;
        if (skip_list->search_element(key)) {
            std::cout << "Search Success" << std::endl;
        }
        else {
            std::cout << "Search Failed" << std::endl;
        }
    }
    return 0;
}
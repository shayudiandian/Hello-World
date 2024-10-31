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
    SkipList(int);                      // ���캯��
    ~SkipList();                        // ��������
    int get_random_level();             // ���ɽڵ������㼶
    Node<K, V>* create_node(K, V, int); // �ڵ㴴��
    int insert_element(K, V);           // ����ڵ�
    void display_list();                // չʾ�ڵ�
    bool search_element(K);             // �����ڵ�
    void delete_element(K);             // ɾ���ڵ�
    void dump_file();                   // �־û����ݵ��ļ�
    void load_file();                   // ���ļ���������
    void clear(Node<K, V>*);            // �ݹ�ɾ���ڵ�
    int size();                         // �����еĽڵ����
private:
    int _max_level;              // ���������������
    int _skip_list_level;        // ����ǰ�Ĳ���(��ǰ��߲��ǵڼ���)
    Node<K, V>* _header;         // �����ͷ�ڵ�
    int _element_count;          // ��������֯�����нڵ������
    ofstream _file_writer;       // �ļ�д����
    ifstream _file_reader;       // �ļ���ȡ��
};


template<typename K, typename V>
SkipList<K, V>::SkipList(int max_level) : _max_level(max_level), _skip_list_level(0), _element_count(0) {
    K k{};  // Ĭ�ϼ�
    V v{};  // Ĭ��ֵ
    // ����ͷ�ڵ㣬����ʼ����ֵΪĬ��ֵ
    _header = new Node<K, V>(k, v, _max_level);
};

template<typename K, typename V>
int SkipList<K, V>::get_random_level() {
    int level = 1;       // ��ʼ���㼶��ÿ���ڵ����ٳ����ڵ�һ��
    // ����㼶���ӣ�ʹ�� rand() % 2 ʵ����Ӳ��Ч���������Ƿ����㣨���������������㣩
    while (rand() % 2) {
        level++;
    }
    level = std::min(level, _max_level);   // �㼶���ƣ�ȷ���ڵ�㼶���������ֵ _max_level
    return level;         // ���ز㼶������ȷ���Ĳ㼶ֵ�������ڵ����Ĳ�
};

template<typename K, typename V>
Node<K, V>* SkipList<K, V>::create_node(const K k, const V v, int level) {
    Node<K, V>* n = new Node<K, V>(k, v, level);       // ʵ�����½ڵ㣬��Ϊ�����ָ���ļ���ֵ�Ͳ㼶
    return n; // �����´����Ľڵ�
}

template<typename K, typename V>
bool SkipList<K, V>::search_element(K key) {
    Node<K, V>* current = _header;                           // ����һ��ָ�� current����ʼ��Ϊ�����ͷ�ڵ� _header

    for (int i = _skip_list_level; i >= 0; i--) {            // ���������߲㿪ʼ����
        while (current->forward[i] && current->forward[i]->get_key() < key) {         // ������ǰ�㼶��ֱ����һ���ڵ�ļ�ֵ���ڻ���ڴ����ҵļ�ֵ
            current = current->forward[i];          // �ƶ�����ǰ�����һ���ڵ�
        }
        // ��ǰ�ڵ����һ���ڵ�ļ�ֵ���ڴ����ҵļ�ֵʱ�������³�����һ��
        // �³�����ͨ��ѭ���� i-- ʵ��
    }
    // ��鵱ǰ�㣨��ײ㣩����һ���ڵ�ļ�ֵ�Ƿ�Ϊ�����ҵļ�ֵ
    current = current->forward[0];
    if (current && current->get_key() == key) {
        // ����ҵ�ƥ��ļ�ֵ������ true
        // cout << "Found key: " << key << ", value: " << current->get_value() << endl;
        return true;
    }
    // ���û���ҵ�ƥ��ļ�ֵ������ false
    // cout << "Not Found Key: " << key << endl;
    return false;
}

template <typename K, typename V>
int SkipList<K, V>::insert_element(K key, V value) {
    Node<K, V>* current = _header;
    // �����ڸ������ָ�������
    vector<Node<K, V>*> update(_max_level + 1, 0);     // ���ڼ�¼ÿ���д�����ָ��Ľڵ�

    // ����߲�������������λ��
    for (int i = _skip_list_level; i >= 0; i--) {
        // Ѱ�ҵ�ǰ������ӽ���С�� key �Ľڵ�
        while (current->forward[i] != NULL && current->forward[i]->get_key() < key) {
            current = current->forward[i]; // �ƶ�����һ�ڵ�
        }
        // ����ÿ���иýڵ㣬�Ա��������ʱ����ָ��
        update[i] = current;
    }

    // �ƶ�����ײ����һ�ڵ㣬׼���������
    current = current->forward[0];
    // ��������Ľڵ�ļ��Ƿ��Ѵ���
    if (current != NULL && current->get_key() == key) {
        return 1;         // ���Ѵ��ڣ�ȡ������
    }
    // ��������Ľڵ��Ƿ��Ѵ�����������
    if (current == NULL || current->get_key() != key) {
        // ͨ��������������½ڵ�Ĳ㼶�߶�
        int random_level = get_random_level();
        // ����½ڵ�Ĳ㼶����������ĵ�ǰ��߲㼶
        if (random_level > _skip_list_level) {
            // �������µĸ��߲㼶����ͷ�ڵ�����Ϊ���ǵ�ǰ���ڵ�
            for (int i = _skip_list_level + 1; i <= random_level; i++) {
                update[i] = _header;
            }
            // ��������ĵ�ǰ��߲㼶Ϊ�½ڵ�Ĳ㼶
            _skip_list_level = random_level;
        }

        Node<K, V>* inserted_node = create_node(key, value, random_level);
        // �ڸ�������½ڵ㣬ͬʱ����ǰ���ڵ��forwardָ��
        for (int i = 0; i <= random_level; i++) {
            // �½ڵ�ָ��ǰ�ڵ����һ���ڵ�
            inserted_node->forward[i] = update[i]->forward[i];
            // ��ǰ�ڵ����һ���ڵ����Ϊ�½ڵ�
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

    // ����
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
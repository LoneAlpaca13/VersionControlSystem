#include <iostream>
#include <vector>
#include <string>
#include <ctime>
using namespace std;

template<typename T>
void mySwap(T &a, T &b) {
    T temp = a;
    a = b;
    b = temp;
};

unsigned long fnv1a(string &s) { 
    unsigned long FNV_prime = 1099511628211u;
    unsigned long hash = 1469598103934665603u;
    for (char c : s)
        hash = (hash ^ c) * FNV_prime;
    return hash;
};

template<typename V>
class HashMap {
private:
    int capacity;
    vector<V> Arr;
    int currsize;

public:
    HashMap() {
        capacity = 1e5; 
        Arr.resize(capacity, V());  
        currsize = 0;
    }

    void add(int key, V val) {
        Arr[key % capacity] = val;  
        currsize++;
    }

    void remove(int key) {
        Arr[key % capacity] = V();
        currsize--;
    }

    V lookup(int key) {
        return Arr[key % capacity];
    }

    bool contains(int key) {
        return !(Arr[key % capacity] == V());
    }
};

struct TreeNode{
    int version_id ;
    string content;
    string message; 
    time_t created_timestamp ;
    time_t snapshot_timestamp ; 
    bool snapshot;
    TreeNode* parent;
    vector < TreeNode*> children ;
};

struct File{
    TreeNode* root; 
    TreeNode* active_version ;
    HashMap<TreeNode*> version_map;
    int total_versions ;
    time_t last_updated;
    string name;
};

HashMap<File*> myMap;
vector<File*>pointer;

void heapifyRecent(vector<File*>& arr, int n, int i) {
    int largest = i;
    int left = 2*i + 1, right = 2*i + 2;

    if (left < n && arr[left]->last_updated > arr[largest]->last_updated)
        largest = left;
    if (right < n && arr[right]->last_updated > arr[largest]->last_updated)
        largest = right;

    if (largest != i) {
        mySwap(arr[i], arr[largest]);
        heapifyRecent(arr, n, largest);
    }
};

void heapifyBiggest(vector<File*>& arr, int n, int i) {
    int largest = i;
    int left = 2*i + 1, right = 2*i + 2;

    if (left < n && arr[left]->total_versions > arr[largest]->total_versions)
        largest = left;
    if (right < n && arr[right]->total_versions > arr[largest]->total_versions)
        largest = right;

    if (largest != i) {
        mySwap(arr[i], arr[largest]);
        heapifyBiggest(arr, n, largest);
    }
};

void RECENT(int k) {
    if (pointer.empty()) {
        cout << "No files exist." << endl;
        return;
    }

    vector<File*> arr = pointer;
    int n = arr.size();

    if (n < k) {
        cout << "Num greater than no of files";
        return;
    }

    for (int i = n/2 - 1; i >= 0; i--)
        heapifyRecent(arr, n, i);

    cout << "Top " << k << " recent files:" << endl;

    for (int i = 0; i < k; i++) {
        File* f = arr[0];
        cout << "File: " << f->name
             << " | Last updated: " << string(ctime(&(f->last_updated))).substr(0,24)
             << " | Versions: " << f->total_versions << endl;

        mySwap(arr[0], arr[n-1-i]);
        heapifyRecent(arr, n-1-i, 0);
    }
}

void BIGGEST(int k) {
    if (pointer.empty()) {
        cout << "No files exist." << endl;
        return;
    }

    vector<File*> arr = pointer;
    int n = arr.size();

    if (n < k) {
        cout << "Num greater than no of files";
        return;
    }

    for (int i = n/2 - 1; i >= 0; i--)
        heapifyBiggest(arr, n, i);

    cout << "Top " << k << " biggest trees:" << endl;

    for (int i = 0; i < k; i++) {
        File* f = arr[0];
        cout << "File: " << f->name
             << " | Total versions: " << f->total_versions
             << " | Last updated: " << string(ctime(&(f->last_updated))).substr(0,24)
             << endl;

        mySwap(arr[0], arr[n-1-i]);
        heapifyBiggest(arr, n-1-i, 0);
    }
}

void CREATE(string filename){
    int key = fnv1a(filename);

    if (myMap.contains(key)){
        cout << "Filename already exists" << endl;
        return;
    }

    TreeNode* root = new TreeNode();
    root->version_id = 0;
    root->content = "";
    root->message = "Initial snapshot";
    root->created_timestamp = time(nullptr);
    root->snapshot_timestamp = time(nullptr);
    root->snapshot = true;
    root->parent = nullptr;

    File* file = new File();
    file->root = root;
    file->active_version = root;
    file->version_map.add(0, root);
    file->total_versions = 1;
    file->last_updated = time(nullptr);
    pointer.push_back(file);
    file->name = filename;

    myMap.add(key, file);
    cout << "Command executed" << endl;
}

void READ(string filename){
    int key = fnv1a(filename);

    if (!myMap.contains(key)){
        cout << "File does not exist" << endl;
        return;
    }

    File* file = myMap.lookup(key);
    cout << file->active_version->content << endl;
}

void SNAPSHOT(string filename, string message){
    int key = fnv1a(filename);

    if (!myMap.contains(key)){
        cout << "File does not exist" << endl;
        return;
    }

    File* file = myMap.lookup(key);
    TreeNode* active = file->active_version;
    file->last_updated = time(nullptr);

    active->message = message;
    active->snapshot = true;
    active->snapshot_timestamp = time(nullptr);

    cout << "Command executed" << endl;
}

void INSERT(string filename, string content){
    int key = fnv1a(filename);

    if (!myMap.contains(key)){
        cout << "File does not exist" << endl;
        return;
    }

    File* file = myMap.lookup(key);
    TreeNode* active = file->active_version;
    file->last_updated = time(nullptr);

    if (!active->snapshot){
        active->content += content;
    } else {
        int new_id = file->total_versions;
        TreeNode* node = new TreeNode();
        node->version_id = new_id;
        node->content = active->content + content;
        node->message = "";
        node->created_timestamp = time(nullptr);
        node->parent = active;

        active->children.push_back(node);

        file->active_version = node;
        file->version_map.add(new_id, node);
        file->total_versions++;
    }

    cout << "Command executed" << endl;
}

void UPDATE(string filename, string content){
    int key = fnv1a(filename);

    if (!myMap.contains(key)){
        cout << "Error: File does not exist" << endl;
        return;
    }

    File* file = myMap.lookup(key);
    TreeNode* active = file->active_version;
    file->last_updated = time(nullptr);

    if (content == "RESET") {
        content = ""; 
    }

    if (!active->snapshot){
        active->content = content;
        active->created_timestamp = time(nullptr);
    } else {
        int new_id = file->total_versions;
        TreeNode* node = new TreeNode();
        node->version_id = new_id;
        node->content = content;
        node->message = "";
        node->created_timestamp = time(nullptr);
        node->parent = active;

        active->children.push_back(node);

        file->active_version = node;
        file->version_map.add(new_id, node);
        file->total_versions++;
    }

    cout << "Command executed" << endl;
}

void ROLLBACK(string filename, int versionID = -1){
    int key = fnv1a(filename);

    if (!myMap.contains(key)){
        cout << "Error: File does not exist" << endl;
        return;
    }

    File* file = myMap.lookup(key);
    file->last_updated = time(nullptr);

    if (versionID == -1){
        TreeNode* cur = file->active_version;
        if (cur->parent == nullptr){
            cout << "Error: Already at root version, cannot rollback further." << endl;
            return;
        }
        file->active_version = cur->parent;
        cout << "Rolled back to parent version." << endl;
    } else {
        if (!file->version_map.contains(versionID)){
            cout << "Error: Version " << versionID
                 << " does not exist in file '" << filename << "'." << endl;
            return;
        }
        file->active_version = file->version_map.lookup(versionID);
        cout << "Active version set to " << versionID << "." << endl;
    }
}

void HISTORY(string filename){
    int key = fnv1a(filename);

    if (!myMap.contains(key)){
        cout << "Error: File does not exist" << endl;
        return;
    }

    File* file = myMap.lookup(key);
    TreeNode* point = file->active_version;
    while (point != nullptr){
        cout << "Version " << point->version_id
             << " | Time: " << string(ctime(&(point->snapshot_timestamp))).substr(0,24)
             << " | Message: " << (point->message.empty() ? "(none)" : point->message)
             << endl;
        point = point->parent;
    }
}

int main(){
    while(1){
        string input,filename;
        cin >> input;
        if (input == "RECENT_FILES") {
            int k; cin >> k;
            RECENT(k);
        }
        else if (input == "BIGGEST_FILES") {
            int k; cin >> k;
            BIGGEST(k);
        }
        else{
            cin >> filename;
            if (input == "CREATE"){
                CREATE(filename);
            }
            else if(input == "READ"){
                READ(filename);
            }
            else if (input == "SNAPSHOT"){
                string message;
                cin >> message;
                SNAPSHOT(filename, message);
            }
            else if (input == "INSERT"){
                string content;
                cin >> content;
                INSERT(filename, content);
            }
            else if (input == "UPDATE"){
                string content;
                cin >> content;
                UPDATE(filename, content);
            }
            else if (input == "ROLLBACK"){
                int versionID;
                if (cin.peek() == '\n'){  
                    ROLLBACK(filename);
                }
                else if (cin >> versionID){
                    ROLLBACK(filename, versionID);
                }
                else {
                    cout << "Error: Invalid ROLLBACK command." << endl;
                }
            }
            else if (input == "HISTORY"){
                HISTORY(filename);
            }
        }
    }
};

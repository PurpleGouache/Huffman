#include <iostream> 
#include <fstream>
#include <vector>
#include <map>
#include <list>

using namespace std;

class Node{
	
public:
	int key;
	char s;
	Node *left, *right;
	Node(){
		left = NULL; 
		right = NULL; 
	}
	Node(Node *l, Node *r){
		left = l;
		right = r;
		key = l->key + r->key;
	}
};

struct Comp{
	bool operator() (const Node* l, const Node* r){
		return l->key < r->key;
	}
};

vector<bool> code;
map<char, vector<bool>> table;

void Tree(Node *root){
	if (!root)
		return;
	if (root->left != NULL){
		code.push_back(0);
		Tree(root->left); 
	}
	if (root->right != NULL){
		code.push_back(1);
		Tree(root->right);
	}
	if (root->right == NULL && root->left == NULL){
		table[root->s] = code;
	}
	if (!code.empty())
		code.pop_back();
}

int fileSize(const char* add){
	ifstream mySource;
	mySource.open(add, ios_base::binary);
	mySource.seekg(0, ios_base::end);
	int size = mySource.tellg();
	mySource.close();
	return size;
}

size_t hashfunc(string s){
	hash<string> hasher;
	ifstream t(s, ios::binary);
	string buffer;
	getline(t, buffer, char(-1));
    size_t hash_buf = hasher(buffer);
	t.close();
	return hash_buf;
}

int main(){
	ifstream readf("encoded.txt", ios::binary);
	if(!readf){
		cerr << "Cannot open encoded.txt" << "\n";
		return -1;
	}
	ofstream writef("decoded.txt", ios::binary);
	if(!writef){
		cerr << "Cannot open decoded.txt" << "\n";
		return -2;
	}
	int x, y;
	char s;	
	map<char, int> rep;
	readf.read((char*)&x, sizeof(x));
	while (x-- > 0){
		readf.read((char*)&s, sizeof(s));
		readf.read((char*)&y, sizeof(y));
		rep[s] = y;
	}
	list<Node*> l;
	for (const auto& x : rep){
		Node* ptr = new Node;
		ptr->s = x.first;
		ptr->key = x.second;
		l.push_back(ptr);
	}

	if (l.size() == 1){
		auto ptr = new Node;
		ptr->left = l.front();
		ptr->key = ptr->left->key;
		l.push_front(ptr);
	}
	else{
		while (l.size() > 1){
			l.sort(Comp());
			Node *Left = l.front();
			l.pop_front(); 
			Node *Right = l.front(); 
			l.pop_front();
			Node *nnode = new Node(Left, Right);
			l.push_back(nnode);
		}
	}
	Node *root = l.front(); 
	Tree(root);
	int count = 0;
	Node *ptr = root;
	char byte = readf.get();
	while (!readf.eof()){
		const bool bit = byte & (1 << (7 - count));
		if (bit){
			ptr = ptr->right; 
		}
		else{
			ptr = ptr->left;
		}
		if (ptr->left == NULL && ptr->right == NULL){
			if (rep[ptr->s]-- > 0)
				writef << ptr->s;
			ptr = root;
		}
		count++;
		if (count == 8){
			count = 0;
			byte = readf.get();
		}
	}
    readf.close();
    writef.close();
	
	int code_size = fileSize("encoded.txt");
	int decode_size = fileSize("decoded.txt");
	cout << "encoded:" << code_size << "\n";
	cout << "decoded:" << decode_size << "\n";
	cout << "compress:" << int(code_size * 10000.0 /decode_size )/100.0  << "%\n";

	size_t hash1 = hashfunc("text.txt");
	size_t hash3 = hashfunc("decoded.txt");

	cout << "hash: files are equal\n";

	return 0;
}
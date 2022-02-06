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

int main(){
	ifstream readf("text.txt", ios::binary);
	if(!readf){
		cerr << "Cannot open text.txt" << "\n";
		return -1;
	}
	ofstream writef("encoded.txt", ios::binary);
	if(!writef){
		cerr << "Cannot open encoded.txt" << "\n";
		return -2;
	}
	map<char, int> rep;
	char sym;
	while (readf.get(sym)){
     	rep[sym]++;
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
	int count = rep.size();
	writef.write((char*)(&count), sizeof(count));
	for(const auto& x : rep){
        writef.write((char*)(&x.first), sizeof(x.first));
        writef.write((char*)(&x.second), sizeof(x.second));
    }
	readf.clear();
	readf.seekg(0);
	vector<bool> symvec;
	char tmp = 0;
	count = 0;
	while (!readf.eof()){
		char sym = readf.get();
		symvec = table[sym];
		for (int j = 0; j < symvec.size(); j++){
			tmp = tmp | symvec[j] << (7 - count);
			count++;
			if (count == 8){
				count = 0; 
				writef << tmp;
				tmp = 0;
			}
		}
	}
	readf.close();

	if (count!=0)
		writef << tmp;
	writef.close();
    for(const auto& x : table){
        cout << x.first << " ";
        for(const auto& y : x.second)
            cout << y;
        cout << '\n';
    }
	return 0;
}
#include <iostream>
#include <cstdlib>
#include <vector>
#include <map>
#include <list>
#include <fstream>
#include <ctime>

using namespace std;


/**
* class Node
* for tree
*/
class Node {
	public: 
		int a;
		char c;
		Node *left, *right;

		Node() {
			left = right = NULL;
		}

		Node(Node * l, Node * r) {
			a = l->a + r->a;
			left = l;
			right = r;
		}

};

/**
* class myCompare
* class for comparing
*/
class myCompare {
	public:
		bool operator()(Node * l, Node * r) const {
			return l->a < r->a;
		}
};

/**
* test function for printing tree
*/
void print(Node * root, unsigned int k = 0) {
	if(root != NULL) {
		print(root->left, k+3);
		
		for(unsigned int i = 0; i < k; i++) {
			cout << "   ";
		}

		if(root->c) {
			cout << root->a << " (" << root->c << ") " << endl;
		} else {
			cout << root->a << endl;
		}

		print(root->right, k+3);
	}
}

// global variables
// code - vector for each symbol's code
vector<bool> code;
map<char, vector<bool> > table;

/**
* function buildTable
* recursive function for building associative array - table
*/
void buildTable(Node *root, ofstream &f) {
	if(root->left != NULL) {
		code.push_back(0);
		buildTable(root->left, f);
	}
	if(root->right != NULL) {
		code.push_back(1);
		buildTable(root->right, f);
	}

	if(root->c) {
		for(int i = 0; i < code.size(); i++) {
			f << code[i];
		}

		f << " " << root->c << '\n';
		table[root->c] = code;
	}

	code.pop_back();
}

/**
* function convertFromTree
* convert from tree to file
*/
void convertFromTree(Node * root, ofstream &f) {
	if(root->left) {
		if(root->left->c) {
			f << 0 << root->left->c;
		} else {
			f << 1;
		}

		convertFromTree(root->left, f);
	}

	if(root->right) {
		if(root->right->c) {
			f << 0 << root->right->c;
		} else {
			f << 1;
		}

		convertFromTree(root->right, f);
	}
}

/**
* function convertToTree
* convert form file to tree
*/
void convertToTree(Node ** root, ifstream &f) {
	int c = f.get();
	if(c == 49) {
		(*root)->left = new Node();
		(*root)->right = new Node();
		convertToTree(&((*root)->left), f);
		convertToTree(&((*root)->right), f);
	} else if(c == 48) {
		char sym = f.get();
		(*root)->c = sym;
	}
}


int main() {
	bool exit = false;
	srand(time(NULL));

	while(!exit) {
		// getting a file type
		int type;
		cout << "If you want to encode, put 0; decode - put 1" << endl;
		cin >> type;
		cin.ignore(1);

		// getting files
		string input, output;
		cout << "Please, input in filename" << endl;
		cin >> input;
		cin.ignore();
		cout << "Please, input out filename" << endl;
		cin >> output;
		map<char, int> chars;

		// for measuring of time
		clock_t progTime = clock();
		// type == 0 - encode
		if(type == 0) {
			ifstream f(input.c_str(), ios::binary);
			
			if(f.fail()) {
				cout << "File is not exist" << endl;
				return 0;
			}

			// getting the chars
			while(!f.eof()) {
				char c = f.get();
				chars[c]++;
			}
			
			map<char, int>::iterator itr;
			list<Node*> tree;

			// creating of the tree
			for(itr = chars.begin(); itr != chars.end(); ++itr) {
				Node *p = new Node();
				p->c = itr->first;
				p->a = itr->second;
				tree.push_back(p);
			}

			while(tree.size() != 1) {
				tree.sort(myCompare());

				Node *sonL = tree.front();
				tree.pop_front();
				Node *sonR = tree.front();
				tree.pop_front();

				Node *parent = new Node(sonL, sonR);

				tree.push_back(parent);
			}
			Node * root = tree.front();

			// creatinf o statistic file
			string outputStatistic = output + "_statistic.txt";
			ofstream outStat(outputStatistic.c_str(), ios::out | ios::binary);
			
			// building of a table
			buildTable(root, outStat);
			
			f.clear();
			f.seekg(0);

			ofstream out((output + ".bin").c_str(), ios::out | ios::binary);
			
			// writing of tree into the file
			out << 1;
			convertFromTree(root, out);

			// writing the code to the file
			int count = 0; char buf = 0;
			while(!f.eof()) {
				char c = f.get();

				vector<bool> x = table[c];

				for(int n = 0; n < x.size(); n++) {
					buf = buf | x[n] << (7 - count);

					if(++count == 8) {
						count = 0;
						out << buf;
						buf = 0;
					}
				}
			}
			f.close();
			out.close();
		} else { // type == 1 - decode
			ifstream in(input.c_str(), ios::in | ios::binary);
			ofstream out(output.c_str(), ios::out | ios::binary);
			Node * treeFromFile = new Node();

			// building a tree from the file
			convertToTree(&treeFromFile, in);

			int count = 0;
			char byte;
			Node *p = treeFromFile;

			// decoding
			byte = in.get();
			while(!in.eof()) {
				bool b = byte & (1 << (7 - count));
				if(b) {
					p = p->right;
				} else {
					p = p->left;
				}
				if(p->left == NULL && p->right == NULL) {
					out << p->c;
					p = treeFromFile;
				}

				count++;
				
				if(count == 8) {
					byte = in.get();
					count = 0;
				}
			}

			in.close();
			out.close();
			treeFromFile = NULL;
		}

		progTime = clock() - progTime;
		cout << "Time: " <<(double)progTime / CLOCKS_PER_SEC << " seconds" << endl;
		
		code.clear();
		table.clear();

		cout << "Exit? 1/0" << endl;
		cin >> exit;
	}
	return 0;
}
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include "ArgumentManager.h"


using namespace std;


vector<int>lvector; //vector for level
vector<int>numVector;  //vector to store the numbers from the input file
const int degree = 1000; //max degree thats possible 
bool ioTraverse;
int intFromFile;


template<class recType, int degree>
struct btreeNode {
	int recCount; //num of elements
	recType list[degree - 1]; 
	btreeNode *children[degree];
};

template<class recType, int degree>
class btree {
protected:
	btreeNode<recType, degree> *root;

public:

	void searchNode(btreeNode<recType, degree> *cu, recType item, bool &found, int &location);
	bool search(recType &item);
	void inOrderTraversal(ofstream &outFile);
	void inOrderRecursion(btreeNode<recType, degree> *cu, ofstream &outFile);
	void insert(recType& item);
	bool insertBtree(btreeNode<recType, degree> *cu, recType &item, recType &median,
		btreeNode<recType, degree> *&rightchild, bool &isTaller); //pseudo code from DS malik textbook
	void insertNode(btreeNode<recType, degree> *cu,  recType item, btreeNode<recType, degree> *&rightchild, int insertPos);
	void splitNode(btreeNode<recType, degree> *cu, recType &item, btreeNode<recType, degree> *rightchild,
		int insertPos, btreeNode<recType, degree> *&rightNode, recType &median);

	//-------------------------------------------------
	// all the above functions were referred from the DS MALIK DATA STRUCTURES TEXTBOOOK 

	//default constructor 
	btree() {
		root = nullptr;
	}
	int deg = 0; //initial degree

	//overloaded constructor to set the degree
	btree(int d) {
		deg = d;
	}

	void BuildTree(vector<int>&numVector);
	void printbylevel(btreeNode<recType, degree> *cu, ofstream &outFile, int level, int pos);//print by level 
	int levelRecursion(btreeNode<recType, degree> *l);
	int getTheLevel();//get the level
	void traverseBylevel(ofstream &outFile, int level);

};

template<class recType, int degree>
void btree<recType, degree>::btree::searchNode(btreeNode<recType, degree> *cu, recType item, bool &found, int &location) {
	location = 0;
	while (location < cu->recCount && item >cu->list[location]) {
		location++;
	}
	if (location < cu->recCount && item == cu->list[location]) {
		found = true;
	}
	else {
		found = false;
	}
}//searchNode 

template<class recType, int degree>
bool btree<recType, degree>::search(recType &item) {
	bool found = false;
	int location;

	btreeNode<recType, degree> *cu;
	cu = root; //search needs to start at root 

	while (cu != nullptr && !found) {
		searchNode(cu, item, found, location);
		if (!found) {
			cu = cu->children[location];
		}
	}
	return found;
}//search funtion

template<class recType, int degree>
void btree<recType, degree>::inOrderTraversal(ofstream &outFile) {
	btreeNode<recType, degree> *cu = root;
	inOrderRecursion(root, outFile);
}//in order traversal


template<class recType, int degree>
void btree<recType, degree>::inOrderRecursion(btreeNode<recType, degree> *cu, ofstream &outFile) {

	if (outFile.is_open()) {
		if (cu != nullptr) {
			inOrderRecursion(cu->children[0], outFile);
			for (int i = 0; i < cu->recCount; i++) {
				cout << cu->list[i] << " ";
				outFile << cu->list[i] << " ";
				inOrderRecursion(cu->children[i + 1], outFile);
			}
		}
	}
}//in order recursion function 


template<class recType, int degree>
void btree<recType, degree>::insert(recType& item) {
	
	bool isTaller = false;
	int median;

	btreeNode<recType, degree> *rightChild;
	insertBtree(root, item, median, rightChild, isTaller);

	if (isTaller==true)
	{
		btreeNode<recType, degree> *temproot;
		temproot = new btreeNode<recType, degree>;
		temproot->recCount = 1;
		temproot->list[0] = median;
		temproot->children[0] = root;
		temproot->children[1] = rightChild;

		root = temproot;
	}

}//insert function

template<class recType, int degree>
bool btree<recType, degree>::insertBtree(btreeNode<recType, degree> *cu, recType &item, recType &median,
	btreeNode<recType, degree> *&rightchild, bool &isTaller) {

	if (cu == nullptr) {
		median = item;
		rightchild = nullptr;
		isTaller = true;
	}

	else {
		int index = 0;
		bool found;
		searchNode(cu, item, found, index);
		
		//if (found == true) {
		//	//make sure
		//}
		if(found == false){
			insertBtree(cu->children[index], item, median, rightchild, isTaller);
			if (isTaller == true) {
				if (cu->recCount != deg - 1) { //checking if node is not full 
					//if its not full, then insertnode 
					insertNode(cu, item, rightchild, index);
					isTaller = false; //since its not full anymore and does not need to be split 
				}
				else {//needs to be split since there's no space in the node anymore
					
					btreeNode<recType, degree> *rightNode;
					splitNode(cu, item, rightchild, index, rightNode, median);
					rightchild = rightNode;
					item = median;
				}
			}
		}
	}
	return isTaller;

} //insertBtree function 

template<class recType, int degree>
void btree<recType, degree>::insertNode(btreeNode<recType, degree> *cu,  recType item, btreeNode<recType, degree> *&rightchild, int insertPos) {

	int index;

	for (index = cu->recCount; index > insertPos; index--) {
		cu->list[index] = cu->list[index - 1];
		cu->children[index + 1] = cu->children[index];

	}
	cu->list[index] = item;
	cu->children[index + 1] = rightchild;
	cu->recCount++;
}//insertnode function

template<class recType, int degree>
void btree<recType, degree>::splitNode(btreeNode<recType, degree> *cu,  recType &item, btreeNode<recType, degree> *rightchild,
	int insertPos, btreeNode<recType, degree> *&rightNode, recType &median) {


	rightNode = new btreeNode<recType, degree>;
	int middle = (deg - 1) / 2; // we need to take the ceiling of degree -1 / 2 so we can get the middle item that we need to transfer up

	if (insertPos <= middle) // this means that the node goes in the first half
	{
		int index = 0;

		int i = middle;
		while (i < deg - 1) //while not full
		{
			rightNode->list[index] = cu->list[i];
			rightNode->children[index + 1] = cu->children[i + 1];
			index++;
			i++;
		}

		cu->recCount = middle;
		insertNode(cu, item, rightchild, insertPos);
		(cu->recCount)--;

		median = cu->list[cu->recCount];
		rightNode->recCount = index;
		rightNode->children[0] = cu->children[cu->recCount + 1];
	}
	else { //goes in the right half 
		int index = 0;

		int i = middle + 1; //starting from the first element of the right half
		while (i < deg - 1) //while its not full
		{
			rightNode->list[index] = cu->list[i];
			rightNode->children[index + 1] = cu->children[i + 1];
			index++;
			i++;
		}
		cu->recCount = middle;
		rightNode->recCount = index;
		median = cu->list[middle];
		insertNode(rightNode, item, rightchild, insertPos - middle - 1);
		rightNode->children[0] = cu->children[cu->recCount + 1];
	}
} //splitnode function

template<class recType, int degree>
void btree<recType, degree>::printbylevel(btreeNode<recType, degree> *cu, ofstream &outFile, int level, int pos) { //taken help from mentor
	
	//print by level using bfs

	if (cu == 0) {
		cout << "nothing there" << endl;
		return;
	}
	else if (pos != level) {
		for (int i = 0; i <= cu->recCount; i++) {
			//cout << "cu->children[i]" << " ";
			printbylevel(cu->children[i], outFile, level, pos+1);
		}
	}
	else {
		for (int i = 0; i < cu->recCount; i++) {
			cout << cu->list[i] << " ";
			outFile << cu->list[i] << " ";
		}
	}

}//printing by level

template<class recType, int degree>
int btree<recType, degree>::getTheLevel() {

	btreeNode <recType, degree> *l = root;
	return levelRecursion(l);
} //getlevel()


template<class recType, int degree>
int btree<recType, degree>::levelRecursion(btreeNode<recType, degree> *l)//getting the level
{
	if (l == 0) {
		return 0;
	}
	else {
		return 1 + levelRecursion(l->children[0]);
	}
} //int level();

template<class recType, int degree>
void btree<recType, degree>::traverseBylevel(ofstream &outFile, int level) {
	btreeNode<recType, degree> *cu = root;
	printbylevel(cu, outFile, level, 1);
}//traverse by level function 



template<class recType, int degree>
void btree<recType, degree>::BuildTree(vector<int>&numVector) {

	for (int k = 0; k < numVector.size(); k++) {
		insert(numVector[k]);
	}
}//buid the tree




void ReadFile(string str) {

	stringstream ss(str);
	string temp;
	if (str.find("Degree=") != string::npos) {
		getline(ss, temp, '=');
		getline(ss, temp, '=');
		ss.str(temp);
		stringstream geek(temp);
		int tempint = 0;
		geek >> tempint;
		intFromFile = tempint;

	}
	else if (str.find("Inorder Traversal") != string::npos) {
		ioTraverse = 1;
	}
	else if (str.find("Level ") != string::npos) {
		getline(ss, temp, ' ');
		getline(ss, temp, ' ');
		ss.str(temp);
		stringstream geek(temp);
		int x = 0;
		geek >> x;
		lvector.push_back(x);
	}
}


int main() {
//int main(int argc, char* argv[]) {
//	ArgumentManager am(argc, argv);
//	const string in = am.get("input");
//	const string comm = am.get("command");
//	const string out = am.get("output");
//
//	if (argc != 2 && argc != 3)
//
//	{
//
//		return

	}



	ifstream inFile;
	ifstream commandFile;
	ofstream outFile;

	string in = "input74.txt";
	string out = "output.txt";
	string comm = "command74.txt";

	int num;
	string str;

	commandFile.open(comm);
	while (commandFile.is_open()) {
		cout << "commandFile is open" << endl;

		while (!commandFile.eof()) {
			getline(commandFile, str);
			if (str != "") {
				ReadFile(str);
			}
		}
		commandFile.close();
	}

	btree<int, degree> mytree(intFromFile); //intFromFile is taken initially from calling the readFile function

	inFile.open(in);
	while (inFile.is_open())
	{
		cout << "inputFile is open" << endl;
		while (!inFile.eof())
		{
			while (inFile >> num) {
				numVector.push_back(num);
			}
		}
		inFile.close();
	}
	mytree.BuildTree(numVector);

	outFile.open(out);
	while (outFile.is_open()) {
		cout << "outFile is open" << endl;

		if (ioTraverse == 1) {
			mytree.inOrderTraversal(outFile);
			outFile << endl;
		}
		for (int i = 0; i < lvector.size(); i++) {
			if (mytree.getTheLevel() < lvector[i]) { //level does not exist 
				outFile << "empty";
			}
			else {
				mytree.traverseBylevel(outFile, lvector[i]);
			}
			outFile << endl;
		}

		outFile.close();
	}

	system("pause");
	return 0;
}



// concurrent5.cpp : This file contains the 'main' function. Program execution begins and ends there.
//Evaluating an expression tree
//The file tree.cpp implements an expression tree.
//Study the program and change it so that expressions are evaluated in a parallel fashion.
//Make use of std::async and std::future to evaluate different branches of the expression in separate threads.

#include "pch.h"
#include <future>
#include <thread>
#include <iostream>

using namespace std;

int valtab[127]; // used for integer values of variables

class Tree; // forward declare

class Node {
protected:
	Node() { use = 1; }
	virtual void print(ostream &os) = 0;
	virtual ~Node() { }
	virtual int eval() = 0;
private:
	friend class Tree;
	friend ostream& operator<<(ostream&, const Tree&);
	int use; // reference count
};

class Tree {
public:
	Tree(int n); // constant
	Tree(char id); // variable
	Tree(char op, Tree t); // unary operator
	Tree(char op, Tree left, Tree right); // binary operator
	Tree(const Tree &t) { p = t.p; ++p->use; }
	~Tree() { if (--p->use == 0) delete p; }
	void operator=(const Tree &t);
	int eval() { return p->eval(); }
private:
	friend class Node;
	friend ostream& operator<<(ostream &os, const Tree &t);
	Node *p; // polymorphic hierarchy
};

void Tree::operator=(const Tree &t)
{
	++t.p->use;
	if (--p->use == 0)
		delete p;
	p = t.p;
}

ostream& operator<<(ostream &os, const Tree &t)
{
	t.p->print(os);
	return os;
}

class LeafNode : public Node {
private:
	friend class Tree;
	void print(ostream &os) = 0;
	virtual int eval() = 0;
};

class IntNode : public LeafNode {
public:
	int eval() { return n; }
private:
	friend class Tree;
	int n;
	void print(ostream &os) { os << n; }
	IntNode(int k) : n(k) { }
};

class IdNode : public LeafNode {
public:
	int eval() { return valtab[name]; }
private:
	friend class Tree;
	char name;
	void print(ostream& o) { o << name; }
	IdNode(char id) : name(id) { }
};

class UnaryNode : public Node {
public:
	int eval();
private:
	friend class Tree;
	const char op;
	Tree opnd;
	UnaryNode(char a, Tree b) : op(a), opnd(b) { }
	void print(ostream& o) { o << "(" << op << opnd << ")"; }
};

int UnaryNode::eval()
{
	switch (op) {
	case '-': return (-opnd.eval());
	case '+': return (+opnd.eval());
	default: cerr << "no operand" << endl;
		return 0;
	}
}

class BinaryNode : public Node {
public:
	int eval();
private:
	friend class Tree;
	const char op;
	Tree left;
	Tree right;
	BinaryNode(char a, Tree b, Tree c) : op(a), left(b), right(c) { }
	void print(ostream &os) { os << "(" << left << op << right << ")"; }
};

int BinaryNode::eval()
{
	switch (op) {
	case '-': {
		//future<int> left = async(launch::async, left.eval());
		//future<int> right = async(launch::async, right.eval());
		auto eLeft = std::async(std::launch::async, std::bind(&Tree::eval, &this->left));
		auto eRight = std::async(std::launch::async, std::bind(&Tree::eval, &this->right));
		return (eLeft.get() - eRight.get());
	}
			  //return (left.eval() - right.eval());
	case '+': {//return (left.eval() + right.eval());
		auto eLeft = std::async(std::launch::async, std::bind(&Tree::eval, &this->left));
		auto eRight = std::async(std::launch::async, std::bind(&Tree::eval, &this->right));
		return (eLeft.get() + eRight.get());
	}
	case '*': {//return (left.eval() * right.eval());
		auto eLeft = std::async(std::launch::async, std::bind(&Tree::eval, &this->left));
		auto eRight = std::async(std::launch::async, std::bind(&Tree::eval, &this->right));
		return (eLeft.get() * eRight.get());
	}
	default: cerr << "no operand" << endl;
		return 0;
	}
}

Tree::Tree(int n) { p = new IntNode(n); }
Tree::Tree(char id) { p = new IdNode(id); }
Tree::Tree(char op, Tree t) { p = new UnaryNode(op, t); }
Tree::Tree(char op, Tree left, Tree right) { p = new BinaryNode(op, left, right); }

//async
void called_from_async() {
	std::cout << "Async call" << std::endl;
}

int main()
{
	valtab['A'] = 3; valtab['B'] = 4;
	cout << "A = 3, B = 4" << endl;
	//std::future<int> eval =std::async(t1.eval)
	Tree t1 = Tree('*', Tree('-', 5), Tree('+', 'A', 4));
	Tree t2 = Tree('+', Tree('-', 'A', 1), Tree('+', t1, 'B'));
	cout << "t1 = " << t1 << ", t2 = " << t2 << endl;
	cout << "t1 = " << t1.eval() << ", t2 = " << t2.eval() << endl;
	//cout << "t1 = " << std::future<int> myEval = std::async(std::launch::async, t1.eval) << ", t2 = " << std::future<int> myEval = std::async(std::launch::async, t2.eval) << endl;

	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

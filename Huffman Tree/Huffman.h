#pragma once
#include<iostream>
#include"Heap.h"
#include"Press.h"
using namespace std;

template<class T>
struct HuffmanTreeNode // 哈夫曼的节点
{
	typedef HuffmanTreeNode<T> Node;
	T m_tWeight; // 权重
	Node* m_pLeft; // 左孩子指针
	Node* m_pRight; // 右孩子指针
	Node* m_pParent; // 父指针
	HuffmanTreeNode(const T& w) // 构造函数，通过权重构造，指针默认为空
		:m_tWeight(w),
		m_pLeft(NULL),
		m_pRight(NULL),
		m_pParent(NULL)
	{}
};

template<class T>
class HuffmanTree // 哈夫曼树类
{
public:
	typedef HuffmanTreeNode<T> Node; // 节点
	HuffmanTree() // 默认构造函数,根节点设置为空
		:m_pRoot(NULL)
	{}
	~HuffmanTree() // 析构函数，删除树的所有结点
	{
		destory(m_pRoot);
	}
	Node* GetRoot() // 返回根节点
	{
		return m_pRoot;
	}
	template<class T> 
	struct Less // 构造比较结构体，为 < 比较
	{
		bool operator()(const T& left, const T&right)
		{
			return left->m_tWeight < right->m_tWeight;
		}
	};
	HuffmanTree(T* a, int size, T invalid)   // 通过权集，权的大小和结束权构建树
	{

		Heap<Node*, Less<Node*>> hp;   // 构建最小堆
		for (int i = 0; i < size; i++) // 用各个字符的权来生成最小堆
		{
			if (a[i] != invalid)
			{
				Node* tmp = new Node(a[i]);
				hp.Push(tmp); // 这里在加入进去的时候根据权值，出现次数，自动调整成最小堆了
			}

		}
		while (hp.Size() > 1) // 不断取权最小的两个节点生成一个父节点再放入堆中
		{
			Node* left = hp.Top();
			hp.Pop();

			Node* right = hp.Top();
			hp.Pop();

			Node* parent = new Node(left->m_tWeight + right->m_tWeight);
			hp.Push(parent);

			parent->m_pLeft = left;
			parent->m_pRight = right;
			left->m_pParent = parent;
			right->m_pParent = parent;
		}
		m_pRoot = hp.Top(); // 最后获取堆的顶端为根节点
	}
protected:
	void destory(Node* root) // 递归摧毁哈夫曼树
	{
		if (NULL == root)
			return;
		destory(root->m_pLeft);
		destory(root->m_pRight);

		delete root;
	}
private:
	Node* m_pRoot;
};
#pragma once

// binary search tree

#include "node.h"
#include <iostream>

template<typename T>
class TBst
{
public:
	TBst();
	~TBst();

public:
	void Insert(const T value);
	void Delete(const T value);
	// The length of arr must be equal to or large then count of nodes in this tree 
	const void GetInorderArray(T *arr) const;
	// Inorder traversing this tree by recursion call
	// The length of arr must be equal to or large then count of nodes in this tree 
	const void GetInorderArrayByRecursion(T *arr) const;

public:
	inline int GetCount() const
	{
		return count;
	}
	inline const TNode<T>* GetRootNode() const
	{
		return root;
	}

private:
	int InternalGetInorderArrayByRecursion(TNode<T> *node, T *arr, int index) const;

private:
	int count;
	TNode<T> *root;
};

template<typename T>
TBst<T>::TBst()
	: count(0)
	, root(nullptr)
{
}

template<typename T>
TBst<T>::~TBst()
{
	if (!root)
	{
		return;
	}

	TNode<T> **stack = new TNode<T> *[count];
	int stack_count = 0;
	stack[stack_count++] = root;
	root = nullptr;

	TNode<T> *iter = nullptr;
	while (stack_count > 0)
	{
		iter = stack[--stack_count];
		if (iter->left)
		{
			stack[stack_count++] = iter->left;
		}
		if (iter->right)
		{
			stack[stack_count++] = iter->right;
		}
	
		delete iter;
	}
	
	delete[] stack;
}

template<typename T>
void TBst<T>::Insert(const T value)
{
	TNode<T> *node = new TNode<T>(value);

	if (root)
	{
		// find insert location
		TNode<T> *parent = nullptr;
		{
			TNode<T> *location = root;
			while (location)
			{
				parent = location;
				location = (location->value > value)
					? location->left
					: location->right;
			}
		}

		if (parent->value > value)
		{
			parent->left = node;
		}
		else
		{
			parent->right = node;
		}
	}
	else
	{
		root = node;
	}
	count++;
}

template<typename T>
void TBst<T>::Delete(const T value)
{
	if (!root)
	{
		return;
	}

	TNode<T> *iter = root;
	TNode<T> *parent = nullptr;
	while (iter)
	{
		if (iter->value == value)
		{
			// found target. iter is the target to be delete
			TNode<T> *replace_iterator = nullptr;
			// iter is leaf node
			if (!iter->left
				&& !iter->right)
			{
				replace_iterator = nullptr;
			}
			// iter has tow children
			else if (iter->left
				&& iter->right)
			{
				TNode<T> *leftleaf_in_iterator_right = iter->right;
				// right subtree value is definitely larget than left subtree
				// so replace iter with right subnode 
				replace_iterator = leftleaf_in_iterator_right;

				// then move left subtree to smallest node on right subtree
				while (leftleaf_in_iterator_right->left)
				{
					leftleaf_in_iterator_right = leftleaf_in_iterator_right->left;
				}
				leftleaf_in_iterator_right->left = iter->left;
			}
			// iter has one children
			else
			{
				replace_iterator = (iter->left)
					? iter->left
					: iter->right;
			}

			if (iter == root)
			{
				root = (iter->right)
					? iter->right
					: iter->left;
			}

			if (parent)
			{
				if (parent->left == iter)
				{
					parent->left = replace_iterator;
				}
				else
				{
					parent->right = replace_iterator;
				}
			}
			// iter is root
			else
			{
				root = replace_iterator;
			}

			delete iter;
			count--;
			break;
		}
		else
		{
			parent = iter;
			iter = iter->value > value
				? iter->left
				: iter->right;
		}
	}
}

template<typename T>
int TBst<T>::InternalGetInorderArrayByRecursion(TNode<T> *node, T *arr, int index) const
{
	if (node)
	{
		index = InternalGetInorderArrayByRecursion(node->left, arr, index);
		arr[index++] = node->value;
		index = InternalGetInorderArrayByRecursion(node->right, arr, index);
		return index;
	}
	else
	{
		return index;
	}
}

template<typename T>
const void TBst<T>::GetInorderArrayByRecursion(T *arr) const
{
	InternalGetInorderArrayByRecursion(root, arr, 0);
}

template<typename T>
inline const void TBst<T>::GetInorderArray(T * arr) const
{
	TNode<T> **stack = new TNode<T> *[count];
	int stack_count = 0;
	int arr_index = 0;
	TNode<T> *node = root;

	while (node
		|| stack_count > 0)
	{
		while (node)
		{
			stack[stack_count++] = node;
			node = node->left;
		}

		if (stack_count > 0)
		{
			node = stack[--stack_count];
			arr[arr_index++] = node->value;
			node = node->right;
		}
	}
	delete[] stack;
}
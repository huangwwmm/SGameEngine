#pragma once

// binary search tree

#include "node.h"
#include <iostream>

template<typename TItem>
class TBst
{
private:
	int count;
	TNode<TItem> *root;

public:
	TBst();
	~TBst();

public:
	void Insert(const TItem value);
	void Delete(const TItem value);
	// The length of arr must be equal to or large then count of nodes in this tree 
	const void GetInorderArray(TItem *arr) const;
	// Inorder traversing this tree by recursion call
	// The length of arr must be equal to or large then count of nodes in this tree 
	const void GetInorderArrayByRecursion(TItem *arr) const;

public:
	inline int GetCount() const
	{
		return count;
	}
	inline const TNode<TItem>* GetRootNode() const
	{
		return root;
	}

private:
	int InternalGetInorderArrayByRecursion(TNode<TItem> *node, TItem *arr, int index) const;
};

template<typename TItem>
TBst<TItem>::TBst()
	: count(0)
	, root(nullptr)
{
}

template<typename TItem>
TBst<TItem>::~TBst()
{
	if (!root)
	{
		return;
	}

	TNode<TItem> **stack = new TNode<TItem> *[count];
	int stack_count = 0;
	stack[stack_count++] = root;
	root = nullptr;

	TNode<TItem> *iter = nullptr;
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

template<typename TItem>
void TBst<TItem>::Insert(const TItem value)
{
	TNode<TItem> *node = new TNode<TItem>(value);

	if (root)
	{
		// find insert location
		TNode<TItem> *parent = nullptr;
		{
			TNode<TItem> *location = root;
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

template<typename TItem>
void TBst<TItem>::Delete(const TItem value)
{
	if (!root)
	{
		return;
	}

	TNode<TItem> *iter = root;
	TNode<TItem> *parent = nullptr;
	while (iter)
	{
		if (iter->value == value)
		{
			// found target. iter is the target to be delete
			TNode<TItem> *replace_iterator = nullptr;
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
				TNode<TItem> *leftleaf_in_iterator_right = iter->right;
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

template<typename TItem>
int TBst<TItem>::InternalGetInorderArrayByRecursion(TNode<TItem> *node, TItem *arr, int index) const
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

template<typename TItem>
const void TBst<TItem>::GetInorderArrayByRecursion(TItem *arr) const
{
	InternalGetInorderArrayByRecursion(root, arr, 0);
}

template<typename TItem>
inline const void TBst<TItem>::GetInorderArray(TItem * arr) const
{
	TNode<TItem> **stack = new TNode<TItem> *[count];
	int stack_count = 0;
	int arr_index = 0;
	TNode<TItem> *node = root;

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
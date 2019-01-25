#pragma once

// red black tree

#include <iostream>

enum ERbtColor { kRbtColorRed, kRbtColorBlack };

template<typename T>
struct TRbtNode
{
	T value;

	ERbtColor color;

	TRbtNode *parent;
	TRbtNode *left;
	TRbtNode *right;

	TRbtNode(T value);
	TRbtNode(T value, ERbtColor color);
};

template<typename T>
class TRbt
{
public:
	TRbt();
	~TRbt();

public:
	void Insert(const T value);
	void Delete(const T value);
	// The length of arr must be equal to or large then count of nodes in this tree 
	const void GetInorderArray(T *arr) const;
	// This is the debugging function. In fact it is always vaild
	bool IsVaildRbt() const;

	inline int GetCount() const;
	inline const TRbtNode<T>* GetRootNode() const;
	inline TRbtNode<T>* GetBrotherNode(const TRbtNode<T> *node) const;
	// Return true when node is nullptr and color is black
	inline bool IsColor(TRbtNode<T> *node, ERbtColor color) const;

private:
	inline void RepairInsert(TRbtNode<T> *node);
	inline void RepairDelete(TRbtNode<T> *node, TRbtNode<T> *parent);
	void RotateLeft(TRbtNode<T> *node);
	void RotateRight(TRbtNode<T> *node);
	// This is the debugging function. In fact it is always vaild
	bool IsVaildRbt(TRbtNode<T> *node, int all_black_count_each_path, int black_count_in_this_path) const;

private:
	int count;
	TRbtNode<T> *root;
};

template<typename T>
TRbt<T>::TRbt()
	: count(0)
	, root(nullptr)
{
}

template<typename T>
TRbt<T>::~TRbt()
{
	if (!root)
	{
		return;
	}

	TRbtNode<T> **stack = new TRbtNode<T> *[count];
	int stack_count = 0;
	stack[stack_count++] = root;
	root = nullptr;

	TRbtNode<T> *iter = nullptr;
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
void TRbt<T>::Insert(const T value)
{
	if (root)
	{
		TRbtNode<T> *node = new TRbtNode<T>(value, kRbtColorRed);

		// Find insert location
		TRbtNode<T> *parent = nullptr;
		{
			TRbtNode<T> *iter = root;
			while (iter)
			{
				parent = iter;
				iter = (iter->value > value)
					? iter->left
					: iter->right;
			}
		}

		// Insert node
		node->parent = parent;
		(parent->value > value ? parent->left : parent->right) = node;

		RepairInsert(node);
	}
	// Node is at the root of the tree. root color must be black
	else
	{
		root = new TRbtNode<T>(value, kRbtColorBlack);
	}

	count++;
}

template<typename T>
void TRbt<T>::Delete(const T value)
{
	// Find value
	TRbtNode<T> *node = nullptr;
	TRbtNode<T> *iter = root;
	while (iter)
	{
		if (iter->value == value)
		{
			node = iter;
			break;
		}
		else
		{
			iter = iter->value > value
				? iter->left
				: iter->right;
		}
	}

	// Not found
	if (!node)
	{
		return;
	}

	// Delete node
	TRbtNode<T> *child, *parent;
	ERbtColor color;

	if (node->left && node->right)
	{
		TRbtNode<T> *replace = node;

		replace = replace->right;
		while (replace->left)
		{
			replace = replace->left;
		}

		if (node->parent)
		{
			(node->parent->left == node ? node->parent->left : node->parent->right) = replace;
		}
		// Node is Root
		else
		{
			root = replace;
		}

		child = replace->right;
		parent = replace->parent;
		color = replace->color;

		if (parent == node)
		{
			parent = replace;
		}
		else
		{
			if (child)
			{
				child->parent = parent;
			}
			parent->left = child;

			replace->right = node->right;
			node->right->parent = replace;
		}

		replace->parent = node->parent;
		replace->color = node->color;
		replace->left = node->left;
		node->left->parent = replace;

		if (color == kRbtColorBlack)
		{
			RepairDelete(child, parent);
		}
	}
	else
	{
		child = node->left ? node->left : node->right;
		parent = node->parent;
		color = node->color;

		if (child)
		{
			child->parent = parent;
		}

		if (parent)
		{
			(parent->left == node ? parent->left : parent->right) = child;
		}
		else
		{
			root = child;
		}

		if (color == kRbtColorBlack)
		{
			RepairDelete(child, parent);
		}
	}

	delete node;
	count--;
}

template<typename T>
inline const TRbtNode<T>* TRbt<T>::GetRootNode() const
{
	return root;
}

template<typename T>
inline TRbtNode<T>* TRbt<T>::GetBrotherNode(const TRbtNode<T>* node) const
{
	return node->parent
		? (node->parent->left == node
			? node->parent->right
			: node->parent->left)
		: nullptr;
}

template<typename T>
inline bool TRbt<T>::IsColor(TRbtNode<T>* node, ERbtColor color) const
{
	return node
		? node->color == color
		: color == kRbtColorBlack;
}

template<typename T>
inline void TRbt<T>::RepairInsert(TRbtNode<T>* node)
{
	TRbtNode<T> *parent, *grandparent;
	while (node != root
		&& node->color == kRbtColorRed
		&& node->parent->color == kRbtColorRed)
	{
		parent = node->parent;
		grandparent = parent->parent;

		TRbtNode<T> *uncle = GetBrotherNode(parent);
		if (IsColor(uncle, kRbtColorRed))
		{
			uncle->color = kRbtColorBlack;
			parent->color = kRbtColorBlack;
			grandparent->color = kRbtColorRed;
			node = grandparent;
		}
		else
		{
			if (parent == grandparent->left)
			{
				if (node == parent->right)
				{
					RotateLeft(parent);
					node = parent;
					parent = node->parent;
				}
				RotateRight(grandparent);
			}
			else
			{
				if (node == parent->left)
				{
					RotateRight(parent);
					node = parent;
					parent = node->parent;
				}
				RotateLeft(grandparent);
			}
			// Swap color
			ERbtColor swap_color = parent->color;
			parent->color = grandparent->color;
			grandparent->color = swap_color;

			node = parent;
		}
	}

	// root color must be black
	root->color = kRbtColorBlack;
}

template<typename T>
inline void TRbt<T>::RepairDelete(TRbtNode<T> *node, TRbtNode<T> *parent)
{
	TRbtNode<T> *uncle;

	while (IsColor(node, kRbtColorBlack)
		&& node != root)
	{
		if (parent->left == node)
		{
			uncle = parent->right;
			if (uncle->color == kRbtColorRed)
			{
				uncle->color = kRbtColorBlack;
				parent->color = kRbtColorRed;
				RotateLeft(parent);
				uncle = parent->right;
			}
			if (IsColor(uncle->left, kRbtColorBlack)
				&& IsColor(uncle->right, kRbtColorBlack))
			{
				uncle->color = kRbtColorRed;
				node = parent;
				parent = node->parent;
			}
			else
			{
				if (IsColor(uncle->right, kRbtColorBlack))
				{
					uncle->left->color = kRbtColorBlack;
					uncle->color = kRbtColorRed;
					RotateRight(uncle);
					uncle = parent->right;
				}

				uncle->color = parent->color;
				parent->color = kRbtColorBlack;
				uncle->right->color = kRbtColorBlack;
				RotateLeft(parent);
				node = root;
				break;
			}
		}
		else
		{
			uncle = parent->left;
			if (uncle->color == kRbtColorRed)
			{
				uncle->color = kRbtColorBlack;
				parent->color = kRbtColorRed;
				RotateRight(parent);
				uncle = parent->left;
			}
			if (IsColor(uncle->left, kRbtColorBlack)
				&& IsColor(uncle->right, kRbtColorBlack))
			{
				uncle->color = kRbtColorRed;
				node = parent;
				parent = node->parent;
			}
			else
			{
				if (IsColor(uncle->left, kRbtColorBlack))
				{
					uncle->right->color = kRbtColorBlack;
					uncle->color = kRbtColorRed;
					RotateLeft(uncle);
					uncle = parent->left;
				}
				uncle->color = parent->color;
				parent->color = kRbtColorBlack;
				uncle->left->color = kRbtColorBlack;
				RotateRight(parent);
				node = root;
				break;
			}
		}
	}

	if (node)
	{
		node->color = kRbtColorBlack;
	}
}

template<typename T>
void TRbt<T>::RotateLeft(TRbtNode<T> *node)
{
	TRbtNode<T> *right = node->right;
	node->right = right->left;

	if (node->right)
	{
		node->right->parent = node;
	}
	right->parent = node->parent;

	if (!node->parent)
	{
		root = right;
	}
	else
	{
		(node->parent->right == node ? node->parent->right : node->parent->left) = right;
	}

	right->left = node;
	node->parent = right;
}

template<typename T>
void TRbt<T>::RotateRight(TRbtNode<T> *node)
{
	TRbtNode<T> *left = node->left;
	node->left = left->right;

	if (node->left)
	{
		node->left->parent = node;
	}
	left->parent = node->parent;

	if (!node->parent)
	{
		root = left;
	}
	else
	{
		(node->parent->right == node ? node->parent->right : node->parent->left) = left;
	}
	left->right = node;
	node->parent = left;
}

template<typename T>
bool TRbt<T>::IsVaildRbt(TRbtNode<T>* node, int all_black_count_each_path, int black_count_in_this_path) const
{
	if (!node)
	{
		if (all_black_count_each_path == black_count_in_this_path)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	if (node->color == kRbtColorBlack)
	{
		black_count_in_this_path++;
	}

	return IsVaildRbt(node->left, all_black_count_each_path, black_count_in_this_path)
		&& IsVaildRbt(node->right, all_black_count_each_path, black_count_in_this_path);
}

template<typename T>
TRbtNode<T>::TRbtNode(T value)
	: value(value)
	, parent(nullptr)
	, left(nullptr)
	, right(nullptr)
	, color(kRbtColorBlack)
{
}

template<typename T>
TRbtNode<T>::TRbtNode(T value, ERbtColor color)
	: value(value)
	, parent(nullptr)
	, left(nullptr)
	, right(nullptr)
	, color(color)
{
}

template<typename T>
inline const void TRbt<T>::GetInorderArray(T * arr) const
{
	TRbtNode<T> **stack = new TRbtNode<T> *[count];
	int stack_count = 0;
	int arr_index = 0;
	TRbtNode<T> *node = root;

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

template<typename T>
inline bool TRbt<T>::IsVaildRbt() const
{
	if (count == 0)
	{
		return root == nullptr;
	}

	TRbtNode<T> **arr = new TRbtNode<T> *[count];
	TRbtNode<T> **stack = new TRbtNode<T> *[count];
	int stack_count = 0;
	int arr_index = 0;
	TRbtNode<T> *node = root;

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
			arr[arr_index++] = node;
			node = node->right;
		}
	}
	delete[] stack;

	if (root->color == kRbtColorBlack)
	{
		for (int iNode = 0; iNode < count; iNode++)
		{
			TRbtNode<T> *iter = arr[iNode];
			if (iter->color == kRbtColorRed)
			{
				if (IsColor(iter->left, kRbtColorRed)
					|| IsColor(iter->right, kRbtColorRed))
				{
					return false;
				}
			}

			TRbtNode<T> *iter_root = iter;
			while (iter_root)
			{
				if (!iter_root->parent
					&& iter_root != root)
				{
					return false;
				}
				iter_root = iter_root->parent;
			}
		}
		int all_black_count_each_path = 0;
		TRbtNode<T> *left_leaf = root;
		while (left_leaf)
		{
			if (left_leaf->color == kRbtColorBlack)
			{
				all_black_count_each_path++;
			}
			left_leaf = left_leaf->left;
		}
		return IsVaildRbt(root, all_black_count_each_path, 0);
	}
	else
	{
		return false;
	}
}

template<typename T>
inline int TRbt<T>::GetCount() const
{
	return count;
}
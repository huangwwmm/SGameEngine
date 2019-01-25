#pragma once

// red black tree

#include <iostream>

enum ERbtColor { ERbtColorRed, ERbtColorBlack };

template<typename TItem>
struct TRbtNode
{
	TItem value;

	ERbtColor color;

	TRbtNode *parent;
	TRbtNode *left;
	TRbtNode *right;

	TRbtNode(TItem value);
	TRbtNode(TItem value, ERbtColor color);
};

template<typename TItem>
class TRbt
{
private:
	int count;
	TRbtNode<TItem> *root;

public:
	TRbt();
	~TRbt();

public:
	void Insert(const TItem value);
	void Delete(const TItem value);
	// The length of arr must be equal to or large then count of nodes in this tree 
	const void GetInorderArray(TItem *arr) const;
	// This is the debugging function. In fact it is always vaild
	bool IsVaildRbt() const;

	inline int GetCount() const;
	inline const TRbtNode<TItem>* GetRootNode() const;
	inline TRbtNode<TItem>* GetBrotherNode(const TRbtNode<TItem> *node) const;
	// Return true when node is nullptr and color is black
	inline bool IsColor(TRbtNode<TItem> *node, ERbtColor color) const;

private:
	inline void RepairInsert(TRbtNode<TItem> *node);
	inline void RepairDelete(TRbtNode<TItem> *node, TRbtNode<TItem> *parent);
	void RotateLeft(TRbtNode<TItem> *node);
	void RotateRight(TRbtNode<TItem> *node);
	// This is the debugging function. In fact it is always vaild
	bool IsVaildRbt(TRbtNode<TItem> *node, int all_black_count_each_path, int black_count_in_this_path) const;
};

template<typename TItem>
TRbt<TItem>::TRbt()
	: count(0)
	, root(nullptr)
{
}

template<typename TItem>
TRbt<TItem>::~TRbt()
{
	if (!root)
	{
		return;
	}

	TRbtNode<TItem> **stack = new TRbtNode<TItem> *[count];
	int stack_count = 0;
	stack[stack_count++] = root;
	root = nullptr;

	TRbtNode<TItem> *iter = nullptr;
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
void TRbt<TItem>::Insert(const TItem value)
{
	if (root)
	{
		TRbtNode<TItem> *node = new TRbtNode<TItem>(value, ERbtColorRed);

		// Find insert location
		TRbtNode<TItem> *parent = nullptr;
		{
			TRbtNode<TItem> *iter = root;
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
		root = new TRbtNode<TItem>(value, ERbtColorBlack);
	}

	count++;
}

template<typename TItem>
void TRbt<TItem>::Delete(const TItem value)
{
	// Find value
	TRbtNode<TItem> *node = nullptr;
	TRbtNode<TItem> *iter = root;
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
	TRbtNode<TItem> *child, *parent;
	ERbtColor color;

	if (node->left && node->right)
	{
		TRbtNode<TItem> *replace = node;

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

		if (color == ERbtColorBlack)
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

		if (color == ERbtColorBlack)
		{
			RepairDelete(child, parent);
		}
	}

	delete node;
	count--;
}

template<typename TItem>
inline const TRbtNode<TItem>* TRbt<TItem>::GetRootNode() const
{
	return root;
}

template<typename TItem>
inline TRbtNode<TItem>* TRbt<TItem>::GetBrotherNode(const TRbtNode<TItem>* node) const
{
	return node->parent
		? (node->parent->left == node
			? node->parent->right
			: node->parent->left)
		: nullptr;
}

template<typename TItem>
inline bool TRbt<TItem>::IsColor(TRbtNode<TItem>* node, ERbtColor color) const
{
	return node
		? node->color == color
		: color == ERbtColorBlack;
}

template<typename TItem>
inline void TRbt<TItem>::RepairInsert(TRbtNode<TItem>* node)
{
	TRbtNode<TItem> *parent, *grandparent;
	while (node != root
		&& node->color == ERbtColorRed
		&& node->parent->color == ERbtColorRed)
	{
		parent = node->parent;
		grandparent = parent->parent;

		TRbtNode<TItem> *uncle = GetBrotherNode(parent);
		if (IsColor(uncle, ERbtColorRed))
		{
			uncle->color = ERbtColorBlack;
			parent->color = ERbtColorBlack;
			grandparent->color = ERbtColorRed;
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
	root->color = ERbtColorBlack;
}

template<typename TItem>
inline void TRbt<TItem>::RepairDelete(TRbtNode<TItem> *node, TRbtNode<TItem> *parent)
{
	TRbtNode<TItem> *uncle;

	while (IsColor(node, ERbtColorBlack)
		&& node != root)
	{
		if (parent->left == node)
		{
			uncle = parent->right;
			if (uncle->color == ERbtColorRed)
			{
				uncle->color = ERbtColorBlack;
				parent->color = ERbtColorRed;
				RotateLeft(parent);
				uncle = parent->right;
			}
			if (IsColor(uncle->left, ERbtColorBlack)
				&& IsColor(uncle->right, ERbtColorBlack))
			{
				uncle->color = ERbtColorRed;
				node = parent;
				parent = node->parent;
			}
			else
			{
				if (IsColor(uncle->right, ERbtColorBlack))
				{
					uncle->left->color = ERbtColorBlack;
					uncle->color = ERbtColorRed;
					RotateRight(uncle);
					uncle = parent->right;
				}

				uncle->color = parent->color;
				parent->color = ERbtColorBlack;
				uncle->right->color = ERbtColorBlack;
				RotateLeft(parent);
				node = root;
				break;
			}
		}
		else
		{
			uncle = parent->left;
			if (uncle->color == ERbtColorRed)
			{
				uncle->color = ERbtColorBlack;
				parent->color = ERbtColorRed;
				RotateRight(parent);
				uncle = parent->left;
			}
			if (IsColor(uncle->left, ERbtColorBlack)
				&& IsColor(uncle->right, ERbtColorBlack))
			{
				uncle->color = ERbtColorRed;
				node = parent;
				parent = node->parent;
			}
			else
			{
				if (IsColor(uncle->left, ERbtColorBlack))
				{
					uncle->right->color = ERbtColorBlack;
					uncle->color = ERbtColorRed;
					RotateLeft(uncle);
					uncle = parent->left;
				}
				uncle->color = parent->color;
				parent->color = ERbtColorBlack;
				uncle->left->color = ERbtColorBlack;
				RotateRight(parent);
				node = root;
				break;
			}
		}
	}

	if (node)
	{
		node->color = ERbtColorBlack;
	}
}

template<typename TItem>
void TRbt<TItem>::RotateLeft(TRbtNode<TItem> *node)
{
	TRbtNode<TItem> *right = node->right;
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

template<typename TItem>
void TRbt<TItem>::RotateRight(TRbtNode<TItem> *node)
{
	TRbtNode<TItem> *left = node->left;
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

template<typename TItem>
bool TRbt<TItem>::IsVaildRbt(TRbtNode<TItem>* node, int all_black_count_each_path, int black_count_in_this_path) const
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

	if (node->color == ERbtColorBlack)
	{
		black_count_in_this_path++;
	}

	return IsVaildRbt(node->left, all_black_count_each_path, black_count_in_this_path)
		&& IsVaildRbt(node->right, all_black_count_each_path, black_count_in_this_path);
}

template<typename TItem>
TRbtNode<TItem>::TRbtNode(TItem value)
	: value(value)
	, parent(nullptr)
	, left(nullptr)
	, right(nullptr)
	, color(ERbtColorBlack)
{
}

template<typename TItem>
TRbtNode<TItem>::TRbtNode(TItem value, ERbtColor color)
	: value(value)
	, parent(nullptr)
	, left(nullptr)
	, right(nullptr)
	, color(color)
{
}

template<typename TItem>
inline const void TRbt<TItem>::GetInorderArray(TItem * arr) const
{
	TRbtNode<TItem> **stack = new TRbtNode<TItem> *[count];
	int stack_count = 0;
	int arr_index = 0;
	TRbtNode<TItem> *node = root;

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

template<typename TItem>
inline bool TRbt<TItem>::IsVaildRbt() const
{
	if (count == 0)
	{
		return root == nullptr;
	}

	TRbtNode<TItem> **arr = new TRbtNode<TItem> *[count];
	TRbtNode<TItem> **stack = new TRbtNode<TItem> *[count];
	int stack_count = 0;
	int arr_index = 0;
	TRbtNode<TItem> *node = root;

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

	if (root->color == ERbtColorBlack)
	{
		for (int iNode = 0; iNode < count; iNode++)
		{
			TRbtNode<TItem> *iter = arr[iNode];
			if (iter->color == ERbtColorRed)
			{
				if (IsColor(iter->left, ERbtColorRed)
					|| IsColor(iter->right, ERbtColorRed))
				{
					return false;
				}
			}

			TRbtNode<TItem> *iter_root = iter;
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
		TRbtNode<TItem> *left_leaf = root;
		while (left_leaf)
		{
			if (left_leaf->color == ERbtColorBlack)
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

template<typename TItem>
inline int TRbt<TItem>::GetCount() const
{
	return count;
}
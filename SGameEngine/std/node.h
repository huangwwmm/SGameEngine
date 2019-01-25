#pragma once
template<typename TItem>
struct TNode
{
	TItem value;
	TNode *left;
	TNode *right;

	TNode(TItem value);
};

template<typename TItem>
TNode<TItem>::TNode(TItem value)
	: value(value)
	, left(nullptr)
	, right(nullptr)
{
}
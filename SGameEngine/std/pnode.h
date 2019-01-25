#pragma once

template<typename TItem>
struct TPNode
{
	TItem value;

	TPNode *parent;
	TPNode *left;
	TPNode *right;

	TPNode(TItem value);
};

template<typename TItem>
TPNode<TItem>::TPNode(TItem value)
	: value(value)
	, parent(nullptr)
	, left(nullptr)
	, right(nullptr)
{
}
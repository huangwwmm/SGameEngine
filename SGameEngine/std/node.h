#pragma once
template<typename T>
struct TNode
{
	T value;
	TNode *left;
	TNode *right;

	TNode(T value);
};

template<typename T>
TNode<T>::TNode(T value)
	: value(value)
	, left(nullptr)
	, right(nullptr)
{
}
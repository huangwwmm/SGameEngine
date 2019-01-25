#pragma once

template<typename T>
struct TPNode
{
	T value;

	TPNode *parent;
	TPNode *left;
	TPNode *right;

	TPNode(T value);
};

template<typename T>
TPNode<T>::TPNode(T value)
	: value(value)
	, parent(nullptr)
	, left(nullptr)
	, right(nullptr)
{
}
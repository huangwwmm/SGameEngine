#pragma once
#include "../debug.h"

template<typename TItem>
class TArray
{
private:
	const int kDefaultSize = 0x4;
	const int kMaxSize = 0xFFFF;

private:
	TItem* arr;
	int size;
	int max_size;
	// equal last item index + 1
	int count;

public:
	TArray();
	~TArray();

public:
	void Add(const TItem item);
	void Remove(const TItem item);
	void RemoveAt(const int index);
	// Not shrink if new_size less zero
	void Clear(int new_size);
	void Shrink();
	TItem *Begin();
	TItem *End();

private:
	void ResizeTo(const int new_size);
};

template<typename TItem>
inline TArray<TItem>::TArray()
{
	size = kDefaultSize;
	max_size = kMaxSize;
	count = 0;

	arr = (TItem*)malloc(sizeof(TItem) * size);
	FASSERT(arr, "malloc failed");
}

template<typename TItem>
inline TArray<TItem>::~TArray()
{
	free(arr);
}

template<typename TItem>
inline void TArray<TItem>::Add(const TItem item)
{
	if (count >= size)
	{
		// TODO Maybe can let the user decide
		// But i dont know how to implement. use function pointer?
		ResizeTo(size * 2);
	}

	arr[count++] = item;
}

template<typename TItem>
inline void TArray<TItem>::Remove(const TItem item)
{
	for (TItem *iter = Begin(); iter != End(); iter++)
	{
		if (*iter == item)
		{
			memmove(iter, iter + 1, sizeof(TItem) * (End() - iter));

			// Must be at the end, because End() dependent count
			count--;
			break;
		}
	}
}

template<typename TItem>
inline void TArray<TItem>::RemoveAt(const int index)
{
	FASSERT(index > 0 && index < count && count > 0, "index out of range");

	TItem *removed = arr + index;
	memmove(removed, removed + 1, sizeof(TItem) * (End() - removed));

	// Must be at the end, because End() dependent count
	count--;
}

template<typename TItem>
inline void TArray<TItem>::Clear(const int new_size)
{
	memset(arr, 0, sizeof(TItem) * count);
	count = 0;
}

template<typename TItem>
inline void TArray<TItem>::Shrink()
{
	if (count > 0)
	{
		ResizeTo(count);
	}
}

template<typename TItem>
inline TItem *TArray<TItem>::Begin()
{
	return arr;
}

template<typename TItem>
inline TItem *TArray<TItem>::End()
{
	return arr + count;
}

template<typename TItem>
inline void TArray<TItem>::ResizeTo(int new_size)
{
	if (new_size > max_size)
	{
		new_size = max_size;
	}

	FASSERT(new_size >= count, "new_size less then count");

	realloc(arr, sizeof(TItem) * new_size);
	FASSERT(arr, "realloc failed");

	size = new_size;
	if (count > size)
	{
		// UNDONE Need warning log
		count = size;
	}
}
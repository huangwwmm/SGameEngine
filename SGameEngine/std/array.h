#pragma once
#include "../debug.h"

template<typename TItem>
class TArray
{
private:
	static int kDefaultSize = 0x4;
	static int kMaxSize = 0xFFFF;

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
	void InsertAt(const TItem item, const int index);
	void Remove(const TItem item);
	void RemoveAt(const TItem item, const int index);
	int IndexOf(const TItem item) const;
	// Not shrink if new_size less zero
	void Clear(const int new_size);
	void Shrink();

private:
	void ResizeTo(const int new_size);
};

#include <vector>

template<typename TItem>
inline TArray<TItem>::TArray()
{
	size = kDefaultSize;
	max_size = kMaxSize;
	count = 0;

	arr = malloc(sizeof(TItem) * size);
	ASSERT(arr, "malloc failed");
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
inline void TArray<TItem>::Clear(const int new_size)
{
	memset(arr, 0, sizeof(TItem) * count);
	count = 0;
}

template<typename TItem>
inline void TArray<TItem>::ResizeTo(const int new_size)
{
	ASSERT(new_size <= max_size, "new_size large then max_size");

	realloc(arr, sizeof(TItem) * new_size);
	ASSERT(arr, "realloc failed");

	size = new_size;
	if (count > size)
	{
		// UNDONE Need warning log
		count = size;
	}
}
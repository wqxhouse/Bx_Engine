#pragma once

#include <list>

template <typename T>
class Buffer
{
public:
	int size;
	int count;

	void setData(std::list<T>* data)
	{
		bufferData = data;
	}
	std::list<T>* getData()
	{
		return bufferData;
	}
private:
	std::list<T>* bufferData;
};
#pragma once

#include "Buffer.h"
#include "../Core/TypeDef.h"

#include <unordered_map>

template <typename T>
class VertexBuffer : public Buffer<T>
{
public:
	VertexBuffer();
	void setBuffer(Buffer<T> buffer, VertexElement vertexElement);
	void getBuffer(VertexElement vertexElement);
	void combileBuffers();
	~VertexBuffer();
private:
	
};
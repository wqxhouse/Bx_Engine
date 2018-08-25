#pragma once

#include "Buffer.h"

#include <unordered_map>

template <typename T>
class VertexBuffer : public Buffer<T>
{
public:
	enum VertexElement
	{
		POSITION, 
		NORMAL, 
		TANGENT, 
		TEXTURE, 
		TEXTURE1, 
		TEXTURE2, 
		TEXTURE3, 
		TEXTURE4,
		TEXTURE5,
		TEXTURE6,
		TEXTURE7,
		TEXTURE8,
		TEXTURE9
	};
	VertexBuffer();
	void setBuffer(Buffer<T> buffer, VertexElement vertexElement);
	void getBuffer(VertexElement vertexElement);
	void combileBuffers();
	~VertexBuffer();
private:
	
};
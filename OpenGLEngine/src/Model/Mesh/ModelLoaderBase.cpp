

#include <sstream>
#include <iostream>

#include "ModelLoaderBase.h"


ModelLoaderBase::ModelLoaderBase()
{
}

void ModelLoaderBase::LoadModel(const string& fileName)
{

}


ModelLoaderBase::~ModelLoaderBase()
{
}

void ModelLoaderBase::split(const string & str, char delim, vector<string>* splitedStrings)
{
	stringstream m_stringstream(str);
	string item;
	while (getline(m_stringstream, item, delim)) 
	{
		if (item.length() > 0)
		{
			splitedStrings->push_back(item);
		}
	}
}

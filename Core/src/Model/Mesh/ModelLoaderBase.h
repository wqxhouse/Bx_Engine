#pragma once

#include "../../Core/PCH.h"
#include "../../Math/Vector3.h"

using namespace std;

namespace Object
{
    namespace Model
    {
        class ModelLoaderBase
        {
        public:
            ModelLoaderBase();
            void virtual LoadModel(const string &fileName);
            ~ModelLoaderBase();
        };
    }
}

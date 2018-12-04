//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "../Core/PCH.h"

namespace Structure
{
    template<typename T>
    struct LinkedListNode
    {
        std::unique_ptr<LinkedListNode> pNext;
        T* data;
    };

    template<typename T>
    class LinkedList
    {
    public:
        LinkedList();
        ~LinkedList();

        BOOL insertBack(const T* data);

    private:
        std::shared_ptr<LinkedListNode> pHead;
        std::shared_ptr<LinkedListNode> pTail;

        UINT length;
    };
}
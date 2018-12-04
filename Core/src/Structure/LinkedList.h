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
        LinkedListNode(
            const T&              data,
            LinkedListNode*       pNext)
            : m_data(data), m_pNext(pNext) {}

        T               m_data;
        LinkedListNode* m_pNext;
    };

    template<typename T>
    struct DoublyLinkedListNode
    {
        DoublyLinkedListNode(
            const T&              data,
            DoublyLinkedListNode* pNext,
            DoublyLinkedListNode* pPrevious)
            : m_data(data), m_pNext(pNext), m_pPrevious(pPrevious) {}

        T                     m_data;
        DoublyLinkedListNode* m_pNext;
        DoublyLinkedListNode* m_pPrevious;
    };

    template<typename T>
    class LinkedList
    {
    public:
        LinkedList();
        ~LinkedList();

        void insertBack(const T& data);
        void insertFront(const T& data);
        void insert(const UINT i, const T& data);

        void removeBack();
        void removeFront();
        void removeNode(const UINT i);

        void clear();

    private:
        LinkedListNode<T>* m_pHead;
        LinkedListNode<T>* m_pTail;

        UINT m_length;
    };
}

#include "LinkedList.inl"
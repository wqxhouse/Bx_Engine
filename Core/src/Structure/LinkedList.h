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
    template <typename T>
    struct LinkedListNode
    {
        LinkedListNode(
            const T&              data,
            LinkedListNode*       pNext)
            : m_data(data), m_pNext(pNext) {}

        T m_data;

        union
        {
            LinkedListNode<T>* m_pNext; // Pointer to next node
            LinkedListNode<T>* m_pMask; // XOR linked list mask
        };
    };

    template <typename T>
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

    template <typename T>
    class LinkedListBase
    {
    public:
        LinkedListBase()
        {
            m_pHead = NULL;
            m_pTail = NULL;
            m_length = 0;
        }

        ~LinkedListBase()
        {
            m_pHead = NULL;
            m_pTail = NULL;
        }

        // Modify functions
        virtual void insertBack(const T& data) = 0;
        virtual void insertFront(const T& data) = 0;
        virtual void insert(const UINT i, const T& data) = 0;

        virtual void removeBack() = 0;
        virtual void removeFront() = 0;
        virtual void removeNode(const UINT i) = 0;

        virtual void clear() = 0;

        virtual void modify(const UINT i,
            const T& data) = 0;

        // Data functions
        virtual const T& get(const UINT i) const = 0;

    protected:
        virtual T& getRef(const UINT i) = 0;

        LinkedListNode<T>* m_pHead;
        LinkedListNode<T>* m_pTail;

        UINT m_length;
    };

    template <typename T>
    class LinkedList : public LinkedListBase<T>
    {
    public:
        LinkedList() {}
        ~LinkedList() {}

        void insertBack(const T& data);
        void insertFront(const T& data);
        void insert(const UINT i, const T& data);

        void removeBack();
        void removeFront();
        void removeNode(const UINT i);

        void clear();

        void modify(const UINT i,
            const T& data);

        const T& get(const UINT i) const;

    private:
        T& getRef(const UINT i);
    };

    template<typename T>
    class XorLinkedList : public LinkedListBase<T>
    {
    public:
        XorLinkedList() {}
        ~XorLinkedList() {}

        void insertBack(const T& data);
        void insertFront(const T& data);
        void insert(const UINT i, const T& data);

        void removeBack();
        void removeFront();
        void removeNode(const UINT i);

        void clear();

        void modify(const UINT i,
            const T& data);

        const T& get(const UINT i) const;

    private:
        T& getRef(const UINT i);
    };
}

#include "LinkedList.inl"
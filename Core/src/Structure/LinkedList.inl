#include "LinkedList.h"
//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

// LinkedList implementation
namespace Structure
{
    template<typename T>
    INLINE void LinkedList<T>::insertBack(const T& data)
    {
        LinkedListNode<T>* newNode = new LinkedListNode<T>(data, NULL);

        if (m_pHead != NULL)
        {
            m_pTail->m_pNext           = newNode;
            m_pTail                    = newNode;
        }
        else
        {
            m_pHead = newNode;
            m_pTail = m_pHead;
        }

        m_length++;
    }

    template<typename T>
    INLINE void LinkedList<T>::insertFront(const T& data)
    {
    }

    template<typename T>
    INLINE void LinkedList<T>::insert(const UINT i, const T& data)
    {
    }

    template<typename T>
    INLINE void LinkedList<T>::removeBack()
    {
    }

    template<typename T>
    INLINE void LinkedList<T>::removeFront()
    {
    }

    template<typename T>
    INLINE void LinkedList<T>::removeNode(const UINT i)
    {
    }

    template<typename T>
    INLINE void LinkedList<T>::clear()
    {
        LinkedListNode<T>** ppCur = &m_pHead;
        LinkedListNode<T>** ppNext;

        while (*ppCur != NULL)
        {
            LinkedListNode<T>* pNext = (*ppCur)->m_pNext;
            ppNext = &pNext;

            SafeDelete(*ppCur);
            *ppCur = NULL;

            *ppCur = *ppNext;
        }

        m_length = 0;
    }
}

// XorLinkedList implementation
namespace Structure
{
    template<typename T>
    INLINE void XorLinkedList<T>::insertBack(
        const T& data)
    {
        LinkedListNode<T>* newNode = new LinkedListNode<T>(data, NULL);

        if (m_pHead != NULL)
        {
            if (m_pHead == m_pTail)
            {
                m_pHead->m_pMask = newNode;
                m_pTail = newNode;
                m_pTail->m_pMask = m_pHead;
            }
            else
            {
                LinkedListNode<T>* prevNode = m_pTail->m_pMask;
                m_pTail->m_pMask = reinterpret_cast<LinkedListNode<T>*>(
                    reinterpret_cast<uintptr_t>(prevNode) ^ reinterpret_cast<uintptr_t>(newNode));
                m_pTail = newNode;
            }
        }
        else
        {
            m_pHead = newNode;
            m_pTail = m_pHead;
        }
    }

    template<typename T>
    INLINE void XorLinkedList<T>::insertFront(
        const T& data)
    {

    }

    template<typename T>
    INLINE void XorLinkedList<T>::insert(
        const UINT i,
        const T&   data)
    {
    }

    template<typename T>
    INLINE void XorLinkedList<T>::removeBack()
    {

    }

    template<typename T>
    INLINE void XorLinkedList<T>::removeFront()
    {

    }

    template<typename T>
    INLINE void XorLinkedList<T>::removeNode(
        const UINT i)
    {

    }

    template<typename T>
    INLINE void XorLinkedList<T>::clear()
    {
        LinkedListNode<T>** ppPrev = &m_pHead;
        LinkedListNode<T>*  pCur   = ((m_pHead != NULL) ? (m_pHead->m_pMask) : NULL);

        if (m_pHead != NULL && pCur == NULL)
        {
            SafeDelete(m_pHead);
            m_pHead = NULL;
        }

        LinkedListNode<T>* tempPCur;
        while (*ppPrev != m_pTail)
        {
            LinkedListNode<T>* pNext = reinterpret_cast<LinkedListNode<T>*>(
                reinterpret_cast<uintptr_t>(*ppPrev) ^ reinterpret_cast<uintptr_t>(pCur->m_pMask));

            SafeDelete(*ppPrev);
            *ppPrev = NULL;

            tempPCur = pCur;
            ppPrev   = &tempPCur;
            pCur     = pNext;

            // Release the last element
            if (*ppPrev == m_pTail)
            {
                SafeDelete(*ppPrev);
                *ppPrev = NULL;
                m_pTail = NULL;
            }
        }
    }
}
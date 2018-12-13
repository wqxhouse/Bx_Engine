//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

namespace Structure
{
    template<typename T>
    INLINE LinkedList<T>::LinkedList()
    {
        m_pHead  = NULL;
        m_pTail  = NULL;
        m_length = 0;
    }

    template<typename T>
    INLINE LinkedList<T>::~LinkedList()
    {
        clear();
    }

    template<typename T>
    INLINE void LinkedList<T>::insertBack(const T& data)
    {
        if (m_pHead != NULL)
        {
            LinkedListNode<T>* newNode = new LinkedListNode<T>(std::move(data), NULL);
            m_pTail->m_pNext           = newNode;
            m_pTail                    = newNode;
        }
        else
        {
            m_pHead = new LinkedListNode<T>(std::move(data), NULL);
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

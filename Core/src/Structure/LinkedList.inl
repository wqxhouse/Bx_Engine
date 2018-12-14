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
    INLINE void LinkedList<T>::insertBack(
        const T& data)
    {
        LinkedListNode<T>* newNode = new LinkedListNode<T>(data, NULL);

        if (m_pHead != NULL)
        {
            m_pTail->m_pNext = newNode;
            m_pTail = newNode;
        }
        else
        {
            m_pHead = newNode;
            m_pTail = m_pHead;
        }

        m_length++;
    }

    template<typename T>
    INLINE void LinkedList<T>::insertFront(
        const T& data)
    {
        LinkedListNode<T>* newNode = new LinkedListNode<T>(data, NULL);

        if (m_pHead != NULL)
        {
            newNode->m_pNext = m_pHead;
            m_pHead = newNode;
        }
        else
        {
            m_pTail = newNode;
            m_pHead = m_pTail;
        }

        m_length++;
    }

    template<typename T>
    INLINE void LinkedList<T>::insert(
        const UINT i,
        const T&   data)
    {
        assert(i <= m_length);

        if (i == 0) { insertFront(data); }
        else if (i == m_length) { insertBack(data); }
        else
        {
            LinkedListNode<T>* pPrev = m_pHead;

            for (UINT j = 0; j < i; ++j)
            {
                pPrev = pPrev->m_pNext;
            }

            assert(pPrev != NULL);

            LinkedListNode<T>* pCur = pPrev->m_pNext;

            LinkedListNode<T>* newNode = new LinkedListNode<T>(data, NULL);

            pPrev->m_pNext = newNode;
            newNode->m_pNext = pCur;

            m_length++;
        }
    }

    template<typename T>
    INLINE void LinkedList<T>::removeBack()
    {
        assert(m_length > 0);

        LinkedListNode<T>* ptr = m_pHead;

        if (ptr != m_pTail)
        {
            while (ptr->m_pNext != m_pTail)
            {
                ptr = ptr->m_pNext;
            }
            SafeDelete(m_pTail);

            ptr->m_pNext = NULL;
            m_pTail = ptr;
        }
        else
        {
            SafeDelete(ptr);
            m_pHead = NULL;
            m_pTail = NULL;
        }

        m_length--;
    }

    template<typename T>
    INLINE void LinkedList<T>::removeFront()
    {
        assert(m_length > 0);

        LinkedListNode<T>* ptr = m_pHead->m_pNext;
        SafeDelete(m_pHead);
        m_pHead = ptr;

        m_length--;
    }

    template<typename T>
    INLINE void LinkedList<T>::removeNode(
        const UINT i)
    {
        assert(i <= m_length);

        if (i == 0) { removeFront(); }
        else if (i == m_length) { removeBack(); }
        else
        {
            LinkedListNode<T>* pPrev = m_pHead;

            for (UINT j = 0; j < i - 1; ++j)
            {
                pPrev = pPrev->m_pNext;
            }

            assert(pPrev != NULL);

            LinkedListNode<T>* pCur = pPrev->m_pNext;

            pPrev->m_pNext = ((pCur != NULL) ? (pCur->m_pNext) : NULL);
        }

        m_length--;
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

    template<typename T>
    INLINE void LinkedList<T>::modify(
        const UINT i,
        const T&   data)
    {
        assert(i < m_length);

        T& oriData = getRef(i);
        oriData = data;
    }

    template<typename T>
    inline const T& LinkedList<T>::get(
        const UINT i) const
    {
        assert(i < m_length);

        LinkedListNode<T>* ptr = m_pHead;
        for (UINT j = 0; j < i; ++j)
        {
            ptr = ptr->m_pNext;
        }

        return ptr->m_data;
    }

    template<typename T>
    inline T& LinkedList<T>::getRef(
        const UINT i)
    {
        assert(i < m_length);

        LinkedListNode<T>* ptr = m_pHead;
        for (UINT j = 0; j < i; ++j)
        {
            ptr = ptr->m_pNext;
        }

        return ptr->m_data;
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
                newNode->m_pMask = m_pTail;
                m_pTail = newNode;
            }
        }
        else
        {
            m_pHead = newNode;
            m_pTail = m_pHead;
        }

        m_length++;
    }

    template<typename T>
    INLINE void XorLinkedList<T>::insertFront(
        const T& data)
    {
        LinkedListNode<T>* newNode = new LinkedListNode<T>(data, NULL);

        if (m_pHead != NULL)
        {
            if (m_pHead == m_pTail)
            {
                m_pTail->m_pMask = newNode;
                m_pHead = newNode;
                m_pHead->m_pMask = m_pTail;
            }
            else
            {
                LinkedListNode<T>* nextNode = m_pHead->m_pMask;
                m_pHead->m_pMask = reinterpret_cast<LinkedListNode<T>*>(
                    reinterpret_cast<uintptr_t>(nextNode) ^ reinterpret_cast<uintptr_t>(newNode));
                newNode->m_pMask = m_pHead;
                m_pHead = newNode;
            }
        }
        else
        {
            m_pHead = newNode;
            m_pTail = m_pHead;
        }

        m_length++;
    }

    template<typename T>
    INLINE void XorLinkedList<T>::insert(
        const UINT i,
        const T&   data)
    {
        // Insert in the middle will cause updating the whole list, we shouldn't support this operation
        NotSupported();
    }

    template<typename T>
    INLINE void XorLinkedList<T>::removeBack()
    {
        if (m_length > 1)
        {
            LinkedListNode<T>* pCur = m_pTail->m_pMask;
            LinkedListNode<T>* pPrev = reinterpret_cast<LinkedListNode<T>*>(
                reinterpret_cast<uintptr_t>(m_pTail) ^ reinterpret_cast<uintptr_t>(pCur->m_pMask));

            SafeDelete(m_pTail);

            pCur->m_pMask = pPrev;
            m_pTail = pCur;
        }
        else
        {
            SafeDelete(m_pTail);
            m_pHead = NULL;
            m_pTail = NULL;
        }

        m_length--;
    }

    template<typename T>
    INLINE void XorLinkedList<T>::removeFront()
    {
        if (m_length > 1)
        {
            LinkedListNode<T>* pCur = m_pHead->m_pMask;
            LinkedListNode<T>* pNext = reinterpret_cast<LinkedListNode<T>*>(
                reinterpret_cast<uintptr_t>(m_pHead) ^ reinterpret_cast<uintptr_t>(pCur->m_pMask));

            SafeDelete(m_pHead);

            pCur->m_pMask = pNext;
            m_pHead = pCur;
        }
        else
        {
            SafeDelete(m_pTail);
            m_pHead = NULL;
            m_pTail = NULL;
        }

        m_length--;
    }

    template<typename T>
    INLINE void XorLinkedList<T>::removeNode(
        const UINT i)
    {
        // Remove in the middle will cause updating the whole list, we shouldn't support this operation
        NotSupported();
    }

    template<typename T>
    INLINE void XorLinkedList<T>::clear()
    {
        LinkedListNode<T>** ppPrev = &m_pHead;
        LinkedListNode<T>*  pCur = ((m_pHead != NULL) ? (m_pHead->m_pMask) : NULL);

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
            ppPrev = &tempPCur;
            pCur = pNext;

            // Release the last element
            if (*ppPrev == m_pTail)
            {
                SafeDelete(*ppPrev);
                *ppPrev = NULL;
                m_pTail = NULL;
            }
        }
    }
    template<typename T>
    inline void XorLinkedList<T>::modify(
        const UINT i,
        const T&   data)
    {
        assert(i < m_length);

        T& oriData = getRef(i);
        oriData = data;
    }

    template<typename T>
    inline const T& XorLinkedList<T>::get(
        const UINT i) const
    {
        assert(i < m_length);

        UINT halfIndex = (m_length >> 1);

        LinkedListNode<T>* resPtr = NULL;

        if (i < halfIndex)
        {
            LinkedListNode<T>* pPrev = m_pHead;
            LinkedListNode<T>* pCur = ((m_pHead != NULL) ? (m_pHead->m_pMask) : NULL);
            LinkedListNode<T>* pNext;

            if (i == 0) { pCur = pPrev; }
            else
            {
                for (UINT j = 0; j < i - 1; ++j)
                {
                    pNext = reinterpret_cast<LinkedListNode<T>*>(
                        reinterpret_cast<uintptr_t>(pPrev) ^ reinterpret_cast<uintptr_t>(pCur->m_pMask));

                    pPrev = pCur;
                    pCur = pNext;
                }
            }

            resPtr = pCur;
        }
        else
        {
            LinkedListNode<T>* pNext = m_pTail;
            LinkedListNode<T>* pCur = ((m_pTail != NULL) ? (m_pTail->m_pMask) : NULL);
            LinkedListNode<T>* pPrev;

            if (i == m_length - 1) { pCur = pNext; }
            else
            {
                UINT indexFromTail = m_length - i - 1;
                for (UINT j = 0; j < indexFromTail - 1; ++j)
                {
                    pPrev = reinterpret_cast<LinkedListNode<T>*>(
                        reinterpret_cast<uintptr_t>(pNext) ^ reinterpret_cast<uintptr_t>(pCur->m_pMask));

                    pNext = pCur;
                    pCur = pPrev;
                }
            }

            resPtr = pCur;
        }

        return resPtr->m_data;
    }

    template<typename T>
    inline T& XorLinkedList<T>::getRef(
        const UINT i)
    {
        assert(i < m_length);

        UINT halfIndex = (m_length >> 1);

        LinkedListNode<T>* resPtr = NULL;

        if (i < halfIndex)
        {
            LinkedListNode<T>* pPrev = m_pHead;
            LinkedListNode<T>* pCur = ((m_pHead != NULL) ? (m_pHead->m_pMask) : NULL);
            LinkedListNode<T>* pNext;

            if (i == 0) { pCur = pPrev; }
            else
            {
                for (UINT j = 0; j < i - 1; ++j)
                {
                    pNext = reinterpret_cast<LinkedListNode<T>*>(
                        reinterpret_cast<uintptr_t>(pPrev) ^ reinterpret_cast<uintptr_t>(pCur->m_pMask));

                    pPrev = pCur;
                    pCur = pNext;
                }
            }

            resPtr = pCur;
        }
        else
        {
            LinkedListNode<T>* pNext = m_pTail;
            LinkedListNode<T>* pCur = ((m_pTail != NULL) ? (m_pTail->m_pMask) : NULL);
            LinkedListNode<T>* pPrev;

            if (i == m_length - 1) { pCur = pNext; }
            else
            {
                UINT indexFromTail = m_length - i - 1;
                for (UINT j = 0; j < indexFromTail - 1; ++j)
                {
                    pPrev = reinterpret_cast<LinkedListNode<T>*>(
                        reinterpret_cast<uintptr_t>(pNext) ^ reinterpret_cast<uintptr_t>(pCur->m_pMask));

                    pNext = pCur;
                    pCur = pPrev;
                }
            }

            resPtr = pCur;
        }

        return resPtr->m_data;
    }
}
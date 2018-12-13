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
    INLINE Queue<T>::Queue(
        const UINT capacity,
        const BOOL isRing)
        : m_capacity(capacity),
        m_isRing(isRing)
    {
        m_pDataQueue = static_cast<T*>(malloc(capacity * sizeof(T));
        m_front = m_back = 0;

        m_size = 0;
    }

    template<typename T>
    INLINE Queue<T>::~Queue()
    {
        free(static_cast<void*>(m_pDataQueue));
    }

    template<typename T>
    INLINE void Queue<T>::push(
        const T& val)
    {
        assert(m_size <= m_capacity);

        m_pDataQueue[m_back] = val;
        m_back++;

        m_size++;

        if (m_back >= m_capacity)
        {
            if (m_isRing == TRUE)
            {
                m_back = 0;
            }
            else
            {
                printf("The queue is full!\n");
                assert(FALSE);
            }
        }
    }

    template<typename T>
    INLINE T Queue<T>::pop()
    {
        T& result = m_pDataQueue[m_front];
        m_size--;

        if (m_isRing == TRUE)
        {
            if (m_isRing == TRUE)
            {
                m_front = 0;
            }
            else
            {
                printf("The queue has been used up!\n");
                assert(FALSE);
            }
        }

        return result;
    }
}
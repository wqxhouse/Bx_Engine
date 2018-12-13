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
    template<typename T, size_t SIZE>
    INLINE QueueFixed<T, SIZE>::QueueFixed(
        const BOOL isRing)
        : m_capacity(SIZE),
          m_isRing(isRing)
    {
        m_front = 0;
        m_back  = 0;
        m_size  = 0;
    }

    template<typename T, size_t SIZE>
    INLINE QueueFixed<T, SIZE>::~QueueFixed()
    {
    }

    template<typename T, size_t SIZE>
    INLINE void QueueFixed<T, SIZE>::push(
        const T& val)
    {
        assert(m_size <= m_capacity);

        m_dataQueue[m_back] = val;
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

    template<typename T, size_t SIZE>
    INLINE T QueueFixed<T, SIZE>::pop()
    {
        T& result = m_dataQueue[m_front++];
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
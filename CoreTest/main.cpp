//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#include <iostream>

#include <Structure/LinkedList.h>
#include <Structure/Queue.h>

using namespace Structure;

int main()
{
    Memory::MemoryPool m_memPool(1024);

    Memory::MemoryPoolAllocator m_allocator(&m_memPool, 3, sizeof(int), 0, Memory::Allocator::DEFAULT_ALIGNMENT_SIZE);
    Memory::MemoryPoolArena arena(&m_allocator);

    int* a = BX_NEW(int, arena)(5);
    int *b = BX_NEW(int, arena)(10);
    int *c = BX_NEW(int, arena)(100);

    BX_DELETE(a, &arena);
    BX_DELETE(b, &arena);
    BX_DELETE(c, &arena);

    //arena.clear();

    a = BX_NEW(int, arena)(5);
    b = BX_NEW(int, arena)(10);
    c = BX_NEW(int, arena)(100);

    BX_RELEASE(arena);

    LinkedList<int> list;
    list.insert(0, 1);
    list.insertBack(2);
    list.insertFront(3);

    int val = list.get(0);
    val     = list.get(1);
    val     = list.get(2);

    LinkedList<int> newList = list.copy();
    newList.insertBack(4);
    newList.clear();

    list.removeBack();
    list.removeFront();
    list.insertBack(4);
    list.removeFront();

    list.clear();

    Structure::XorLinkedList<int> xorList;
    xorList.insertBack(3);
    xorList.insertBack(4);
    xorList.insertBack(5);
    xorList.insertFront(2);
    xorList.insertFront(1);

    val = xorList.get(0);
    val = xorList.get(1);
    val = xorList.get(2);
    val = xorList.get(3);
    val = xorList.get(4);

    xorList.removeBack();

    val = xorList.get(0);
    val = xorList.get(1);
    val = xorList.get(2);
    val = xorList.get(3);

    xorList.removeFront();

    val = xorList.get(0);
    val = xorList.get(1);
    val = xorList.get(2);

    xorList.insertBack(6);

    val = xorList.get(0);
    val = xorList.get(1);
    val = xorList.get(2);
    val = xorList.get(3);

    xorList.removeFront();

    val = xorList.get(0);
    val = xorList.get(1);
    val = xorList.get(2);

    xorList.clear();

    return 0;
}
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
    /*LinkedList<int> list;
    list.insert(0, 1);
    list.insertBack(2);
    list.insertFront(3);

    int val = list.get(0);
    val     = list.get(1);
    val     = list.get(2);

    list.removeBack();
    list.removeFront();
    list.insertBack(4);
    list.removeFront();

    list.clear();*/

    Structure::XorLinkedList<int> list;
    list.insertBack(3);
    list.insertBack(4);
    list.insertBack(5);
    list.insertFront(2);
    list.insertFront(1);

    int val = list.get(0);
    val = list.get(1);
    val = list.get(2);
    val = list.get(3);
    val = list.get(4);

    list.removeBack();

    val = list.get(0);
    val = list.get(1);
    val = list.get(2);
    val = list.get(3);

    list.removeFront();

    val = list.get(0);
    val = list.get(1);
    val = list.get(2);

    list.insertBack(6);

    val = list.get(0);
    val = list.get(1);
    val = list.get(2);
    val = list.get(3);

    list.removeFront();

    val = list.get(0);
    val = list.get(1);
    val = list.get(2);

    list.clear();

    return 0;
}
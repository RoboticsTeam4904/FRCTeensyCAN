#ifndef __LinkedList_H__
#define __LinkedList_H__

template <typename T>
struct LinkedListNode {
	T * data;
	LinkedListNode<T> * next;
};

#endif // __LinkedList_H__

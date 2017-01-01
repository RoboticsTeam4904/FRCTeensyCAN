#ifndef __LinkedList_H__
#define __LinkedList_H__

/**
   Struct for a node in a LinkedList
   A LinkedListNode is of a certain
   type, T.
 */
template <typename T>
struct LinkedListNode {
	/**
	   Pointer to the data
	 */
	T * data;
	/**
	   Pointer to the next
	   node in the list
	*/
	LinkedListNode<T> * next;
};

#endif // __LinkedList_H__

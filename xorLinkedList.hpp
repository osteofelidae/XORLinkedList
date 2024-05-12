#pragma once

// === DEPENDENCIES ===========================================================
#include <cstddef>
#include <cstdint>
#include <functional>
#include <stdexcept>
#include <iostream>

// === NODE CLASS =============================================================
template <typename T> class Node {

	// --- Private members ----------------------------------------------------
	private:
		uintptr_t ptr;  // XOR value of previous and next
		T data;

	// --- Public members -----------------------------------------------------
	public:
		Node();  // Constructor
		Node(T data);  // Constructor with data
		~Node();  // Destructor

		void setData(T data);  // Set data
		T getData();  // Get data

		Node *getNode(Node *other);  // Get previous or next node

		void link(Node *prev, Node *next);  // Link to previous and next node
		void relink(Node *node, Node *old);  // Relink one side of the node
		void unlink();  // Remove all links
};

// --- Constructor ------------------------------------------------------------
template <typename T> Node<T>::Node() : Node(0) {}

// --- Constructor with data --------------------------------------------------
template <typename T> Node<T>::Node(T data) {
	this->data = data;  // Set data to given value
	this->ptr = (uintptr_t)NULL;  // Set ptr
}

// --- Destructor -------------------------------------------------------------
template <typename T> Node<T>::~Node() {
	
}

// --- Set data ---------------------------------------------------------------
template <typename T> void Node<T>::setData(T data) {
	this->data = data;  // Set data to given value
}

// --- Get data ---------------------------------------------------------------
template <typename T> T Node<T>::getData() {
	return data;  // Return data
}

// --- Get previous or next node ----------------------------------------------
template <typename T> Node<T> *Node<T>::getNode(Node *other) {
	Node<T> *result = (Node<T> *)((uintptr_t)this->ptr ^ (uintptr_t)other);
	return result;
}

// --- Link to previous and next node -----------------------------------------
template <typename T> void Node<T>::link(Node *prev, Node *next) {
	this->ptr = (uintptr_t)prev ^ (uintptr_t)next;  // Set this->ptr to xor of previous and next
}

// --- Relink one side of the node --------------------------------------------
template <typename T> void Node<T>::relink(Node *node, Node *old) {
	this->ptr = this->ptr ^ (uintptr_t)old ^ (uintptr_t)node;  // Relink pointer
}

// --- Unlink -----------------------------------------------------------------
template <typename T> void Node<T>::unlink() {
	this->ptr = (uintptr_t)NULL;  // Set this->ptr to NULL
}

// === LINKED LIST CLASS ======================================================
template <typename T> class XorLinkedList {
	// --- Private members ----------------------------------------------------
	private:
		int size;  // Length of list
		Node<T> *head;  // Head node
		Node<T> *tail;  // Tail node

		Node<T> *cachedPrev;  // Cached previous node
		Node<T> *cachedCurr;  // Cached current node
		int cachedIndex;  // Cached index

	// --- Public members -----------------------------------------------------
	public:
		XorLinkedList();  // Constructor
		~XorLinkedList();  // Destructor

		int getSize();  // Get size
		bool isEmpty();  // Whether list is empty or not

		bool contains(T data);  // Whether list contains an element

		void add(T data);  // Add specified data to list
		void insert(T data, int index);  // Insert data into index
		void remove(T data);  // Remove first instance of specified data to list
		T extract(int index);  // Delete node at index (and return it)
		void set(T data, int index);  // Set data at index

		void clear();  // Remove all items from list
		void reverse();  // Reverse list

		T get(int index);  // Get item at index
		int getIndex(T data);  // Get index of item

		bool traverse(std::function<void(Node<T> *, Node<T> *, bool *, int)> func,
					  auto match, int matchType = -1, bool updateCache = true);
		void resetCache();  // Reset internal cache
		void updateCacheIndex(int index, bool increased, Node<T> *newNode = NULL);  // Update cached index

		void print();  // Print, for diagnostic purposes
};

// --- Constructor ------------------------------------------------------------
template <typename T> XorLinkedList<T>::XorLinkedList() {
	this->size = 0;  // Initialize instance variables
	this->head = (uintptr_t)NULL;
	this->tail = (uintptr_t)NULL;
	this->cachedIndex = -1;
}

// --- Destructor -------------------------------------------------------------
template <typename T> XorLinkedList<T>::~XorLinkedList() {
	this->clear();
}

// --- Get size ---------------------------------------------------------------
template <typename T> int XorLinkedList<T>::getSize() {
	return this->size;  // Return size
}

// --- Is empty ---------------------------------------------------------------
template <typename T> bool XorLinkedList<T>::isEmpty() {
	return this->size == 0;
}

// --- Contains ---------------------------------------------------------------
template <typename T> bool XorLinkedList<T>::contains(T data) {

	// Result variable
	bool result = false;

	// Traverse using lambda checking function
	this->traverse([&](Node<T> *node, Node<T> *prev, bool *interrupt, int index) {

		// If node's data matches
		if (node->getData() == data) {

			//Interrupt traverse loop
			*interrupt = true;

			// Return true
			result = true;
		}
	}, data, 1);


	return result;
}

// --- Add --------------------------------------------------------------------
template <typename T> void XorLinkedList<T>::add(T data) {

	// Create node
	Node<T> *node = new Node<T>(data);
	
	// Special case: is first element
	if (this->isEmpty()) {
		this->head = node;
	} else {

		// Node pointers
		Node<T> *prev;
		Node<T> *curr;
		int index;
		
		// Traverse and find node
		this->traverse([&](Node<T> *node, Node<T> *last, bool *interrupt, int i) {
			if (i == this->size-1) {
				prev = last;
				curr = node;
				index = i;
				*interrupt = true;
			}
		}, this->size-2, 0);

		// Set next node
		curr->link(prev, node);
		node->link(curr, NULL);
	}

	// Increment size
	this->size += 1;

	// Set head
	this->tail = node;
}

// --- Insert -----------------------------------------------------------------
template <typename T> void XorLinkedList<T>::insert(T data, int index) {

	// Check if index is out of range
	if (index > this->size)
		throw std::out_of_range("List index out of range");

	// New node
	Node<T> *newNode = new Node(data);

	// Return node
	Node<T> *prev;
	Node<T> *curr;

	// Special case: first node
	if (index == 0) {

		// Link head with new node and vice versa
		this->head->relink(newNode, NULL);
		newNode->link(NULL, this->head);

		// Assign new head
		this->head = newNode;

		// Increment size
		this->size += 1;

		return;

	} else {
	
		// Traverse until correct node found
		this->traverse([&](Node<T> *node, Node<T> *last, bool *interrupt, int i) {
			if (i == index-1) {
				prev = last;
				curr = node;
				*interrupt = true;
			}
		}, index-1, 0, false);

		// Get next node
		Node<T> *next = curr->getNode(prev);

		// Relink curr from next to newNode
		curr->relink(newNode, next);

		// Link new node to correct nodes
		newNode->link(curr, next);

		// If last node
		if (index == this->size) {

			// Set tail
			this->tail = newNode;

		// If not last node
		} else {

			// Relink next node
			next->relink(newNode, curr);
		}
		
	}

	// Update cache index
	this->updateCacheIndex(index, true, newNode);

	// Increment size
	this->size += 1;
}

// --- Remove -----------------------------------------------------------------
template <typename T> void XorLinkedList<T>::remove(T data) {

	// Node pointers
	Node<T> *prev;
	Node<T> *curr;
	int index;
	
	// Traverse and find node
	bool found = this->traverse([&](Node<T> *node, Node<T> *last, bool *interrupt, int i) {
		if (data == node->getData()) {
			prev = last;
			curr = node;
			index = i;
			*interrupt = true;
		}
	}, data, 1, false);

	// If found
	if (found) {

		// Next node
		Node<T> *next = curr->getNode(prev);

		// Special case: first element
		if (index == 0) {

			// Set head
			this->head = next;

		} else {

			// Relink previous
			prev->relink(next, curr);
		}

		// Special case: last element
		if (index == this->size-1) {

			// Set tail
			this->tail = prev;

		} else {

			// Relink next
			next->relink(prev, curr);
		}

		// Delete victim
		delete curr;

		// Decrement size
		this->size -= 1;

		// Update cache index
		this->updateCacheIndex(index, false);

	// If not found, throw an exception
	} else
		throw std::runtime_error("Element not found");

}

// --- Extract ----------------------------------------------------------------
template <typename T> T XorLinkedList<T>::extract(int index) {

	// Check if index is out of range
	if (index > this->size-1)
		throw std::out_of_range("List index out of range");

	// Node pointers
	Node<T> *prev;
	Node<T> *curr;
	T data;
	
	// Traverse and find node
	this->traverse([&](Node<T> *node, Node<T> *last, bool *interrupt, int i) {
		if (index == i) {
			prev = last;
			curr = node;
			data = node->getData();
			*interrupt = true;
		}
	}, index, 0, false);

	// Next node
	Node<T> *next = curr->getNode(prev);

	// Special case: first element
	if (index == 0) {

		// Set head
		this->head = next;

	} else {

		// Relink previous
		prev->relink(next, curr);
	}

	// Special case: last element
	if (index == this->size-1) {

		// Set tail
		this->tail = prev;

	} else {

		// Relink next
		next->relink(prev, curr);
	}

	// Delete victim
	delete curr;

	// Decrement size
	this->size -= 1;

	// Update cache index
	this->updateCacheIndex(index, false);

	// Return found data
	return data;

}

// --- Set --------------------------------------------------------------------
template <typename T> void XorLinkedList<T>::set(T data, int index) {

	// Check if index is out of range
	if (index > this->size-1)
		throw std::out_of_range("List index out of range");

	// Node pointers
	Node<T> *prev;
	Node<T> *curr;
	
	// Traverse and find node
	this->traverse([&](Node<T> *node, Node<T> *last, bool *interrupt, int i) {
		if (index == i) {
			prev = last;
			curr = node;
			*interrupt = true;
		}
	}, index, 0);

	// Set data
	curr->setData(data);

}

// --- Clear ------------------------------------------------------------------
template <typename T> void XorLinkedList<T>::clear() {
	
	// Traverse and delete all nodes
	this->traverse([](Node<T> *node, Node<T> *last, bool *interrupt, int i) {

		// Delete previous node
		delete last;
	}, 0);

	// Delete tail node
	delete this->tail;

	// Reset cache
	this->resetCache();

	// Reset size
	this->size = 0;

}

// --- Reverse ----------------------------------------------------------------
template <typename T> void XorLinkedList<T>::reverse() {

	// Swap head and tail nodes
	Node<T> *temp = this->head;
	this->head = this->tail;
	this->tail = temp;

	// Reset cache
	this->resetCache();
}

// --- Get --------------------------------------------------------------------
template <typename T> T XorLinkedList<T>::get(int index) {

	// Check if index is out of range
	if (index > this->size-1)
		throw std::out_of_range("List index out of range");

	// Node pointer
	Node<T> *curr;
	
	// Traverse and find node
	this->traverse([&](Node<T> *node, Node<T> *last, bool *interrupt, int i) {
		if (index == i) {
			curr = node;
			*interrupt = true;
		}
	}, index, 0);

	// Return data
	return curr->getData();
	
}

// --- Get index --------------------------------------------------------------
template <typename T> int XorLinkedList<T>::getIndex(T data) {

	// Result variable
	int result = -1;

	// Traverse using lambda checking function
	this->traverse([&](Node<T> *node, Node<T> *prev, bool *interrupt, int index) {

		// If node's data matches
		if (node->getData() == data) {

			//Interrupt traverse loop
			*interrupt = true;

			// Return true
			result = index;
		}
	}, data, 1);

	return result;
}

// --- Traverse ---------------------------------------------------------------
template <typename T> bool XorLinkedList<T>::traverse(
	std::function<void (Node<T> *, Node<T> *, bool *, int)> func,  // Function takes 3 args: the current node, a variable to update when done prematurely, and the current loop index
	auto match,
	int matchType,  // Type of cache matching to perform; -1=none, 0=index, 1=data
	bool updateCache
) {

	// If empty, throw exception
	if (this->isEmpty())
		throw std::out_of_range("List is empty");

	// Interrupt variable
	bool interrupt = false;

	// If cache is set and there is a match
	if (this->cachedIndex != -1
		&& ((matchType == 0 && match == this->cachedIndex) 
			|| (matchType == 1 && match == this->cachedCurr->getData()))) {

		// Call function on cached node
		func(this->cachedCurr, this->cachedPrev, &interrupt, this->cachedIndex);

	}

	// Loop variables
	Node<T> *temp;
	Node<T> *prev = (uintptr_t)NULL;
	Node<T> *curr = head;

	// Iterate over list until done or interrrupted
	for (int i = 0; i < this->size && !interrupt; i++) {

		// Call function on current node
		func(curr, prev, &interrupt, i);

		// Set cache if interrupted and allowed to
		if (interrupt && updateCache) {
			this->cachedCurr = curr;
			this->cachedPrev = prev;
			this->cachedIndex = i;

			// TODO TEMP
			//std::cout << "Cache set: node " << this->cachedIndex << std::endl;
		}

		// Set curr to next node
		temp = curr;
		curr = curr->getNode(prev);
		prev = temp;
	}

	// if interrupted == true, it seems fine
	return interrupt;
}

// --- Reset cache --------------------------------------------------------------------------------
template <typename T> void XorLinkedList<T>::resetCache() {
	this->cachedIndex = -1;
}

// --- Update cache index -------------------------------------------------------------------------
template <typename T> void XorLinkedList<T>::updateCacheIndex(int index, bool increased, Node<T> *newNode) {

	// If cached node or prev is deleted
	if ((index == this->cachedIndex || index == this->cachedIndex-1) && !increased) {

		// Reset
		this->resetCache();

	}
	
	// If index is before or equal to cached (i.e. if cached index would have changed)
	if (index <= this->cachedIndex) {

		// Update index according to whether a node is added or removed
		if (increased) {
			this->cachedIndex += 1;

			// If updated node is cached prev
			if (index == cachedIndex-1) {

				// Set cached prev to new node
				this->cachedPrev = newNode;
			}

		} else {
			this->cachedIndex -= 1;
		}
	}
}

// --- Print --------------------------------------------------------------------------------------
template <typename T> void XorLinkedList<T>::print() {

	// Traverse using lambda print function
	this->traverse([](Node<T> *node, Node<T> *prev, bool *interrupt, int index) {
		std::cout << "[" << node->getData() << "] ";
	}, 0);

	// New line
	std::cout << std::endl;
}
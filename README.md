## XOR Linked List
`template <typename T> class XorLinkedList { ...`\
The XOR Linked List implements similar methods to a normal doubly-linked list, but using one pointer per node. This pointer contains the result of XORing the addresses of the previous and the next node (`prev ^ next`), allowing for some really cool tricks like being able to reverse the list simply by swapping the start and end nodes, or getting the next and previous node using the exact same logic. Additionally, I have implemented a caching system, making repeated function calls on nodes much more efficient (`O(n), O(1), O(1), O(1), ...` instead of `O(n), O(n), O(n), O(n), ...`). This caching system is baked into a method called `traverse`, which automates the `O(n)` sequential traversal of the list and allows users to make custom traversal functions with much less boilerplate. Enjoy!\

### Traversal function & caching system
The traversal function allows the user to define a lambda function which is then applied to each node in the list.\

**Lambda function**\
The lambda function is applied to each node in the list, and must take the following parameters:
* `Node<T> *node`: current node.
* `Node<T> *prev`: previous node (usually used to calculate the next node, if needed).
* `bool *interrupt`: if set to true, this boolean will interrupt the loop and end it early.
* `int index`: index of current node.\
In other words, the lambda function must be of this form *exactly*:
```cpp
<snip> 
	[](Node<T> *node, Node<T> *last, bool *interrupt, int i) {
		// FUNCTION BODY
	}
</snip>
```

**Internal definition & arguments**\
`bool traverse(std::function<void(Node<T> *, Node<T> *, bool *, int)> func, auto match, int matchType = -1, bool updateCache = true);`
* `std::function<void(Node<T> *, Node<T> *, bool *, int)> func`: lambda function.
* `auto match`: the value (index or data) for the cache to match. If you do not intend to use the cache, just pass `0`.
* `int matchType = -1` (OPTIONAL): specifies whether to match index, data, or neither.
	* `-1` (default): do not match (do not use cache.)
	* `0`: match index (`match` parameter must be the index to match.)
	* `1`: match data (`match` parameter must be the data to match.)
* `bool updateCache = true` (OPTIONAL): specifies whether to update the cache is the function is interrupted.
	* `true` (default): cache will be updated with the current node if the traverse loop is ended early.
	* `false`: cache will not be updated.

**Returning a value from the lambda function**\
It is often useful to return a value from the lambda function. To achieve this, we can use `&` as the capture clause:
```cpp
<snip> 
	int x;  // Example variable

	[&](Node<T> *node, Node<T> *last, bool *interrupt, int i) {

		x = 0;  // The lambda function can now access x in teh current scope

	}
</snip>
```
Notice the `&` between the square brackets at the start of the function. This will allow the lambda function to access varaibles in the current scope freely.

**Caching**\
The following are provided for manual cache manipulation:
* `resetcache()`: clears the cache; the next traversal is guaranteed to not check cache at all.
* `updateCacheIndex(int index, bool increased, Node<T> *newNode = NULL)`: updates the cache in the case of item insertion or deletion.
	* `int index`: index at which node was inserted or deleted
	* `bool increased`: set to `true` if node was inserted, `false` if one was deleted.
	* `newNode`: if `increased = true`, provide the new node via this parameter.

**Example usage:**\
```cpp
Node<T> *prev;
Node<T> *curr;

this->traverse([&](Node<T> *node, Node<T> *last, bool *interrupt, int i) {
	if (i == index-1) {
		prev = last;
		curr = node;
		*interrupt = true;
	}
}, searchIndex-1, 0, false);
```
This code is snipped from the `XorLinkedList<T>::insert` implementation, and finds the node previous to the insertion point `index`.
* `index = searchIndex-1`: the cache will be checked to see if it contains `searchIndex-1`, and if it does, the cached node will be used.
* `matchType = 0`: `index` will be matched against the cached index, NOT the data.
* `updateCache = false`: cache will not be updated.

### Linked List interface

#### Constructor
**Usage:** `XorLinkedList<int> *xll = new XorLinkedList<int>();`\
**Internal declaration:** `XorLinkedList();`\
**Description:** Nothing much to see here, just your standard constructor.\

#### Destructor
**Usage:** `delete xll;`\
**Internal declaration:** `~XorLinkedList();`\
**Description:** Again, nothing much. Just a destructor.\

#### Get size
**Usage:** `int s = xll->getSize();`\
**Internal declaration:** `int getSize();`
**Description:** Returns the current size of the list.\
**Does not use cache | Does not set cache**\

#### Is empty?
**Usage:** `bool e = xll->isEmpty();`\
**Internal declaration:** `int getSize();`
**Description:** Returns `true` if the list is currently empty.\
**Does not use cache | Does not set cache**\

#### Contains element?
**Usage:** `bool c = xll->contains(3);`\
**Internal declaration:** `bool contains(T data);`\
* `T data`: element of type `T` that the function searches for.\
**Description:** Returns `true` if the list contains at least one instance of `data`.\
**Uses cache (matches data) | Sets cache**\

#### Add element
**Usage:** `xll->add(4);`\
**Internal declaration:** `void add(T data);`\
* `T data`: element of type `T` that is added to the list.\
**Description:** Adds a new node containing `data` to the end (tail) of the list.\
**Uses cache (matches index) | Sets cache**\

#### Insert element
**Usage:** `xll->insert(5, 2);`\
**Internal declaration:** `void insert(T data, int index);`\
* `T data`: element of type `T` that is added to the list.\
* `int index`: where the element is added to the list (i.e. if `index = 2`, `data` becomes the new `xll[2]`.)
**Description:** Adds a new node containing `data` at location `index`.\
**Uses cache (matches index) | Does not set cache**\

#### Remove element
**Usage:** `xll->remove(5);`\
**Internal declaration:** `void remove(T data);`\
* `T data`: element of type `T` that is removed from the list.\
**Description:** Removes an instance of `data` from the list.\
**Uses cache (matches data) | Does not set cache**\

#### Extract element
**Usage:** `int e = xll->remove(2);`\
**Internal declaration:** `T extract(int index);`\
* `int index`: index of element that is to be extracted from the list.\
**Description:** Removes the element at location `index` from the list and returns it.\
**Uses cache (matches index) | Does not set cache**\

#### Set element
**Usage:** `xll->set(5, 2);`\
**Internal declaration:** `void set(T data, int index);`\
* `T data`: value that node is to contain.
* `int index`: index of node that is to be set.\
**Description:** Replaces the current value of the node at location `index` with `data`.
**Uses cache (matches index) | Sets cache**\

#### Clear list
**Usage:** `xll->clear();`\
**Internal declaration:** `void clear();`\
**Description:** Empties the list.\
**Does not use cache | Resets cache**\

#### Reverse list
**Usage:** `xll->reverse();`\
**Internal declaration:** `void reverse();`\
**Description:** Reverses the list.\
**Does not use cache | Resets cache**\

#### Get element
**Usage:** `int e = xll->get(2);`\
**Internal declaration:** `T get(int index);`\
* `int index`: location from which data is read.
**Description:** Returns the data at location `index`.\
**Uses cache (matches index) | Sets cache**\

#### Get index of element
**Usage:** `int e = xll->getIndex(5);`\
**Internal declaration:** `int getIndex(T data);`\
* `T data`: element which is searched for.
**Description:** Returns the index of an instance of `data`.\
**Uses cache (matches data) | Sets cache**\

#### Traverse
See "traversal function" section above.
#ifndef SplayTree_Included
#define SplayTree_Included

#include <algorithm>
#include <functional> 
#include <utility>
#include <iterator> 
#include <stdexcept> 
#include "reverse_iterator.h"
#include "allocator.h"


template <typename Key, typename Value, typename Comparator = std::less<Key> >
class SplayTree {
public:

	/**
	* Usage: SplayTree<string, int> mySplayTree;
	* Usage: SplayTree<string, int> mySplayTree(MyComparisonFunction);
	*/
	SplayTree(Comparator comp = Comparator());

	~SplayTree();

	/* Usage: SplayTree<string, int> one = two; */
	SplayTree(const SplayTree& other);
	SplayTree& operator= (const SplayTree& other);

	class iterator;
	class const_iterator;

    typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

	/* Usage: mySplayTree.emplace("Skiplist", 137); */
	std::pair<iterator, bool> emplace(const Key& key, const Value& value);

	/* Usage: mySplayTree.erase("Key"); */
	bool erase(const Key& key);

	/* Usage: mySplayTree.erase(mySplayTree.begin()); */
	iterator erase(iterator where);

	/* Usage: (mySplayTree.find("const Key& key") */
	iterator find(const Key& key);
	const_iterator find(const Key& key) const;

	/* Usage: mySplayTree["skiplist"] = 137; */
	Value& operator[] (const Key& key);

	/* Usage: mySplayTree.at("skiplist") = 137; */
	Value& at(const Key& key);
	const Value& at(const Key& key) const;

	/**
	* Usage: for (SplayTree<TKey, TValue>::iterator itr = t.begin();
	*             itr != t.end(); ++itr) { ... }
	*/
	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;

	/**
	* Usage: for (SplayTree<TKey, TValue>::reverse_iterator itr = s.rbegin();
	*             itr != s.rend(); ++itr) { ... }
	*/
	reverse_iterator rbegin();
	reverse_iterator rend();
	const_reverse_iterator rbegin() const;
	const_reverse_iterator rend() const;

	size_t size() const;

	bool empty() const;

	/* Usage: one.swap(two); */
	void swap(SplayTree& other);

private:
	struct Node {

		std::pair<const Key, Value> mValue;
		Node* mChildren[2];
		Node* mParent;

		/* Pointer to the next and previous node in the sorted sequence. */
		Node* mNext, *mPrev;

		Node(const Key& key, const Value& value);

		void* operator new(size_t size) {
			try {
				auto ptr = allocator().allocate(size);
				return ptr;
			}
			catch (std::bad_alloc badAlloc) {
                throw std::runtime_error("Can't allocate the memory");
			}
		}

		void operator delete(void* p, size_t size) {
			allocator().deallocate(static_cast<std::pair<const Key, Value>*>(p), size);
		}

	private:
		static Pool_alloc<std::pair<const Key, Value>>& allocator()
		{
			static Pool_alloc<std::pair<const Key, Value>> allocator_;
			return allocator_;
		}
	};


	Node* mHead, *mTail;
	mutable Node* mRoot;
	Comparator mComp;
	size_t mSize;

	/* The Curiously-Recurring template Pattern.*/
	template <typename DerivedType, typename Pointer, typename Reference>
	class IteratorBase;
	template <typename DerivedType, typename Pointer, typename Reference>
	friend class IteratorBase;

	friend class iterator;
	friend class const_iterator;

	void rotateUp(Node* child) const;
	void splay(Node* where) const;

	std::pair<Node*, Node*> findNode(const Key& key) const;

	Node* mergeTrees(Node* left, Node* right) const;

	static Node* cloneTree(Node* toClone, Node* parent);

	static Node* rethreadLinkedList(Node* root, Node* predecessor);
};


template <typename Key, typename Value, typename Comparator>
bool operator<  (const SplayTree<Key, Value, Comparator>& lhs,
	const SplayTree<Key, Value, Comparator>& rhs);
template <typename Key, typename Value, typename Comparator>
bool operator<= (const SplayTree<Key, Value, Comparator>& lhs,
	const SplayTree<Key, Value, Comparator>& rhs);
template <typename Key, typename Value, typename Comparator>
bool operator== (const SplayTree<Key, Value, Comparator>& lhs,
	const SplayTree<Key, Value, Comparator>& rhs);
template <typename Key, typename Value, typename Comparator>
bool operator!= (const SplayTree<Key, Value, Comparator>& lhs,
	const SplayTree<Key, Value, Comparator>& rhs);
template <typename Key, typename Value, typename Comparator>
bool operator>= (const SplayTree<Key, Value, Comparator>& lhs,
	const SplayTree<Key, Value, Comparator>& rhs);
template <typename Key, typename Value, typename Comparator>
bool operator>  (const SplayTree<Key, Value, Comparator>& lhs,
	const SplayTree<Key, Value, Comparator>& rhs);



template <typename Key, typename Value, typename Comparator>
template <typename DerivedType, typename Pointer, typename Reference>
class SplayTree<Key, Value, Comparator>::IteratorBase {
public:

	typedef std::bidirectional_iterator_tag iterator_category;
	typedef typename std::pair<const Key, Value> value_type;
	typedef typename std::pair<const Key, Value>& reference;
    typedef typename std::ptrdiff_t difference_type;
	typedef typename std::pair<const Key, Value>*  pointer;

	typedef typename SplayTree<Key, Value, Comparator>::Node Node;

	DerivedType& operator++ () {
		mCurr = mCurr->mNext;
		return static_cast<DerivedType&>(*this);
	}
	const DerivedType operator++ (int) {
		DerivedType result = static_cast<DerivedType&>(*this);
		++*this;
		return result;
	}

	DerivedType& operator-- () {
		if (mCurr == NULL) mCurr = mOwner->mTail;
		else mCurr = mCurr->mPrev;
		return static_cast<DerivedType&>(*this);
	}

	const DerivedType operator-- (int) {
		DerivedType result = static_cast<DerivedType&>(*this);
		--*this;
		return result;
	}

	template <typename DerivedType2, typename Pointer2, typename Reference2>
	bool operator== (const IteratorBase<DerivedType2, Pointer2, Reference2>& rhs) {
		return mOwner == rhs.mOwner && mCurr == rhs.mCurr;
	}

	template <typename DerivedType2, typename Pointer2, typename Reference2>
	bool operator!= (const IteratorBase<DerivedType2, Pointer2, Reference2>& rhs) {
		return !(*this == rhs);
	}

	Reference operator* () const {
		return mCurr->mValue;
	}

	Pointer operator-> () const {
		return &**this;
	}

protected:
	const SplayTree* mOwner;
	Node* mCurr;
	template <typename Derived2, typename Pointer2, typename Reference2>
	friend class IteratorBase;
	IteratorBase(const SplayTree* owner = NULL, Node* curr = NULL) : mOwner(owner), mCurr(curr) { }
};


template <typename Key, typename Value, typename Comparator>
class SplayTree<Key, Value, Comparator>::iterator : 
	public IteratorBase<iterator, std::pair<const Key, Value>*, std::pair<const Key, Value>&> {

public:
	iterator() { }

private:
	iterator(const SplayTree* owner,
		typename SplayTree<Key, Value, Comparator>::Node* node) :
		IteratorBase<iterator,
		std::pair<const Key, Value>*,
		std::pair<const Key, Value>&>(owner, node) { }

	friend class SplayTree;
	friend class const_iterator;
};

template <typename Key, typename Value, typename Comparator>
class SplayTree<Key, Value, Comparator>::const_iterator :
	public IteratorBase<const_iterator, const std::pair<const Key, Value>*, const std::pair<const Key, Value>&> {

public:
	const_iterator() { }

	const_iterator(iterator itr) :
		IteratorBase<const_iterator,
		const std::pair<const Key, Value>*,
		const std::pair<const Key, Value>&>(itr.mOwner, itr.mCurr) {
	}

private:
	const_iterator(const SplayTree* owner,
		typename SplayTree<Key, Value, Comparator>::Node* node) :
		IteratorBase<const_iterator,
		const std::pair<const Key, Value>*,
		const std::pair<const Key, Value>&>(owner, node) {
	}

	friend class SplayTree;
};

/* SplayTree::Node Implementation. */
template <typename Key, typename Value, typename Comparator>
SplayTree<Key, Value, Comparator>::Node::Node(const Key& key,
	const Value& value)
	: mValue(key, value) { }

/* SplayTree Implementation */
template <typename Key, typename Value, typename Comparator>
SplayTree<Key, Value, Comparator>::SplayTree(Comparator comp) : mComp(comp) {
	mHead = mTail = mRoot = NULL;
	mSize = 0;
}

template <typename Key, typename Value, typename Comparator>
SplayTree<Key, Value, Comparator>::~SplayTree() {
	Node* curr = mHead;
	while (curr != NULL) {
		Node* next = curr->mNext;
		delete curr;
		curr = next;
	}
}

template <typename Key, typename Value, typename Comparator>
std::pair<typename SplayTree<Key, Value, Comparator>::iterator, bool>
SplayTree<Key, Value, Comparator>::emplace(const Key& key, const Value& value) {
	/* Recursively walk down the tree from the root, looking for where the value
	should go */
	Node* lastLeft = NULL, *lastRight = NULL;
	Node** curr = &mRoot;
	Node*  parent = NULL;

	while (*curr != NULL) {
		parent = *curr;
		if (mComp(key, (*curr)->mValue.first)) {
			lastLeft = *curr;
			curr = &(*curr)->mChildren[0];
		}
		else if (mComp((*curr)->mValue.first, key)) {
			lastRight = *curr;
			curr = &(*curr)->mChildren[1];
		}
		else {
			Node* toReturn = *curr;
			splay(toReturn);
			return std::make_pair(iterator(this, toReturn), false);
		}
	}

	Node* toInsert = new Node(key, value);
	toInsert->mParent = parent;
	*curr = toInsert;
	toInsert->mChildren[0] = toInsert->mChildren[1] = NULL;
	toInsert->mNext = lastLeft;
	toInsert->mPrev = lastRight;
	if (toInsert->mNext)
		toInsert->mNext->mPrev = toInsert;
	else mTail = toInsert;

	if (toInsert->mPrev)
		toInsert->mPrev->mNext = toInsert;
	else mHead = toInsert;

	splay(toInsert);
	++mSize;
	return std::make_pair(iterator(this, toInsert), true);
}

template <typename Key, typename Value, typename Comparator>
void SplayTree<Key, Value, Comparator>::rotateUp(Node* node) const {
	const int side = (node != node->mParent->mChildren[0]);
	const int otherSide = !side;
	Node* child = node->mChildren[otherSide];
	Node* parent = node->mParent;
	node->mParent = parent->mParent;
	node->mChildren[otherSide] = parent;
	parent->mChildren[side] = child;
	if (child)
		child->mParent = parent;
	if (parent->mParent) {
		const int parentSide = (parent != parent->mParent->mChildren[0]);
		parent->mParent->mChildren[parentSide] = node;
	}
	else mRoot = node;
	parent->mParent = node;
}

template <typename Key, typename Value, typename Comparator>
void SplayTree<Key, Value, Comparator>::splay(Node* node) const {
	while (node && node->mParent) {
		Node* parent = node->mParent;
		/* Zig case: If the parent is the root, do just one rotation. */
		if (parent->mParent == NULL)
			rotateUp(node);
		else if ((parent->mParent->mChildren[0] == parent) ==
			(node->mParent->mChildren[0] == node)) {
			rotateUp(parent);
			rotateUp(node);
		}

		/* the zig-zag case */
		else {
			rotateUp(node);
			rotateUp(node);
		}
	}
}

template <typename Key, typename Value, typename Comparator>
typename SplayTree<Key, Value, Comparator>::const_iterator
SplayTree<Key, Value, Comparator>::find(const Key& key) const {
	std::pair<Node*, Node*> result = findNode(key);
	splay(result.first ? result.first : result.second);
	return const_iterator(this, result.first);
}

template <typename Key, typename Value, typename Comparator>
typename SplayTree<Key, Value, Comparator>::iterator
SplayTree<Key, Value, Comparator>::find(const Key& key) {
	const_iterator itr = static_cast<const SplayTree*>(this)->find(key);
	return iterator(itr.mOwner, itr.mCurr);
}

template <typename Key, typename Value, typename Comparator>
std::pair<typename SplayTree<Key, Value, Comparator>::Node*,
	typename SplayTree<Key, Value, Comparator>::Node*>
	SplayTree<Key, Value, Comparator>::findNode(const Key& key) const {
	Node* curr = mRoot, *prev = NULL;
	while (curr != NULL) {
		prev = curr;
		if (mComp(key, curr->mValue.first))
			curr = curr->mChildren[0];
		else if (mComp(curr->mValue.first, key))
			curr = curr->mChildren[1];
		else
			return std::make_pair(curr, curr->mParent);
	}
	return std::make_pair((Node*)NULL, prev);
}

template <typename Key, typename Value, typename Comparator>
typename SplayTree<Key, Value, Comparator>::iterator
SplayTree<Key, Value, Comparator>::begin() {
	return iterator(this, mHead);
}
template <typename Key, typename Value, typename Comparator>
typename SplayTree<Key, Value, Comparator>::const_iterator
SplayTree<Key, Value, Comparator>::begin() const {
	return const_iterator(this, mHead);
}
template <typename Key, typename Value, typename Comparator>
typename SplayTree<Key, Value, Comparator>::iterator
SplayTree<Key, Value, Comparator>::end() {
	return iterator(this, NULL);
}
template <typename Key, typename Value, typename Comparator>
typename SplayTree<Key, Value, Comparator>::const_iterator
SplayTree<Key, Value, Comparator>::end() const {
	return const_iterator(this, NULL);
}
template <typename Key, typename Value, typename Comparator>
typename SplayTree<Key, Value, Comparator>::reverse_iterator
SplayTree<Key, Value, Comparator>::rbegin() {
	return reverse_iterator(end());
}
template <typename Key, typename Value, typename Comparator>
typename SplayTree<Key, Value, Comparator>::const_reverse_iterator
SplayTree<Key, Value, Comparator>::rbegin() const {
	return const_reverse_iterator(end());
}
template <typename Key, typename Value, typename Comparator>
typename SplayTree<Key, Value, Comparator>::reverse_iterator
SplayTree<Key, Value, Comparator>::rend() {
	return reverse_iterator(begin());
}
template <typename Key, typename Value, typename Comparator>
typename SplayTree<Key, Value, Comparator>::const_reverse_iterator
SplayTree<Key, Value, Comparator>::rend() const {
	return const_reverse_iterator(begin());
}
template <typename Key, typename Value, typename Comparator>
size_t SplayTree<Key, Value, Comparator>::size() const {
	return mSize;
}
template <typename Key, typename Value, typename Comparator>
bool SplayTree<Key, Value, Comparator>::empty() const {
	return size() == 0;
}

template <typename Key, typename Value, typename Comparator>
typename SplayTree<Key, Value, Comparator>::iterator
SplayTree<Key, Value, Comparator>::erase(iterator where) {
	Node* node = where.mCurr;
	splay(node);
	Node* lhs = node->mChildren[0];
	Node* rhs = node->mChildren[1];
	if (lhs) lhs->mParent = NULL;
	if (rhs) rhs->mParent = NULL;
	mRoot = mergeTrees(lhs, rhs);
	if (node->mNext)
		node->mNext->mPrev = node->mPrev;
	else
		mTail = node->mPrev;

	if (node->mPrev)
		node->mPrev->mNext = node->mNext;
	else
		mHead = node->mNext;

	iterator result(this, node->mNext);
	delete node;
	--mSize;
	return result;
}

template <typename Key, typename Value, typename Comparator>
typename SplayTree<Key, Value, Comparator>::Node*
SplayTree<Key, Value, Comparator>::mergeTrees(Node* lhs, Node* rhs) const {
	if (lhs == NULL) return rhs;
	if (rhs == NULL) return lhs;
	Node* maxElem = lhs;
	while (maxElem->mChildren[1] != NULL) maxElem = maxElem->mChildren[1];
	splay(maxElem);
	maxElem->mChildren[1] = rhs;
	rhs->mParent = maxElem;
	return maxElem;
}

template <typename Key, typename Value, typename Comparator>
bool SplayTree<Key, Value, Comparator>::erase(const Key& key) {
	iterator where = find(key);
	if (where == end()) return false;
	erase(where);
	return true;
}

template <typename Key, typename Value, typename Comparator>
Value& SplayTree<Key, Value, Comparator>::operator[] (const Key& key) {
	return emplace(key, Value()).first->second;
}

template <typename Key, typename Value, typename Comparator>
const Value& SplayTree<Key, Value, Comparator>::at(const Key& key) const {
	const_iterator result = find(key);
	if (result == end())
		throw std::out_of_range("Key not found in splay tree.");
	return result->second;
}

template <typename Key, typename Value, typename Comparator>
Value& SplayTree<Key, Value, Comparator>::at(const Key& key) {
	return const_cast<Value&>(static_cast<const SplayTree*>(this)->at(key));
}

template <typename Key, typename Value, typename Comparator>
SplayTree<Key, Value, Comparator>::SplayTree(const SplayTree& other) {
	mSize = other.mSize;
	mComp = other.mComp;
	mRoot = cloneTree(other.mRoot, NULL);
	rethreadLinkedList(mRoot, NULL);
	mTail = mHead = mRoot;
	while (mHead && mHead->mChildren[0]) mHead = mHead->mChildren[0];
	while (mTail && mTail->mChildren[1]) mTail = mTail->mChildren[1];
}

template <typename Key, typename Value, typename Comparator>
typename SplayTree<Key, Value, Comparator>::Node*
SplayTree<Key, Value, Comparator>::cloneTree(Node* toClone, Node* parent) {
	if (toClone == NULL) return NULL;
	Node* result = new Node(toClone->mValue.first, toClone->mValue.second);
	for (int i = 0; i < 2; ++i)
		result->mChildren[i] = cloneTree(toClone->mChildren[i], result);
	result->mParent = parent;
	return result;
}

template <typename Key, typename Value, typename Comparator>
typename SplayTree<Key, Value, Comparator>::Node*
SplayTree<Key, Value, Comparator>::rethreadLinkedList(Node* root, Node* predecessor) {
	if (root == NULL) return predecessor;
	predecessor = rethreadLinkedList(root->mChildren[0], predecessor);
	root->mPrev = predecessor;
	if (predecessor)
		predecessor->mNext = root;
	root->mNext = NULL;
	return rethreadLinkedList(root->mChildren[1], root);
}

template <typename Key, typename Value, typename Comparator>
SplayTree<Key, Value, Comparator>&
SplayTree<Key, Value, Comparator>::operator= (const SplayTree& other) {
	SplayTree clone = other;
	swap(clone);
	return *this;
}

/* element-by-element swap. */
template <typename Key, typename Value, typename Comparator>
void SplayTree<Key, Value, Comparator>::swap(SplayTree& other) {
	std::swap(mRoot, other.mRoot);
	std::swap(mSize, other.mSize);
	std::swap(mHead, other.mHead);
	std::swap(mTail, other.mTail);
	std::swap(mComp, other.mComp);
}

template <typename Key, typename Value, typename Comparator>
bool operator<  (const SplayTree<Key, Value, Comparator>& lhs,
	const SplayTree<Key, Value, Comparator>& rhs) {
	return std::lexicographical_compare(lhs.begin(), lhs.end(),
		rhs.begin(), rhs.end());
}
template <typename Key, typename Value, typename Comparator>
bool operator== (const SplayTree<Key, Value, Comparator>& lhs,
	const SplayTree<Key, Value, Comparator>& rhs) {
	return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(),
		rhs.begin());
}


template <typename Key, typename Value, typename Comparator>
bool operator<= (const SplayTree<Key, Value, Comparator>& lhs,
	const SplayTree<Key, Value, Comparator>& rhs) {
	return !(rhs < lhs);
}
template <typename Key, typename Value, typename Comparator>
bool operator!= (const SplayTree<Key, Value, Comparator>& lhs,
	const SplayTree<Key, Value, Comparator>& rhs) {
	return !(lhs == rhs);
}
template <typename Key, typename Value, typename Comparator>
bool operator>= (const SplayTree<Key, Value, Comparator>& lhs,
	const SplayTree<Key, Value, Comparator>& rhs) {
	return !(lhs < rhs);
}
template <typename Key, typename Value, typename Comparator>

bool operator>(const SplayTree<Key, Value, Comparator>& lhs,
	const SplayTree<Key, Value, Comparator>& rhs) {
	return rhs < lhs;
}

#endif
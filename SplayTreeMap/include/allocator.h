#ifndef ALLOCATOR_H
#define ALLOCATOR_H

template<class T>class Pool_alloc : public std::allocator <T> {
using typename std::allocator<T>:: pointer;
public:

	Pool_alloc()throw() :head(0), chunks(0) {}
    pointer allocate(size_t n) {
		if (n*esize > Ch_size) throw std::bad_alloc();
		if (!head) grow(); else if (n != 1) grow(true);
		Link* p = head;
		while (n--) head = head->next;
        return reinterpret_cast<pointer>(p);
	}

	void deallocate(pointer p, size_t n) {
		while (n--) {
			reinterpret_cast<Link*>(p)->next = head;
			head = reinterpret_cast<Link*>(p);
		}
	}

	~Pool_alloc()throw() {
		Chunk* n = chunks;
		while (n) {
			Chunk* p = n;
			n = n->next;
			delete p;
		}
	}

	template<class _Other>
	struct rebind {
		typedef Pool_alloc<_Other> other;
	};
	template<class _Other>Pool_alloc(const Pool_alloc<_Other>& p)throw() {
		head = 0; chunks = 0;
	}
	template<class _Other>Pool_alloc<T>& operator=(const Pool_alloc<_Other>&) {
		head = 0; chunks = 0;
		return (*this);
	}

	Pool_alloc(const Pool_alloc<T>& p) throw() {
		head = 0; chunks = 0;
	}

private:
	static const size_t Ch_size = 4086;
	struct Link { Link* next; };
	struct Chunk {
		Chunk* next;
		char mem[Ch_size];
	};

	Link* head;
	Chunk* chunks;
	static const size_t esize = sizeof(T) < sizeof(Link*) ? sizeof(Link*) : sizeof(T);
	static const size_t nelem = Ch_size / esize;

	void grow(bool spec = false) { // Increse a pull
		Chunk* n = new Chunk;
		n->next = chunks;
		chunks = n;
		char* start = n->mem;
		char* last = &n->mem[(nelem - 1)*esize];
		for (char* p = start; p < last; p += esize)
			reinterpret_cast<Link*>(p)->next = reinterpret_cast<Link*>(p + esize);
		reinterpret_cast<Link*>(last)->next = spec ? head : 0;
		head = reinterpret_cast<Link*>(start);
	}
};

#endif // ALLOCATOR_H

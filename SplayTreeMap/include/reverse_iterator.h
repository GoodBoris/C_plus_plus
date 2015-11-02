#ifndef REVERSE_ITERATOR_H
#define REVERSE_ITERATOR_H

template <typename Iterator>
struct iterator_traits{
	typedef typename Iterator::reference reference;
};

template <typename T>
struct iterator_traits<T*>{
	typedef T & reference;
};

template <typename Iterator>
struct reverse_iterator
{
	typedef Iterator iterator;
	typedef typename iterator_traits<Iterator>::reference reference;
	inline explicit reverse_iterator(const iterator & it) :_it(it){}
	inline reverse_iterator() : _it(0x0)                {}

	inline iterator base() const                        { iterator it = _it; return --it; }
	inline reverse_iterator operator ++ () { 
		reverse_iterator tmp = *this;
		++*this;
		return tmp;
	}
	inline reverse_iterator operator -- () {
		reverse_iterator tmp = *this;
		--*this;
		return tmp; 
	}

	inline reverse_iterator operator ++ (int val)       { _it -= val; return reverse_iterator(_it); }
	inline reverse_iterator operator -- (int val)       { _it += val; return reverse_iterator(_it); }
	inline reverse_iterator operator += (int val)       { _it -= val; return reverse_iterator(_it); }
	inline reverse_iterator operator -= (int val)       { _it += val; return reverse_iterator(_it); }

	inline reverse_iterator operator + (int val) const  { iterator it = _it - val; return reverse_iterator(it); }
	inline reverse_iterator operator - (int val) const  { iterator it = _it + val; return reverse_iterator(it); }

	bool operator != (const iterator & other) const     { return other != base(); }

	reference operator*() const { return *base(); }
	iterator operator->() const { return base(); }

private:
	iterator _it;
};

#endif // REVERSE_ITERATOR_H


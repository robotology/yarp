/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///                                                                   ///
/// This Academic Free License applies to any software and associated ///
/// documentation (the "Software") whose owner (the "Licensor") has   ///
/// placed the statement "Licensed under the Academic Free License    ///
/// Version 1.0" immediately after the copyright notice that applies  ///
/// to the Software.                                                  ///
/// Permission is hereby granted, free of charge, to any person       ///
/// obtaining a copy of the Software (1) to use, copy, modify, merge, ///
/// publish, perform, distribute, sublicense, and/or sell copies of   ///
/// the Software, and to permit persons to whom the Software is       ///
/// furnished to do so, and (2) under patent claims owned or          ///
/// controlled by the Licensor that are embodied in the Software as   ///
/// furnished by the Licensor, to make, use, sell and offer for sale  ///
/// the Software and derivative works thereof, subject to the         ///
/// following conditions:                                             ///
/// Redistributions of the Software in source code form must retain   ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers. ///
/// Redistributions of the Software in executable form must reproduce ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers  ///
/// in the documentation and/or other materials provided with the     ///
/// distribution.                                                     ///
///                                                                   ///
/// Neither the names of Licensor, nor the names of any contributors  ///
/// to the Software, nor any of their trademarks or service marks,    ///
/// may be used to endorse or promote products derived from this      ///
/// Software without express prior written permission of the Licensor.///
///                                                                   ///
/// DISCLAIMERS: LICENSOR WARRANTS THAT THE COPYRIGHT IN AND TO THE   ///
/// SOFTWARE IS OWNED BY THE LICENSOR OR THAT THE SOFTWARE IS         ///
/// DISTRIBUTED BY LICENSOR UNDER A VALID CURRENT LICENSE. EXCEPT AS  ///
/// EXPRESSLY STATED IN THE IMMEDIATELY PRECEDING SENTENCE, THE       ///
/// SOFTWARE IS PROVIDED BY THE LICENSOR, CONTRIBUTORS AND COPYRIGHT  ///
/// OWNERS "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, /// 
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   ///
/// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO      ///
/// EVENT SHALL THE LICENSOR, CONTRIBUTORS OR COPYRIGHT OWNERS BE     ///
/// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   ///
/// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN ///
/// CONNECTION WITH THE SOFTWARE.                                     ///
///                                                                   ///
/// This license is Copyright (C) 2002 Lawrence E. Rosen. All rights  ///
/// reserved. Permission is hereby granted to copy and distribute     ///
/// this license without modification. This license may not be        ///
/// modified without the express written permission of its copyright  ///
/// owner.                                                            ///
///                                                                   ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
/// YARPList wrapped by pasa.
///

///
/// $Id: YARPList.h,v 1.1 2006-03-13 13:35:18 eshuy Exp $
///
///

#ifndef __YARPListh__
#define __YARPListh__

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Containers_T.h>


#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

/**
 * \file YARPList.h a STL-like list class derived from ACE List container.
 */

/**
 * the YARPList iterator. The main difference with C++ STL
 * is that the iterator here is initialized for a specific
 * object and keeps a pointer to it internally.
 */
template <class T>
class YARPListIterator : public ACE_DLList_Iterator<T>
{
public:
	/**
	 * Constructor. Creates a new iterator for a specific instance of YARPList.
	 * @param i an ACE_DLList reference to link the iterator to.
	 */
	YARPListIterator(ACE_DLList<T>& i) : ACE_DLList_Iterator<T>(i) {}

	/**
	 * Constructor. Creates a new iterator for a specific instance of YARPList: same
	 * as above but const parameter.
	 * @param i an ACE_DLList reference to link the iterator to.
	 */
	YARPListIterator(const YARPListIterator& i) : ACE_DLList_Iterator<T>(i.dllist_) {}

	/**
	 * Destructor.
	 */
	~YARPListIterator() {}

	/**
	 * move the iterator to the beginning of the list.
	 */
	int go_head(void) { return ACE_DLList_Iterator<T>::go_head(); }

	/**
	 * move the iterator to the end of the list.
	 */
	int go_tail(void) { return ACE_DLList_Iterator<T>::go_tail(); }

	/**
	 * dereferencing operator.
	 * @return a reference to the current item pointed to by the iterator.
	 */
	T& operator *() const { return *(T*)(ACE_DLList_Iterator<T>::operator*().item_); }
};

/**
 * A generic double-linked templatized list derived from the ACE correspondent list.
 */
template <class T>
class YARPList : public ACE_DLList<T>
{
public:
	/**
	 * defines the iterator to be class YARPList<T>::iterator
	 */
	typedef YARPListIterator<T> iterator;

	/**
	 * defines the constant iterator to be class YARPList<T>::const_iterator
	 */
	typedef const YARPListIterator<T> const_iterator;

	friend class YARPListIterator<T>;

	/**
	 * Constructor.
	 */
	YARPList(void) : ACE_DLList<T> () {}

	/**
	 * Copy constructor.
	 * @param l is a reference to an instance of the same class type.
	 */
	YARPList(const YARPList<T>& l) : ACE_DLList<T> (l) {}
	
	/**
	 * Copy operator.
	 * @param l is a reference to an instance of another list to copy from.
	 */
	void operator= (const ACE_DLList<T> &l) { ACE_DLList<T>::operator=(l); }
	
	/**
	 * Copy operator from the ACE list.
	 * @param l is a reference to an ACE double linked list.
	 */
	void operator= (const YARPList<T> &l) { ACE_DLList<T>::operator=(l); } 

	/**
	 * Destructor.
	 */
	~YARPList() { ACE_DLList<T>::reset(); }

	/**
	 * Adds a new item to the tail of the list.
	 * @param new_item is a reference to the element type of the list.
	 */
	void push_back (const T& new_item) { T* el = new T; *el = new_item; insert_tail(el); }

	/**
	 * Adds a new item to the head of the list.
	 * @param new_item is a reference to the element type of the list.
	 */
	void push_front (const T& new_item) { T* el = new T; *el = new_item; insert_head(el); }

	/**
	 * Removes the last element of the list. 
	 */
	void pop_back (void) { T* el = ACE_DLList<T>::delete_tail(); delete el; }

	/**
	 * Removes the first element of the list.
	 */
	void pop_front (void) { T* el = ACE_DLList<T>::delete_head(); delete el; }

	/**
	 * Erases the element pointed by the iterator.
	 * @param it is the iterator pointing to the element to be removed.
	 & @return negative on failure.
	 */
	int erase(iterator &it)	{ return it.remove(); }

	/**
	 * Checks whether the list is empty.
	 * @return true if it's empty.
	 */
	bool empty() { return (ACE_DLList<T>::size() == 0); }

	/**
	 * Clears the list and frees memory.
	 */
	void clear() { ACE_DLList<T>::reset(); }
};

template <class T> class YARPVector;

/**
 * simpler iterator for vector (simpler than ACE version).
 * not safe after resize, need at least a call to go_head().
 * not safe iterator anyway...
*/
template <class T>
class YARPVectorIterator 
{
private:
	const YARPVector<T>& _owner;
	int _it;

public:
	/**
	 * Constructor. Builds the iterator and links it to an existing vector.
	 * @param i is a reference to an instance of another YARPVector.
	 */
	YARPVectorIterator(YARPVector<T>& i) : _owner(i) { _it = 0; }

	/**
	 * Constructor. Builds the iterator and links it to an existing vector.
	 * @param i is a reference to an instance of another ACEArray.
	 */
	YARPVectorIterator(ACE_Array<T>& i) : _owner(i) { _it = 0; }

	/**
	 * Copy constructor. Copies the iterator from an existing one.
	 * @param i is a reference to an instance of another iterator.
	 */
	YARPVectorIterator(const YARPVectorIterator<T>& i) : _owner(i._owner) { _it = i._it; }

	/**
	 * Destructor.
	 */
	~YARPVectorIterator() {}

	/**
	 * Moves the iterator to the first element of the vector.
	 * @return zero.
	 */
	int go_head(void) { _it = 0; return _it; }

	/**
	 * Moves the iterator to the last element of the vector.
	 * @return the position in the array.
	 */
	int go_tail(void) { _it = _owner.size(); return _it; }

	/**
	 * Tells whether the iterator is at the end of the vector.
	 * @return true if the end is reached.
	 */
	bool done()
	{
		if (_it == _owner.size() )
			return true;
		else
			return false;
	}

	/**
	 * De-references the iterator.
	 * @return a reference to the item pointed by the iterator.
	 */
	const T& operator *() const { ACE_ASSERT (_owner.size() != 0 && _it >= 0 && _it < _owner.size()); return _owner[_it]; }

	/**
	 * Increment operator.
	 * @return the new position reached by the iterator.
	 */
	int operator ++() { _it++; return _it; }

	/**
	 * Decrement operator.
	 * @return the new position reached by the iterator.
	 */
	int operator --() { _it--; return _it; }

	/**
	 * Increment operator.
	 * @return the new position reached by the iterator.
	 */
	int operator ++(int)
	{
		int tmp = _it;
		_it++;
		return tmp;
	}

	/**
	 * Decrement operator.
	 * @return the new position reached by the iterator.
	 */
	int operator --(int)
	{
		int tmp = _it;
		_it--;
		return tmp;
	}

	/**
	 * Compares the iterator with an integer.
	 * @param i is the number to check with.
	 * @return true if the iterator points to the i-th element.
	 */
	bool operator== (int i) { return (_it == i) ? true : false; }

	/**
	 * Compares the iterator with another interator.
	 * @param i is the iterator to compare with.
	 * @return true if the iterator points to the same element as the argument.
	 */
	bool operator== (const YARPVectorIterator<T>& i) { return (_it == i._it) ? true : false; }

	/*
	 * Cast operator. Transforms the iterator into an integer.
	 * @return an integer.
	 */
	operator int() { return _it; }

	/*
	 * Assign a value to the iterator.
	 * @param i is the value to assign.
	 * @return the value of the iterator after assignment.
	 */
	int operator= (int i) { _it = i; return _it; } 
};

/**
 * A simple vector class derived from the ACE_Array class.
 */
template <class T>
class YARPVector : public ACE_Array<T>
{
public:
	friend class YARPVectorIterator<T>;
	typedef YARPVectorIterator<T> iterator;

	/**
	 * Constructor.
	 * @param size is the size of the array at construction time.
	 */
	YARPVector(size_t size = 0) : ACE_Array<T>(size) {}

	/**
	 * Constructor.
	 * @param size is the size of the array.
	 * @param default_value is the value to assign the element of the array to.
	 */
	YARPVector(size_t size, const T& default_value) : ACE_Array<T>(size, default_value) {}

	/**
	 * Copy constructor.
	 * @param s is a reference to an instance of a YARPVector.
	 */
	YARPVector(const YARPVector<T>& s) : ACE_Array<T>(s) {}

	/**
	 * Constructor.
	 * @param s is a reference to an instance of an ACE_Array.
	 */
	YARPVector(const ACE_Array<T>& s) : ACE_Array<T>(s) {}
	
	/**
	 * Destructor.
	 */
	~YARPVector() {}

	/**
	 * Copy operator.
	 * @param s is a reference to an instance of a YARPVector.
	 */
	void operator= (const ACE_Array<T> &s) { ACE_Array<T>::operator= (s); }

	/**
	 * Copy operator.
	 * @param s is a reference to an instance of an ACE_Array.
	 */
	void operator= (const YARPVector<T> &s) { ACE_Array<T>::operator= (s); }

	/**
	 * Resizes the array to a new length.
	 * @param sz is the new size.
	 * @return the old size of the array.
	 */
	int resize (size_t sz) 
	{ 
		int old_size = ACE_Array<T>::size();
		ACE_Array<T>::size (sz);
		return old_size;
	}

	/**
	 * Adds an element to the end of the array.
	 * @param element is the item to be added.
	 * @return the new size of the array.
	 */
	int add_tail (const T& element)
	{
		int old_size = resize (ACE_Array<T>::size()+1);
		ACE_Array<T>::operator[] (old_size) = element;
		return old_size+1;
	}

	/**
	 * Reserves a certain number of elements by pre-allocating memory.
	 * @param sz is the number of elements to reserve.
	 * @return the old reserved size of the array.
	 */
	int reserve (size_t sz)
	{
		int old_size = ACE_Array<T>::max_size();
		ACE_Array<T>::max_size(sz);
		return old_size;
	}
};





#endif


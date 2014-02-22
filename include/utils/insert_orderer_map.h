/*
 * insert_orderer_map.h
 *
 *  Created on: Feb 21, 2014
 *      Author: djvibegga
 */

#ifndef INSERT_ORDERER_MAP_H_
#define INSERT_ORDERER_MAP_H_

#include <vector>
#include <map>

using namespace std;

template <class _K, class _V> class insert_ordered_map
{
private:
	vector<_V> _values;
	vector<_K> _keys;

public:
	insert_ordered_map()
	{
	}

	insert_ordered_map(const insert_ordered_map & other)
	: _values(other._values),
	  _keys(other._keys)
	{
	}

	class iterator
	{
		friend class insert_ordered_map;

	private:
		typename vector<_K>::const_iterator _keyIter;
		typename vector<_V>::const_iterator _valueIter;
		const insert_ordered_map<_K, _V> & _iomap;

	public:
		iterator(
			const insert_ordered_map<_K, _V> & iomap,
			typename vector<_K>::const_iterator keyIter,
			typename vector<_V>::const_iterator valueIter)
		: _keyIter(keyIter),
		  _valueIter(valueIter),
		  _iomap(iomap)
		{
		}

		iterator(const insert_ordered_map<_K, _V>::iterator & other)
		: _keyIter(other._keyIter),
		  _valueIter(other._valueIter),
		  _iomap(other._iomap)
		{
		}

		insert_ordered_map<_K, _V>::iterator & operator ++()
	    {
		   ++_keyIter;
		   ++_valueIter;
		   return *this;
	    }

		insert_ordered_map<_K, _V>::iterator & operator =(const insert_ordered_map<_K, _V>::iterator & other)
		{
		   if (this != &other) {
			   _iomap = other._iomap;
			   _keyIter = other._keyIter;
			   _valueIter = other._valueIter;
		   }
		   return *this;
	   }

		bool operator ==(const insert_ordered_map<_K, _V>::iterator & other)
	    {
		    return _keyIter == other._keyIter;
	    }

		bool operator !=(const insert_ordered_map<_K, _V>::iterator & other)
		{
			return _keyIter != other._keyIter;
		}

		_K first() const
		{
			return *_keyIter;
		}

		_V second() const
		{
			return *_valueIter;
		}
	};

	insert_ordered_map<_K, _V>::iterator begin() const
	{
		return insert_ordered_map<_K, _V>::iterator(
			*this,
			_keys.begin(),
			_values.begin()
		);
	}

	insert_ordered_map<_K, _V>::iterator end() const
	{
		return insert_ordered_map<_K, _V>::iterator(
			*this,
			_keys.end(),
			_values.end()
		);
	}

	bool empty() const
	{
		return _keys.empty();
	}

	void push(const _K & key, const _V & value)
	{
		typename vector<_K>::iterator kIter = _keys.begin();
		typename vector<_V>::iterator vIter = _values.begin();
		for (; kIter != _keys.end(); ++kIter, ++vIter) {
			if (*kIter == key) {
				break;
			}
		}
		if (kIter == _keys.end()) {
			_keys.push_back(key);
			_values.push_back(value);
		} else {
			*vIter = value;
		}
	}

	_V operator [](const _K & key) const
	{
		return find(key).second();
	}

	insert_ordered_map<_K, _V>::iterator find(const _K & key) const
	{
		typename vector<_K>::const_iterator kIter = _keys.begin();
		typename vector<_V>::const_iterator vIter = _values.begin();
		for (; kIter != _keys.end(); ++kIter, ++vIter) {
			if (*kIter == key) {
				break;
			}
		}
		return insert_ordered_map<_K, _V>::iterator(*this, kIter, vIter);
	}
};


#endif /* INSERT_ORDERER_MAP_H_ */

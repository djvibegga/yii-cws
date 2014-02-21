/*
 * insert_orderer_map.h
 *
 *  Created on: Feb 21, 2014
 *      Author: djvibegga
 */

#ifndef INSERT_ORDERER_MAP_H_
#define INSERT_ORDERER_MAP_H_

#include <list>
#include <map>

using namespace std;

template <class _K, class _V> class insert_ordered_map
{
private:
	map<_K, _V> _map;
	list<_K> _order;

public:
	insert_ordered_map()
	{
	}

	insert_ordered_map(const insert_ordered_map & other)
	: _map(other._map),
	  _order(other._order)
	{
	}

	class iterator
	{
	private:
		typename list<_K>::const_iterator _position;
		const insert_ordered_map<_K, _V> & _iomap;

	public:
		_K first;
		_V second;

	public:
		iterator(const insert_ordered_map<_K, _V> & iomap, typename list<_K>::const_iterator position)
		: _position(position),
		  _iomap(iomap)
		{
			if (!_iomap._order.empty() && position != _iomap._order.end()) {
				first = *position;
				typename map<_K, _V>::const_iterator found = _iomap._map.find(first);
				if (found != _iomap._map.end()) {
					second = found->second;
				}
			}
		}

		iterator(const insert_ordered_map<_K, _V>::iterator & other)
		: _position(other._position),
		  _iomap(other._iomap),
		  first(other.first),
		  second(other.second)
		{
		}

		insert_ordered_map<_K, _V>::iterator & operator ++()
	    {
		   ++_position;
		   if (_position != _iomap._order.end()) {
			   first = *_position;
			   second = _iomap._map.find(first)->second;
		   }
		   return *this;
	    }

		insert_ordered_map<_K, _V>::iterator & operator =(const insert_ordered_map<_K, _V>::iterator & other)
		{
		   if (this != &other) {
			   _iomap = other._iomap;
			   _position = other._position;
			   first = other.first;
			   second = other.second;
		   }
		   return *this;
	   }

		bool operator ==(const insert_ordered_map<_K, _V>::iterator & other)
	    {
		    return _position == other._position;
	    }

		bool operator !=(const insert_ordered_map<_K, _V>::iterator & other)
		{
		    return _position != other._position;
		}
	};

	insert_ordered_map<_K, _V>::iterator begin() const
	{
		return insert_ordered_map<_K, _V>::iterator(*this, _order.begin());
	}

	insert_ordered_map<_K, _V>::iterator end() const
	{
		return insert_ordered_map<_K, _V>::iterator(*this, _order.end());
	}

	bool empty() const
	{
		return _order.empty();
	}

	void push(const _K & key, const _V & value)
	{
		_map[key] = value;
		if (find(key) == end()) {
			_order.push_back(key);
		}
	}

	_V operator [](const _K & key)
	{
		return _map[key];
	}

	insert_ordered_map<_K, _V>::iterator find(const _K & key) const
	{
		typename list<_K>::const_iterator iter;
		for (iter = _order.begin(); iter != _order.end(); ++iter) {
			if (*iter == key) {
				break;
			}
		}
		return insert_ordered_map<_K, _V>::iterator(*this, iter);
	}
};


#endif /* INSERT_ORDERER_MAP_H_ */

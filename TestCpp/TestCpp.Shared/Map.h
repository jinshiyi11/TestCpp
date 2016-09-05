#ifndef __EASY_MAP_H__
#define __EASY_MAP_H__

#include "RbTree.h"

namespace easy {

    /// map
    ///
    /// Implements a canonical map. 
    ///
    /// The large majority of the implementation of this class is found in the rbtree
    /// base class. We control the behaviour of rbtree via template parameters.
    ///
    /// Pool allocation
    /// If you want to make a custom memory pool for a map container, your pool 
    /// needs to contain items of type map::node_type. So if you have a memory
    /// pool that has a constructor that takes the size of pool items and the
    /// count of pool items, you would do this (assuming that MemoryPool implements
    /// the Allocator interface):
    ///     typedef map<Widget, int, less<Widget>, MemoryPool> WidgetMap;  // Delare your WidgetMap type.
    ///     MemoryPool myPool(sizeof(WidgetMap::node_type), 100);          // Make a pool of 100 Widget nodes.
    ///     WidgetMap myMap(&myPool);                                      // Create a map that uses the pool.
    ///
    template <typename Key, typename T, typename Compare = easy::less<Key> >
    class map
        : public rbtree<Key, easy::pair<const Key, T>, Compare, easy::use_first<easy::pair<const Key, T> >, true, true>
    {
    public:
        typedef rbtree<Key, easy::pair<const Key, T>, Compare,
            easy::use_first<easy::pair<const Key, T> >, true, true>   base_type;
        typedef map<Key, T, Compare>                                     this_type;
        typedef typename base_type::size_type                                       size_type;
        typedef typename base_type::key_type                                        key_type;
        typedef T                                                                   mapped_type;
        typedef typename base_type::value_type                                      value_type;
        typedef typename base_type::node_type                                       node_type;
        typedef typename base_type::iterator                                        iterator;
        typedef typename base_type::const_iterator                                  const_iterator;
        typedef typename base_type::insert_return_type                              insert_return_type;
        typedef typename base_type::extract_key                                     extract_key;
        // Other types are inherited from the base class.

        using base_type::begin;
        using base_type::end;
        using base_type::find;
        using base_type::lower_bound;
        using base_type::upper_bound;
        using base_type::mCompare;
        using base_type::insert;
        using base_type::erase;

        class value_compare
        {
        protected:
            friend class map;
            Compare compare;
            value_compare(Compare c) : compare(c) {}

        public:
            typedef bool       result_type;
            typedef value_type first_argument_type;
            typedef value_type second_argument_type;

            bool operator()(const value_type& x, const value_type& y) const
            {
                return compare(x.first, y.first);
            }
        };

    public:
        map();
        map(const Compare& compare);
        map(const this_type& x);

        template <typename Iterator>
        map(Iterator itBegin, Iterator itEnd); // allocator arg removed because VC7.1 fails on the default arg. To consider: Make a second version of this function without a default arg.

    public:
        value_compare value_comp() const;

        size_type erase(const Key& key);
        size_type count(const Key& key) const;

        easy::pair<iterator, iterator>             equal_range(const Key& key);
        easy::pair<const_iterator, const_iterator> equal_range(const Key& key) const;
    }; // map


    ///////////////////////////////////////////////////////////////////////
    // map
    ///////////////////////////////////////////////////////////////////////

    template <typename Key, typename T, typename Compare>
    inline map<Key, T, Compare>::map()
        : base_type()
    {
    }


    template <typename Key, typename T, typename Compare>
    inline map<Key, T, Compare>::map(const Compare& compare)
        : base_type(compare)
    {
    }


    template <typename Key, typename T, typename Compare>
    inline map<Key, T, Compare>::map(const this_type& x)
        : base_type(x)
    {
    }

    template <typename Key, typename T, typename Compare>
    template <typename Iterator>
    inline map<Key, T, Compare>::map(Iterator itBegin, Iterator itEnd)
        : base_type(itBegin, itEnd, Compare())
    {
    }


    template <typename Key, typename T, typename Compare>
    inline typename map<Key, T, Compare>::value_compare
        map<Key, T, Compare>::value_comp() const
    {
        return value_compare(mCompare);
    }


    template <typename Key, typename T, typename Compare>
    inline typename map<Key, T, Compare>::size_type
        map<Key, T, Compare>::erase(const Key& key)
    {
        const iterator it(find(key));

        if (it != end()) // If it exists...
        {
            base_type::erase(it);
            return 1;
        }
        return 0;
    }


    template <typename Key, typename T, typename Compare>
    inline typename map<Key, T, Compare>::size_type
        map<Key, T, Compare>::count(const Key& key) const
    {
        const const_iterator it(find(key));
        return (it != end()) ? 1 : 0;
    }


    template <typename Key, typename T, typename Compare>
    inline easy::pair<typename map<Key, T, Compare>::iterator,
        typename map<Key, T, Compare>::iterator>
        map<Key, T, Compare>::equal_range(const Key& key)
    {
        // The resulting range will either be empty or have one element,
        // so instead of doing two tree searches (one for lower_bound and 
        // one for upper_bound), we do just lower_bound and see if the 
        // result is a range of size zero or one.
        const iterator itLower(lower_bound(key));

        if ((itLower == end()) || mCompare(key, itLower.mpNode->mValue.first)) // If at the end or if (key is < itLower)...
            return easy::pair<iterator, iterator>(itLower, itLower);

        iterator itUpper(itLower);
        return easy::pair<iterator, iterator>(itLower, ++itUpper);
    }


    template <typename Key, typename T, typename Compare>
    inline easy::pair<typename map<Key, T, Compare>::const_iterator,
        typename map<Key, T, Compare>::const_iterator>
        map<Key, T, Compare>::equal_range(const Key& key) const
    {
        // See equal_range above for comments.
        const const_iterator itLower(lower_bound(key));

        if ((itLower == end()) || mCompare(key, itLower.mpNode->mValue.first)) // If at the end or if (key is < itLower)...
            return easy::pair<const_iterator, const_iterator>(itLower, itLower);

        const_iterator itUpper(itLower);
        return easy::pair<const_iterator, const_iterator>(itLower, ++itUpper);
    }


}// end of namespace
#endif // __EASY_MAP_H__

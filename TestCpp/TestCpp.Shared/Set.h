#ifndef __EASY_SET_H__
#define __EASY_SET_H__

#include "RbTree.h"

namespace easy
{
    /// set
    ///
    /// Implements a canonical set. 
    ///
    /// The large majority of the implementation of this class is found in the rbtree
    /// base class. We control the behaviour of rbtree via template parameters.
    ///
    /// Note that the 'bMutableIterators' template parameter to rbtree is set to false.
    /// This means that set::iterator is const and the same as set::const_iterator.
    /// This is by design and it follows the C++ standard defect report recommendation.
    /// If the user wants to modify a container element, the user needs to either use
    /// mutable data members or use const_cast on the iterator's data member. Both of 
    /// these solutions are recommended by the C++ standard defect report.
    /// To consider: Expose the bMutableIterators template policy here at the set level
    /// so the user can have non-const set iterators via a template parameter.
    template <typename Key, typename Compare = easy::less<Key> >
    class set
        : public rbtree<Key, Key, Compare, easy::use_self<Key>, false, true>
    {
    public:
        typedef rbtree<Key, Key, Compare, easy::use_self<Key>, false, true> base_type;
        typedef set<Key, Compare>                                            this_type;
        typedef typename base_type::size_type                                           size_type;
        typedef typename base_type::value_type                                          value_type;
        typedef typename base_type::iterator                                            iterator;
        typedef typename base_type::const_iterator                                      const_iterator;
        typedef typename base_type::reverse_iterator                                    reverse_iterator;
        typedef typename base_type::const_reverse_iterator                              const_reverse_iterator;
        typedef Compare                                                                 value_compare;
        // Other types are inherited from the base class.

        using base_type::begin;
        using base_type::end;
        using base_type::find;
        using base_type::lower_bound;
        using base_type::upper_bound;
        using base_type::mCompare;

    public:
        set();
        set(const Compare& compare);
        set(const this_type& x);

        template <typename Iterator>
        set(Iterator itBegin, Iterator itEnd); // allocator arg removed because VC7.1 fails on the default arg. To do: Make a second version of this function without a default arg.
    public:
        value_compare value_comp() const;

        size_type erase(const Key& k);
        iterator  erase(const_iterator position);
        iterator  erase(const_iterator first, const_iterator last);

        reverse_iterator erase(const_reverse_iterator position);
        reverse_iterator erase(const_reverse_iterator first, const_reverse_iterator last);

        size_type count(const Key& k) const;

        easy::pair<iterator, iterator>             equal_range(const Key& k);
        easy::pair<const_iterator, const_iterator> equal_range(const Key& k) const;

    }; // set


       ///////////////////////////////////////////////////////////////////////
       // set
       ///////////////////////////////////////////////////////////////////////

    template <typename Key, typename Compare>
    inline set<Key, Compare>::set()
        : base_type()
    {
    }


    template <typename Key, typename Compare>
    inline set<Key, Compare>::set(const Compare& compare)
        : base_type(compare)
    {
    }


    template <typename Key, typename Compare>
    inline set<Key, Compare>::set(const this_type& x)
        : base_type(x)
    {
    }


    template <typename Key, typename Compare>
    inline set<Key, Compare>::set(std::initializer_list<value_type> ilist, const Compare& compare)
        : base_type(ilist.begin(), ilist.end(), compare)
    {
    }


    template <typename Key, typename Compare>
    template <typename Iterator>
    inline set<Key, Compare>::set(Iterator itBegin, Iterator itEnd)
        : base_type(itBegin, itEnd, Compare())
    {
    }


    template <typename Key, typename Compare>
    inline typename set<Key, Compare>::value_compare
        set<Key, Compare>::value_comp() const
    {
        return mCompare;
    }


    template <typename Key, typename Compare>
    inline typename set<Key, Compare>::size_type
        set<Key, Compare>::erase(const Key& k)
    {
        const iterator it(find(k));

        if (it != end()) // If it exists...
        {
            base_type::erase(it);
            return 1;
        }
        return 0;
    }


    template <typename Key, typename Compare>
    inline typename set<Key, Compare>::iterator
        set<Key, Compare>::erase(const_iterator position)
    {
        // We need to provide this version because we override another version 
        // and C++ hiding rules would make the base version of this hidden.
        return base_type::erase(position);
    }


    template <typename Key, typename Compare>
    inline typename set<Key, Compare>::iterator
        set<Key, Compare>::erase(const_iterator first, const_iterator last)
    {
        // We need to provide this version because we override another version 
        // and C++ hiding rules would make the base version of this hidden.
        return base_type::erase(first, last);
    }


    template <typename Key, typename Compare>
    inline typename set<Key, Compare>::size_type
        set<Key, Compare>::count(const Key& k) const
    {
        const const_iterator it(find(k));
        return (it != end()) ? (size_type)1 : (size_type)0;
    }


    template <typename Key, typename Compare>
    inline typename set<Key, Compare>::reverse_iterator
        set<Key, Compare>::erase(const_reverse_iterator position)
    {
        return reverse_iterator(erase((++position).base()));
    }


    template <typename Key, typename Compare>
    inline typename set<Key, Compare>::reverse_iterator
        set<Key, Compare>::erase(const_reverse_iterator first, const_reverse_iterator last)
    {
        // Version which erases in order from first to last.
        // difference_type i(first.base() - last.base());
        // while(i--)
        //     first = erase(first);
        // return first;

        // Version which erases in order from last to first, but is slightly more efficient:
        return reverse_iterator(erase((++last).base(), (++first).base()));
    }


    template <typename Key, typename Compare>
    inline easy::pair<typename set<Key, Compare>::iterator,
        typename set<Key, Compare>::iterator>
        set<Key, Compare>::equal_range(const Key& k)
    {
        // The resulting range will either be empty or have one element,
        // so instead of doing two tree searches (one for lower_bound and 
        // one for upper_bound), we do just lower_bound and see if the 
        // result is a range of size zero or one.
        const iterator itLower(lower_bound(k));

        if ((itLower == end()) || mCompare(k, *itLower)) // If at the end or if (k is < itLower)...
            return easy::pair<iterator, iterator>(itLower, itLower);

        iterator itUpper(itLower);
        return easy::pair<iterator, iterator>(itLower, ++itUpper);
    }


    template <typename Key, typename Compare>
    inline easy::pair<typename set<Key, Compare>::const_iterator,
        typename set<Key, Compare>::const_iterator>
        set<Key, Compare>::equal_range(const Key& k) const
    {
        // See equal_range above for comments.
        const const_iterator itLower(lower_bound(k));

        if ((itLower == end()) || mCompare(k, *itLower)) // If at the end or if (k is < itLower)...
            return easy::pair<const_iterator, const_iterator>(itLower, itLower);

        const_iterator itUpper(itLower);
        return easy::pair<const_iterator, const_iterator>(itLower, ++itUpper);
    }

} // namespace 

#endif // Header include guard

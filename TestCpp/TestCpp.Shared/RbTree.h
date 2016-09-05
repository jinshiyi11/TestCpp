#ifndef __EASY_RBTREE_H__
#define __EASY_RBTREE_H__
/**
 * 红黑树，暂不支持reverse_iterator
 */

#ifndef EASTL_API // If the build file hasn't already defined this to be dllexport...
#if EASTL_DLL 
#if defined(_MSC_VER)
#define EASTL_API      __declspec(dllimport)
#define EASTL_LOCAL
#elif defined(__CYGWIN__)
#define EASTL_API      __attribute__((dllimport))
#define EASTL_LOCAL
#elif (defined(__GNUC__) && (__GNUC__ >= 4))
#define EASTL_API      __attribute__ ((visibility("default")))
#define EASTL_LOCAL    __attribute__ ((visibility("hidden")))
#else
#define EASTL_API
#define EASTL_LOCAL
#endif
#else
#define EASTL_API
#define EASTL_LOCAL
#endif
#endif

#ifndef NULL
#define NULL    0
#endif

#define EASY_LIKELY(x)   (x)
#define EASY_VALIDATE_COMPARE(expression)
#define EA_ANALYSIS_ASSUME(x)

namespace easy
{
    template <typename T, T v>
    struct integral_constant
    {
        static const T value = v;
        typedef T value_type;
        typedef integral_constant<T, v> type;
    };


    ///////////////////////////////////////////////////////////////////////
    // true_type / false_type
    //
    // These are commonly used types in the implementation of type_traits.
    // Other integral constant types can be defined, such as those based on int.
    //
    typedef integral_constant<bool, true>  true_type;
    typedef integral_constant<bool, false> false_type;

    /// pair
    ///
    /// Implements a simple pair, just like the C++ std::pair.
    ///
    template <typename T1, typename T2>
    struct pair
    {
        typedef T1           first_type;
        typedef T2           second_type;
        typedef pair<T1, T2> this_type;

        T1 first;
        T2 second;

        pair()
            : first(),
            second() {}

        pair(const T1 &first_, const T2 &second_)
        {
            first = first_;
            second = second_;
        }
        pair(const pair<T1, T2> & pair_)
        {
            first = pair_.first;
            second = pair_.second;
        }
    };

    template <typename T1, typename T2>
    inline pair<T1, T2> make_pair(const T1& a, const T2& b)
    {
        return easy::pair<T1, T2>(a, b);
    }

    // type_select
    //
    // This is used to declare a type from one of two type options. 
    // The result is based on the condition type. This has certain uses
    // in template metaprogramming.
    //
    // Example usage:
    //    typedef ChosenType = typename type_select<is_integral<SomeType>::value, ChoiceAType, ChoiceBType>::type;
    //
    template <bool bCondition, class ConditionIsTrueType, class ConditionIsFalseType>
    struct type_select { typedef ConditionIsTrueType type; };

    template <typename ConditionIsTrueType, class ConditionIsFalseType>
    struct type_select<false, ConditionIsTrueType, ConditionIsFalseType> { typedef ConditionIsFalseType type; };

    template <typename Argument1, typename Argument2, typename Result>
    struct binary_function
    {
        typedef Argument1 first_argument_type;
        typedef Argument2 second_argument_type;
        typedef Result    result_type;
    };

    template <typename T = void>
    struct less : public binary_function<T, T, bool>
    {
        bool operator()(const T& a, const T& b) const
        {
            return a < b;
        }
    };

    template <typename InputIterator1, typename InputIterator2>
    inline bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2)
    {
        for (; first1 != last1; ++first1, ++first2)
        {
            if (!(*first1 == *first2))
                return false;
        }
        return true;
    }

    

    template <typename T>
    inline void swap(T& a, T& b)
    {
        T temp=a;
        a = b;
        b = temp;
    }

    /// use_self
    ///
    /// operator()(x) simply returns x. Used in sets, as opposed to maps.
    /// This is a template policy implementation; it is an alternative to 
    /// the use_first template implementation.
    ///
    /// The existance of use_self may seem odd, given that it does nothing,
    /// but these kinds of things are useful, virtually required, for optimal 
    /// generic programming.
    ///
    template <typename T>
    struct use_self             // : public unary_function<T, T> // Perhaps we want to make it a subclass of unary_function.
    {
        typedef T result_type;

        const T& operator()(const T& x) const
        {
            return x;
        }
    };

    /// use_first
    ///
    /// operator()(x) simply returns x.first. Used in maps, as opposed to sets.
    /// This is a template policy implementation; it is an alternative to 
    /// the use_self template implementation. This is the same thing as the
    /// SGI SGL select1st utility.
    ///
    template <typename Pair>
    struct use_first            // : public unary_function<Pair, typename Pair::first_type> // Perhaps we want to make it a subclass of unary_function.
    {
        typedef Pair argument_type;
        typedef typename Pair::first_type result_type;

        const result_type& operator()(const Pair& x) const
        {
            return x.first;
        }
    };


    /// RBTreeColor
    ///
    enum RBTreeColor
    {
        kRBTreeColorRed,
        kRBTreeColorBlack
    };

    /// RBTreeColor
    ///
    enum RBTreeSide
    {
        kRBTreeSideLeft,
        kRBTreeSideRight
    };

    /// rbtree_node_base
    ///
    /// We define a rbtree_node_base separately from rbtree_node (below), because it 
    /// allows us to have non-templated operations, and it makes it so that the 
    /// rbtree anchor node doesn't carry a T with it, which would waste space and 
    /// possibly lead to surprising the user due to extra Ts existing that the user 
    /// didn't explicitly create. The downside to all of this is that it makes debug 
    /// viewing of an rbtree harder, given that the node pointers are of type 
    /// rbtree_node_base and not rbtree_node.
    ///
    struct rbtree_node_base
    {
        typedef rbtree_node_base this_type;

    public:
        this_type* mpNodeRight;  // Declared first because it is used most often.
        this_type* mpNodeLeft;
        this_type* mpNodeParent;
        char       mColor;       // We only need one bit here, would be nice if we could stuff that bit somewhere else.
    };


    /// rbtree_node
    ///
    template <typename Value>
    struct rbtree_node : public rbtree_node_base
    {
        Value mValue; // For set and multiset, this is the user's value, for map and multimap, this is a pair of key/value.
    };



    /// rbtree_iterator
    ///
    template <typename T, typename Pointer, typename Reference>
    struct rbtree_iterator
    {
        typedef rbtree_iterator<T, Pointer, Reference>      this_type;
        typedef rbtree_iterator<T, T*, T&>                  iterator;
        typedef rbtree_iterator<T, const T*, const T&>      const_iterator;
        typedef unsigned int                                size_type;
        typedef int                                         difference_type;
        typedef T                                           value_type;
        typedef rbtree_node_base                            base_node_type;
        typedef rbtree_node<T>                              node_type;
        typedef Pointer                                     pointer;
        typedef Reference                                   reference;

    public:
        node_type* mpNode;

    public:
        rbtree_iterator();
        explicit rbtree_iterator(const node_type* pNode);
        rbtree_iterator(const iterator& x);

        reference operator*() const;
        pointer   operator->() const;

        rbtree_iterator& operator++();
        rbtree_iterator  operator++(int);

        rbtree_iterator& operator--();
        rbtree_iterator  operator--(int);

    private:
        /// RBTreeIncrement
        /// Returns the next item in a sorted red-black tree.
        ///
        EASTL_API rbtree_node_base* RBTreeIncrement(const rbtree_node_base* pNode)
        {
            if (pNode->mpNodeRight)
            {
                pNode = pNode->mpNodeRight;

                while (pNode->mpNodeLeft)
                    pNode = pNode->mpNodeLeft;
            } else
            {
                rbtree_node_base* pNodeTemp = pNode->mpNodeParent;

                while (pNode == pNodeTemp->mpNodeRight)
                {
                    pNode = pNodeTemp;
                    pNodeTemp = pNodeTemp->mpNodeParent;
                }

                if (pNode->mpNodeRight != pNodeTemp)
                    pNode = pNodeTemp;
            }

            return const_cast<rbtree_node_base*>(pNode);
        }

    }; // rbtree_iterator



       ///////////////////////////////////////////////////////////////////////////////
       // rb_base
       //
       // This class allows us to use a generic rbtree as the basis of map, multimap,
       // set, and multiset transparently. The vital template parameters for this are 
       // the ExtractKey and the bUniqueKeys parameters.
       //
       // If the rbtree has a value type of the form pair<T1, T2> (i.e. it is a map or
       // multimap and not a set or multiset) and a key extraction policy that returns 
       // the first part of the pair, the rbtree gets a mapped_type typedef. 
       // If it satisfies those criteria and also has unique keys, then it also gets an 
       // operator[] (which only map and set have and multimap and multiset don't have).
       //
       ///////////////////////////////////////////////////////////////////////////////



       /// rb_base
       /// This specialization is used for 'set'. In this case, Key and Value 
       /// will be the same as each other and ExtractKey will be easy::use_self.
       ///
    template <typename Key, typename Value, typename Compare, typename ExtractKey, bool bUniqueKeys, typename RBTree>
    struct rb_base
    {
        typedef ExtractKey extract_key;

    public:
        Compare mCompare; // To do: Make sure that empty Compare classes go away via empty base optimizations.

    public:
        rb_base() : mCompare() {}
        rb_base(const Compare& compare) : mCompare(compare) {}
    };


    /// rb_base
    /// This class is used for 'multiset'.
    /// In this case, Key and Value will be the same as each 
    /// other and ExtractKey will be easy::use_self.
    ///
    template <typename Key, typename Value, typename Compare, typename ExtractKey, typename RBTree>
    struct rb_base<Key, Value, Compare, ExtractKey, false, RBTree>
    {
        typedef ExtractKey extract_key;

    public:
        Compare mCompare; // To do: Make sure that empty Compare classes go away via empty base optimizations.

    public:
        rb_base() : mCompare() {}
        rb_base(const Compare& compare) : mCompare(compare) {}
    };


    /// rb_base
    /// This specialization is used for 'map'.
    ///
    template <typename Key, typename Pair, typename Compare, typename RBTree>
    struct rb_base<Key, Pair, Compare, easy::use_first<Pair>, true, RBTree>
    {
        typedef easy::use_first<Pair> extract_key;

    public:
        Compare mCompare; // To do: Make sure that empty Compare classes go away via empty base optimizations.

    public:
        rb_base() : mCompare() {}
        rb_base(const Compare& compare) : mCompare(compare) {}
    };


    /// rb_base
    /// This specialization is used for 'multimap'.
    ///
    template <typename Key, typename Pair, typename Compare, typename RBTree>
    struct rb_base<Key, Pair, Compare, easy::use_first<Pair>, false, RBTree>
    {
        typedef easy::use_first<Pair> extract_key;

    public:
        Compare mCompare; // To do: Make sure that empty Compare classes go away via empty base optimizations.

    public:
        rb_base() : mCompare() {}
        rb_base(const Compare& compare) : mCompare(compare) {}
    };



    /// rbtree
    ///
    /// rbtree is the red-black tree basis for the map, multimap, set, and multiset 
    /// containers. Just about all the work of those containers is done here, and 
    /// they are merely a shell which sets template policies that govern the code
    /// generation for this rbtree.
    ///
    /// This rbtree implementation is pretty much the same as all other modern 
    /// rbtree implementations, as the topic is well known and researched. We may
    /// choose to implement a "relaxed balancing" option at some point in the 
    /// future if it is deemed worthwhile. Most rbtree implementations don't do this.
    ///
    /// The primary rbtree member variable is mAnchor, which is a node_type and 
    /// acts as the end node. However, like any other node, it has mpNodeLeft,
    /// mpNodeRight, and mpNodeParent members. We do the conventional trick of 
    /// assigning begin() (left-most rbtree node) to mpNodeLeft, assigning 
    /// 'end() - 1' (a.k.a. rbegin()) to mpNodeRight, and assigning the tree root
    /// node to mpNodeParent. 
    ///
    /// Compare (functor): This is a comparison class which defaults to 'less'.
    /// It is a common STL thing which takes two arguments and returns true if  
    /// the first is less than the second.
    ///
    /// ExtractKey (functor): This is a class which gets the key from a stored
    /// node. With map and set, the node is a pair, whereas with set and multiset
    /// the node is just the value. ExtractKey will be either easy::use_first (map and multimap)
    /// or easy::use_self (set and multiset).
    ///
    /// bMutableIterators (bool): true if rbtree::iterator is a mutable
    /// iterator, false if iterator and const_iterator are both const iterators. 
    /// It will be true for map and multimap and false for set and multiset.
    ///
    /// bUniqueKeys (bool): true if the keys are to be unique, and false if there
    /// can be multiple instances of a given key. It will be true for set and map 
    /// and false for multiset and multimap.
    ///
    /// To consider: Add an option for relaxed tree balancing. This could result 
    /// in performance improvements but would require a more complicated implementation.
    ///
    ///////////////////////////////////////////////////////////////////////
    /// find_as
    /// In order to support the ability to have a tree of strings but
    /// be able to do efficiently lookups via char pointers (i.e. so they
    /// aren't converted to string objects), we provide the find_as
    /// function. This function allows you to do a find with a key of a
    /// type other than the tree's key type. See the find_as function
    /// for more documentation on this.
    ///
    template <typename Key, typename Value, typename Compare,
        typename ExtractKey, bool bMutableIterators, bool bUniqueKeys>
    class rbtree
        : public rb_base<Key, Value, Compare, ExtractKey, bUniqueKeys,
        rbtree<Key, Value, Compare, ExtractKey, bMutableIterators, bUniqueKeys> >
    {
    public:
        typedef int                                                                             difference_type;
        typedef unsigned int                                                                    size_type;     // See config.h for the definition of eastl_size_t, which defaults to uint32_t.
        typedef Key                                                                             key_type;
        typedef Value                                                                           value_type;
        typedef rbtree_node<value_type>                                                         node_type;
        typedef value_type&                                                                     reference;
        typedef const value_type&                                                               const_reference;
        typedef value_type*                                                                     pointer;
        typedef const value_type*                                                               const_pointer;

        typedef typename type_select<bMutableIterators,
            rbtree_iterator<value_type, value_type*, value_type&>,
            rbtree_iterator<value_type, const value_type*, const value_type&> >::type   iterator;
        typedef rbtree_iterator<value_type, const value_type*, const value_type&>               const_iterator;

        typedef Compare                                                                         key_compare;
        typedef typename type_select<bUniqueKeys, easy::pair<iterator, bool>, iterator>::type  insert_return_type;  // map/set::insert return a pair, multimap/multiset::iterator return an iterator.
        typedef rbtree<Key, Value, Compare,
            ExtractKey, bMutableIterators, bUniqueKeys>                             this_type;
        typedef rb_base<Key, Value, Compare, ExtractKey, bUniqueKeys, this_type>                base_type;
        typedef integral_constant<bool, bUniqueKeys>                                            has_unique_keys_type;
        typedef typename base_type::extract_key                                                 extract_key;

        using base_type::mCompare;

    public:
        rbtree_node_base  mAnchor;      /// This node acts as end() and its mpLeft points to begin(), and mpRight points to rbegin() (the last node on the right).
        size_type         mnSize;       /// Stores the count of nodes in the tree (not counting the anchor node).

    public:
        // ctor/dtor
        rbtree();
        rbtree(const Compare& compare);
        rbtree(const this_type& x);

        template <typename InputIterator>
        rbtree(InputIterator first, InputIterator last, const Compare& compare);

        ~rbtree();

    public:
        // properties
        const key_compare& key_comp() const { return mCompare; }
        key_compare&       key_comp() { return mCompare; }

        this_type& operator=(const this_type& x);

        void swap(this_type& x);

    public:
        // iterators
        iterator        begin() ;
        const_iterator  begin() const ;
        const_iterator  cbegin() const ;

        iterator        end() ;
        const_iterator  end() const ;
        const_iterator  cend() const ;

    public:
        bool      empty() const ;
        size_type size() const ;

        /// map::insert and set::insert return a pair, while multimap::insert and
        /// multiset::insert return an iterator.
        insert_return_type insert(const value_type& value);

        // C++ standard: inserts value if and only if there is no element with 
        // key equivalent to the key of t in containers with unique keys; always 
        // inserts value in containers with equivalent keys. Always returns the 
        // iterator pointing to the element with key equivalent to the key of value. 
        // iterator position is a hint pointing to where the insert should start
        // to search. However, there is a potential defect/improvement report on this behaviour:
        // LWG issue #233 (http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2005/n1780.html)
        // We follow the same approach as SGI STL/STLPort and use the position as
        // a forced insertion position for the value when possible.
        iterator insert(const_iterator position, const value_type& value);

        template <typename InputIterator>
        void insert(InputIterator first, InputIterator last);

        iterator erase(const_iterator position);
        iterator erase(const_iterator first, const_iterator last);

        // For some reason, multiple STL versions make a specialization 
        // for erasing an array of key_types. I'm pretty sure we don't
        // need this, but just to be safe we will follow suit. 
        // The implementation is trivial. Returns void because the values
        // could well be randomly distributed throughout the tree and thus
        // a return value would be nearly meaningless.
        void erase(const key_type* first, const key_type* last);

        void clear();
        void reset_lose_memory(); // This is a unilateral reset to an initially empty state. No destructors are called, no deallocation occurs.

        iterator       find(const key_type& key);
        const_iterator find(const key_type& key) const;

        /// Implements a find whereby the user supplies a comparison of a different type
        /// than the tree's value_type. A useful case of this is one whereby you have
        /// a container of string objects but want to do searches via passing in char pointers.
        /// The problem is that without this kind of find, you need to do the expensive operation
        /// of converting the char pointer to a string so it can be used as the argument to the
        /// find function.
        ///
        /// Example usage (note that the compare uses string as first type and char* as second):
        ///     set<string> strings;
        ///     strings.find_as("hello", less_2<string, const char*>());
        ///
        template <typename U, typename Compare2>
        iterator       find_as(const U& u, Compare2 compare2);

        template <typename U, typename Compare2>
        const_iterator find_as(const U& u, Compare2 compare2) const;

        iterator       lower_bound(const key_type& key);
        const_iterator lower_bound(const key_type& key) const;

        iterator       upper_bound(const key_type& key);
        const_iterator upper_bound(const key_type& key) const;
    protected:
        void       DoFreeNode(node_type* pNode);

        node_type* DoCreateNode(const value_type& value);
        node_type* DoCreateNode(const node_type* pNodeSource, node_type* pNodeParent);

        node_type* DoCopySubtree(const node_type* pNodeSource, node_type* pNodeDest);
        void       DoNukeSubtree(node_type* pNode);

        easy::pair<iterator, bool> DoInsertValue(true_type, const value_type& value);
        iterator DoInsertValue(false_type, const value_type& value);
        iterator DoInsertValueImpl(node_type* pNodeParent, bool bForceToLeft, const key_type& key, const value_type& value);

        iterator DoInsertValueHint(true_type, const_iterator position, const value_type& value);
        iterator DoInsertValueHint(false_type, const_iterator position, const value_type& value);

        node_type* DoGetKeyInsertionPositionUniqueKeys(bool& canInsert, const key_type& key);
        node_type* DoGetKeyInsertionPositionNonuniqueKeys(const key_type& key);

        node_type* DoGetKeyInsertionPositionUniqueKeysHint(const_iterator position, bool& bForceToLeft, const key_type& key);
        node_type* DoGetKeyInsertionPositionNonuniqueKeysHint(const_iterator position, bool& bForceToLeft, const key_type& key);

    private:
        // rbtree_node_base functions
        //
        // These are the fundamental functions that we use to maintain the 
        // tree. The bulk of the work of the tree maintenance is done in 
        // these functions.
        //
        EASTL_API inline rbtree_node_base* RBTreeGetMinChild(const rbtree_node_base* pNodeBase)
        {
            while (pNodeBase->mpNodeLeft)
                pNodeBase = pNodeBase->mpNodeLeft;
            return const_cast<rbtree_node_base*>(pNodeBase);
        }

        EASTL_API inline rbtree_node_base* RBTreeGetMaxChild(const rbtree_node_base* pNodeBase)
        {
            while (pNodeBase->mpNodeRight)
                pNodeBase = pNodeBase->mpNodeRight;
            return const_cast<rbtree_node_base*>(pNodeBase);
        }

        /// RBTreeIncrement
        /// Returns the previous item in a sorted red-black tree.
        ///
        EASTL_API rbtree_node_base* RBTreeDecrement(const rbtree_node_base* pNode)
        {
            if ((pNode->mpNodeParent->mpNodeParent == pNode) && (pNode->mColor == kRBTreeColorRed))
                return pNode->mpNodeRight;
            else if (pNode->mpNodeLeft)
            {
                rbtree_node_base* pNodeTemp = pNode->mpNodeLeft;

                while (pNodeTemp->mpNodeRight)
                    pNodeTemp = pNodeTemp->mpNodeRight;

                return pNodeTemp;
            }

            rbtree_node_base* pNodeTemp = pNode->mpNodeParent;

            while (pNode == pNodeTemp->mpNodeLeft)
            {
                pNode = pNodeTemp;
                pNodeTemp = pNodeTemp->mpNodeParent;
            }

            return const_cast<rbtree_node_base*>(pNodeTemp);
        }



        /// RBTreeGetBlackCount
        /// Counts the number of black nodes in an red-black tree, from pNode down to the given bottom node.  
        /// We don't count red nodes because red-black trees don't really care about
        /// red node counts; it is black node counts that are significant in the 
        /// maintenance of a balanced tree.
        ///
        EASTL_API size_t RBTreeGetBlackCount(const rbtree_node_base* pNodeTop, const rbtree_node_base* pNodeBottom)
        {
            size_t nCount = 0;

            for (; pNodeBottom; pNodeBottom = pNodeBottom->mpNodeParent)
            {
                if (pNodeBottom->mColor == kRBTreeColorBlack)
                    ++nCount;

                if (pNodeBottom == pNodeTop)
                    break;
            }

            return nCount;
        }


        /// RBTreeRotateLeft
        /// Does a left rotation about the given node. 
        /// If you want to understand tree rotation, any book on algorithms will
        /// discussion the topic in good detail.
        rbtree_node_base* RBTreeRotateLeft(rbtree_node_base* pNode, rbtree_node_base* pNodeRoot)
        {
            rbtree_node_base* const pNodeTemp = pNode->mpNodeRight;

            pNode->mpNodeRight = pNodeTemp->mpNodeLeft;

            if (pNodeTemp->mpNodeLeft)
                pNodeTemp->mpNodeLeft->mpNodeParent = pNode;
            pNodeTemp->mpNodeParent = pNode->mpNodeParent;

            if (pNode == pNodeRoot)
                pNodeRoot = pNodeTemp;
            else if (pNode == pNode->mpNodeParent->mpNodeLeft)
                pNode->mpNodeParent->mpNodeLeft = pNodeTemp;
            else
                pNode->mpNodeParent->mpNodeRight = pNodeTemp;

            pNodeTemp->mpNodeLeft = pNode;
            pNode->mpNodeParent = pNodeTemp;

            return pNodeRoot;
        }



        /// RBTreeRotateRight
        /// Does a right rotation about the given node. 
        /// If you want to understand tree rotation, any book on algorithms will
        /// discussion the topic in good detail.
        rbtree_node_base* RBTreeRotateRight(rbtree_node_base* pNode, rbtree_node_base* pNodeRoot)
        {
            rbtree_node_base* const pNodeTemp = pNode->mpNodeLeft;

            pNode->mpNodeLeft = pNodeTemp->mpNodeRight;

            if (pNodeTemp->mpNodeRight)
                pNodeTemp->mpNodeRight->mpNodeParent = pNode;
            pNodeTemp->mpNodeParent = pNode->mpNodeParent;

            if (pNode == pNodeRoot)
                pNodeRoot = pNodeTemp;
            else if (pNode == pNode->mpNodeParent->mpNodeRight)
                pNode->mpNodeParent->mpNodeRight = pNodeTemp;
            else
                pNode->mpNodeParent->mpNodeLeft = pNodeTemp;

            pNodeTemp->mpNodeRight = pNode;
            pNode->mpNodeParent = pNodeTemp;

            return pNodeRoot;
        }




        /// RBTreeInsert
        /// Insert a node into the tree and rebalance the tree as a result of the 
        /// disturbance the node introduced.
        ///
        EASTL_API void RBTreeInsert(rbtree_node_base* pNode,
            rbtree_node_base* pNodeParent,
            rbtree_node_base* pNodeAnchor,
            RBTreeSide insertionSide)
        {
            rbtree_node_base*& pNodeRootRef = pNodeAnchor->mpNodeParent;

            // Initialize fields in new node to insert.
            pNode->mpNodeParent = pNodeParent;
            pNode->mpNodeRight = NULL;
            pNode->mpNodeLeft = NULL;
            pNode->mColor = kRBTreeColorRed;

            // Insert the node.
            if (insertionSide == kRBTreeSideLeft)
            {
                pNodeParent->mpNodeLeft = pNode; // Also makes (leftmost = pNode) when (pNodeParent == pNodeAnchor)

                if (pNodeParent == pNodeAnchor)
                {
                    pNodeAnchor->mpNodeParent = pNode;
                    pNodeAnchor->mpNodeRight = pNode;
                } else if (pNodeParent == pNodeAnchor->mpNodeLeft)
                    pNodeAnchor->mpNodeLeft = pNode; // Maintain leftmost pointing to min node
            } else
            {
                pNodeParent->mpNodeRight = pNode;

                if (pNodeParent == pNodeAnchor->mpNodeRight)
                    pNodeAnchor->mpNodeRight = pNode; // Maintain rightmost pointing to max node
            }

            // Rebalance the tree.
            while ((pNode != pNodeRootRef) && (pNode->mpNodeParent->mColor == kRBTreeColorRed))
            {
                EA_ANALYSIS_ASSUME(pNode->mpNodeParent != NULL);
                rbtree_node_base* const pNodeParentParent = pNode->mpNodeParent->mpNodeParent;

                if (pNode->mpNodeParent == pNodeParentParent->mpNodeLeft)
                {
                    rbtree_node_base* const pNodeTemp = pNodeParentParent->mpNodeRight;

                    if (pNodeTemp && (pNodeTemp->mColor == kRBTreeColorRed))
                    {
                        pNode->mpNodeParent->mColor = kRBTreeColorBlack;
                        pNodeTemp->mColor = kRBTreeColorBlack;
                        pNodeParentParent->mColor = kRBTreeColorRed;
                        pNode = pNodeParentParent;
                    } else
                    {
                        if (pNode->mpNodeParent && pNode == pNode->mpNodeParent->mpNodeRight)
                        {
                            pNode = pNode->mpNodeParent;
                            pNodeRootRef = RBTreeRotateLeft(pNode, pNodeRootRef);
                        }

                        EA_ANALYSIS_ASSUME(pNode->mpNodeParent != NULL);
                        pNode->mpNodeParent->mColor = kRBTreeColorBlack;
                        pNodeParentParent->mColor = kRBTreeColorRed;
                        pNodeRootRef = RBTreeRotateRight(pNodeParentParent, pNodeRootRef);
                    }
                } else
                {
                    rbtree_node_base* const pNodeTemp = pNodeParentParent->mpNodeLeft;

                    if (pNodeTemp && (pNodeTemp->mColor == kRBTreeColorRed))
                    {
                        pNode->mpNodeParent->mColor = kRBTreeColorBlack;
                        pNodeTemp->mColor = kRBTreeColorBlack;
                        pNodeParentParent->mColor = kRBTreeColorRed;
                        pNode = pNodeParentParent;
                    } else
                    {
                        EA_ANALYSIS_ASSUME(pNode != NULL && pNode->mpNodeParent != NULL);

                        if (pNode == pNode->mpNodeParent->mpNodeLeft)
                        {
                            pNode = pNode->mpNodeParent;
                            pNodeRootRef = RBTreeRotateRight(pNode, pNodeRootRef);
                        }

                        pNode->mpNodeParent->mColor = kRBTreeColorBlack;
                        pNodeParentParent->mColor = kRBTreeColorRed;
                        pNodeRootRef = RBTreeRotateLeft(pNodeParentParent, pNodeRootRef);
                    }
                }
            }

            EA_ANALYSIS_ASSUME(pNodeRootRef != NULL);
            pNodeRootRef->mColor = kRBTreeColorBlack;

        } // RBTreeInsert




          /// RBTreeErase
          /// Erase a node from the tree.
          ///
        EASTL_API void RBTreeErase(rbtree_node_base* pNode, rbtree_node_base* pNodeAnchor)
        {
            rbtree_node_base*& pNodeRootRef = pNodeAnchor->mpNodeParent;
            rbtree_node_base*& pNodeLeftmostRef = pNodeAnchor->mpNodeLeft;
            rbtree_node_base*& pNodeRightmostRef = pNodeAnchor->mpNodeRight;
            rbtree_node_base*  pNodeSuccessor = pNode;
            rbtree_node_base*  pNodeChild = NULL;
            rbtree_node_base*  pNodeChildParent = NULL;

            if (pNodeSuccessor->mpNodeLeft == NULL)         // pNode has at most one non-NULL child.
                pNodeChild = pNodeSuccessor->mpNodeRight;  // pNodeChild might be null.
            else if (pNodeSuccessor->mpNodeRight == NULL)   // pNode has exactly one non-NULL child.
                pNodeChild = pNodeSuccessor->mpNodeLeft;   // pNodeChild is not null.
            else
            {
                // pNode has two non-null children. Set pNodeSuccessor to pNode's successor. pNodeChild might be NULL.
                pNodeSuccessor = pNodeSuccessor->mpNodeRight;

                while (pNodeSuccessor->mpNodeLeft)
                    pNodeSuccessor = pNodeSuccessor->mpNodeLeft;

                pNodeChild = pNodeSuccessor->mpNodeRight;
            }

            // Here we remove pNode from the tree and fix up the node pointers appropriately around it.
            if (pNodeSuccessor == pNode) // If pNode was a leaf node (had both NULL children)...
            {
                pNodeChildParent = pNodeSuccessor->mpNodeParent;  // Assign pNodeReplacement's parent.

                if (pNodeChild)
                    pNodeChild->mpNodeParent = pNodeSuccessor->mpNodeParent;

                if (pNode == pNodeRootRef) // If the node being deleted is the root node...
                    pNodeRootRef = pNodeChild; // Set the new root node to be the pNodeReplacement.
                else
                {
                    if (pNode == pNode->mpNodeParent->mpNodeLeft) // If pNode is a left node...
                        pNode->mpNodeParent->mpNodeLeft = pNodeChild;  // Make pNode's replacement node be on the same side.
                    else
                        pNode->mpNodeParent->mpNodeRight = pNodeChild;
                    // Now pNode is disconnected from the bottom of the tree (recall that in this pathway pNode was determined to be a leaf).
                }

                if (pNode == pNodeLeftmostRef) // If pNode is the tree begin() node...
                {
                    // Because pNode is the tree begin(), pNode->mpNodeLeft must be NULL.
                    // Here we assign the new begin() (first node).
                    if (pNode->mpNodeRight && pNodeChild)
                    {
                        pNodeLeftmostRef = RBTreeGetMinChild(pNodeChild);
                    } else
                        pNodeLeftmostRef = pNode->mpNodeParent; // This  makes (pNodeLeftmostRef == end()) if (pNode == root node)
                }

                if (pNode == pNodeRightmostRef) // If pNode is the tree last (rbegin()) node...
                {
                    // Because pNode is the tree rbegin(), pNode->mpNodeRight must be NULL.
                    // Here we assign the new rbegin() (last node)
                    if (pNode->mpNodeLeft && pNodeChild)
                    {
                        pNodeRightmostRef = RBTreeGetMaxChild(pNodeChild);
                    } else // pNodeChild == pNode->mpNodeLeft
                        pNodeRightmostRef = pNode->mpNodeParent; // makes pNodeRightmostRef == &mAnchor if pNode == pNodeRootRef
                }
            } else // else (pNodeSuccessor != pNode)
            {
                // Relink pNodeSuccessor in place of pNode. pNodeSuccessor is pNode's successor.
                // We specifically set pNodeSuccessor to be on the right child side of pNode, so fix up the left child side.
                pNode->mpNodeLeft->mpNodeParent = pNodeSuccessor;
                pNodeSuccessor->mpNodeLeft = pNode->mpNodeLeft;

                if (pNodeSuccessor == pNode->mpNodeRight) // If pNode's successor was at the bottom of the tree... (yes that's effectively what this statement means)
                    pNodeChildParent = pNodeSuccessor; // Assign pNodeReplacement's parent.
                else
                {
                    pNodeChildParent = pNodeSuccessor->mpNodeParent;

                    if (pNodeChild)
                        pNodeChild->mpNodeParent = pNodeChildParent;

                    pNodeChildParent->mpNodeLeft = pNodeChild;

                    pNodeSuccessor->mpNodeRight = pNode->mpNodeRight;
                    pNode->mpNodeRight->mpNodeParent = pNodeSuccessor;
                }

                if (pNode == pNodeRootRef)
                    pNodeRootRef = pNodeSuccessor;
                else if (pNode == pNode->mpNodeParent->mpNodeLeft)
                    pNode->mpNodeParent->mpNodeLeft = pNodeSuccessor;
                else
                    pNode->mpNodeParent->mpNodeRight = pNodeSuccessor;

                // Now pNode is disconnected from the tree.

                pNodeSuccessor->mpNodeParent = pNode->mpNodeParent;
                easy::swap(pNodeSuccessor->mColor, pNode->mColor);
            }

            // Here we do tree balancing as per the conventional red-black tree algorithm.
            if (pNode->mColor == kRBTreeColorBlack)
            {
                while ((pNodeChild != pNodeRootRef) && ((pNodeChild == NULL) || (pNodeChild->mColor == kRBTreeColorBlack)))
                {
                    if (pNodeChild == pNodeChildParent->mpNodeLeft)
                    {
                        rbtree_node_base* pNodeTemp = pNodeChildParent->mpNodeRight;

                        if (pNodeTemp->mColor == kRBTreeColorRed)
                        {
                            pNodeTemp->mColor = kRBTreeColorBlack;
                            pNodeChildParent->mColor = kRBTreeColorRed;
                            pNodeRootRef = RBTreeRotateLeft(pNodeChildParent, pNodeRootRef);
                            pNodeTemp = pNodeChildParent->mpNodeRight;
                        }

                        if (((pNodeTemp->mpNodeLeft == NULL) || (pNodeTemp->mpNodeLeft->mColor == kRBTreeColorBlack)) &&
                            ((pNodeTemp->mpNodeRight == NULL) || (pNodeTemp->mpNodeRight->mColor == kRBTreeColorBlack)))
                        {
                            pNodeTemp->mColor = kRBTreeColorRed;
                            pNodeChild = pNodeChildParent;
                            pNodeChildParent = pNodeChildParent->mpNodeParent;
                        } else
                        {
                            if ((pNodeTemp->mpNodeRight == NULL) || (pNodeTemp->mpNodeRight->mColor == kRBTreeColorBlack))
                            {
                                pNodeTemp->mpNodeLeft->mColor = kRBTreeColorBlack;
                                pNodeTemp->mColor = kRBTreeColorRed;
                                pNodeRootRef = RBTreeRotateRight(pNodeTemp, pNodeRootRef);
                                pNodeTemp = pNodeChildParent->mpNodeRight;
                            }

                            pNodeTemp->mColor = pNodeChildParent->mColor;
                            pNodeChildParent->mColor = kRBTreeColorBlack;

                            if (pNodeTemp->mpNodeRight)
                                pNodeTemp->mpNodeRight->mColor = kRBTreeColorBlack;

                            pNodeRootRef = RBTreeRotateLeft(pNodeChildParent, pNodeRootRef);
                            break;
                        }
                    } else
                    {
                        // The following is the same as above, with mpNodeRight <-> mpNodeLeft.
                        rbtree_node_base* pNodeTemp = pNodeChildParent->mpNodeLeft;

                        if (pNodeTemp->mColor == kRBTreeColorRed)
                        {
                            pNodeTemp->mColor = kRBTreeColorBlack;
                            pNodeChildParent->mColor = kRBTreeColorRed;

                            pNodeRootRef = RBTreeRotateRight(pNodeChildParent, pNodeRootRef);
                            pNodeTemp = pNodeChildParent->mpNodeLeft;
                        }

                        if (((pNodeTemp->mpNodeRight == NULL) || (pNodeTemp->mpNodeRight->mColor == kRBTreeColorBlack)) &&
                            ((pNodeTemp->mpNodeLeft == NULL) || (pNodeTemp->mpNodeLeft->mColor == kRBTreeColorBlack)))
                        {
                            pNodeTemp->mColor = kRBTreeColorRed;
                            pNodeChild = pNodeChildParent;
                            pNodeChildParent = pNodeChildParent->mpNodeParent;
                        } else
                        {
                            if ((pNodeTemp->mpNodeLeft == NULL) || (pNodeTemp->mpNodeLeft->mColor == kRBTreeColorBlack))
                            {
                                pNodeTemp->mpNodeRight->mColor = kRBTreeColorBlack;
                                pNodeTemp->mColor = kRBTreeColorRed;

                                pNodeRootRef = RBTreeRotateLeft(pNodeTemp, pNodeRootRef);
                                pNodeTemp = pNodeChildParent->mpNodeLeft;
                            }

                            pNodeTemp->mColor = pNodeChildParent->mColor;
                            pNodeChildParent->mColor = kRBTreeColorBlack;

                            if (pNodeTemp->mpNodeLeft)
                                pNodeTemp->mpNodeLeft->mColor = kRBTreeColorBlack;

                            pNodeRootRef = RBTreeRotateRight(pNodeChildParent, pNodeRootRef);
                            break;
                        }
                    }
                }

                if (pNodeChild)
                    pNodeChild->mColor = kRBTreeColorBlack;
            }

        } // RBTreeErase

    }; // rbtree


    // The rest of the functions are non-trivial and are found in 
    // the corresponding .cpp file to this file.



    ///////////////////////////////////////////////////////////////////////
    // rbtree_iterator functions
    ///////////////////////////////////////////////////////////////////////

    template <typename T, typename Pointer, typename Reference>
    rbtree_iterator<T, Pointer, Reference>::rbtree_iterator()
        : mpNode(NULL) { }


    template <typename T, typename Pointer, typename Reference>
    rbtree_iterator<T, Pointer, Reference>::rbtree_iterator(const node_type* pNode)
        : mpNode(static_cast<node_type*>(const_cast<node_type*>(pNode))) { }


    template <typename T, typename Pointer, typename Reference>
    rbtree_iterator<T, Pointer, Reference>::rbtree_iterator(const iterator& x)
        : mpNode(x.mpNode) { }


    template <typename T, typename Pointer, typename Reference>
    typename rbtree_iterator<T, Pointer, Reference>::reference
        rbtree_iterator<T, Pointer, Reference>::operator*() const
    {
        return mpNode->mValue;
    }


    template <typename T, typename Pointer, typename Reference>
    typename rbtree_iterator<T, Pointer, Reference>::pointer
        rbtree_iterator<T, Pointer, Reference>::operator->() const
    {
        return &mpNode->mValue;
    }


    template <typename T, typename Pointer, typename Reference>
    typename rbtree_iterator<T, Pointer, Reference>::this_type&
        rbtree_iterator<T, Pointer, Reference>::operator++()
    {
        mpNode = static_cast<node_type*>(RBTreeIncrement(mpNode));
        return *this;
    }


    template <typename T, typename Pointer, typename Reference>
    typename rbtree_iterator<T, Pointer, Reference>::this_type
        rbtree_iterator<T, Pointer, Reference>::operator++(int)
    {
        this_type temp(*this);
        mpNode = static_cast<node_type*>(RBTreeIncrement(mpNode));
        return temp;
    }


    template <typename T, typename Pointer, typename Reference>
    typename rbtree_iterator<T, Pointer, Reference>::this_type&
        rbtree_iterator<T, Pointer, Reference>::operator--()
    {
        mpNode = static_cast<node_type*>(RBTreeDecrement(mpNode));
        return *this;
    }


    template <typename T, typename Pointer, typename Reference>
    typename rbtree_iterator<T, Pointer, Reference>::this_type
        rbtree_iterator<T, Pointer, Reference>::operator--(int)
    {
        this_type temp(*this);
        mpNode = static_cast<node_type*>(RBTreeDecrement(mpNode));
        return temp;
    }


    // The C++ defect report #179 requires that we support comparisons between const and non-const iterators.
    // Thus we provide additional template paremeters here to support this. The defect report does not
    // require us to support comparisons between reverse_iterators and const_reverse_iterators.
    template <typename T, typename PointerA, typename ReferenceA, typename PointerB, typename ReferenceB>
    inline bool operator==(const rbtree_iterator<T, PointerA, ReferenceA>& a,
        const rbtree_iterator<T, PointerB, ReferenceB>& b)
    {
        return a.mpNode == b.mpNode;
    }


    template <typename T, typename PointerA, typename ReferenceA, typename PointerB, typename ReferenceB>
    inline bool operator!=(const rbtree_iterator<T, PointerA, ReferenceA>& a,
        const rbtree_iterator<T, PointerB, ReferenceB>& b)
    {
        return a.mpNode != b.mpNode;
    }


    // We provide a version of operator!= for the case where the iterators are of the 
    // same type. This helps prevent ambiguity errors in the presence of rel_ops.
    template <typename T, typename Pointer, typename Reference>
    inline bool operator!=(const rbtree_iterator<T, Pointer, Reference>& a,
        const rbtree_iterator<T, Pointer, Reference>& b)
    {
        return a.mpNode != b.mpNode;
    }




    ///////////////////////////////////////////////////////////////////////
    // rbtree functions
    ///////////////////////////////////////////////////////////////////////

    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    inline rbtree<K, V, C, E, bM, bU>::rbtree()
        : mAnchor(),
        mnSize(0)
    {
        reset_lose_memory();
    }

    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    inline rbtree<K, V, C, E, bM, bU>::rbtree(const C& compare)
        : base_type(compare),
        mAnchor(),
        mnSize(0)
    {
        reset_lose_memory();
    }


    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    inline rbtree<K, V, C, E, bM, bU>::rbtree(const this_type& x)
        : base_type(x.mCompare),
        mAnchor(),
        mnSize(0)
    {
        reset_lose_memory();

        if (x.mAnchor.mpNodeParent) // mAnchor.mpNodeParent is the rb_tree root node.
        {
            mAnchor.mpNodeParent = DoCopySubtree((const node_type*)x.mAnchor.mpNodeParent, (node_type*)&mAnchor);
            mAnchor.mpNodeRight = RBTreeGetMaxChild(mAnchor.mpNodeParent);
            mAnchor.mpNodeLeft = RBTreeGetMinChild(mAnchor.mpNodeParent);
            mnSize = x.mnSize;
        }
    }


    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    template <typename InputIterator>
    inline rbtree<K, V, C, E, bM, bU>::rbtree(InputIterator first, InputIterator last, const C& compare)
        : base_type(compare),
        mAnchor(),
        mnSize(0)
    {
        reset_lose_memory();

        for (; first != last; ++first)
            insert(*first);
    }


    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    inline rbtree<K, V, C, E, bM, bU>::~rbtree()
    {
        // Erase the entire tree. DoNukeSubtree is not a 
        // conventional erase function, as it does no rebalancing.
        DoNukeSubtree((node_type*)mAnchor.mpNodeParent);
    }


    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    inline typename rbtree<K, V, C, E, bM, bU>::size_type
        rbtree<K, V, C, E, bM, bU>::size() const 
    {
        return mnSize;
    }


    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    inline bool rbtree<K, V, C, E, bM, bU>::empty() const 
    {
        return (mnSize == 0);
    }


    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    inline typename rbtree<K, V, C, E, bM, bU>::iterator
        rbtree<K, V, C, E, bM, bU>::begin() 
    {
        return iterator(static_cast<node_type*>(mAnchor.mpNodeLeft));
    }


    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    inline typename rbtree<K, V, C, E, bM, bU>::const_iterator
        rbtree<K, V, C, E, bM, bU>::begin() const 
    {
        return const_iterator(static_cast<node_type*>(const_cast<rbtree_node_base*>(mAnchor.mpNodeLeft)));
    }


    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    inline typename rbtree<K, V, C, E, bM, bU>::const_iterator
        rbtree<K, V, C, E, bM, bU>::cbegin() const 
    {
        return const_iterator(static_cast<node_type*>(const_cast<rbtree_node_base*>(mAnchor.mpNodeLeft)));
    }


    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    inline typename rbtree<K, V, C, E, bM, bU>::iterator
        rbtree<K, V, C, E, bM, bU>::end() 
    {
        return iterator(static_cast<node_type*>(&mAnchor));
    }


    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    inline typename rbtree<K, V, C, E, bM, bU>::const_iterator
        rbtree<K, V, C, E, bM, bU>::end() const 
    {
        return const_iterator(static_cast<node_type*>(const_cast<rbtree_node_base*>(&mAnchor)));
    }


    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    inline typename rbtree<K, V, C, E, bM, bU>::const_iterator
        rbtree<K, V, C, E, bM, bU>::cend() const 
    {
        return const_iterator(static_cast<node_type*>(const_cast<rbtree_node_base*>(&mAnchor)));
    }

    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    inline typename rbtree<K, V, C, E, bM, bU>::this_type&
        rbtree<K, V, C, E, bM, bU>::operator=(const this_type& x)
    {
        if (this != &x)
        {
            clear();

            base_type::mCompare = x.mCompare;

            if (x.mAnchor.mpNodeParent) // mAnchor.mpNodeParent is the rb_tree root node.
            {
                mAnchor.mpNodeParent = DoCopySubtree((const node_type*)x.mAnchor.mpNodeParent, (node_type*)&mAnchor);
                mAnchor.mpNodeRight = RBTreeGetMaxChild(mAnchor.mpNodeParent);
                mAnchor.mpNodeLeft = RBTreeGetMinChild(mAnchor.mpNodeParent);
                mnSize = x.mnSize;
            }
        }
        return *this;
    }


    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    void rbtree<K, V, C, E, bM, bU>::swap(this_type& x)
    {
        const this_type temp(*this); // Can't call easy::swap because that would
        *this = x;                   // itself call this member swap function.
        x = temp;
    }


    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    inline typename rbtree<K, V, C, E, bM, bU>::insert_return_type // map/set::insert return a pair, multimap/multiset::iterator return an iterator.
        rbtree<K, V, C, E, bM, bU>::insert(const value_type& value)
    {
        return DoInsertValue(has_unique_keys_type(), value);
    }


    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    typename rbtree<K, V, C, E, bM, bU>::iterator
        rbtree<K, V, C, E, bM, bU>::insert(const_iterator position, const value_type& value)
    {
        return DoInsertValueHint(has_unique_keys_type(), position, value);
    }


    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    typename rbtree<K, V, C, E, bM, bU>::node_type*
        rbtree<K, V, C, E, bM, bU>::DoGetKeyInsertionPositionUniqueKeys(bool& canInsert, const key_type& key)
    {
        // This code is essentially a slightly modified copy of the the rbtree::insert 
        // function whereby this version takes a key and not a full value_type.
        extract_key extractKey;

        node_type* pCurrent = (node_type*)mAnchor.mpNodeParent; // Start with the root node.
        node_type* pLowerBound = (node_type*)&mAnchor;             // Set it to the container end for now.
        node_type* pParent;                                        // This will be where we insert the new node.

        bool bValueLessThanNode = true; // If the tree is empty, this will result in an insertion at the front.

                                        // Find insertion position of the value. This will either be a position which 
                                        // already contains the value, a position which is greater than the value or
                                        // end(), which we treat like a position which is greater than the value.
        while (EASY_LIKELY(pCurrent)) // Do a walk down the tree.
        {
            bValueLessThanNode = mCompare(key, extractKey(pCurrent->mValue));
            pLowerBound = pCurrent;

            if (bValueLessThanNode)
            {
                EASY_VALIDATE_COMPARE(!mCompare(extractKey(pCurrent->mValue), key)); // Validate that the compare function is sane.
                pCurrent = (node_type*)pCurrent->mpNodeLeft;
            } else
                pCurrent = (node_type*)pCurrent->mpNodeRight;
        }

        pParent = pLowerBound; // pLowerBound is actually upper bound right now (i.e. it is > value instead of <=), but we will make it the lower bound below.

        if (bValueLessThanNode) // If we ended up on the left side of the last parent node...
        {
            if (EASY_LIKELY(pLowerBound != (node_type*)mAnchor.mpNodeLeft)) // If the tree was empty or if we otherwise need to insert at the very front of the tree...
            {
                // At this point, pLowerBound points to a node which is > than value.
                // Move it back by one, so that it points to a node which is <= value.
                pLowerBound = (node_type*)RBTreeDecrement(pLowerBound);
            } else
            {
                canInsert = true;
                return pLowerBound;
            }
        }

        // Since here we require values to be unique, we will do nothing if the value already exists.
        if (mCompare(extractKey(pLowerBound->mValue), key)) // If the node is < the value (i.e. if value is >= the node)...
        {
            EASY_VALIDATE_COMPARE(!mCompare(key, extractKey(pLowerBound->mValue))); // Validate that the compare function is sane.
            canInsert = true;
            return pParent;
        }

        // The item already exists (as found by the compare directly above), so return false.
        canInsert = false;
        return pLowerBound;
    }


    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    typename rbtree<K, V, C, E, bM, bU>::node_type*
        rbtree<K, V, C, E, bM, bU>::DoGetKeyInsertionPositionNonuniqueKeys(const key_type& key)
    {
        // This is the pathway for insertion of non-unique keys (multimap and multiset, but not map and set).
        node_type* pCurrent = (node_type*)mAnchor.mpNodeParent; // Start with the root node.
        node_type* pRangeEnd = (node_type*)&mAnchor;             // Set it to the container end for now.
        extract_key extractKey;

        while (pCurrent)
        {
            pRangeEnd = pCurrent;

            if (mCompare(key, extractKey(pCurrent->mValue)))
            {
                EASY_VALIDATE_COMPARE(!mCompare(extractKey(pCurrent->mValue), key)); // Validate that the compare function is sane.
                pCurrent = (node_type*)pCurrent->mpNodeLeft;
            } else
                pCurrent = (node_type*)pCurrent->mpNodeRight;
        }

        return pRangeEnd;
    }


    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    easy::pair<typename rbtree<K, V, C, E, bM, bU>::iterator, bool>
        rbtree<K, V, C, E, bM, bU>::DoInsertValue(true_type, const value_type& value) // true_type means keys are unique.
    {
        extract_key extractKey;
        key_type    key(extractKey(value));
        bool        canInsert;
        node_type*  pPosition = DoGetKeyInsertionPositionUniqueKeys(canInsert, extractKey(value));

        if (canInsert)
        {
            const iterator itResult(DoInsertValueImpl(pPosition, false, key, value));
            return pair<iterator, bool>(itResult, true);
        }

        return pair<iterator, bool>(iterator(pPosition), false);
    }


    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    typename rbtree<K, V, C, E, bM, bU>::iterator
        rbtree<K, V, C, E, bM, bU>::DoInsertValue(false_type, const value_type& value) // false_type means keys are not unique.
    {
        extract_key extractKey;
        key_type    key(extractKey(value));
        node_type*  pPosition = DoGetKeyInsertionPositionNonuniqueKeys(key);

        return DoInsertValueImpl(pPosition, false, key, value);
    }


    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    typename rbtree<K, V, C, E, bM, bU>::iterator
        rbtree<K, V, C, E, bM, bU>::DoInsertValueImpl(node_type* pNodeParent, bool bForceToLeft, const key_type& key, const value_type& value)
    {
        RBTreeSide  side;
        extract_key extractKey;

        // The reason we may want to have bForceToLeft == true is that pNodeParent->mValue and value may be equal.
        // In that case it doesn't matter what side we insert on, except that the C++ LWG #233 improvement report
        // suggests that we should use the insert hint position to force an ordering. So that's what we do.
        if (bForceToLeft || (pNodeParent == &mAnchor) || mCompare(key, extractKey(pNodeParent->mValue)))
            side = kRBTreeSideLeft;
        else
            side = kRBTreeSideRight;

        node_type* const pNodeNew = DoCreateNode(value); // Note that pNodeNew->mpLeft, mpRight, mpParent, will be uninitialized.
        RBTreeInsert(pNodeNew, pNodeParent, &mAnchor, side);
        mnSize++;

        return iterator(pNodeNew);
    }


    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    typename rbtree<K, V, C, E, bM, bU>::node_type*
        rbtree<K, V, C, E, bM, bU>::DoGetKeyInsertionPositionUniqueKeysHint(const_iterator position, bool& bForceToLeft, const key_type& key)
    {
        extract_key extractKey;

        if ((position.mpNode != mAnchor.mpNodeRight) && (position.mpNode != &mAnchor)) // If the user specified a specific insertion position...
        {
            iterator itNext(position.mpNode);
            ++itNext;

            // To consider: Change this so that 'position' specifies the position after 
            // where the insertion goes and not the position before where the insertion goes.
            // Doing so would make this more in line with user expectations and with LWG #233.
            const bool bPositionLessThanValue = mCompare(extractKey(position.mpNode->mValue), key);

            if (bPositionLessThanValue) // If (value > *position)...
            {
                EASY_VALIDATE_COMPARE(!mCompare(key, extractKey(position.mpNode->mValue))); // Validate that the compare function is sane.

                const bool bValueLessThanNext = mCompare(key, extractKey(itNext.mpNode->mValue));

                if (bValueLessThanNext) // If value < *itNext...
                {
                    EASY_VALIDATE_COMPARE(!mCompare(extractKey(itNext.mpNode->mValue), key)); // Validate that the compare function is sane.

                    if (position.mpNode->mpNodeRight)
                    {
                        bForceToLeft = true; // Specifically insert in front of (to the left of) itNext (and thus after 'position').
                        return itNext.mpNode;
                    }

                    bForceToLeft = false;
                    return position.mpNode;
                }
            }

            bForceToLeft = false;
            return NULL;  // The above specified hint was not useful, then we do a regular insertion.
        }

        if (mnSize && mCompare(extractKey(((node_type*)mAnchor.mpNodeRight)->mValue), key))
        {
            EASY_VALIDATE_COMPARE(!mCompare(key, extractKey(((node_type*)mAnchor.mpNodeRight)->mValue))); // Validate that the compare function is sane.
            bForceToLeft = false;
            return (node_type*)mAnchor.mpNodeRight;
        }

        bForceToLeft = false;
        return NULL; // The caller can do a default insert.
    }


    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    typename rbtree<K, V, C, E, bM, bU>::node_type*
        rbtree<K, V, C, E, bM, bU>::DoGetKeyInsertionPositionNonuniqueKeysHint(const_iterator position, bool& bForceToLeft, const key_type& key)
    {
        extract_key extractKey;

        if ((position.mpNode != mAnchor.mpNodeRight) && (position.mpNode != &mAnchor)) // If the user specified a specific insertion position...
        {
            iterator itNext(position.mpNode);
            ++itNext;

            // To consider: Change this so that 'position' specifies the position after 
            // where the insertion goes and not the position before where the insertion goes.
            // Doing so would make this more in line with user expectations and with LWG #233.
            if (!mCompare(key, extractKey(position.mpNode->mValue)) && // If value >= *position && 
                !mCompare(extractKey(itNext.mpNode->mValue), key))     // if value <= *itNext...
            {
                if (position.mpNode->mpNodeRight) // If there are any nodes to the right... [this expression will always be true as long as we aren't at the end()]
                {
                    bForceToLeft = true; // Specifically insert in front of (to the left of) itNext (and thus after 'position').
                    return itNext.mpNode;
                }

                bForceToLeft = false;
                return position.mpNode;
            }

            bForceToLeft = false;
            return NULL; // The above specified hint was not useful, then we do a regular insertion.
        }

        // This pathway shouldn't be commonly executed, as the user shouldn't be calling 
        // this hinted version of insert if the user isn't providing a useful hint.
        if (mnSize && !mCompare(key, extractKey(((node_type*)mAnchor.mpNodeRight)->mValue))) // If we are non-empty and the value is >= the last node...
        {
            bForceToLeft = false;
            return (node_type*)mAnchor.mpNodeRight;
        }

        bForceToLeft = false;
        return NULL;
    }


    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    typename rbtree<K, V, C, E, bM, bU>::iterator
        rbtree<K, V, C, E, bM, bU>::DoInsertValueHint(true_type, const_iterator position, const value_type& value) // true_type means keys are unique.
    {
        // This is the pathway for insertion of unique keys (map and set, but not multimap and multiset).
        //
        // We follow the same approach as SGI STL/STLPort and use the position as
        // a forced insertion position for the value when possible.

        extract_key extractKey;
        key_type    key(extractKey(value));
        bool        bForceToLeft;
        node_type*  pPosition = DoGetKeyInsertionPositionUniqueKeysHint(position, bForceToLeft, key);

        if (pPosition)
            return DoInsertValueImpl(pPosition, bForceToLeft, key, value);
        else
            return DoInsertValue(has_unique_keys_type(), value).first;
    }


    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    typename rbtree<K, V, C, E, bM, bU>::iterator
        rbtree<K, V, C, E, bM, bU>::DoInsertValueHint(false_type, const_iterator position, const value_type& value) // false_type means keys are not unique.
    {
        // This is the pathway for insertion of non-unique keys (multimap and multiset, but not map and set).
        //
        // We follow the same approach as SGI STL/STLPort and use the position as
        // a forced insertion position for the value when possible.
        extract_key extractKey;
        key_type    key(extractKey(value));
        bool        bForceToLeft;
        node_type*  pPosition = DoGetKeyInsertionPositionNonuniqueKeysHint(position, bForceToLeft, key);

        if (pPosition)
            return DoInsertValueImpl(pPosition, bForceToLeft, key, value);
        else
            return DoInsertValue(has_unique_keys_type(), value);
    }


    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    template <typename InputIterator>
    void rbtree<K, V, C, E, bM, bU>::insert(InputIterator first, InputIterator last)
    {
        for (; first != last; ++first)
            DoInsertValue(has_unique_keys_type(), *first); // Or maybe we should call 'insert(end(), *first)' instead. If the first-last range was sorted then this might make some sense.
    }


    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    inline void rbtree<K, V, C, E, bM, bU>::clear()
    {
        // Erase the entire tree. DoNukeSubtree is not a 
        // conventional erase function, as it does no rebalancing.
        DoNukeSubtree((node_type*)mAnchor.mpNodeParent);
        reset_lose_memory();
    }

    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    inline void rbtree<K, V, C, E, bM, bU>::reset_lose_memory()
    {
        // The reset_lose_memory function is a special extension function which unilaterally 
        // resets the container to an empty state without freeing the memory of 
        // the contained objects. This is useful for very quickly tearing down a 
        // container built into scratch memory.
        mAnchor.mpNodeRight = &mAnchor;
        mAnchor.mpNodeLeft = &mAnchor;
        mAnchor.mpNodeParent = NULL;
        mAnchor.mColor = kRBTreeColorRed;
        mnSize = 0;
    }


    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    inline typename rbtree<K, V, C, E, bM, bU>::iterator
        rbtree<K, V, C, E, bM, bU>::erase(const_iterator position)
    {
        const iterator iErase(position.mpNode);
        --mnSize; // Interleave this between the two references to itNext. We expect no exceptions to occur during the code below.
        ++position;
        RBTreeErase(iErase.mpNode, &mAnchor);
        DoFreeNode(iErase.mpNode);
        return iterator(position.mpNode);
    }


    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    typename rbtree<K, V, C, E, bM, bU>::iterator
        rbtree<K, V, C, E, bM, bU>::erase(const_iterator first, const_iterator last)
    {
        // We expect that if the user means to clear the container, they will call clear.
        if (EASY_LIKELY((first.mpNode != mAnchor.mpNodeLeft) || (last.mpNode != &mAnchor))) // If (first != begin or last != end) ...
        {
            // Basic implementation:
            while (first != last)
                first = erase(first);
            return iterator(first.mpNode);

            // Inlined implementation:
            //size_type n = 0;
            //while(first != last)
            //{
            //    const iterator itErase(first);
            //    ++n;
            //    ++first;
            //    RBTreeErase(itErase.mpNode, &mAnchor);
            //    DoFreeNode(itErase.mpNode);
            //}
            //mnSize -= n;
            //return first;
        }

        clear();
        return iterator((node_type*)&mAnchor); // Same as: return end();
    }


    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    inline void rbtree<K, V, C, E, bM, bU>::erase(const key_type* first, const key_type* last)
    {
        // We have no choice but to run a loop like this, as the first/last range could
        // have values that are discontiguously located in the tree. And some may not 
        // even be in the tree.
        while (first != last)
            erase(*first++);
    }


    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    typename rbtree<K, V, C, E, bM, bU>::iterator
        rbtree<K, V, C, E, bM, bU>::find(const key_type& key)
    {
        // To consider: Implement this instead via calling lower_bound and 
        // inspecting the result. The following is an implementation of this:
        //    const iterator it(lower_bound(key));
        //    return ((it.mpNode == &mAnchor) || mCompare(key, extractKey(it.mpNode->mValue))) ? iterator(&mAnchor) : it;
        // We don't currently implement the above because in practice people tend to call 
        // find a lot with trees, but very uncommonly call lower_bound.
        extract_key extractKey;

        node_type* pCurrent = (node_type*)mAnchor.mpNodeParent; // Start with the root node.
        node_type* pRangeEnd = (node_type*)&mAnchor;             // Set it to the container end for now.

        while (EASY_LIKELY(pCurrent)) // Do a walk down the tree.
        {
            if (EASY_LIKELY(!mCompare(extractKey(pCurrent->mValue), key))) // If pCurrent is >= key...
            {
                pRangeEnd = pCurrent;
                pCurrent = (node_type*)pCurrent->mpNodeLeft;
            } else
            {
                EASY_VALIDATE_COMPARE(!mCompare(key, extractKey(pCurrent->mValue))); // Validate that the compare function is sane.
                pCurrent = (node_type*)pCurrent->mpNodeRight;
            }
        }

        if (EASY_LIKELY((pRangeEnd != &mAnchor) && !mCompare(key, extractKey(pRangeEnd->mValue))))
            return iterator(pRangeEnd);
        return iterator((node_type*)&mAnchor);
    }


    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    inline typename rbtree<K, V, C, E, bM, bU>::const_iterator
        rbtree<K, V, C, E, bM, bU>::find(const key_type& key) const
    {
        typedef rbtree<K, V, C, E, bM, bU> rbtree_type;
        return const_iterator(const_cast<rbtree_type*>(this)->find(key));
    }


    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    template <typename U, typename Compare2>
    typename rbtree<K, V, C, E, bM, bU>::iterator
        rbtree<K, V, C, E, bM, bU>::find_as(const U& u, Compare2 compare2)
    {
        extract_key extractKey;

        node_type* pCurrent = (node_type*)mAnchor.mpNodeParent; // Start with the root node.
        node_type* pRangeEnd = (node_type*)&mAnchor;             // Set it to the container end for now.

        while (EASY_LIKELY(pCurrent)) // Do a walk down the tree.
        {
            if (EASY_LIKELY(!compare2(extractKey(pCurrent->mValue), u))) // If pCurrent is >= u...
            {
                pRangeEnd = pCurrent;
                pCurrent = (node_type*)pCurrent->mpNodeLeft;
            } else
            {
                EASY_VALIDATE_COMPARE(!compare2(u, extractKey(pCurrent->mValue))); // Validate that the compare function is sane.
                pCurrent = (node_type*)pCurrent->mpNodeRight;
            }
        }

        if (EASY_LIKELY((pRangeEnd != &mAnchor) && !compare2(u, extractKey(pRangeEnd->mValue))))
            return iterator(pRangeEnd);
        return iterator((node_type*)&mAnchor);
    }


    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    template <typename U, typename Compare2>
    inline typename rbtree<K, V, C, E, bM, bU>::const_iterator
        rbtree<K, V, C, E, bM, bU>::find_as(const U& u, Compare2 compare2) const
    {
        typedef rbtree<K, V, C, E, bM, bU> rbtree_type;
        return const_iterator(const_cast<rbtree_type*>(this)->find_as(u, compare2));
    }


    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    typename rbtree<K, V, C, E, bM, bU>::iterator
        rbtree<K, V, C, E, bM, bU>::lower_bound(const key_type& key)
    {
        extract_key extractKey;

        node_type* pCurrent = (node_type*)mAnchor.mpNodeParent; // Start with the root node.
        node_type* pRangeEnd = (node_type*)&mAnchor;             // Set it to the container end for now.

        while (EASY_LIKELY(pCurrent)) // Do a walk down the tree.
        {
            if (EASY_LIKELY(!mCompare(extractKey(pCurrent->mValue), key))) // If pCurrent is >= key...
            {
                pRangeEnd = pCurrent;
                pCurrent = (node_type*)pCurrent->mpNodeLeft;
            } else
            {
                EASY_VALIDATE_COMPARE(!mCompare(key, extractKey(pCurrent->mValue))); // Validate that the compare function is sane.
                pCurrent = (node_type*)pCurrent->mpNodeRight;
            }
        }

        return iterator(pRangeEnd);
    }


    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    inline typename rbtree<K, V, C, E, bM, bU>::const_iterator
        rbtree<K, V, C, E, bM, bU>::lower_bound(const key_type& key) const
    {
        typedef rbtree<K, V, C, E, bM, bU> rbtree_type;
        return const_iterator(const_cast<rbtree_type*>(this)->lower_bound(key));
    }


    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    typename rbtree<K, V, C, E, bM, bU>::iterator
        rbtree<K, V, C, E, bM, bU>::upper_bound(const key_type& key)
    {
        extract_key extractKey;

        node_type* pCurrent = (node_type*)mAnchor.mpNodeParent; // Start with the root node.
        node_type* pRangeEnd = (node_type*)&mAnchor;             // Set it to the container end for now.

        while (EASY_LIKELY(pCurrent)) // Do a walk down the tree.
        {
            if (EASY_LIKELY(mCompare(key, extractKey(pCurrent->mValue)))) // If key is < pCurrent...
            {
                EASY_VALIDATE_COMPARE(!mCompare(extractKey(pCurrent->mValue), key)); // Validate that the compare function is sane.
                pRangeEnd = pCurrent;
                pCurrent = (node_type*)pCurrent->mpNodeLeft;
            } else
                pCurrent = (node_type*)pCurrent->mpNodeRight;
        }

        return iterator(pRangeEnd);
    }


    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    inline typename rbtree<K, V, C, E, bM, bU>::const_iterator
        rbtree<K, V, C, E, bM, bU>::upper_bound(const key_type& key) const
    {
        typedef rbtree<K, V, C, E, bM, bU> rbtree_type;
        return const_iterator(const_cast<rbtree_type*>(this)->upper_bound(key));
    }

    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    inline void rbtree<K, V, C, E, bM, bU>::DoFreeNode(node_type* pNode)
    {
        delete pNode;
    }

    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    typename rbtree<K, V, C, E, bM, bU>::node_type*
        rbtree<K, V, C, E, bM, bU>::DoCreateNode(const value_type& value)
    {
        node_type* const pNode = new node_type();
        pNode->mValue=value;

        return pNode;
    }

    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    typename rbtree<K, V, C, E, bM, bU>::node_type*
        rbtree<K, V, C, E, bM, bU>::DoCreateNode(const node_type* pNodeSource, node_type* pNodeParent)
    {
        node_type* const pNode = DoCreateNode(pNodeSource->mValue);

        pNode->mpNodeRight = NULL;
        pNode->mpNodeLeft = NULL;
        pNode->mpNodeParent = pNodeParent;
        pNode->mColor = pNodeSource->mColor;

        return pNode;
    }


    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    typename rbtree<K, V, C, E, bM, bU>::node_type*
        rbtree<K, V, C, E, bM, bU>::DoCopySubtree(const node_type* pNodeSource, node_type* pNodeDest)
    {
        node_type* const pNewNodeRoot = DoCreateNode(pNodeSource, pNodeDest);

        // Copy the right side of the tree recursively.
        if (pNodeSource->mpNodeRight)
            pNewNodeRoot->mpNodeRight = DoCopySubtree((const node_type*)pNodeSource->mpNodeRight, pNewNodeRoot);

        node_type* pNewNodeLeft;

        for (pNodeSource = (node_type*)pNodeSource->mpNodeLeft, pNodeDest = pNewNodeRoot;
        pNodeSource;
            pNodeSource = (node_type*)pNodeSource->mpNodeLeft, pNodeDest = pNewNodeLeft)
        {
            pNewNodeLeft = DoCreateNode(pNodeSource, pNodeDest);

            pNodeDest->mpNodeLeft = pNewNodeLeft;

            // Copy the right side of the tree recursively.
            if (pNodeSource->mpNodeRight)
                pNewNodeLeft->mpNodeRight = DoCopySubtree((const node_type*)pNodeSource->mpNodeRight, pNewNodeLeft);
        }

        return pNewNodeRoot;
    }


    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    void rbtree<K, V, C, E, bM, bU>::DoNukeSubtree(node_type* pNode)
    {
        while (pNode) // Recursively traverse the tree and destroy items as we go.
        {
            DoNukeSubtree((node_type*)pNode->mpNodeRight);

            node_type* const pNodeLeft = (node_type*)pNode->mpNodeLeft;
            DoFreeNode(pNode);
            pNode = pNodeLeft;
        }
    }



    ///////////////////////////////////////////////////////////////////////
    // global operators
    ///////////////////////////////////////////////////////////////////////

    template <typename K, typename V, typename C, typename E, bool bM, bool bU>
    inline void swap(rbtree<K, V, C, E, bM, bU>& a, rbtree<K, V, C, E, bM, bU>& b)
    {
        a.swap(b);
    }


} // namespace eastl


#endif // __EASY_RBTREE_H__

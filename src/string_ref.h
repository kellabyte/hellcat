// Copyright 2012 Google Inc. All Rights Reserved.  -*- C++ -*-
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/// \file string_ref

#pragma once

#include "array_ref.h"

#include <assert.h>
#include <algorithm>
#include <string>
#include <type_traits>

namespace std {
    
    template<typename charT, typename traits = char_traits<charT>>
    class basic_string_ref;
    
    /// \name Common specializations:
    /// \relates std::basic_string_ref
    /// \xmlonly <nonmember/> \endxmlonly
    /// @{
    typedef basic_string_ref<char> string_ref;
    typedef basic_string_ref<char16_t> u16string_ref;
    typedef basic_string_ref<char32_t> u32string_ref;
    typedef basic_string_ref<wchar_t> wstring_ref;
    /// @}
    
    /// A string-like object that points to a sized piece of memory,
    /// similar to an \c array_ref<charT>.
    ///
    /// We provide implicit constructors so users can pass in a
    /// <code>const char*</code> or a \c std::string wherever a \c
    /// string_ref is expected.
    ///
    /// It is expected that user-defined string-like types will define an
    /// implicit conversion to \c string_ref (or another appropriate
    /// instance of \c basic_string_ref) to interoperate with functions
    /// that need to read strings.
    ///
    /// Unlike \c std::strings and string literals, \c data() may return a
    /// pointer to a buffer that is not null-terminated.  Therefore it is
    /// typically a mistake to pass \c data() to a routine that takes just
    /// a <code>const charT*</code> and expects a null-terminated string.
    ///
    /// \c std::hash specializations shall be provided for \c string_ref,
    /// \c wstring_ref, \c u16string_ref, and \c u32string_ref.
    ///
    /// Methods on \c basic_string_ref<charT> with the same name as
    /// methods on \c array_ref<charT> are intended to have the same
    /// semantics unless the difference is noted specifically.
    ///
    /// \todo Should \c string_ref inherit from \c array_ref? It seems
    /// like a straightforward extension, but there's a risk in that some
    /// of <code>basic_string_ref<charT, traits></code>'s behavior depends
    /// on that \c traits parameter, which isn't propagated to the \c
    /// array_ref<charT> base class.  If \c traits::eq isn't simply \c
    /// operator==(charT), this would cause the behavior to depend on
    /// whether a \c basic_string_ref has been upcasted to \c array_ref.
    /// For the 4 specializations of \c char_traits<> defined by the
    /// standard, this is guaranteed, but it's not necessarily true for
    /// user-defined types.
    template<typename charT, typename traits>
    class basic_string_ref {
    private:
        const charT*      ptr_;
        size_t        length_;
        
    public:
        /// \name types
        /// @{
        typedef charT value_type;
        typedef const charT* pointer;
        typedef const charT& reference;
        typedef const charT& const_reference;
        /// \xmlonly <implementation-defined-type/> \endxmlonly
        /// random-access, contiguous iterator type
        typedef const charT* const_iterator;
        /// Because \c string_ref controls a constant sequence, \c iterator
        /// and \c const_iterator are the same type.
        typedef const_iterator iterator;
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
        typedef const_reverse_iterator reverse_iterator;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        
        static constexpr size_type npos = size_type(-1);
        /// @}
        
        /// \name construct/copy
        ///
        /// \c std::string's substring constructor is omitted because
        /// <code>string_ref(str).substr(pos, n)</code> is clearer
        
        /// @{
        /// \post <code>empty() == true</code>
        constexpr basic_string_ref() : ptr_(nullptr), length_(0) {}
        constexpr basic_string_ref(const basic_string_ref&) = default;
        basic_string_ref& operator=(const basic_string_ref&) = default;
        
        /// \post <code>data() == str</code>, and \c size() is \c
        /// 0 if <code>str == nullptr</code> or \c strlen(str) otherwise.
        basic_string_ref(const charT* str)
        : ptr_(str), length_(str == nullptr ? 0 : traits::length(str)) {}
        /// \post <code>data() == str.data() && size() ==
        /// str.size()</code>
        ///
        /// \todo Arguably, this conversion should be a std::basic_string
        /// conversion operator.
        template<typename Allocator>
        basic_string_ref(const basic_string<charT, traits, Allocator>& str)
        : ptr_(str.data()), length_(str.size()) {}
        /// \post <code>data() == str && size() == len</code>
        constexpr basic_string_ref(const charT* str, size_type len)
        : ptr_(str), length_(len) {
        }
        /// @}
        
        /// \name iterators
        /// @{
        constexpr const_iterator begin() const { return ptr_; }
        constexpr const_iterator end() const { return ptr_ + length_; }
        constexpr const_iterator cbegin() const { return begin(); }
        constexpr const_iterator cend() const { return end(); }
        const_reverse_iterator rbegin() const {
            return const_reverse_iterator(end());
        }
        const_reverse_iterator rend() const {
            return const_reverse_iterator(begin());
        }
        const_reverse_iterator crbegin() const { return rbegin(); }
        const_reverse_iterator crend() const { return rend(); }
        /// @}
        
        /// \name capacity
        /// @{
        constexpr size_type size() const { return length_; }
        constexpr size_type max_size() const {
            return numeric_limits<size_type>::max() / sizeof(charT);
        }
        constexpr bool empty() const { return length_ == 0; }
        
        /// Alias for \c size() for string compatibility.
        constexpr size_type length() const { return size(); }
        /// @}
        
        /// \name element access
        /// @{
        constexpr const charT& operator[](size_t i) const { return ptr_[i]; }
        const charT& at(size_t i) const {
            // This makes at() constexpr as long as the argument is within the
            // bounds of the array_ref.
            return i >= size() ? throw out_of_range("at() argument out of range")
            : ptr_[i];
        }
        
        constexpr const charT& front() const { return ptr_[0]; }
        constexpr const charT& back() const { return ptr_[length_-1]; }
        
        /// \returns A pointer such that [<code>data()</code>,<code>data() +
        /// size()</code>) is a valid range. For a non-empty basic_string_ref,
        /// <code>data() == &front()</code>.
        constexpr const charT* data() const { return ptr_; }
        /// @}
        
        /// \name Outgoing conversion operators
        /// @{
        
        /// \returns An \c array_ref for this \c basic_string_ref's sequence
        /// of characters.
        constexpr operator array_ref<const charT>() const {
            return array_ref<const charT>(data(), size());
        }
        
        /// \returns A new \c string containing the sequence referenced by
        /// \c *this.
        ///
        /// The operator is \c explicit to avoid accidental O(N)
        /// operations on type mismatches.
        ///
        /// Unlike \c array_ref, this operator preserves the \c traits type.
        ///
        /// \todo Arguably, this conversion should be a std::basic_string
        /// constructor.
        template<typename Allocator>
        explicit operator basic_string<charT, traits, Allocator>() const {
            return basic_string<charT, traits, Allocator>(begin(), end());
        }
        /// \returns <code>basic_string<charT, traits>(*this);</code>
        ///
        /// Unlike \c array_ref, this function preserves the \c traits type.
        basic_string<charT, traits> str() const {
            return basic_string<charT, traits>(*this);
        }
        
        /// @}
        
        /// \name mutators
        /// @{
        
        /// \par Effects:
        /// Resets *this to its default-constructed state.
        void clear() { *this = string_ref(); }
        
        /// \par Effects:
        /// Advances the start pointer of this basic_string_ref past \p n
        /// elements without moving the end pointer.
        void remove_prefix(size_type n) {
            assert(length_ >= n);
            ptr_ += n;
            length_ -= n;
        }
        
        /// \par Effects:
        /// Moves the end pointer of this basic_string_ref earlier by \p n elements
        /// without moving the start pointer.
        void remove_suffix(size_type n) {
            assert(length_ >= n);
            length_ -= n;
        }
        /// \par Effects:
        /// <code>remove_suffix(1)</code>
        void pop_back() {
            remove_suffix(1);
        }
        /// \par Effects:
        /// <code>remove_prefix(1)</code>
        void pop_front() {
            remove_prefix(1);
        }
        /// @}
        
        /// \name string operations with the same semantics as std::basic_string
        ///
        /// Unlike std::string, string_ref provides no whole-string methods
        /// with \c position or \c length parameters.  Instead, use the \c
        /// substr() method to create the character sequence you're actually
        /// interested in, and use that.
        ///
        /// @{
        int compare(basic_string_ref x) const {
            const int min_size = size() < x.size() ? size() : x.size();
            int r = min_size == 0 ? 0 : traits::compare(data(), x.data(), min_size);
            if (r == 0) {
                if (size() < x.size()) r = -1;
                else if (size() > x.size()) r = +1;
            }
            return r;
        }
        
        /// \todo It would be nice to have a function that accepts negative
        /// arguments and interprets them like <a
        /// href="http://docs.python.org/py3k/library/stdtypes.html#sequence-types-str-bytes-bytearray-list-tuple-range">Python</a>,
        /// <a
        /// href="http://www.ruby-doc.org/core-1.9.3/String.html#method-i-5B-5D">Ruby</a>,
        /// and <a
        /// href="https://developer.mozilla.org/en/JavaScript/Reference/Global_Objects/Array/slice">JavaScript</a>?
        /// However, \c substr() takes a start and a \em length, not a start
        /// and an \em end, which prevents this from being a simple
        /// extension.  Should we add \c slice() and deprecate \c substr()?
        constexpr basic_string_ref substr(size_type pos, size_type n = npos) const {
            // Recursive implementation to satisfy constexpr.
            return (pos > size()     ? substr(size(), n)
                    : n > size() - pos ? substr(pos, size() - pos)
                    :                    basic_string_ref(data() + pos, n));
        }
        
        /// Copy is more likely than the other methods to be used with an \c
        /// <var>n</var> parameter since it can be the valid length of the
        /// character buffer.  Still <code>this->copy(buf, n)</code> can be
        /// replaced with <code>this->substr(0, n).copy(buf)</code>, so I've
        /// removed the \c n from here too.
        size_type copy(charT* buf) const;
        
        size_type find(basic_string_ref s) const;
        size_type find(charT c) const;
        size_type rfind(basic_string_ref s) const;
        size_type rfind(charT c) const;
        
        size_type find_first_of(basic_string_ref s) const;
        size_type find_first_of(charT c) const { return find(c); }
        size_type find_first_not_of(basic_string_ref s) const;
        size_type find_first_not_of(charT c) const;
        size_type find_last_of(basic_string_ref s) const;
        size_type find_last_of(charT c) const { return rfind(c); }
        size_type find_last_not_of(basic_string_ref s) const;
        size_type find_last_not_of(charT c) const;
        /// @}
        
        /// \name new string operations
        /// @{
        
        /// \returns \c true if \p x is a prefix of \c *this.
        bool starts_with(basic_string_ref x) const {
            return (size() >= x.size() &&
                    traits::compare(data(), x.data(), x.size()) == 0);
        }
        
        /// \returns \c true if \p x is a suffix of \c *this.
        bool ends_with(basic_string_ref x) const {
            return (size() >= x.size() &&
                    traits::compare(data() + (size()-x.size()), x.data(), x.size()) == 0);
        }
        /// @}
    };
    
    template<typename charT, typename traits>
    constexpr typename basic_string_ref<charT, traits>::size_type
    basic_string_ref<charT, traits>::npos;
    
    
    /// \name Comparison operators
    /// \relates std::basic_string_ref
    /// \xmlonly <nonmember/> \endxmlonly
    /// @{
    
    /// Note that equality can be implemented faster than comparison.
    template<typename charT, typename traits>
    inline bool operator==(basic_string_ref<charT, traits> x,
                           basic_string_ref<charT, traits> y) {
        typedef typename basic_string_ref<charT, traits>::size_type size_type;
        size_type len = x.size();
        if (len != y.size()) {
            return false;
        }
        
        const charT* p1 = x.data();
        const charT* p2 = y.data();
        if (p1 == p2) {
            return true;
        }
        if (len <= 0) {
            return true;
        }
        
        // Test last byte in case strings share large common prefix
        if (!traits::eq(p1[len-1], p2[len-1])) return false;
        if (len == 1) return true;
        
        // At this point we can, but don't have to, ignore the last byte.  We use
        // this observation to fold the odd-length case into the even-length case.
        len &= ~size_type(1);
        
        return traits::compare(p1, p2, len) == 0;
    }
    
    template<typename charT, typename traits>
    inline bool operator!=(basic_string_ref<charT, traits> x,
                           basic_string_ref<charT, traits> y) {
        return !(x == y);
    }
    
    template<typename charT, typename traits>
    inline bool operator<(basic_string_ref<charT, traits> x,
                          basic_string_ref<charT, traits> y) {
        return x.compare(y) < 0;
    }
    
    template<typename charT, typename traits>
    inline bool operator>(basic_string_ref<charT, traits> x,
                          basic_string_ref<charT, traits> y) {
        return x.compare(y) > 0;
    }
    
    template<typename charT, typename traits>
    inline bool operator<=(basic_string_ref<charT, traits> x,
                           basic_string_ref<charT, traits> y) {
        return x.compare(y) <= 0;
    }
    
    template<typename charT, typename traits>
    inline bool operator>=(basic_string_ref<charT, traits> x,
                           basic_string_ref<charT, traits> y) {
        return x.compare(y) >= 0;
    }
    
    /// @}
    
    /// \name numeric conversions
    /// Each function <code>sto<var>x</var></code> in this section behaves
    /// as if it calls
    /// <pre><code>sto<var>x</var>(str.str(), idx, base)</code></pre>
    /// Ideally with less copying.
    ///
    /// \todo Instead of passing the idx parameter, we could define
    /// <code>sto<var>x</var>_consume(str, base)</code> as <br>
    /// <code>size_t idx; <br>
    /// auto result = sto<var>x</var>(str.str(), &idx, base); <br>
    /// str.remove_prefix(idx); <br>
    /// return result;</code>
    ///
    /// \relates std::basic_string_ref
    /// \xmlonly <nonmember/> \endxmlonly
    /// @{
    int stoi(const string_ref& str, size_t *idx = 0, int base = 10);
    long stol(const string_ref& str, size_t *idx = 0, int base = 10);
    unsigned long stoul(const string_ref& str, size_t *idx = 0, int base = 10);
    long long stoll(const string_ref& str, size_t *idx = 0, int base = 10);
    unsigned long long stoull(const string_ref& str, size_t *idx = 0, int base = 10);
    float stof(const string_ref& str, size_t *idx = 0);
    double stod(const string_ref& str, size_t *idx = 0);
    long double stold(const string_ref& str, size_t *idx = 0);
    
    int stoi(const wstring_ref& str, size_t *idx = 0, int base = 10);
    long stol(const wstring_ref& str, size_t *idx = 0, int base = 10);
    unsigned long stoul(const wstring_ref& str, size_t *idx = 0, int base = 10);
    long long stoll(const wstring_ref& str, size_t *idx = 0, int base = 10);
    unsigned long long stoull(const wstring_ref& str, size_t *idx = 0, int base = 10);
    float stof(const wstring_ref& str, size_t *idx = 0);
    double stod(const wstring_ref& str, size_t *idx = 0);
    long double stold(const wstring_ref& str, size_t *idx = 0);
    /// @}
    
    /// \name hash support
    /// The template specializations shall meet the requirements of class
    /// template \c hash.
    /// \relates std::basic_string_ref
    /// \xmlonly <nonmember/> \endxmlonly
    /// @{
    template <class T> struct hash;
    template <> struct hash<string_ref>;
    template <> struct hash<u16string_ref>;
    template <> struct hash<u32string_ref>;
    template <> struct hash<wstring_ref>;
    /// @}
    
    // The rest is implementation, not documentation.
    /// @{
    /// @}
    template<typename charT, typename traits>
    auto basic_string_ref<charT, traits>::copy(charT* buf) const -> size_type {
        traits::copy(buf, data(), size());
        return size();
    }
    
    template<typename charT, typename traits>
    auto basic_string_ref<charT, traits>::find(basic_string_ref s) const -> size_type {
        const charT* result = std::search(data(), data() + size(),
                                          s.data(), s.data() + s.size(),
                                          &traits::eq);
        const size_type xpos = result - data();
        return xpos + s.size() <= size() ? xpos : npos;
    }
    
    template<typename charT, typename traits>
    auto basic_string_ref<charT, traits>::find(charT c) const -> size_type {
        if (empty()) {
            return npos;
        }
        const charT* result = traits::find(data(), size(), c);
        return result != NULL ? result - data() : npos;
    }
    
    template<typename charT, typename traits>
    auto basic_string_ref<charT, traits>::rfind(basic_string_ref s) const -> size_type {
        if (size() < s.size()) return npos;
        const size_type ulen = size();
        if (s.size() == 0) return ulen;
        
        const charT* last = data() + (ulen - s.size()) + s.size();
        const charT* result = std::find_end(data(), last,
                                            s.data(), s.data() + s.size(),
                                            &traits::eq);
        return result != last ? result - data() : npos;
    }
    
    // Search range is [0..size()).  If pos == npos, search everything.
    template<typename charT, typename traits>
    auto basic_string_ref<charT, traits>::rfind(charT c) const -> size_type {
        // Note: memrchr() is not available on Windows.
        if (empty()) return npos;
        for (size_type i = size(); i > 0; --i) {
            if (traits::eq(data()[i-1], c)) {
                return i-1;
            }
        }
        return npos;
    }
    
    template<typename charT, typename traits>
    auto basic_string_ref<charT, traits>::find_first_of(basic_string_ref s) const -> size_type {
        if (empty() || s.empty()) return npos;
        // Character search is faster when that's possible.
        if (s.size() == 1) return find(s[0]);
            
            for (size_t i = 0; i < size(); ++i) {
                if (s.find(data()[i]) != npos) {
                    return i;
                }
            }
        return npos;
    }
    
    template<typename charT, typename traits>
    auto basic_string_ref<charT, traits>::find_first_not_of(basic_string_ref s) const -> size_type {
        if (empty()) return npos;
        if (s.empty()) return 0;
        // Character search is faster when that's possible.
        if (s.size() == 1) return find_first_not_of(s[0]);
            
            for (size_t i = 0; i < size(); ++i) {
                if (s.find(data()[i]) == npos) {
                    return i;
                }
            }
        return npos;
    }
    
    template<typename charT, typename traits>
    auto basic_string_ref<charT, traits>::find_first_not_of(charT c) const -> size_type {
        if (size() <= 0) return npos;
        
        for (size_t i = 0; i < size(); ++i) {
            if (!traits::eq(data()[i], c)) {
                return i;
            }
        }
        return npos;
    }
    
    template<typename charT, typename traits>
    auto basic_string_ref<charT, traits>::find_last_of(
                                                       basic_string_ref s) const -> size_type {
        if (empty() || s.empty()) return npos;
        // Character search is faster when that's possible.
        if (s.size() == 1) return rfind(s[0]);
            
            for (size_type i = size(); i > 0; --i) {
                if (s.find(data()[i - 1]) != npos) {
                    return i - 1;
                }
            }
        return npos;
    }
    
    template<typename charT, typename traits>
    auto basic_string_ref<charT, traits>::find_last_not_of(
                                                           basic_string_ref s) const -> size_type {
        if (empty()) return npos;
        
        size_type i = size();
        if (s.empty()) return i - 1;
        
        // Character search is faster when that's possible.
        if (s.size() == 1) return find_last_not_of(s[0]);
            
            for (; i > 0; --i) {
                if (s.find(data()[i - 1]) == npos) {
                    return i - 1;
                }
            }
        return npos;
    }
    
    template<typename charT, typename traits>
    auto basic_string_ref<charT, traits>::find_last_not_of(
                                                           charT c) const -> size_type {
        if (empty()) return npos;
        
        for (size_type i = size(); i > 0; --i) {
            if (!traits::eq(data()[i - 1], c)) {
                return i - 1;
            }
        }
        return npos;
    }
    
}  // namespace std
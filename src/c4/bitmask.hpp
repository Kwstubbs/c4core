#ifndef _C4_BITMASK_HPP_
#define _C4_BITMASK_HPP_

#include <cstring>
#include <type_traits>

#include "c4/enum.hpp"
#include "c4/to_str.hpp"


namespace c4 {

//-----------------------------------------------------------------------------
/** write a bitmask to a stream, formatted as a string */
template< class Enum, class Stream >
Stream& bm2stream(typename std::underlying_type<Enum>::type bits, Stream &s)
{
    using I = typename std::underlying_type<Enum>::type;
    bool written = false;

    auto const& pairs = esyms<Enum>();

    // do reverse iteration to give preference to composite enum symbols,
    // which are likely to appear at the end of the enum sequence
    for(size_t i=pairs.size()-1; i != size_t(-1); --i)
    {
        auto p = pairs[i];
        I b(p.value);
        if((b != 0) && ((bits & b) == b))
        {
            if(written) s << '|'; // append bit-or character
            written = true;
            s << p.name; // append bit string
            bits &= ~b;
        }
    }
    if(!written)
    {
        s << 0;
    }
}

//-----------------------------------------------------------------------------

// some utility macros, undefed below

/** this macro simplifies the code for bm2str()
 * @todo improve this by writing from the end and moving only once. */
#define _c4prependchars(code, num)                                      \
    if(str && (pos + num <= sz))                                        \
    {                                                                   \
        /* move the current string to the right */                      \
        memmove(str + num, str, pos);                                   \
        /* now write in the beginning of the string */                  \
        code;                                                           \
    }                                                                   \
    else if(str && sz) { C4_ERROR("cannot write to string pos=%d num=%d sz=%d", (int)pos, (int)num, (int)sz); } \
    pos += num

#define _c4appendchars(code, num)                                       \
    if(str && (pos + num <= sz))                                        \
    {                                                                   \
        code;                                                           \
    }                                                                   \
    else if(str && sz) { C4_ERROR("cannot write to string pos=%d num=%d sz=%d", (int)pos, (int)num, (int)sz); } \
    pos += num


/** convert a bitmask to string.
 * return the number of characters written. To find the needed size,
 * call first with str=nullptr and sz=0 */
template< class Enum >
size_t bm2str
(
    typename std::underlying_type<Enum>::type bits,
    char *str=nullptr,
    size_t sz=0,
    EnumOffsetType offst=EOFFS_PFX
)
{
    using I = typename std::underlying_type<Enum>::type;
    C4_ASSERT((str == nullptr) == (sz == 0));

    auto syms = esyms<Enum>();
    size_t pos = 0;
    typename EnumSymbols<Enum>::Sym const* zero = nullptr;

    // do reverse iteration to give preference to composite enum symbols,
    // which are likely to appear later in the enum sequence
    for(size_t i=syms.size()-1; i != size_t(-1); --i)
    {
        auto const p = syms[i];
        I b = static_cast<I>(p.value);
        if(b == 0)
        {
            zero = &p; // save this symbol for later
        }
        else if((bits & b) == b)
        {
            bits &= ~b;
            // append bit-or character
            if(pos > 0)
            {
                _c4prependchars(*str = '|', 1);
            }
            // append bit string
            const char *pname = p.name_offs(offst);
            size_t len = strlen(pname);
            _c4prependchars(strncpy(str, pname, len), len);
        }
    }

    C4_CHECK_MSG(bits == 0, "could not find all bits");
    if(pos == 0) // make sure at least something is written
    {
        if(zero) // if we have a zero symbol, use that
        {
            const char *pname = zero->name_offs(offst);
            size_t len = strlen(pname);
            _c4prependchars(strncpy(str, pname, len), len);
        }
        else // otherwise just write an integer zero
        {
            _c4prependchars(*str = '0', 1);
        }
    }
    _c4appendchars(str[pos] = '\0', 1);

    return pos;
}


// cleanup!
#undef _c4appendchars
#undef _c4prependchars


/** scoped enums do not convert automatically to their underlying type,
 * so this SFINAE overload will accept scoped enum symbols and cast them
 * to the underlying type */
template< class Enum >
typename std::enable_if< is_scoped_enum<Enum>::value, size_t >::type
bm2str
(
    Enum bits,
    char *str=nullptr,
    size_t sz=0,
    EnumOffsetType offst=EOFFS_PFX
)
{
    using I = typename std::underlying_type<Enum>::type;
    return bm2str<Enum>(static_cast<I>(bits), str, sz, offst);
}


//-----------------------------------------------------------------------------

namespace detail {

template< class Enum >
typename std::underlying_type<Enum>::type str2bm_read_one(const char *str, size_t sz, bool alnum)
{
    using I = typename std::underlying_type<Enum>::type;
    auto pairs = esyms<Enum>();
    if(alnum)
    {
        auto *p = pairs.find(str, sz);
        C4_CHECK_MSG(p != nullptr, "no valid enum pair name for '%.*s'", (int)sz, str);
        return static_cast<I>(p->value);
    }
    I tmp;
    size_t len = uncat(csubstr(str, sz), tmp);
    C4_CHECK_MSG(len != csubstr::npos, "could not read string as an integral type: '%.*s'", (int)sz, str);
    return tmp;
}

} // namespace detail

/** convert a string to a bitmask */
template< class Enum >
typename std::underlying_type<Enum>::type str2bm(const char *str, size_t sz)
{
    using I = typename std::underlying_type<Enum>::type;

    I val = 0;
    bool started = false;
    bool alnum = false, num = false;
    const char *f = nullptr, *pc = str;
    for( ; pc < str+sz; ++pc)
    {
        const char c = *pc;
        if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_')
        {
            C4_CHECK(( ! num) || ((pc - f) == 1 && (c == 'x' || c == 'X'))); // accept hexadecimal numbers
            if( ! started)
            {
                f = pc;
                alnum = started = true;
            }
        }
        else if(c >= '0' && c <= '9')
        {
            C4_CHECK( ! alnum);
            if(!started)
            {
                f = pc;
                num = started = true;
            }
        }
        else if(c == ':' || c == ' ')
        {
            // skip this char
        }
        else if(c == '|' || c == '\0')
        {
            C4_ASSERT(num != alnum);
            C4_ASSERT(pc > f+1);
            val |= detail::str2bm_read_one<Enum>(f, pc-f-1, alnum);
            started = num = alnum = false;
            if(c == '\0')
            {
                return val;
            }
        }
        else
        {
            C4_ERROR("bad character '%c' in bitmask string", c);
        }
    }

    if(f)
    {
        C4_ASSERT(num != alnum);
        val |= detail::str2bm_read_one<Enum>(f, pc-f, alnum);
    }

    return val;
}

/** convert a string to a bitmask */
template< class Enum >
typename std::underlying_type<Enum>::type str2bm(const char *str)
{
    return str2bm<Enum>(str, strlen(str));
}


} // namespace c4

#endif // _C4_BITMASK_HPP_

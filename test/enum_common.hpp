#ifndef _C4_ENUM_COMMON_HPP_
#define _C4_ENUM_COMMON_HPP_

#include <c4/enum.hpp>

typedef enum {
    FOO = 0,
    BAR,
    BAZ,
} MyEnum;

namespace c4 {
template<>
inline const EnumSymbols<MyEnum> esyms<MyEnum>()
{
    static const EnumSymbols< MyEnum > r({
        {FOO, "FOO"},
        {BAR, "BAR"},
        {BAZ, "BAZ"},
    });
    return r;
}
} // namespace c4


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

enum class MyEnumClass {
    FOO = 0,
    BAR,
    BAZ,
};

namespace c4 {
template<>
inline const EnumSymbols< MyEnumClass > esyms< MyEnumClass >()
{
    static const EnumSymbols< MyEnumClass > r({
        {MyEnumClass::FOO, "MyEnumClass::FOO"},
        {MyEnumClass::BAR, "MyEnumClass::BAR"},
        {MyEnumClass::BAZ, "MyEnumClass::BAZ"},
    });
    return r;
}

template<>
inline size_t eoffs_cls< MyEnumClass >()
{
    return 13; // same as strlen("MyEnumClass::")
}
} // namespace c4


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

typedef enum {
    BM_NONE = 0,
    BM_FOO = 1 << 0,
    BM_BAR = 1 << 1,
    BM_BAZ = 1 << 2,
    BM_FOO_BAR = BM_FOO|BM_BAR,
    BM_FOO_BAR_BAZ = BM_FOO|BM_BAR|BM_BAZ,
} MyBitmask;

namespace c4 {

template<>
inline const EnumSymbols< MyBitmask > esyms< MyBitmask >()
{
    static const EnumSymbols< MyBitmask > r({
        {BM_NONE, "BM_NONE"},
        {BM_FOO, "BM_FOO"},
        {BM_BAR, "BM_BAR"},
        {BM_BAZ, "BM_BAZ"},
        {BM_FOO_BAR, "BM_FOO_BAR"},
        {BM_FOO_BAR_BAZ, "BM_FOO_BAR_BAZ"},
    });
    return r;
}

template<>
inline size_t eoffs_pfx< MyBitmask >()
{
    return 3; // same as strlen("BM_")
}

} // namespace c4


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

enum class MyBitmaskClass {
    BM_NONE = 0,
    BM_FOO = 1 << 0,
    BM_BAR = 1 << 1,
    BM_BAZ = 1 << 2,
    BM_FOO_BAR = BM_FOO|BM_BAR,
    BM_FOO_BAR_BAZ = BM_FOO|BM_BAR|BM_BAZ,
};

namespace c4 {

template<>
inline const EnumSymbols< MyBitmaskClass > esyms< MyBitmaskClass >()
{
    static const EnumSymbols< MyBitmaskClass > r({
        {MyBitmaskClass::BM_NONE,        "MyBitmaskClass::BM_NONE"},
        {MyBitmaskClass::BM_FOO,         "MyBitmaskClass::BM_FOO"},
        {MyBitmaskClass::BM_BAR,         "MyBitmaskClass::BM_BAR"},
        {MyBitmaskClass::BM_BAZ,         "MyBitmaskClass::BM_BAZ"},
        {MyBitmaskClass::BM_FOO_BAR,     "MyBitmaskClass::BM_FOO_BAR"},
        {MyBitmaskClass::BM_FOO_BAR_BAZ, "MyBitmaskClass::BM_FOO_BAR_BAZ"},
    });
    return r;
}

template<> inline size_t eoffs_cls< MyBitmaskClass >()
{
    return 16; // same as strlen("MyBitmaskClass::")
}
template<> inline size_t eoffs_pfx< MyBitmaskClass >()
{
    return 19; // same as strlen("MyBitmaskClass::BM_")
}

} // namespace c4



#endif /* _C4_ENUM_COMMON_HPP_ */

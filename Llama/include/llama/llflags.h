/*
  _    _
 | |  | |__ _ _ __  __ _
 | |__| / _` | '  \/ _` |
 |____|_\__,_|_|_|_\__,_|

 Llama Game Library

 > llcore.h
 Typesafe Flag Class
*/

namespace llama
{
    template <typename BaseEnum>
    class Flags
    {
    public:
    
        using EnumType = typename std::underlying_type<BaseEnum>::type;
    
        constexpr Flags() :
            m_value(0)
        { }

        constexpr Flags(BaseEnum value) :
            m_value(static_cast<EnumType>(value))
        { }
    
        constexpr explicit Flags(EnumType value) :
            m_value(value)
        { }
    
        constexpr void set(BaseEnum bit)                { m_value |= static_cast<EnumType>(bit); }
        constexpr void set(Flags bits)                  { m_value |= bits.m_value; }
        constexpr void reset(BaseEnum bit)              { m_value &= ~static_cast<EnumType>(bit); }
        constexpr void reset(Flags bits)                { m_value &= ~bits.m_value; }
        constexpr void toggle(BaseEnum bit)             { m_value ^= static_cast<EnumType>(bit); }
        constexpr void toggle(Flags bits)               { m_value ^= bits.m_value; }
    
        constexpr bool isSet(BaseEnum bit)              { return (m_value & static_cast<EnumType>(bit)) != 0; }
        constexpr bool isSet(Flags bits)                { return (m_value & bits.m_value) != 0; }
    
        constexpr bool operator[](BaseEnum bit)         { return isSet(bit); }
        constexpr bool operator[](Flags bits)           { return isSet(bits); }
    
        constexpr Flags operator|(BaseEnum other)       { return Flags(m_value | static_cast<EnumType>(other)); }
        constexpr Flags operator|(Flags other)          { return Flags(m_value | other.m_value); }
        constexpr Flags operator|=(BaseEnum other)      { return *this = Flags(m_value | other); }
        constexpr Flags operator|=(Flags other)         { return *this = Flags(m_value | other.m_value); }
        constexpr Flags operator&(BaseEnum other)       { return Flags(m_value & static_cast<EnumType>(other)); }
        constexpr Flags operator&(Flags other)          { return Flags(m_value & other); }
        constexpr Flags operator^(BaseEnum other)       { return Flags(m_value ^ static_cast<EnumType>(other)); }
        constexpr Flags operator^(Flags other)          { return Flags(m_value ^ other.m_value); }
        constexpr Flags operator^=(BaseEnum other)      { return *this = Flags(m_value ^ other); }
        constexpr Flags operator^=(Flags other)         { return *this = Flags(m_value ^ other.m_value); }
    
        constexpr Flags operator~()                     { return Flags(~m_value); }
    
    private:
    
        EnumType m_value;
    };
}
#ifndef MONEYBAG_H
#define MONEYBAG_H

#include <compare>
#include <cstdint>
#include <limits>
#include <ostream>

#include "boost/multiprecision/cpp_int.hpp"

class Moneybag {
   public:
    using coin_number_t = uint64_t;

    constexpr Moneybag(coin_number_t livres, coin_number_t soliduses,
                       coin_number_t deniers);

    constexpr coin_number_t livre_number() const;
    constexpr coin_number_t solidus_number() const;
    constexpr coin_number_t denier_number() const;

    constexpr Moneybag& operator+=(const Moneybag& m);
    constexpr Moneybag operator+(const Moneybag& m) const;

    constexpr Moneybag& operator-=(const Moneybag& m);
    constexpr Moneybag operator-(const Moneybag& m) const;

    constexpr Moneybag& operator*=(uint64_t times);
    constexpr Moneybag operator*(uint64_t times) const;
    constexpr friend Moneybag operator*(uint64_t times, const Moneybag& m);

    constexpr explicit operator bool() const;

    constexpr std::partial_ordering operator<=>(const Moneybag& m) const;
    constexpr bool operator==(const Moneybag& m) const;

    inline friend std::ostream& operator<<(std::ostream& os, const Moneybag& m);

   private:
    coin_number_t livres;
    coin_number_t soliduses;
    coin_number_t deniers;
};

class Value {
   public:
    constexpr explicit Value(uint64_t value_in_denier);
    constexpr Value();
    constexpr explicit Value(const Moneybag& m);

    inline explicit operator std::string() const;

    constexpr std::weak_ordering operator<=>(const Value& v) const;
    constexpr bool operator==(const Value& v) const;
    
    constexpr std::weak_ordering operator<=>(uint64_t v) const;
    constexpr bool operator==(uint64_t v) const;

   private:
    using uint128_t = boost::multiprecision::uint128_t;

    uint128_t value_in_denier;
};

constexpr Moneybag::Moneybag(coin_number_t livres, coin_number_t soliduses,
                             coin_number_t deniers)
    : livres(livres), soliduses(soliduses), deniers(deniers) {}

constexpr Moneybag::coin_number_t Moneybag::livre_number() const {
    return livres;
}

constexpr Moneybag::coin_number_t Moneybag::solidus_number() const {
    return soliduses;
}

constexpr Moneybag::coin_number_t Moneybag::denier_number() const {
    return deniers;
}

constexpr Moneybag& Moneybag::operator+=(const Moneybag& m) {
    const coin_number_t MAX = std::numeric_limits<coin_number_t>::max();
    if (m.livres > MAX - livres || m.soliduses > MAX - soliduses ||
        m.deniers > MAX - deniers) {
        throw std::out_of_range("Out of range while adding another moneybag.");
    }
    livres += m.livres;
    soliduses += m.soliduses;
    deniers += m.deniers;

    return *this;
}

constexpr Moneybag Moneybag::operator+(const Moneybag& m) const {
    return Moneybag(*this) += m;
}

constexpr Moneybag& Moneybag::operator-=(const Moneybag& m) {
    if (m.livres > livres || m.soliduses > soliduses || m.deniers > deniers) {
        throw std::out_of_range(
            "Out of range while substracting another moneybag.");
    }
    livres -= m.livres;
    soliduses -= m.soliduses;
    deniers -= m.deniers;

    return *this;
}

constexpr Moneybag Moneybag::operator-(const Moneybag& m) const {
    return Moneybag(*this) -= m;
}

constexpr Moneybag& Moneybag::operator*=(uint64_t times) {
    if (times > 0) {
        const coin_number_t MAX = std::numeric_limits<coin_number_t>::max();
        if (livres > MAX / times || soliduses > MAX / times ||
            deniers > MAX / times) {
            throw std::out_of_range("Out of range while multiplying moneybag.");
        }
    }
    livres *= times;
    soliduses *= times;
    deniers *= times;

    return *this;
}

constexpr Moneybag Moneybag::operator*(uint64_t times) const {
    return Moneybag(*this) *= times;
}

constexpr Moneybag operator*(uint64_t times, const Moneybag& m) {
    return m * times;
}

constexpr Moneybag::operator bool() const {
    return livres > 0 || soliduses > 0 || deniers > 0;
}

constexpr std::partial_ordering Moneybag::operator<=>(const Moneybag& m) const {
    if (livres == m.livres && soliduses == m.soliduses && deniers == m.deniers) {
        return std::partial_ordering::equivalent;
    }
    if (livres >= m.livres && soliduses >= m.soliduses && deniers >= m.deniers) {
        return std::partial_ordering::greater;
    }
    if (livres <= m.livres && soliduses <= m.soliduses && deniers <= m.deniers) {
        return std::partial_ordering::less;
    }
    return std::partial_ordering::unordered;
}

constexpr bool Moneybag::operator==(const Moneybag& m) const {
    return std::is_eq(*this <=> m);
}

inline std::ostream& operator<<(std::ostream& os, const Moneybag& m) {
    os << "(" << m.livres << " livr" << (m.livres == 1 ? "" : "es") << ", "
       << m.soliduses << " solidus" << (m.soliduses == 1 ? "" : "es") << ", "
       << m.deniers << " denier" << (m.deniers == 1 ? "" : "s") << ")";
    return os;
}

constexpr Value::Value(uint64_t value_in_denier)
    : value_in_denier(value_in_denier) {}

constexpr Value::Value() : value_in_denier(0) {}

constexpr Value::Value(const Moneybag& m)
    : value_in_denier(static_cast<uint128_t>(m.livre_number()) * 240 +
                      static_cast<uint128_t>(m.solidus_number()) * 12 +
                      static_cast<uint128_t>(m.denier_number())) {}

inline Value::operator std::string() const {
    return value_in_denier.str();
}

constexpr std::weak_ordering Value::operator<=>(const Value& v) const {
    if (value_in_denier > v.value_in_denier) {
        return std::weak_ordering::greater;
    }
    if (value_in_denier < v.value_in_denier) {
        return std::weak_ordering::less;
    }
    return std::weak_ordering::equivalent;
}

constexpr std::weak_ordering Value::operator<=>(uint64_t v) const {
    if (value_in_denier > v) {
        return std::weak_ordering::greater;
    }
    if (value_in_denier < v) {
        return std::weak_ordering::less;
    }
    return std::weak_ordering::equivalent;
}

constexpr bool Value::operator==(const Value& v) const {
    return std::is_eq(*this <=> v);
}

constexpr bool Value::operator==(uint64_t v) const {
    return std::is_eq(*this <=> v);
}

constexpr Moneybag Livre(1, 0, 0);
constexpr Moneybag Solidus(0, 1, 0);
constexpr Moneybag Denier(0, 0, 1);

#endif  // MONEYBAG_H

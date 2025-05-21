#include <iostream>
#include <cstdint>
#include <type_traits>
#include <stdexcept>

/*
    This is my implementation for Modular Integers.
    
    Modular Integer Class: mint

    This class represents integers under modular arithmetic with a fixed modulus.
    It supports safe and efficient modular operations including addition, subtraction,
    multiplication, division (via modular inverse), and exponentiation.

    Key Features:
    - Automatic normalization ensures that all values remain in the range [0, MOD).
    - Overloaded operators for intuitive usage: +, -, *, /, ==, !=, etc.
    - Bit-shift operators << and >> are redefined as multiplication/division by powers of 2 (modular).
    - Disallowed operations like comparisons and modulus (%) are explicitly guarded via exceptions.

    Example Usage:
        mint a = 10;
        mint b = 20;
        mint c = a + b;         // 30 mod MOD
        mint d = b / a;         // modular inverse of a multiplied by b
        mint e = a << 3;        // a * 2^3 mod MOD
        std::cout << e << '\n'; // output e's value

    Note:
    - This implementation is tailored for use in competitive programming, number theory, or cryptographic contexts.
*/

class mint {
private:
    int64_t val;
    static constexpr int64_t MOD = 998244353;

    static inline void undefined() {
        throw std::logic_error("Undefined behavior for mint class");
    }

    static inline mint fast_pow(mint x, int64_t y) {
        if (y == 0) return mint(1);
        mint a = fast_pow(x, y / 2);
        if (y & 1) return a * a * x;
        return a * a;
    }

    void normalize() {
        ((val %= MOD) += MOD) %= MOD;
    }

public:
    mint() : val(0ll) { }
    mint(int64_t val) : val(val) {
        normalize();
    }
    mint(const mint& other) = default;
    ~mint() = default;
    mint& operator=(const mint& other) = default;
    operator int64_t() const { return val; }
    mint& operator+=(const mint& other) {
        val += other.val;
        if (val >= MOD) val -= MOD;
        return *this;
    }
    mint& operator-=(const mint& other) {
        val -= other.val;
        if (val < 0) val += MOD;
        return *this;
    }
    mint& operator*=(const mint& other) {
        val = (val * other.val) % MOD;
        return *this;
    }
    mint& operator/=(const mint& other) {
        *this *= fast_pow(other, MOD - 2);
        return *this;
    }
    friend mint operator+(mint lhs, const mint& rhs) {
        lhs += rhs;
        return lhs;
    }
    friend mint operator-(mint lhs, const mint& rhs) {
        lhs -= rhs;
        return lhs;
    }
    friend mint operator*(mint lhs, const mint& rhs) {
        lhs *= rhs;
        return lhs;
    }
    friend mint operator/(mint lhs, const mint& rhs) {
        lhs /= rhs;
        return lhs;
    }
    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    mint& operator+=(T other) {
        return *this += mint(static_cast<int64_t>(other));
    }
    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    mint& operator-=(T other) {
        return *this -= mint(static_cast<int64_t>(other));
    }
    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    mint& operator*=(T other) {
        return *this *= mint(static_cast<int64_t>(other));
    }
    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    mint& operator/=(T other) {
        return *this /= mint(static_cast<int64_t>(other));
    }

    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    friend mint operator+(mint lhs, T rhs) {
        lhs += rhs;
        return lhs;
    }
    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    friend mint operator-(mint lhs, T rhs) {
        lhs -= rhs;
        return lhs;
    }
    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    friend mint operator*(mint lhs, T rhs) {
        lhs *= rhs;
        return lhs;
    }
    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    friend mint operator/(mint lhs, T rhs) {
        lhs /= rhs;
        return lhs;
    }
    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    friend mint operator+(T lhs, mint rhs) {
        rhs += lhs;
        return rhs;
    }
    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    friend mint operator-(T lhs, mint rhs) {
        return mint(lhs) - rhs;
    }
    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    friend mint operator*(T lhs, mint rhs) {
        rhs *= lhs;
        return rhs;
    }
    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    friend mint operator/(T lhs, mint rhs) {
        return mint(lhs) / rhs;
    }
    friend mint operator<<(mint val, int64_t shift) {
        if (shift < 0) return val >> (-shift);
        return val * fast_pow(mint(2), shift);
    }
    friend mint operator>>(mint val, int64_t shift) {
        if (shift < 0) return val << (-shift);
        static const mint inv2 = fast_pow(mint(2), MOD - 2);
        return val * fast_pow(inv2, shift);
    }
    bool operator==(const mint& other) const {
        return val == other.val;
    }
    bool operator!=(const mint& other) const {
        return val != other.val;
    }
    bool operator>(const mint&) const {
        undefined(); return false;
    }
    bool operator>=(const mint&) const {
        undefined(); return false;
    }
    bool operator<(const mint&) const {
        undefined(); return false;
    }
    bool operator<=(const mint&) const {
        undefined(); return false;
    }
    friend std::ostream& operator<<(std::ostream& os, const mint& m) {
        return os << m.val;
    }
    friend std::istream& operator>>(std::istream& is, mint& m) {
        int64_t x;
        is >> x;
        m = mint(x);
        return is;
    }
    mint& operator++() { *this += 1; return *this; }
    mint operator++(int32_t) { mint tmp = *this; ++(*this); return tmp; }
    mint& operator--() { *this -= 1; return *this; }
    mint operator--(int32_t) { mint tmp = *this; --(*this); return tmp; }
    mint& operator%=(const mint&) { undefined(); return *this; }
    friend mint operator%(mint, const mint&) { undefined(); return 0; }
};

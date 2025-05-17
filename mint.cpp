#include <iostream>
#include <cstdint>
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

class mint{
private:
    int64_t val;
    static const int64_t MOD = 998244353;
    static inline void undefined() {
        throw std::logic_error("Undefined behavior for mint class");
    }
    static inline mint fast_pow(mint x, int64_t y) {
        if(y==0)return 1;
        mint a = fast_pow(x, y/2);
        if(y & 1)return a * a * x;
        return a * a;
    }
public:
    mint() : val(0ll) { }
    mint(int64_t val) : val(val) {
        if(this->val >= MOD or this->val < 0){
            ((this->val %= MOD) += MOD) %= MOD;
        }
    }
    mint(const mint& other) : val(other.val) { }
    ~mint () { }
    mint& operator=(const mint& other) {
        if (this == &other) return *this;
        this->val = other.val;
        return *this;
    }
    mint& operator=(const int64_t& other) {
        this->val = other;
        return *this;
    }
    bool operator>(const mint& other) const {
        undefined();
        return false;
    }
    bool operator>(const int64_t& other) const {
        undefined();
        return false;
    }
    bool operator>(const int32_t& other) const {
        undefined();
        return false;
    }
    bool operator>=(const mint& other) const {
        undefined();
        return false;
    }
    bool operator>=(const int64_t& other) const {
        undefined();
        return false;
    }
    bool operator>=(const int32_t& other) const {
        undefined();
        return false;
    }
    bool operator<(const mint& other) const {
        undefined();
        return false;
    }
    bool operator<(const int64_t& other) const {
        undefined();
        return false;
    }
    bool operator<(const int32_t& other) const {
        undefined();
        return false;
    }
    bool operator<=(const mint& other) const {
        undefined();
        return false;
    }
    bool operator<=(const int64_t& other) const {
        undefined();
        return false;
    }
    bool operator<=(const int32_t& other) const {
        undefined();
        return false;
    }
    bool operator==(const mint& other) const {
        return val == other.val;
    }
    bool operator==(const int64_t& other) const {
        return *this == mint(other);
    }
    bool operator==(const int32_t& other) const {
        return *this == mint(other);
    }
    bool operator!=(const mint& other) const {
        return val != other.val;
    }
    bool operator!=(const int64_t& other) const {
        return *this != mint(other);
    }
    bool operator!=(const int32_t& other) const {
        return *this != mint(other);
    }
    mint& operator+=(const mint& other) {
        this->val += other.val;
        if(this->val >= MOD) this->val -= MOD;
        return *this;
    }
    mint& operator+=(const int64_t& other){
        *this += mint(other);
        return *this;
    }
    mint& operator+=(const int32_t& other){
        *this += mint(other);
        return *this;
    }
    friend mint operator+(mint lhs, const mint& rhs) {
        lhs += rhs;
        return lhs;
    }
    friend mint operator+(mint lhs, int64_t rhs) {
        lhs += rhs;
        return lhs;
    }
    friend mint operator+(mint lhs, int32_t rhs) {
        lhs += rhs;
        return lhs;
    }
    mint& operator-=(const mint& other) {
        this->val -= other.val;
        if(this->val < 0) this->val += MOD;
        return *this;
    }
    mint& operator-=(const int64_t& other){
        *this -= mint(other);
        return *this;
    }
    mint& operator-=(const int32_t& other){
        *this -= mint(other);
        return *this;
    }
    friend mint operator-(mint lhs, const mint& rhs) {
        lhs -= rhs;
        return lhs;
    }
    friend mint operator-(mint lhs, int64_t rhs) {
        lhs -= rhs;
        return lhs;
    }
    friend mint operator-(mint lhs, int32_t rhs) {
        lhs -= rhs;
        return lhs;
    }
    operator int64_t() const{
        return val;
    }
    operator int32_t() const{
        return val;
    }
    friend mint operator<<(const mint& val, int64_t shift) {
        if (shift < 0) return val >> (-shift);
        if (shift == 0) return val;
        return (val << (shift - 1ll)) * 2ll;
    }
    friend mint operator<<(const mint& val, int32_t shift) {
        if (shift < 0) return val >> (-shift);
        if (shift == 0) return val;
        return (val << (shift - 1)) * 2;
    }
    friend mint operator>>(const mint& val, int64_t shift) {
        if (shift < 0) return val << (-shift);
        if (shift == 0) return val;
        return (val >> (shift - 1ll)) / 2ll;
    }
    friend mint operator>>(const mint& val, int32_t shift) {
        if (shift < 0) return val << (-shift);
        if (shift == 0) return val;
        return (val >> (shift - 1)) / 2;
    }
    mint& operator*=(const mint& other) {
        int64_t result = this->val * other.val;
        *this = result;
        return *this;
    }
    mint& operator*=(const int64_t& other) {
        int64_t result = this->val * other;
        *this = result;
        return *this;
    }
    mint& operator*=(const int32_t& other) {
        int64_t result = this->val * int64_t(other);
        *this = result;
        return *this;
    }
    friend mint operator*(mint lhs, const mint& rhs) {
        lhs *= rhs;
        return lhs;
    }
    friend mint operator*(mint lhs, const int64_t& rhs) {
        lhs *= rhs;
        return lhs;
    }
    friend mint operator*(mint lhs, const int32_t& rhs) {
        lhs *= rhs;
        return lhs;
    }
    mint& operator/=(const mint& other) {
        *this *= fast_pow(other, MOD-2ll);
        return *this;
    }
    mint& operator/=(const int64_t& other) {
        *this /= mint(other);
        return *this;
    }
    mint& operator/=(const int32_t& other) {
        *this /= mint(other);
        return *this;
    }
    friend mint operator/(mint lhs, int64_t rhs) {
        return lhs /= rhs;
    }
    friend mint operator/(mint lhs, int32_t rhs) {
        return lhs /= rhs;
    }
    friend mint operator/(mint lhs, const mint& rhs){
        return lhs /= rhs;
    }
    mint& operator%=(const mint& other) {
        undefined();
        return *this;
    }
    mint& operator%=(const int64_t& other) {
        undefined();
        return *this;
    }
    mint& operator%=(const int32_t& other) {
        undefined();
        return *this;
    }
    friend mint operator%(mint lhs, int64_t rhs) {
        undefined();
        return 0;
    }
    friend mint operator%(mint lhs, int32_t rhs) {
        undefined();
        return 0;
    }
    friend mint operator%(mint lhs, const mint& rhs){
        undefined();
        return 0;
    }
    mint& operator++() {
        *this += 1;
        return *this;
    }
    mint operator++(int32_t) {
        mint temp = *this;
        ++(*this);
        return temp;
    }
    mint& operator--() {
        *this -= 1;
        return *this;
    }
    mint operator--(int32_t) {
        mint temp = *this;
        --(*this);
        return temp;
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
};


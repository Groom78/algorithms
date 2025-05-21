#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <algorithm>
#include <cstdint>
#include <type_traits>

class BigInt {
private:
    std::vector<int64_t> num;
    bool _sign;
    static constexpr int64_t base = 1000000000000000000ll;
    static constexpr size_t KARATSUBA_THRESHOLD = 1024;

    void addAbsolute(const BigInt& other) {
        int64_t carry = 0;
        size_t n = std::max(num.size(), other.num.size());
        num.resize(n, 0);
        for (size_t i = 0; i < n; ++i) {
            __int128 sum = __int128(num[i]) + (i < other.num.size() ? other.num[i] : 0) + carry;
            num[i] = int64_t(sum % base);
            carry = int64_t(sum / base);
        }
        if (carry) num.push_back(carry);
    }

    void subAbsolute(const BigInt& other) {
        int64_t borrow = 0;
        for (size_t i = 0; i < num.size(); ++i) {
            __int128 diff = __int128(num[i]) - (i < other.num.size() ? other.num[i] : 0) - borrow;
            if (diff < 0) {
                diff += base;
                borrow = 1;
            } else {
                borrow = 0;
            }
            num[i] = int64_t(diff);
        }
    }

    void normalize() {
        while (num.size() > 1 && num.back() == 0) num.pop_back();
        if (num.size() == 1 && num[0] == 0) _sign = true;
    }

    bool valid_string(const std::string& s) const {
        if (s.empty()) return false;
        size_t start = (s[0] == '-') ? 1 : 0;
        if (start == 1 && s.size() == 1) return false;
        for (size_t i = start; i < s.size(); ++i) {
            if (!std::isdigit(s[i])) return false;
        }
        return true;
    }

    std::string to_string() const {
        std::ostringstream oss;
        if (!_sign) oss << '-';
        for (int32_t i = int32_t(num.size()) - 1; i >= 0; --i) {
            if (i == int32_t(num.size()) - 1)
                oss << num[i];
            else
                oss << std::setw(18) << std::setfill('0') << num[i];
        }
        return oss.str();
    }

    bool is_zero() const {
        return num.size() == 1 && num[0] == 0;
    }

    BigInt classic_multiply(const BigInt& a, const BigInt& b) const {
        BigInt result;
        result.num.assign(a.num.size() + b.num.size(), 0);
        for (size_t i = 0; i < a.num.size(); ++i) {
            __int128 carry = 0;
            for (size_t j = 0; j < b.num.size() || carry; ++j) {
                __int128 cur = result.num[i + j] + __int128(a.num[i]) * (j < b.num.size() ? b.num[j] : 0) + carry;
                result.num[i + j] = int64_t(cur % base);
                carry = cur / base;
            }
        }
        result._sign = (a._sign == b._sign);
        result.normalize();
        return result;
    }

    BigInt higher_half(const BigInt& x, size_t m) const {
        if (x.num.size() <= m) return BigInt(0);
        BigInt res;
        res.num.assign(x.num.begin() + m, x.num.end());
        res._sign = true;
        res.normalize();
        return res;
    }

    BigInt lower_half(const BigInt& x, size_t m) const {
        BigInt res;
        if (x.num.size() <= m)
            res.num = x.num;
        else
            res.num.assign(x.num.begin(), x.num.begin() + m);
        res._sign = true;
        res.normalize();
        return res;
    }

    BigInt shift_left(const BigInt& x, size_t m) const {
        if (x.is_zero()) return BigInt(0);
        BigInt res = x;
        res.num.insert(res.num.begin(), m, 0);
        return res;
    }

    BigInt karatsuba(const BigInt& x, const BigInt& y) const {
        size_t n = std::max(x.num.size(), y.num.size());
        if (n <= KARATSUBA_THRESHOLD) return classic_multiply(x, y);
        size_t m = n / 2;
        BigInt x1 = higher_half(x, m);
        BigInt x0 = lower_half(x, m);
        BigInt y1 = higher_half(y, m);
        BigInt y0 = lower_half(y, m);
        BigInt z0 = karatsuba(x0, y0);
        BigInt z2 = karatsuba(x1, y1);
        BigInt z1 = karatsuba(x0 + x1, y0 + y1) - z2 - z0;
        BigInt res = shift_left(z2, 2 * m) + shift_left(z1, m) + z0;
        res._sign = (x._sign == y._sign);
        res.normalize();
        return res;
    }

public:
    BigInt(): num{0}, _sign(true) {}
    BigInt(int64_t x): num(), _sign(true) {
        if (x < 0) { _sign = false; x = -x; }
        while (x) { num.push_back(x % base); x /= base; }
        if (num.empty()) num.push_back(0);
    }
    BigInt(const std::string& s) {
        if (!valid_string(s)) throw std::runtime_error("Invalid number string");
        size_t pos = (s[0] == '-') ? 1 : 0;
        _sign = (s[0] != '-');
        for (int32_t i = int32_t(s.size()) - 1; i >= int32_t(pos); i -= 18) {
            int32_t l = std::max(int32_t(pos), i - 17);
            int64_t part = 0;
            for (int32_t j = l; j <= i; ++j) part = part * 10 + (s[j] - '0');
            num.push_back(part);
        }
        normalize();
    }
    template <size_t N>
    BigInt(const char (&cstr)[N]): BigInt(std::string(cstr)) {}
    BigInt(const BigInt& o) = default;
    BigInt& operator=(const BigInt& o) = default;
    BigInt& operator=(int64_t x) { return *this = BigInt(x); }
    BigInt& operator=(const std::string& s) { return *this = BigInt(s); }
    template <size_t N>
    BigInt& operator=(const char (&cstr)[N]) { return *this = std::string(cstr); }

    BigInt& operator+=(const BigInt& o) {
        bool resultSign;
        if (_sign == o._sign) {
            addAbsolute(o);
            resultSign = _sign;
        } else {
            if (abs() >= o.abs()) {
                subAbsolute(o);
                resultSign = _sign;
            } else {
                BigInt tmp = o;
                tmp.subAbsolute(*this);
                *this = std::move(tmp);
                resultSign = o._sign;
            }
        }
        _sign = resultSign;
        normalize();
        return *this;
    }
    BigInt& operator-=(const BigInt& o) {
        bool resultSign;
        if (_sign != o._sign) {
            addAbsolute(o);
            resultSign = _sign;
        } else {
            if (abs() >= o.abs()) {
                subAbsolute(o);
                resultSign = _sign;
            } else {
                BigInt tmp = o;
                tmp.subAbsolute(*this);
                *this = std::move(tmp);
                resultSign = !_sign;
            }
        }
        _sign = resultSign;
        normalize();
        return *this;
    }
    BigInt& operator*=(const BigInt& o) { *this = karatsuba(*this, o); normalize(); return *this; }
    BigInt& operator/=(const BigInt& o) {
        if (o.is_zero()) throw std::runtime_error("Division by zero");
        bool signRes = (_sign == o._sign);
        BigInt a = abs(), b = o.abs();
        if (a < b) { *this = BigInt(0); _sign = true; return *this; }
        BigInt q(0), r(0);
        for (int32_t i = int32_t(a.num.size()) - 1; i >= 0; --i) {
            r.num.insert(r.num.begin(), a.num[i]); r.normalize();
            int64_t low = 0, high = base - 1;
            while (low <= high) {
                int64_t mid = (low + high) >> 1;
                BigInt t = b * mid;
                if (t <= r) low = mid + 1;
                else high = mid - 1;
            }
            q.num.insert(q.num.begin(), high);
            r -= b * high;
        }
        q._sign = signRes; q.normalize(); *this = q;
        return *this;
    }
    BigInt& operator%=(const BigInt& o) {
        if (o.is_zero()) throw std::runtime_error("Modulo by zero");
        BigInt d = *this / o; *this -= d * o; normalize(); return *this;
    }

    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    BigInt& operator+=(T x) { return *this += BigInt(int64_t(x)); }
    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    BigInt& operator-=(T x) { return *this -= BigInt(int64_t(x)); }
    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    BigInt& operator*=(T x) { return *this *= BigInt(int64_t(x)); }
    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    BigInt& operator/=(T x) { return *this /= BigInt(int64_t(x)); }
    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    BigInt& operator%=(T x) { return *this %= BigInt(int64_t(x)); }

    friend BigInt operator+(BigInt a, const BigInt& b) { return a += b; }
    friend BigInt operator-(BigInt a, const BigInt& b) { return a -= b; }
    friend BigInt operator*(BigInt a, const BigInt& b) { return a *= b; }
    friend BigInt operator/(BigInt a, const BigInt& b) { return a /= b; }
    friend BigInt operator%(BigInt a, const BigInt& b) { return a %= b; }

    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    friend BigInt operator+(BigInt a, T b) { return a += b; }
    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    friend BigInt operator-(BigInt a, T b) { return a -= b; }
    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    friend BigInt operator*(BigInt a, T b) { return a *= b; }
    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    friend BigInt operator/(BigInt a, T b) { return a /= b; }
    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    friend BigInt operator%(BigInt a, T b) { return a %= b; }

    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    friend BigInt operator+(T a, BigInt b) { return b += a; }
    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    friend BigInt operator-(T a, BigInt b) { return BigInt(int64_t(a)) -= b; }
    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    friend BigInt operator*(T a, BigInt b) { return b *= a; }
    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    friend BigInt operator/(T a, BigInt b) { return BigInt(int64_t(a)) /= b; }
    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    friend BigInt operator%(T a, BigInt b) { return BigInt(int64_t(a)) %= b; }

    BigInt& operator&=(const BigInt& o) {
        size_t n = std::min(num.size(), o.num.size());
        for (size_t i = 0; i < n; ++i) num[i] &= o.num[i];
        for (size_t i = n; i < num.size(); ++i) num[i] = 0;
        normalize(); return *this;
    }
    BigInt& operator|=(const BigInt& o) {
        size_t n = std::max(num.size(), o.num.size());
        num.resize(n, 0);
        for (size_t i = 0; i < o.num.size(); ++i) num[i] |= o.num[i];
        normalize(); return *this;
    }
    BigInt& operator^=(const BigInt& o) {
        size_t n = std::max(num.size(), o.num.size());
        num.resize(n, 0);
        for (size_t i = 0; i < o.num.size(); ++i) num[i] ^= o.num[i];
        normalize(); return *this;
    }
    friend BigInt operator&(BigInt a, const BigInt& b) { return a &= b; }
    friend BigInt operator|(BigInt a, const BigInt& b) { return a |= b; }
    friend BigInt operator^(BigInt a, const BigInt& b) { return a ^= b; }

    friend BigInt operator<<(BigInt v, int32_t s) {
        if (s < 0) return v >> -s;
        while (s--) v *= 2;
        return v;
    }
    friend BigInt operator>>(BigInt v, int32_t s) {
        if (s < 0) return v << -s;
        while (s--) v /= 2;
        return v;
    }

    bool operator<(const BigInt& o) const {
        if (_sign != o._sign) return !_sign;
        if (!_sign) return abs() > o.abs();
        if (num.size() != o.num.size()) return num.size() < o.num.size();
        for (int32_t i = int32_t(num.size()) - 1; i >= 0; --i) {
            if (num[i] != o.num[i]) return num[i] < o.num[i];
        }
        return false;
    }
    bool operator>(const BigInt& o) const { return o < *this; }
    bool operator<=(const BigInt& o) const { return !(*this > o); }
    bool operator>=(const BigInt& o) const { return !(*this < o); }
    bool operator==(const BigInt& o) const { return _sign == o._sign && num == o.num; }
    bool operator!=(const BigInt& o) const { return !(*this == o); }

    friend std::ostream& operator<<(std::ostream& os, const BigInt& v) { return os << v.to_string(); }
    friend std::istream& operator>>(std::istream& is, BigInt& v) { std::string s; is >> s; v = BigInt(s); return is; }

    BigInt abs() const { BigInt r = *this; r._sign = true; return r; }
    operator std::string() const { return to_string(); }
    explicit operator int64_t() const { return std::stoll(to_string()); }

    BigInt& operator+=(const std::string& s) { return *this += BigInt(s); }
    BigInt& operator-=(const std::string& s) { return *this -= BigInt(s); }
    BigInt& operator*=(const std::string& s) { return *this *= BigInt(s); }
    BigInt& operator/=(const std::string& s) { return *this /= BigInt(s); }
    BigInt& operator%=(const std::string& s) { return *this %= BigInt(s); }
    friend BigInt operator+(BigInt a, const std::string& s) { return a += s; }
    friend BigInt operator-(BigInt a, const std::string& s) { return a -= s; }
    friend BigInt operator*(BigInt a, const std::string& s) { return a *= s; }
    friend BigInt operator/(BigInt a, const std::string& s) { return a /= s; }
    friend BigInt operator%(BigInt a, const std::string& s) { return a %= s; }
    template <size_t N>
    BigInt& operator+=(const char (&s)[N]) { return *this += std::string(s); }
    template <size_t N>
    BigInt& operator-=(const char (&s)[N]) { return *this -= std::string(s); }
    template <size_t N>
    BigInt& operator*=(const char (&s)[N]) { return *this *= std::string(s); }
    template <size_t N>
    BigInt& operator/=(const char (&s)[N]) { return *this /= std::string(s); }
    template <size_t N>
    BigInt& operator%=(const char (&s)[N]) { return *this %= std::string(s); }
    template <size_t N>
    friend BigInt operator+(BigInt a, const char (&s)[N]) { return a += s; }
    template <size_t N>
    friend BigInt operator-(BigInt a, const char (&s)[N]) { return a -= s; }
    template <size_t N>
    friend BigInt operator*(BigInt a, const char (&s)[N]) { return a *= s; }
    template <size_t N>
    friend BigInt operator/(BigInt a, const char (&s)[N]) { return a /= s; }
    template <size_t N>
    friend BigInt operator%(BigInt a, const char (&s)[N]) { return a %= s; }

    BigInt operator-() const {
        BigInt r = *this;
        if (!r.is_zero()) r._sign = !r._sign;
        return r;
    }

    BigInt& operator++() { return *this += 1; }
    BigInt operator++(int32_t) { BigInt tmp = *this; ++*this; return tmp; }
    BigInt& operator--() { return *this -= 1; }
    BigInt operator--(int32_t) { BigInt tmp = *this; --*this; return tmp; }

    template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    BigInt& operator&=(T x) { return *this &= BigInt(int64_t(x)); }
    template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    BigInt& operator|=(T x) { return *this |= BigInt(int64_t(x)); }
    template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    BigInt& operator^=(T x) { return *this ^= BigInt(int64_t(x)); }
    template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    friend BigInt operator&(BigInt a, T b) { return a &= b; }
    template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    friend BigInt operator|(BigInt a, T b) { return a |= b; }
    template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    friend BigInt operator^(BigInt a, T b) { return a ^= b; }

    friend BigInt operator<<(BigInt v, int64_t s) {
        if (s < 0) return v >> -s;
        while (s--) v *= 2;
        return v;
    }
    friend BigInt operator>>(BigInt v, int64_t s) {
        if (s < 0) return v << -s;
        while (s--) v /= 2;
        return v;
    }
};

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <algorithm>
#include <cstdint>

class BigInt{
private:
    std :: vector<int64_t> num;
    bool _sign;
    const int64_t base = 1000000000000000000ll;
    const int64_t KARATSUBA_THRESHOLD = 1024;
    void addAbsolute(const BigInt& other) {
        int64_t carry = 0, sum;
        int n = std::max(num.size(), other.num.size());
        while(num.size() < n) num.push_back(0);
        for (int i = 0; i < n; ++i) {
            sum = num[i] + (i < other.num.size() ? other.num[i] : 0) + carry;
            num[i] = sum % base;
            carry = sum / base;
        }
        if (carry) num.push_back(carry);
    }
    void subAbsolute(const BigInt& o) {
        int64_t borrow = 0, diff;
        for (size_t i = 0; i < num.size(); ++i) {
            diff = num[i] - (i < o.num.size() ? o.num[i] : 0) - borrow;
            if (diff < 0) {
                diff += base;
                borrow = 1;
            } else {
                borrow = 0;
            }
            num[i] = diff;
        }
    }
    inline void normalize() {
      while (num.size() > 1 && num.back() == 0)
        num.pop_back();
      if (num.size() == 1 && num[0] == 0)
        _sign = true;
    }
    bool valid_string(const std::string& s) const {
        if(s.length() == 0 or (s[0] != '-' and (s[0] > '9' or s[0] < '0'))) return false;
        for(int i=1;i<s.length();i++){
            if(s[i] > '9' or s[i] < '0') return false;
        }
        return true;
    }
    std::string to_string() const {
        std::ostringstream oss;
        if (!_sign) oss << "-";
        for (int i = num.size() - 1; i >= 0; --i) {
            if (i == num.size() - 1)
                oss << num[i];
            else
                oss << std::setw(18) << std::setfill('0') << num[i];
        }
        return oss.str();
    }
    inline bool is_zero() const {
        return num.size() == 1 && num[0] == 0;
    }
    inline BigInt classic_multiply(const BigInt& a, const BigInt& b) {
        BigInt result;
        result.num.assign(a.num.size() + b.num.size(), 0);
        for (size_t i = 0; i < a.num.size(); ++i) {
            __int128 carry = 0;
            for (size_t j = 0; j < b.num.size() || carry != 0; ++j) {
                __int128 cur = result.num[i + j] +
                    (__int128)a.num[i] * (j < b.num.size() ? b.num[j] : 0) + carry;
                result.num[i + j] = (int64_t)(cur % a.base);
                carry = cur / a.base;
            }
        }
        result._sign = (a._sign == b._sign);
        result.normalize();
        return result;
    }
    inline BigInt higher_half(const BigInt& x, size_t m) {
        BigInt res;
        if (x.num.size() <= m) {
            res.num = {0};
            res._sign = true;
            return res;
        }
        res.num.assign(x.num.begin() + m, x.num.end());
        res._sign = true;
        res.normalize();
        return res;
    }
    inline BigInt lower_half(const BigInt& x, size_t m) {
        BigInt res;
        if (x.num.size() <= m) {
            res.num = x.num;
            res._sign = true;
            return res;
        }
        res.num.assign(x.num.begin(), x.num.begin() + m);
        res._sign = true;
        res.normalize();
        return res;
    }
    inline BigInt shift_left(const BigInt& x, size_t m) {
        if (x.is_zero()) return BigInt(0);
        BigInt res = x;
        res.num.insert(res.num.begin(), m, 0);
        return res;
    }
    BigInt karatsuba(const BigInt& x, const BigInt& y) {
        int n = std::max(x.num.size(), y.num.size());
        if (n <= KARATSUBA_THRESHOLD) { 
            return classic_multiply(x, y);
        }
        int m = n / 2;

        BigInt x1 = higher_half(x, m);
        BigInt x0 = lower_half(x, m);
        BigInt y1 = higher_half(y, m);
        BigInt y0 = lower_half(y, m);

        BigInt z0 = karatsuba(x0, y0);
        BigInt z2 = karatsuba(x1, y1);
        BigInt z1 = karatsuba(x0 + x1, y0 + y1) - z2 - z0;

        BigInt result = shift_left(z2, 2 * m) + shift_left(z1, m) + z0;
        result._sign = (x._sign == y._sign);
        result.normalize();
        return result;
    }
public:
    BigInt(){
        _sign = true;
        num = {0};
    }
    BigInt(int64_t x){
        if(x<0){
            this->_sign = false;
            x = -x;
        }
        else{
            this->_sign = true;
        }
        if(x == 0){
            num.push_back(0);
            return;
        }
        while(x > 0){
            num.push_back(x % base);
            x /= base;
        }
    }
    BigInt(const std::string& s){
        if(!valid_string(s)){
            throw std::runtime_error("Not a number!");
        }
        if(s.length() == 0){
            _sign = true;
            num = {0};
            return;
        }
        int end;
        if(s[0] == '-'){
            _sign = false;
            end = 1;
        }
        else{
            _sign = true;
            end = 0;
        }
        int rightidx = s.length()-1, leftidx;
        while(rightidx>=end){
            leftidx = std::max(end, rightidx - 17);
            int64_t x = 0;
            for(int j=leftidx;j<=rightidx;j++){
                x *= 10;
                x += s[j] - '0';
            }
            num.push_back(x);
            rightidx = leftidx-1;
        }
    }
    template <size_t N>
    BigInt(const char (&s)[N]) : BigInt(std::string(s)) {}
    BigInt(const BigInt& other){
        _sign = other._sign;
        num = other.num;
    }
    ~BigInt () { }
    BigInt& operator=(const int64_t& other){
        (*this) = BigInt(int64_t(other));
        return *this;
    }
    BigInt& operator=(const std::string& other){
        (*this) = BigInt(other);
        return *this;
    }
    template <size_t N>
    BigInt& operator=(const char (&s)[N]) {
        return *this = std::string(s);
    }
    BigInt& operator=(const BigInt& other) {
        if (this == &other) return *this;
        this->_sign = other._sign;
        num = other.num;
        return *this;
    }
    bool operator>(const BigInt& other) const{
        if(other._sign and !_sign) return false;
        if(!other._sign and _sign) return true;
        if(!_sign) return other.abs() > this->abs();
        if(num.size() > other.num.size()) return true;
        if(num.size() < other.num.size()) return false;
        for(int i=num.size()-1;i>=0;i--){
            if(num[i] > other.num[i]) return true;
            if(num[i] < other.num[i]) return false;
        }
        return false; //equal
    }
    bool operator>(const int64_t& other) const{
        return (*this) > BigInt(other);
    }
    bool operator>(const std::string& other) const{
        return (*this) > BigInt(other);
    }
    template <size_t N>
    bool operator>(const char (&s)[N]) {
        return *this > std::string(s);
    }
    bool operator>=(const BigInt& other) const{
        if(other._sign and !_sign) return false;
        if(!other._sign and _sign) return true;
        if(!_sign) return other.abs() >= this->abs();
        if(num.size() > other.num.size()) return true;
        if(num.size() < other.num.size()) return false;
        for(int i=num.size()-1;i>=0;i--){
            if(num[i] > other.num[i]) return true;
            if(num[i] < other.num[i]) return false;
        }
        return true; //equal
    }
    bool operator>=(const int64_t& other) const{
        return (*this) >= BigInt(other);
    }
    bool operator>=(const std::string& other) const{
        return (*this) >= BigInt(other);
    }
    template <size_t N>
    bool operator>=(const char (&s)[N]) {
        return *this >= std::string(s);
    }
    bool operator<(const BigInt& other) const{
        if(other._sign and !_sign) return true;
        if(!other._sign and _sign) return false;
        if(!_sign) return other.abs() < this->abs();
        if(num.size() > other.num.size()) return false;
        if(num.size() < other.num.size()) return true;
        for(int i=num.size()-1;i>=0;i--){
            if(num[i] > other.num[i]) return false;
            if(num[i] < other.num[i]) return true;
        }
        return false; //equal
    }
    bool operator<(const int64_t& other) const{
        return (*this) < BigInt(other);
    }
    bool operator<(const std::string& other) const{
        return (*this) < BigInt(other);
    }
    template <size_t N>
    bool operator<(const char (&s)[N]) {
        return *this < std::string(s);
    }
    bool operator<=(const BigInt& other) const{
        if(other._sign and !_sign) return true;
        if(!other._sign and _sign) return false;
        if(!_sign) return other.abs() <= this->abs();
        if(num.size() > other.num.size()) return false;
        if(num.size() < other.num.size()) return true;
        for(int i=num.size()-1;i>=0;i--){
            if(num[i] > other.num[i]) return false;
            if(num[i] < other.num[i]) return true;
        }
        return true; //equal
    }
    bool operator<=(const int64_t& other) const{
        return (*this) <= BigInt(other);
    }
    bool operator<=(const std::string& other) const{
        return (*this) <= BigInt(other);
    }
    template <size_t N>
    bool operator<=(const char (&s)[N]) {
        return *this <= std::string(s);
    }
    bool operator==(const BigInt& other) const{
        if(other._sign and !_sign) return false;
        if(!other._sign and _sign) return false;
        if(!_sign) return other.abs() == this->abs();
        if(num.size() > other.num.size()) return false;
        if(num.size() < other.num.size()) return false;
        for(int i=num.size()-1;i>=0;i--){
            if(num[i] > other.num[i]) return false;
            if(num[i] < other.num[i]) return false;
        }
        return true; //equal
    }
    bool operator==(const int64_t& other) const{
        return (*this) == BigInt(other);
    }
    bool operator==(const std::string& other) const{
        return (*this) == BigInt(other);
    }
    template <size_t N>
    bool operator==(const char (&s)[N]) {
        return *this == std::string(s);
    }
    bool operator!=(const BigInt& other) const{
        return !((*this) == other);
    }
    bool operator!=(const int64_t& other) const{
        return (*this) != BigInt(other);
    }
    bool operator!=(const std::string& other) const{
        return (*this) != BigInt(other);
    }
    template <size_t N>
    bool operator!=(const char (&s)[N]) {
        return *this != std::string(s);
    }
    BigInt abs() const{
        BigInt ret(*this);
        ret.set_sign(true);
        return ret;
    }
    BigInt inv_abs() const{
        BigInt ret(*this);
        ret.set_sign(false);
        return ret;
    }
    BigInt& operator+=(const BigInt& other) {
        if (this->_sign == other._sign) {
            addAbsolute(other);
        } else {
            if (this->abs() >= other.abs()) {
                subAbsolute(other);
            } else {
                BigInt tmp = other;
                tmp.subAbsolute(*this);
                *this = std::move(tmp);
            }
        }
        normalize();
        return *this;
    }
    BigInt& operator+=(int64_t other) {
        return *this += BigInt(other);
    }
    BigInt& operator+=(const std::string& other) {
        return *this += BigInt(other);
    }
    template <size_t N>
    BigInt& operator+=(const char (&s)[N]) {
        return *this += std::string(s);
    }
    friend BigInt operator+(BigInt lhs, const BigInt& rhs) {
        lhs += rhs;
        return lhs;
    }
    friend BigInt operator+(BigInt lhs, int64_t rhs) {
        lhs += rhs;
        return lhs;
    }
    friend BigInt operator+(BigInt lhs, const std::string& rhs) {
        lhs += rhs;
        return lhs;
    }
    template <size_t N>
    friend BigInt operator+(BigInt lhs, const char (&rhs)[N]) {
        lhs += std::string(rhs);
        return lhs;
    }
    BigInt& operator-=(const BigInt& other) {
        if (this->_sign != other._sign) {
            addAbsolute(other);
        } else {
            if (this->abs() >= other.abs()) {
                subAbsolute(other);
            } else {
                BigInt tmp = other;
                tmp.subAbsolute(*this);
                tmp._sign = !tmp._sign;
                *this = std::move(tmp);
            }
        }
        normalize();
        return *this;
    }
    BigInt& operator-=(int64_t other) {
        return *this -= BigInt(other);
    }
    BigInt& operator-=(const std::string& other) {
        return *this -= BigInt(other);
    }
    template <size_t N>
    BigInt& operator-=(const char (&s)[N]) {
        return *this -= std::string(s);
    }
    friend BigInt operator-(BigInt lhs, const BigInt& rhs) {
        lhs -= rhs;
        return lhs;
    }
    friend BigInt operator-(BigInt lhs, int64_t rhs) {
        lhs -= rhs;
        return lhs;
    }
    friend BigInt operator-(BigInt lhs, const std::string& rhs) {
        lhs -= rhs;
        return lhs;
    }
    template <size_t N>
    friend BigInt operator-(BigInt lhs, const char (&rhs)[N]) {
        lhs -= std::string(rhs);
        return lhs;
    }
    friend BigInt operator<<(const BigInt& val, int64_t shift) {
        if (shift < 0) return val >> (-shift);
        if (val == BigInt(0ll)) return BigInt(0ll);

        BigInt result = val;

        while (shift--) {
            int64_t carry = 0;
            for (size_t i = 0; i < result.num.size(); ++i) {
                __int128 temp = (__int128)result.num[i] * 2 + carry;
                result.num[i] = temp % result.base;
                carry = temp / result.base;
            }
            if (carry > 0)
                result.num.push_back(carry);
        }

        result.normalize();
        return result;
    }
    friend BigInt operator>>(const BigInt& val, int64_t shift) {
        if (shift < 0) return val << (-shift);
        if (val == BigInt(0ll)) return BigInt(0ll);

        BigInt result = val;

        while (shift--) {
            int64_t carry = 0;
            for (int i = result.num.size() - 1; i >= 0; --i) {
                __int128 current = (__int128)carry * result.base + result.num[i];
                result.num[i] = current / 2;
                carry = current % 2;
            }
        }

        result.normalize();
        return result;
    }
    BigInt& operator*=(const BigInt& other) {
        *this = karatsuba(*this, other);
        normalize();
        return *this;
    }
    BigInt& operator*=(int64_t other) {
        return *this *= BigInt(other);
    }
    BigInt& operator*=(const std::string& other) {
        return *this *= BigInt(other);
    }
    template <size_t N>
    BigInt& operator*=(const char (&s)[N]) {
        return *this *= std::string(s);
    }
    friend BigInt operator*(BigInt lhs, const BigInt& rhs) {
        lhs *= rhs;
        return lhs;
    }
    friend BigInt operator*(BigInt lhs, int64_t rhs) {
        lhs *= rhs;
        return lhs;
    }
    friend BigInt operator*(BigInt lhs, const std::string& rhs) {
        lhs *= rhs;
        return lhs;
    }
    template <size_t N>
    friend BigInt operator*(BigInt lhs, const char (&rhs)[N]) {
        lhs *= std::string(rhs);
        return lhs;
    }
    BigInt& operator/=(const BigInt& other) {
        if (other.is_zero()) {
            throw std::runtime_error("Division by zero");
        }
        bool result_sign = (_sign == other._sign);
        BigInt dividend = this->abs();
        BigInt divisor = other.abs();
        if (dividend < divisor) {
            *this = BigInt(0);
            _sign = true;
            return *this;
        }
        BigInt quotient(0);
        BigInt current(0);
        for (int i = (int)dividend.num.size() - 1; i >= 0; --i) {
            current.num.insert(current.num.begin(), dividend.num[i]);
            current.normalize();
            BigInt l(0), r(dividend.base), m, t;
            while (l <= r) {
                m = (l + r) >> 1;
                t = divisor * m;
                if (t <= current) {
                    l = m + 1;
                } else {
                    r = m - 1;
                }
            }
            quotient.num.insert(quotient.num.begin(), r.num.empty() ? 0 : r.num[0]);
            current -= divisor * r;
        }
        quotient._sign = result_sign;
        quotient.normalize();
        *this = quotient;
        return *this;
    }
    BigInt& operator/=(int64_t other) {
        return *this /= BigInt(other);
    }
    BigInt& operator/=(const std::string& other) {
        return *this /= BigInt(other);
    }
    template <size_t N>
    BigInt& operator/=(const char (&s)[N]) {
        return *this /= std::string(s);
    }
    friend BigInt operator/(BigInt lhs, const BigInt& rhs) {
        lhs /= rhs;
        return lhs;
    }
    friend BigInt operator/(BigInt lhs, int64_t rhs) {
        lhs /= rhs;
        return lhs;
    }
    friend BigInt operator/(BigInt lhs, const std::string& rhs) {
        lhs /= rhs;
        return lhs;
    }
    template <size_t N>
    friend BigInt operator/(BigInt lhs, const char (&rhs)[N]) {
        lhs /= std::string(rhs);
        return lhs;
    }
    BigInt& operator%=(const BigInt& other) {
        if (other.is_zero()) {
            throw std::runtime_error("Modulo by zero");
        }
        BigInt div = *this / other;
        BigInt mult = div * other;
        *this -= mult;
        normalize();
        return *this;
    }
    BigInt& operator%=(int64_t other) {
        return *this %= BigInt(other);
    }
    BigInt& operator%=(const std::string& other) {
        return *this %= BigInt(other);
    }
    template <size_t N>
    BigInt& operator%=(const char (&s)[N]) {
        return *this %= std::string(s);
    }
    friend BigInt operator%(BigInt lhs, const BigInt& rhs) {
        lhs %= rhs;
        return lhs;
    }
    friend BigInt operator%(BigInt lhs, int64_t rhs) {
        lhs %= rhs;
        return lhs;
    }
    friend BigInt operator%(BigInt lhs, const std::string& rhs) {
        lhs %= rhs;
        return lhs;
    }
    template <size_t N>
    friend BigInt operator%(BigInt lhs, const char (&rhs)[N]) {
        lhs %= std::string(rhs);
        return lhs;
    }
    BigInt& operator++() {
        *this += 1;
        return *this;
    }
    BigInt operator++(int32_t) {
        BigInt temp = *this;
        ++(*this);
        return temp;
    }
    BigInt& operator--() {
        *this -= 1;
        return *this;
    }
    BigInt operator--(int32_t) {
        BigInt temp = *this;
        --(*this);
        return temp;
    }
    bool operator!() const {
        return this->is_zero();
    }
    bool operator&&(const BigInt& other) const {
        return !this->is_zero() && !other.is_zero();
    }
    bool operator&&(int64_t other) const {
        return !this->is_zero() && (other != 0);
    }
    bool operator&&(const std::string& other) const {
        return !this->is_zero() && !(BigInt(other).is_zero());
    }
    template <size_t N>
    bool operator&&(const char (&other)[N]) const {
        return !this->is_zero() && !(BigInt(std::string(other)).is_zero());
    }
    bool operator||(const BigInt& other) const {
        return !this->is_zero() || !other.is_zero();
    }
    bool operator||(int64_t other) const {
        return !this->is_zero() || (other != 0);
    }
    bool operator||(const std::string& other) const {
        return !this->is_zero() || !(BigInt(other).is_zero());
    }
    template <size_t N>
    bool operator||(const char (&other)[N]) const {
        return !this->is_zero() || !(BigInt(std::string(other)).is_zero());
    }
    BigInt& operator&=(const BigInt& other) {
        size_t n = std::min(num.size(), other.num.size());
        for (size_t i = 0; i < n; ++i) {
            num[i] &= other.num[i];
        }
        for (size_t i = n; i < num.size(); ++i) {
            num[i] = 0;
        }
        normalize();
        return *this;
    }
    BigInt& operator&=(int64_t other) {
        return *this &= BigInt(other);
    }
    BigInt& operator&=(const std::string& other) {
        return *this &= BigInt(other);
    }
    template <size_t N>
    BigInt& operator&=(const char (&s)[N]) {
        return *this &= std::string(s);
    }
    friend BigInt operator&(BigInt lhs, const BigInt& rhs) {
        lhs &= rhs;
        return lhs;
    }
    friend BigInt operator&(BigInt lhs, int64_t rhs) {
        lhs &= rhs;
        return lhs;
    }
    friend BigInt operator&(BigInt lhs, const std::string& rhs) {
        lhs &= rhs;
        return lhs;
    }
    template <size_t N>
    friend BigInt operator&(BigInt lhs, const char (&rhs)[N]) {
        lhs &= std::string(rhs);
        return lhs;
    }
    BigInt& operator|=(const BigInt& other) {
        size_t n = std::max(num.size(), other.num.size());
        num.resize(n, 0);
        for (size_t i = 0; i < other.num.size(); ++i) {
            num[i] |= other.num[i];
        }
        normalize();
        return *this;
    }
    BigInt& operator|=(int64_t other) {
        return *this |= BigInt(other);
    }
    BigInt& operator|=(const std::string& other) {
        return *this |= BigInt(other);
    }
    template <size_t N>
    BigInt& operator|=(const char (&s)[N]) {
        return *this |= std::string(s);
    }
    friend BigInt operator|(BigInt lhs, const BigInt& rhs) {
        lhs |= rhs;
        return lhs;
    }
    friend BigInt operator|(BigInt lhs, int64_t rhs) {
        lhs |= rhs;
        return lhs;
    }
    friend BigInt operator|(BigInt lhs, const std::string& rhs) {
        lhs |= rhs;
        return lhs;
    }
    template <size_t N>
    friend BigInt operator|(BigInt lhs, const char (&rhs)[N]) {
        lhs |= std::string(rhs);
        return lhs;
    }
    BigInt& operator^=(const BigInt& other) {
        size_t n = std::max(num.size(), other.num.size());
        num.resize(n, 0);
        for (size_t i = 0; i < other.num.size(); ++i) {
            num[i] ^= other.num[i];
        }
        normalize();
        return *this;
    }
    BigInt& operator^=(int64_t other) {
        return *this ^= BigInt(other);
    }
    BigInt& operator^=(const std::string& other) {
        return *this ^= BigInt(other);
    }
    template <size_t N>
    BigInt& operator^=(const char (&s)[N]) {
        return *this ^= std::string(s);
    }
    friend BigInt operator^(BigInt lhs, const BigInt& rhs) {
        lhs ^= rhs;
        return lhs;
    }
    friend BigInt operator^(BigInt lhs, int64_t rhs) {
        lhs ^= rhs;
        return lhs;
    }
    friend BigInt operator^(BigInt lhs, const std::string& rhs) {
        lhs ^= rhs;
        return lhs;
    }
    template <size_t N>
    friend BigInt operator^(BigInt lhs, const char (&rhs)[N]) {
        lhs ^= std::string(rhs);
        return lhs;
    }
    BigInt operator-() const {
        BigInt result(*this);
        if (!result.is_zero())
            result._sign = !result._sign;
        return result;
    }
    BigInt operator~() const {
        BigInt result(*this);
        for (size_t i = 0; i < result.num.size(); ++i) {
            result.num[i] = ~result.num[i];
        }
        result.normalize();
        return result;
    }
    void set_sign(const bool& s){
        _sign = s;
    }
    bool sign() const{
        return _sign;
    }
    friend std::ostream& operator<<(std::ostream& os, const BigInt& val) {
        if (!val._sign) os << "-";
        for (int i = val.num.size() - 1; i >= 0; --i) {
            if (i == val.num.size() - 1)
                os << val.num[i];
            else
                os << std::setw(18) << std::setfill('0') << val.num[i];
        }
        return os;
    }
    friend std::istream& operator>>(std::istream& is, BigInt& val) {
        std::string s;
        is >> s;
        val = BigInt(s);
        return is;
    }
    operator std::string() const {
        return this->to_string();
    }
    explicit operator int64_t() const {
        std::string s = this->to_string();
        try {
            size_t pos = 0;
            int64_t val = std::stoll(s, &pos);
            if (pos != s.size())
                throw std::overflow_error("Invalid BigInt format for int64_t conversion");
            return val;
        } catch (const std::out_of_range&) {
            throw std::overflow_error("BigInt out of int64_t range");
        } catch (const std::invalid_argument&) {
            throw std::overflow_error("Invalid BigInt format");
        }
    }
    int64_t size(){
        return std::string(*this).length();
    }
};

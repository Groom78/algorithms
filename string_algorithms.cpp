#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

/*
    @brief Computes the suffix array of the given string.

    Appends a sentinel character and returns the starting indices of 
    all suffixes in lexicographical order (excluding the sentinel).

    @param s  Input string.

    @return   A vector containing the lexicographically sorted suffix indices.

    Time complexity is O(N*log(N)^2), where N is the length of string
    But, one of these "log"s is exact log, while other one comes from std::sort()
    Faster sorting algorithms can be used such as radix sort, for sorting a pair vector
    Of course, this version is fast enough almost always
    Space complexity is O(N*log(N)), where N is the length of string
*/
class StringAlgorithms{
public:
    class SuffixArray{
    private:
        std::vector<std::vector<int> >suff;
        std::vector<std::vector<int> >ranks;
    public:
        std::vector<int> calculate(std::string s){
            s.push_back('$');
            int n = s.length();
            int logn = ceil(log2(n));
            int gap, ord;
            suff.resize(logn+1);
            std::vector<std::pair<char,int> >init(n);
            std::vector<int>rank(n);
            for(int i=0;i<n;i++){
                init[i] = {s[i], i};
            }
            std::sort(init.begin(), init.end());
            rank[init[0].second] = 0;
            ord = 0;
            for(int i=1;i<n;i++){
                if(init[i].first != init[i-1].first) ord++;
                rank[init[i].second] = ord;
            }
            suff[0].resize(n-1);
            for(int i=0;i<n-1;i++){
                suff[0][rank[i]-1] = i;
            }
            ranks.push_back(rank);
            for(int counter = 0; counter < logn; counter++){
                gap = 1ll << counter;
                std::vector<std::pair<std::pair<int,int>, int> >helper(n);
                for(int i=0;i<n;i++){
                    if(i+gap >= n){
                        helper[i] = {{rank[i], 0ll}, i};
                    }
                    else{
                        helper[i] = {{rank[i], rank[i+gap]}, i};
                    }
                }
                std::sort(helper.begin(), helper.end());
                rank[helper[0].second] = 0;
                ord = 0;
                for(int i=1;i<n;i++){
                    if(helper[i].first != helper[i-1].first) ord++;
                    rank[helper[i].second] = ord;
                }
                suff[counter+1].resize(n-1);
                for(int i=0;i<n-1;i++){
                    suff[counter+1][rank[i]-1] = i;
                }
                ranks.push_back(rank);
            }
            return suff.back();
        }
        /*
            @brief Computes the longest common prefix (LCP) lengths between
            adjacent suffixes in the suffix array.

            Must be called after calculate(). Returns an array where each element 
            represents the length of the common prefix between two consecutive 
            suffixes in the sorted order.

            @return A vector of LCP values.
            
            Time complexity is O(N*log(N)), where N is the length of the string
            Space complexity is O(N), where N is the length of the string
        */
        std::vector<int> get_difference(){
            if(ranks.size() == 0) return {};
            int n = suff[0].size();
            std::vector<int>result(n);
            int cur, next, ans;
            for(int i=0;i<n-1;i++){
                cur = suff.back()[i];
                next = suff.back()[i+1];
                ans = 0;
                for(int j=ranks.size()-1;j>=0;j--){
                    if(cur >= n or next >= n) break;
                    if(ranks[j][cur] == ranks[j][next]){
                        ans += 1ll << j;
                        cur += 1ll << j;
                        next += 1ll << j;
                    }
                }
                result[i] = ans;
            }
            return result;
        }
        
        /*
            @brief Computes the number of unique substrings (continuous subsequences) of a string
            
            @param s  Input string.
            
            @return Single integer, result
            
            Time and space complexity is O(N*log(N)), where N is the length of the string
            
            @fortesting
            https://atcoder.jp/contests/practice2/tasks/practice2_i
        */
        
        int count_unique_substrings(std::string s){
            calculate(s);
            std :: vector<int> v = get_difference();
            int sum = 0, n = s.length();
            for(int i=0;i<v.size();i++){
                sum += v[i];
            }
            return n*(n+1)/2 - sum;
        }
    };
    SuffixArray suffixarray_obj;
};


#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <random>
#include <chrono>
#include <stdexcept>
#include <cstdint>
#include <utility>
#include <algorithm>

/*
    @class Hash
    @brief Efficient randomized double-modulo rolling hash class for string and substring comparison.
    
    This class provides efficient hashing and comparison functionalities over strings.
    Internally, it uses two randomized modulo systems (MOD1, MOD2) and two randomized bases (base1, base2)
    to calculate robust 64-bit combined hashes, reducing the risk of collisions.
    
    Main use cases:
    - Storing multiple strings and computing their hashes.
    - Quickly comparing full strings or substrings using precomputed rolling hashes.
    - Performing masked hash queries (replace one character with wildcard).
    - Managing a stack of stored strings (push and pop).
    
    ------------------------------------------
    PUBLIC METHODS:
    
    ➤ size_t string_count() const
      → Returns the current number of strings stored in the object.
      → Example: if you pushed 3 strings, this returns 3.
    
    ➤ int64_t push_back(std::string s)
      → Adds a string to the hash system.
      → Computes and stores its prefix hashes.
      → Returns the 64-bit combined hash (based on its full content).
      → Example:
          int64_t h = hash.push_back("apple");
    
    ➤ int64_t get_single(std::string s)
      → Computes the 64-bit combined hash of the given string **without storing** it.
      → Useful for comparing with previously stored strings.
      → Example:
          int64_t h1 = hash.get_single("banana");
          int64_t h2 = hash.get_single("orange");
    
    ➤ int64_t pop_back()
      → Removes the last pushed string.
      → Returns its 64-bit combined hash.
      → Throws std::out_of_range if no strings are stored.
      → Example:
          int64_t last = hash.pop_back();
    
    ➤ std::pair<int32_t, int32_t> get_pair(size_t idx, int32_t i, int32_t j) const
      → Returns the pair of hashes (MOD1, MOD2) for the substring [i..j] of string at index idx.
      → Zero-based indexing.
      → Throws std::out_of_range on invalid index or range.
      → Example:
          auto pair = hash.get_pair(0, 2, 4);  // substring s[2..4] of first pushed string.
    
    ➤ int64_t get_ll(int idx, int i, int j) const
      → Returns the 64-bit combined hash for the substring [i..j] of string at index idx.
      → Equivalent to merging the MOD1 and MOD2 values into a single number.
      → Example:
         int64_t sub_hash = hash.get_ll(1, 0, 2);  // substring s[0..2] of second pushed string.
    
    ➤ int64_t get_masked(size_t idx, int j) const
      → Returns the 64-bit combined hash when the character at position j is replaced by a wildcard.
      → ⚠ Note: Requires alphabet_size to be increased by 1 (currently hardcoded to 26).
      → Example:
          int64_t masked = hash.get_masked(0, 2);  // first string, replacing s[2] with wildcard.
    
    ➤ int32_t compare_substring(int32_t idx1, int32_t i1, int32_t j1,
                                int32_t idx2, int32_t i2, int32_t j2) const
      → Lexicographically compares the substrings [i1..j1] and [i2..j2] 
        from strings at idx1 and idx2 respectively.
      → Returns:
           -1 if first substring < second,
            0 if they are equal,
           +1 if first substring > second.
      → Uses binary search over hashes for efficiency.
      → Example:
          int cmp = hash.compare_substring(0, 0, 2, 1, 0, 2);
    
    ➤ std::string get_string(int32_t idx) const
      → Returns the original stored string at index idx.
      → Throws std::out_of_range on invalid index.
      → Example:
          std::string s = hash.get_string(0);
    
    ------------------------------------------
    ⚠ Important Notes:
    - This hash system is **not cryptographically secure**. It is designed for fast lookups,
      comparisons, and algorithmic use (e.g., pattern matching, deduplication).
    - Internally, strings are indexed zero-based.
    - The alphabet_size is currently set to 26 ('a'..'z').
      To support wildcard hash functions properly, increase it to 27.
*/
class Hash{
private:
    // 0 indexed
    const int32_t alphabet_size = 26;
    int32_t base1, base2, MOD1, MOD2, _string_count, max_string_length;
    std :: vector<int32_t> MODS = {998244353, 1000000007, 1000034507, 1000064501, 1009090909};
    std :: vector<std :: vector<int32_t> > hash1, hash2;
    std :: vector<int32_t> pow1, pow2, shuffled_chars;
    std :: vector<std::string> data;
    std :: mt19937_64 rng;
    int32_t calc_hash(const std :: vector<int32_t>& hash, const std :: vector<int32_t>& po, const size_t& i, const size_t& j, const int32_t& MOD) const {
        if(i==0) return hash[j];
        return ((hash[j] - int64_t(hash[i-1]) * po[j-i+1]) % MOD + MOD) % MOD;
    }
    void build_hash(std :: vector<int32_t>& hash, std :: string& s, int MOD, int base){
        assert(s.size() > 0);
        hash[0] = shuffled_chars[s[0] - 'a'];
        for(int i=1;i<s.length();i++){
            hash[i] = (hash[i-1]*base + shuffled_chars[s[i] - 'a'])%MOD;
        }
    }
    void calculate_pows(std :: vector<int32_t>& po, int base, int MOD, int max_string_length){
        size_t old_length = po.size();
        po.resize(max_string_length);
        for(int i=old_length;i<max_string_length;i++){
            po[i] = int64_t(po[i-1])*base%MOD;
        }
    }
    void shuffle_chars(){
        shuffled_chars.resize(alphabet_size);
        for(int32_t i=0;i<alphabet_size;i++){
            shuffled_chars[i] = i+1;
        }
        for(int32_t i=0;i<alphabet_size-1;i++){
            int32_t j = rng() % (alphabet_size - i);
            std :: swap(shuffled_chars[i], shuffled_chars[i+j]);
        }
    }
public:
    size_t string_count() const {
        return _string_count;
    }
    std :: pair<int32_t,int32_t> get_pair(size_t idx, int32_t i, int32_t j) const {
        if(idx >= hash1.size() or i < 0 or j >= hash1[idx].size()) 
            throw std::out_of_range("get_pair: idx=" + std::to_string(idx) + ", i=" + std::to_string(i) + ", j=" + std::to_string(j));
        return {calc_hash(hash1[idx], pow1, i, j, MOD1), calc_hash(hash2[idx], pow2, i, j, MOD2)};
    }
    int64_t get_ll(int idx, int i, int j) const {
        std :: pair<int64_t, int64_t>pa = get_pair(idx,i,j);
        return pa.first * (1ll<<31) + pa.second;
    }
    int64_t push_back(std :: string s){
        while(s.length() > max_string_length){
            max_string_length *= 2;
            calculate_pows(pow1, base1, MOD1, max_string_length);
            calculate_pows(pow2, base2, MOD2, max_string_length);
        }
        std :: vector<int32_t>_hash1(s.length()), _hash2(s.length());
        build_hash(_hash1, s, MOD1, base1);
        build_hash(_hash2, s, MOD2, base2);
        hash1.push_back(_hash1);
        hash2.push_back(_hash2);
        data.push_back(s);
        ++_string_count;
        return int64_t(_hash1.back()) * (1ll<<31)  + _hash2.back();
    }
    int64_t get_single(std :: string s){
        while(s.length() > max_string_length){
            max_string_length *= 2;
            calculate_pows(pow1, base1, MOD1, max_string_length);
            calculate_pows(pow2, base2, MOD2, max_string_length);
        }
        std :: vector<int32_t>_hash1(s.length()), _hash2(s.length());
        build_hash(_hash1, s, MOD1, base1);
        build_hash(_hash2, s, MOD2, base2);
        return int64_t(_hash1.back()) * (1ll<<31)  + _hash2.back();
    }
    int64_t pop_back(){
        if(_string_count == 0){
            throw std::out_of_range("pop_back : hash object is empty");
        }
        int64_t to_be_ret = int64_t(hash1.back().back()) * (1ll<<31)  + hash2.back().back();
        hash1.pop_back();
        hash2.pop_back();
        data.pop_back();
        --_string_count;
        return to_be_ret;
    }
    int64_t get_masked(size_t idx, int j) const {
        // !!! If you will use this function, please increase alphabet_size by 1.
        int len = hash1[idx].size();
        int32_t pre1 = j > 0 ? calc_hash(hash1[idx], pow1, 0, j-1, MOD1) : 0;
        int32_t pre2 = j > 0 ? calc_hash(hash2[idx], pow2, 0, j-1, MOD2) : 0;
        int32_t suf1 = j+1 < len ? calc_hash(hash1[idx], pow1, j+1, len-1, MOD1) : 0;
        int32_t suf2 = j+1 < len ? calc_hash(hash2[idx], pow2, j+1, len-1, MOD2) : 0;
        int32_t w1 = shuffled_chars[alphabet_size - 1];
        int32_t w2 = w1;
        int rem = len - j - 1;
        int64_t m1 = ( int64_t(pre1) * pow1[rem+1]
                     + int64_t(w1)   * pow1[rem]
                     + int64_t(suf1) ) % MOD1;
        int64_t m2 = ( int64_t(pre2) * pow2[rem+1]
                     + int64_t(w2)   * pow2[rem]
                     + int64_t(suf2) ) % MOD2;
        return (m1 << 31) | m2;
    }
    int32_t compare_substring(int32_t idx1, int32_t i1, int32_t j1, int32_t idx2, int32_t i2, int32_t j2) const{
        if (idx1 < 0 || idx1 >= _string_count || idx2 < 0 || idx2 >= _string_count)
            throw std::out_of_range("compare_substring: invalid string index");
        if (i1 < 0 || j1 < i1 || j1 >= data[idx1].size() || i2 < 0 || j2 < i2 || j2 >= data[idx2].size())
            throw std::out_of_range("compare_substring: invalid substring range");
        int32_t right = std :: min(j1-i1+1, j2-i2+1), left = 1, lcp = 0, mid;
        while(left <= right){
            mid = (left + right) / 2;
            if(get_pair(idx1, i1, i1+mid-1) == get_pair(idx2, i2, i2+mid-1)){
                lcp = std :: max(lcp, mid);
                left = mid + 1;
            }
            else{
                right = mid - 1;
            }
        }
        if(i1 + lcp - 1 == j1 and i2 + lcp - 1 == j2) return 0;
        if(i1 + lcp - 1 == j1) return -1;
        if(i2 + lcp - 1 == j2) return 1;
        if(data[idx1][i1+lcp] < data[idx2][i2+lcp]) return -1;
        return 1;
    }
    std :: string get_string(int32_t idx) const{
        if(idx < 0 or idx >= _string_count) throw std::out_of_range("get_string : invalid argument");
        return data[idx];
    }
    Hash() : rng(std::chrono::steady_clock::now().time_since_epoch().count()){
        shuffle_chars();
        base1 = rng()%10 + alphabet_size + 1;
        base2 = rng()%10 + alphabet_size + 1;
        if(base1 == base2) base2 = base1 + 1;
        MOD1 = rng() % MODS.size();
        MOD2 = rng() % MODS.size();
        if(MOD1 == MOD2) MOD2 = (MOD1+1)%MODS.size();
        MOD1 = MODS[MOD1];
        MOD2 = MODS[MOD2];
        _string_count = 0;
        max_string_length = 1;
        pow1.push_back(1);
        pow2.push_back(1);
    }
};

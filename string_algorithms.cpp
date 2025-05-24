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
    @brief Double-modulo rolling hash class for efficient string hashing.
    
    This class implements a rolling hash (polynomial hash) over strings,
    using two independently randomized modulo bases (MOD1, MOD2) and two
    randomized base values (base1, base2) to compute hash values.
    It is designed to quickly compare or store strings (or substrings)
    by their hash values, minimizing collision risk.
    
    Key features:
    - Uses a shuffled mapping of characters ('a'–'z') to random values,
      making the character-to-value assignment less predictable.
    - Supports both complete string hashing (via get_single) and substring
      hashing (via get_pair or get_ll), leveraging precomputed prefix hashes.
    - Two independent modulos are used to compute a pair of hash values,
      which are then combined into a 64-bit value to greatly reduce collision chances.
    - Dynamically resizes power tables (pow1, pow2) when longer strings are processed.
    - push_back() stores and caches hash arrays for a collection of strings.
      get_single() computes the hash of a standalone string.
    
    Typical use cases:
    - Fast substring comparison (e.g., pattern matching, suffix-prefix checks).
    - String deduplication or lookup in hash-based structures.
    - Rolling window hash checks in algorithms like Rabin-Karp.
    
    Note:
    This hash implementation is for algorithmic use (not cryptographic security).
    It relies on the randomized base and modulo selection to achieve very low
    collision rates for practical input sizes.
*/
class Hash{
private:
    // 0 indexed
    const int32_t alphabet_size = 26;
    int32_t length, base1, base2, MOD1, MOD2, _string_count, max_string_length;
    std :: vector<int32_t> MODS = {998244353, 1000000007, 1000034507, 1000064501, 1009090909};
    std :: vector<std :: vector<int32_t> > hash1, hash2;
    std :: vector<int32_t> pow1, pow2, shuffled_chars;
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

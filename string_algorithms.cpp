class StringAlgorithms{
public:
    class SuffixArray{
    private:
        std::vector<std::vector<int> >suff;
        std::vector<std::vector<int> >ranks;
    public:
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
            
            @return Single integer, result
            
            Time and space complexity is O(N*log(N)), where N is the length of the string
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

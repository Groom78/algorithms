#include <vector>
#include <utility>
#include <cmath>
#include <algorithm>
class GeometryAlgorithms{
private:
    // Finds the "pythagorean distance" between gicen two points
    double find_pythagorean_distance(std::pair<int, int> first, std::pair<int, int> second){
        return sqrt((first.first - second.first)*(first.first - second.first) + (first.second - second.second)*(first.second - second.second));
    }
public:
    /*
        @brief
        Finds the closest pair of points according to "pythagorean distance"
        By using divide and conquer approach

        @param
        points - A vector of points
        
        @return
        {minimum distance ,{first point, second point}}
              
        @fortesting
        https://judge.yosupo.jp/problem/closest_pair
        
        Time and space complexity is O(N*log(N)), where N is the number of points
        
        @important note
        Since the result of algorithm is not always an integer
        Please change the variable types if needed
        For example, you may need non-integer points
    */
    std::pair<double,std::pair<std::pair<int, int>, std::pair<int, int> > > find_closest_pair_distance(std::vector<std::pair<int,int> > points){
        int n = points.size();
        if(n == 0){
            return {5e9,{{0,0},{0,0}}}; // invalid, give a big number
        }
        if(n == 1){
            return {5e9,{{points[0].first,points[0].second},{points[0].first,points[0].second}}}; // give a big number
        }
        bool sorted = true;
        for(int i=0;i<n-1;i++){
            if(points[i] > points[i+1]){
                sorted = false;
                break;
            }
        }
        if(!sorted) std::sort(points.begin(), points.end());
        std::vector<std::pair<int,int> > first_group;
        std::vector<std::pair<int,int> > second_group;
        for(int i=0;i<n/2;i++){
            first_group.push_back(points[i]);
        }
        for(int i=n/2;i<n;i++){
            second_group.push_back(points[i]);
        }
        std::pair<double,std::pair<std::pair<int, int>, std::pair<int, int> > > first_group_result = find_closest_pair_distance(first_group);
        std::pair<double,std::pair<std::pair<int, int>, std::pair<int, int> > > second_group_result = find_closest_pair_distance(second_group);
        std::pair<double,std::pair<std::pair<int, int>, std::pair<int, int> > > result;
        std::vector<std::pair<int, int> >first_candidates;
        std::vector<std::pair<int, int> >second_candidates;
        if(first_group_result.ff < second_group_result.ff){
            result = first_group_result;
        }
        else{
            result = second_group_result;
        }
        int delimiter = first_group.back().first;
        for(int i=0;i<first_group.size();i++){
            if(delimiter - first_group[i].first <= ceil(result.first)){
                first_candidates.push_back({first_group[i].second, first_group[i].first});
            }
        }
        for(int i=0;i<second_group.size();i++){
            if(second_group[i].first - delimiter <= ceil(result.first)){
                second_candidates.push_back({second_group[i].second, second_group[i].first});
            }
        }
        if(second_candidates.size() == 0) return result;
        std :: sort(first_candidates.begin(), first_candidates.end());
        std :: sort(second_candidates.begin(), second_candidates.end());
        int left = 0, right = 0;
        for(int i=0; i < first_candidates.size();i++){
            while(left < second_candidates.size()-1 and first_candidates[i].first - second_candidates[left].first > floor(result.first)){
                left++;
            }
            while(right < second_candidates.size()-1 and second_candidates[right+1].first - first_candidates[i].first <= floor(result.first)){
                right++;
            }
            for(int j=left;j<=right;j++){
                if(result.first > find_pythagorean_distance(first_candidates[i], second_candidates[j])){
                    result = {find_pythagorean_distance(first_candidates[i], second_candidates[j]),{first_candidates[i], second_candidates[j]}};
                    std :: swap(result.second.first.first, result.second.first.second);
                    std :: swap(result.second.second.first, result.second.second.second);
                }
            }
        }
        return result;
    }
};

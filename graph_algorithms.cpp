#include <vector>
#include <queue>
#include <cstdint>
#include <climits>
#include <algorithm>

class GraphAlgorithms{
public:
    class Dijkstra{
    public:
        /*
            @brief
            Clasical Dijkstra algorithm
            Finds shortest path from source to target for a given directed weighted graph

            @param
            adj  adjacency list for the graph - adj[i] is the list of adjacent nodes for node i. Each element should be in the form of {adjacent_node, cost}
            source - index of the source node
            target - index of the target node
            find_path - set "true" if you will need nodes of one of the shortest paths, and call find_path function
            
            @return
            Shortest path from source to target
                  
            Time complexity is O((E+V)*log(E)), where V is the number of nodes, and E is the number of egdes in the graph
            Space complexity is O(V+E), where V is the number of nodes, and E is the number of egdes in the graph
        */
        
        std::vector<int64_t>distance;
        std::vector<int>prev;
        int calculate(std::vector<std::vector<std::pair<int,int> > >& adj, int source, int target, bool find_path = false){
            int n = adj.size(), node;
            int64_t cost;
            std::vector<bool>visited(n, false);
            distance.resize(n, LLONG_MAX);
            if(find_path) prev.resize(n);
            std::priority_queue<std::pair<int64_t,int>, std::vector<std::pair<int64_t,int> >, std::greater<std::pair<int64_t,int> > >pq;
            distance[source] = 0ll;
            pq.push({0ll, source});
            prev[source] = -1;
            while(!pq.empty()){
                node = pq.top().second;
                cost = pq.top().first;
                pq.pop();
                if(visited[node]) continue;
                visited[node] = true;
                for(std::pair<int,int>& u : adj[node]){
                    if(distance[u.first] > cost + u.second){
                        distance[u.first] = cost + u.second;
                        pq.push({cost + u.second, u.first});
                        if(find_path){
                            prev[u.first] = node;
                        }
                    }
                }
            }
            return distance[target];
        }
        
        /*
            @brief
            Finds one of the paths from source to target
            Must be called after 

            @param
            node - the taget node (source node is fixed)
            
            @return
            A vector, containing node index of nodes which are on one of the shortest paths
            
            @fortesting
            https://codeforces.com/problemset/problem/20/C
            
            Time  and space complexity is O(V), where V is the number of nodes in the graph
            (These are individual values, cost of "calculate" is not included)
        */
        
        std::vector<int> find_path(int node){
            std::vector<int>ret;
            if(prev[node] == -1){
                return {};
            }
            while(node != -1){
                ret.push_back(node);
                node = prev[node];
            }
            std::reverse(ret.begin(), ret.end());
            return ret;
        }
    };
    Dijkstra dijkstra_obj;
};

class GraphAlgorithms{
public:
    class Dijkstra{
    public:
        vector<int>distance, prev;
        int calculate(vector<vector<pair<int,int> > >& adj, int source, int target, int find_path = false){
            int n = adj.size(), node, cost;
            vector<bool>visited(n, false);
            distance.resize(n, LLONG_MAX);
            if(find_path) prev.resize(n);
            priority_queue<pair<int,int>, vector<pair<int,int> >, greater<pair<int,int> > >pq;
            distance[source] = 0;
            pq.push({0, source});
            prev[source] = -1;
            while(!pq.empty()){
                node = pq.top().second;
                cost = pq.top().first;
                pq.pop();
                if(visited[node]) continue;
                visited[node] = true;
                distance[node] = cost;
                for(pair<int,int>& u : adj[node]){
                    if(distance[u.first] > cost + u.second){
                        pq.push({cost + u.second, u.first});
                        
                        if(find_path){
                            prev[u.first] = node;
                        }
                    }
                }
            }
            return distance[target];
        }
        
        vector<int> find_path(int node){
            vector<int>ret;
            while(node != -1){
                ret.push_back(node);
                node = prev[node];
            }
            reverse(ret.begin(), ret.end());
            return ret;
        }
    };
    Dijkstra dijkstra_obj;
};

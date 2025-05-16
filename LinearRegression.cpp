/*
Here is my own implementation for Linear Regression in cpp
Usage guide are included in main function below

LinearRegression class complexity overview:

- Data normalization (NormalizeData): O(N * M)
  N = number of samples, M = number of features

- Gradient computation per iteration (GetDerivatives): O(N * M)

- Training loop (Train): O(steps * N * M)
  steps = number of gradient descent iterations

- Denormalization (DenormalizeCoefficients): O(M)

- Prediction (Estimate): O(M)

Overall complexity is dominated by training: O(steps × N × M), typical for batch gradient descent.
*/

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <cmath>
#include <random>
#include <chrono>
#include <iomanip>
#include <algorithm>

std :: ostream& operator<<(std :: ostream& os, const std :: vector<long double>& v){
    std :: cout << "{";
    bool flag = false;
    for(const long double& item : v){
        if(flag){
            std :: cout << ", ";
        }
        flag = true;
        os << std :: fixed << std :: setprecision(3) << item;
    }
    std :: cout << "}";
    std :: cout << std :: endl;
    return os;
}

std :: mt19937_64 rng(std :: chrono :: steady_clock::now().time_since_epoch().count());
namespace VectorOperations{
    template <typename T>
    void Shuffle(std :: vector<T>& v){
        for(int i=0;i<v.size()-1;i++){
            std :: swap(v[i], v[i + rng() % (v.size() - i)]);
        }
    }
}

class LinearRegression{
public:
    void Train(std :: vector <std :: vector<long double> > data, std :: map<std :: string, long double>parameters = {}) {
        std :: map<std :: string, long double>params;
        params["lr"] = 0.01;
        params["steps"] = 1000;
        params["shuffle"] = 1.0;
        params["split_rate"] = 80.0;
        
        for(auto p : parameters){
            params[p.first] = p.second;
        }
        
        long double lr = params["lr"];
        int steps = (int)(params["steps"]);
        bool will_be_shuffled = (params["shuffle"] == 0.0);
        long double split_rate = params["split_rate"];
        
        int i, j;
        
        if(will_be_shuffled) VectorOperations :: Shuffle(data);
        
        std :: vector<long double> results;
        for(i=0;i<data.size();i++){
            results.push_back(data[i].back());
            data[i].pop_back();
        }
        
        std :: vector<long double> means, std_devs;
        long double mean_y = 0.0, std_dev_y = 0.0;
        NormalizeData(data, results, means, std_devs, mean_y, std_dev_y);
        
        int n = data[0].size();
        std :: vector<long double> coefs(n + 1, 0.0);
        if(all_coefficients.size())
            coefs = all_coefficients.back();
        std :: vector<long double> derivatives(data[0].size() + 1, 0.0);
        
        for(i=0;i<steps;i++){
            GetDerivatives(derivatives, data, coefs, results);
            for(int j = 0;j<coefs.size();j++){
                coefs[j] -= lr * derivatives[j];
            }
        }
        
        DenormalizeCoefficients(coefs, means, std_devs, mean_y, std_dev_y);
        
        std :: cout << "Final coefficients: " << coefs;
        all_coefficients.push_back(coefs);
        return;
    }
    
    void NormalizeData(std :: vector <std :: vector<long double> >& data, std :: vector<long double>& results, std :: vector<long double>& means, std :: vector<long double>& std_devs, long double& mean_y, long double& std_dev_y) {
        int n = data[0].size();
        means.resize(n, 0.0);
        std_devs.resize(n, 0.0);
        int i, j;
        int datasize = data.size();

        for(j=0; j<n; j++){
            for(i=0; i<datasize; i++){
                means[j] += data[i][j];
            }
            means[j] /= datasize;
        }

        for(j=0; j<n; j++){
            for(i=0; i<datasize; i++){
                std_devs[j] += (data[i][j] - means[j]) * (data[i][j] - means[j]);
            }
            std_devs[j] = sqrt(std_devs[j] / datasize);
        }

        for(i=0; i<datasize; i++){
            for(j=0; j<n; j++){
                if(std_devs[j] != 0){
                    data[i][j] = (data[i][j] - means[j]) / std_devs[j];
                }
            }
        }

        mean_y = 0.0;
        std_dev_y = 0.0;
        for(i=0; i<datasize; i++){
            mean_y += results[i];
        }
        mean_y /= datasize;
        for(i=0; i<datasize; i++){
            std_dev_y += (results[i] - mean_y) * (results[i] - mean_y);
        }
        std_dev_y = sqrt(std_dev_y / datasize);
        for(i=0; i<datasize; i++){
            if(std_dev_y != 0){
                results[i] = (results[i] - mean_y) / std_dev_y;
            }
        }
    }
    
    void DenormalizeCoefficients(std :: vector<long double>& coefs, const std :: vector<long double>& means, const std :: vector<long double>& std_devs, long double mean_y, long double std_dev_y) {
        int n = coefs.size() - 1;
        
        coefs[0] = coefs[0] * std_dev_y + mean_y;
        
        for(int i = 1; i <= n; i++){
            coefs[i] = (coefs[i] * std_dev_y) / std_devs[i - 1];
            coefs[0] -= (means[i - 1] * coefs[i]);
        }
    }
    
    void GetDerivatives(std :: vector<long double>& derivatives, std :: vector <std :: vector<long double> >& data, std :: vector<long double>& coefs, std :: vector<long double>& results) {
        long double answer, single;
        long double datasize = data.size();
        int i, k;
        
        answer = 0.0;
        for(i=0;i<data.size();i++){
            single = Function(coefs, data[i]) - results[i];
            answer += single;
        }
        answer /= datasize;
        derivatives[0] = answer;

        std :: vector<long double> singles(data.size());
        for(i=0;i<data.size();i++){
            singles[i] = Function(coefs, data[i]) - results[i];
        }
        
        for(k=0;k<data[0].size();k++){
            answer = 0.0;
            for(i=0;i<data.size();i++){
                answer += data[i][k] * singles[i];
            }
            answer /= datasize;
            derivatives[k+1] = answer;
        }
    }
    
    long double SumSquaredError(std :: vector <std :: vector<long double> >& data, std :: vector<long double>& coefs, std :: vector<long double>& results) {
        long double answer = 0.0, single;
        long double n = results.size();
        long double datasize = data.size();
        int i;
        for(i=0;i<data.size();i++){
            single = Function(coefs, data[i]) - results[i];
            answer += single * single;
        }
        return answer / datasize;
    }
    
    long double Function(std :: vector<long double>& coefs, std :: vector<long double>& data) {
        long double answer = coefs[0];
        int i;
        for(i=0;i<data.size();i++){
            answer += coefs[i+1] * data[i];
        }
        return answer;
    }
    
    long double Estimate(std :: vector<long double> x) {
        return Function(all_coefficients.back(), x);
    }
    
    void GetCoefficients() {
        std :: cout << "Coefficients are: " << std :: endl;
        std :: cout << all_coefficients.back();
    }
    
    void GetAllCoefficients() {
        std :: cout << "All coefficients are: " << std :: endl;
        for(std :: vector<long double>& each : all_coefficients){
            std :: cout << each;
        }
        std :: cout << "(old to new)" << std :: endl;
    }
    
    void HelloWorld() {
        std :: cout << "HelloWorld" << std :: endl;
        return;
    }
    
private:
    std :: vector<std :: vector<long double> >all_coefficients;
};

int32_t main(){
    LinearRegression lr;

    // True function: y = 2.5 * x1 - 1.3 * x2 + 4.0
    // Generate 100 samples with noise
    std::vector<std::vector<long double>> data;
    std::mt19937_64 gen(std::chrono::steady_clock::now().time_since_epoch().count());
    std::normal_distribution<long double> noise(0.0, 0.1);

    for(int i = 0; i < 100; i++){
        long double x1 = (long double)(gen() % 1000) / 100.0L; // 0 to 10
        long double x2 = (long double)(gen() % 1000) / 100.0L; // 0 to 10
        long double y = 2.5 * x1 - 1.3 * x2 + 4.0 + noise(gen);

        // Feature vector + label as last element
        std::vector<long double> sample = {x1, x2, y};
        data.push_back(sample);
    }

    // Shuffle data using your shuffle function
    VectorOperations::Shuffle(data);

    // Train linear regression
    lr.Train(data);

    // Test estimation with a sample input
    std::vector<long double> test_x = {5.0L, 3.0L};
    long double estimated_y = lr.Estimate(test_x);

    std::cout << "Estimate for x = {5.0, 3.0}: " << estimated_y << std::endl;

    return 0;
}


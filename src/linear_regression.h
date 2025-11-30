// linear regression model for predicting future prices

#pragma once
#include <vector>
#include <string>

namespace sp {
using namespace std;

class LinearRegression {
public:
    LinearRegression();
    
    bool train(const vector<vector<double>>& features, 
               const vector<double>& targets);
    
    double predict(const vector<double>& features) const;
    vector<double> predict_batch(const vector<vector<double>>& features) const;
    
    double evaluate(const vector<vector<double>>& features,
                    const vector<double>& targets) const;
    
    double r_squared(const vector<vector<double>>& features,
                     const vector<double>& targets) const;
    
    const vector<double>& coefficients() const { return coefficients_; }
    bool is_trained() const { return trained_; }
    
private:
    vector<double> coefficients_;
    bool trained_;
    
    // matrix math helpers for computing coefficients
    vector<vector<double>> transpose(const vector<vector<double>>& matrix) const;
    vector<vector<double>> multiply(const vector<vector<double>>& A,
                                               const vector<vector<double>>& B) const;
    vector<double> multiply_vector(const vector<vector<double>>& A,
                                         const vector<double>& b) const;
    vector<vector<double>> inverse(const vector<vector<double>>& matrix) const;
};

} // namespace sp

#ifndef POLYNOMIAL_HPP
#define POLYNOMIAL_HPP

#include "Function.hpp"
#include <vector>
#include <cmath>

// Polynomial.hpp
// 多项式函数 f(x) = a_n * x^n + a_{n-1} * x^{n-1} + ... + a_1 * x + a_0
template <typename T>
class Polynomial : public Function<T> {
private:
    std::vector<T> coefficients_; // 系数，从a_0到a_n

public:
    // 构造函数，接受系数向量（从常数项a_0到最高次项a_n）
    Polynomial(const std::vector<T>& coefficients) : coefficients_(coefficients) {}

    // 重载操作符()，计算多项式函数值
    T operator()(T x) const override {
        T result = T(); // 初始化为0
        
        // 使用秦九韶算法(Horner's method)计算多项式值
        for (int i = coefficients_.size() - 1; i >= 0; --i) {
            result = result * x + coefficients_[i];
        }
        
        return result;
    }

    // 获取多项式的次数
    size_t getDegree() const {
        return (coefficients_.empty() ? 0 : coefficients_.size() - 1);
    }

    // 获取系数向量
    const std::vector<T>& getCoefficients() const {
        return coefficients_;
    }
};

#endif // POLYNOMIAL_HPP

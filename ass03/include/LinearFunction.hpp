#ifndef LINEAR_FUNCTION_HPP
#define LINEAR_FUNCTION_HPP

#include "Function.hpp"

// LinearFunction.hpp
// 线性函数 f(x) = ax + b
template <typename T>
class LinearFunction : public Function<T> {
private:
    T a_; // 斜率
    T b_; // 截距

public:
    // 构造函数
    LinearFunction(const T& a, const T& b) : a_(a), b_(b) {}

    // 重载操作符()，计算函数值 f(x) = ax + b
    T operator()(T x) const override {
        return a_ * x + b_;
    }

    // 获取斜率
    T getSlope() const {
        return a_;
    }

    // 获取截距
    T getIntercept() const {
        return b_;
    }
};

#endif // LINEAR_FUNCTION_HPP

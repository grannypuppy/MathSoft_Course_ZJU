#include <iostream>
#include <complex>
#include <vector>
#include <iomanip>
#include "../include/Function.hpp"
#include "../include/LinearFunction.hpp"
#include "../include/Polynomial.hpp"

// 如果需要支持GMP库
#ifdef USE_GMP
#include <gmpxx.h>
#endif

// 用于打印复数
template <typename T>
void printComplex(const std::complex<T>& c) {
    if (c.imag() >= 0) {
        std::cout << c.real() << " + " << c.imag() << "i";
    } else {
        std::cout << c.real() << " - " << std::abs(c.imag()) << "i";
    }
}

// 用于打印向量
template <typename T>
void printVector(const std::vector<T>& vec) {
    std::cout << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << vec[i];
        if (i < vec.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "]";
}

// 测试函数模板
template <typename T>
void testLinearFunction(const T& a, const T& b, const T& x) {
    LinearFunction<T> f(a, b);
    std::cout << "Linear function f(x) = " << a << " * x + " << b << std::endl;
    std::cout << "f(" << x << ") = " << f(x) << std::endl;
    std::cout << "Slope: " << f.getSlope() << ", Intercept: " << f.getIntercept() << std::endl;
    std::cout << std::endl;
}

// 复数特化版本
template <>
void testLinearFunction(const std::complex<double>& a, const std::complex<double>& b, const std::complex<double>& x) {
    LinearFunction<std::complex<double>> f(a, b);
    std::cout << "Linear function f(x) = ";
    printComplex(a);
    std::cout << " * x + ";
    printComplex(b);
    std::cout << std::endl;
    
    std::cout << "f(";
    printComplex(x);
    std::cout << ") = ";
    printComplex(f(x));
    std::cout << std::endl;
    
    std::cout << "Slope: ";
    printComplex(f.getSlope());
    std::cout << ", Intercept: ";
    printComplex(f.getIntercept());
    std::cout << std::endl << std::endl;
}

// 测试多项式函数模板
template <typename T>
void testPolynomialFunction(const std::vector<T>& coeffs, const T& x) {
    Polynomial<T> p(coeffs);
    std::cout << "Polynomial function p(x) = ";
    
    for (int i = p.getDegree(); i >= 0; --i) {
        if (i < p.getDegree()) {
            std::cout << " + ";
        }
        std::cout << coeffs[i];
        if (i > 0) {
            std::cout << " * x";
            if (i > 1) {
                std::cout << "^" << i;
            }
        }
    }
    std::cout << std::endl;
    
    std::cout << "p(" << x << ") = " << p(x) << std::endl;
    std::cout << "Degree: " << p.getDegree() << std::endl;
    std::cout << std::endl;
}

// 复数特化版本
template <>
void testPolynomialFunction(const std::vector<std::complex<double>>& coeffs, const std::complex<double>& x) {
    Polynomial<std::complex<double>> p(coeffs);
    std::cout << "Polynomial function p(x) = ";
    
    for (int i = p.getDegree(); i >= 0; --i) {
        if (i < p.getDegree()) {
            std::cout << " + ";
        }
        printComplex(coeffs[i]);
        if (i > 0) {
            std::cout << " * x";
            if (i > 1) {
                std::cout << "^" << i;
            }
        }
    }
    std::cout << std::endl;
    
    std::cout << "p(";
    printComplex(x);
    std::cout << ") = ";
    printComplex(p(x));
    std::cout << std::endl;
    
    std::cout << "Degree: " << p.getDegree() << std::endl;
    std::cout << std::endl;
}

// 向量类型的线性函数测试
void testVectorLinearFunction() {
    // 定义向量类型的线性函数
    std::vector<double> a = {1.0, 2.0, 3.0};
    std::vector<double> b = {4.0, 5.0, 6.0};
    std::vector<double> x = {7.0, 8.0, 9.0};
    
    // 实现向量的加法和乘法操作
    auto vectorAdd = [](const std::vector<double>& v1, const std::vector<double>& v2) {
        std::vector<double> result(v1.size());
        for (size_t i = 0; i < v1.size(); ++i) {
            result[i] = v1[i] + v2[i];
        }
        return result;
    };
    
    auto scalarMult = [](const std::vector<double>& v, double scalar) {
        std::vector<double> result(v.size());
        for (size_t i = 0; i < v.size(); ++i) {
            result[i] = v[i] * scalar;
        }
        return result;
    };
    
    // 创建一个自定义的向量线性函数
    class VectorLinearFunction : public Function<std::vector<double>> {
    private:
        double a_;
        std::vector<double> b_;
        
    public:
        VectorLinearFunction(double a, const std::vector<double>& b) : a_(a), b_(b) {}
        
        std::vector<double> operator()(std::vector<double> x) const override {
            std::vector<double> result(x.size());
            for (size_t i = 0; i < x.size(); ++i) {
                result[i] = a_ * x[i] + b_[i];
            }
            return result;
        }
    };
    
    // 测试向量线性函数
    VectorLinearFunction f(2.5, b);
    std::cout << "Vector Linear function f(x) = " << 2.5 << " * x + ";
    printVector(b);
    std::cout << std::endl;
    
    std::cout << "f(";
    printVector(x);
    std::cout << ") = ";
    printVector(f(x));
    std::cout << std::endl << std::endl;
}

int main() {
    std::cout << "===== Testing LinearFunction =====" << std::endl;
    
    // 测试整数类型
    std::cout << "Testing with int:" << std::endl;
    testLinearFunction<int>(2, 3, 4);
    
    // 测试浮点类型
    std::cout << "Testing with float:" << std::endl;
    testLinearFunction<float>(2.5f, 3.7f, 4.2f);
    
    // 测试双精度浮点类型
    std::cout << "Testing with double:" << std::endl;
    testLinearFunction<double>(2.5, 3.7, 4.2);
    
    // 测试复数类型
    std::cout << "Testing with std::complex<double>:" << std::endl;
    testLinearFunction<std::complex<double>>(
        std::complex<double>(2.0, 1.0),
        std::complex<double>(3.0, -2.0),
        std::complex<double>(1.0, 1.0)
    );
    
    // 测试向量类型
    std::cout << "Testing with vector:" << std::endl;
    testVectorLinearFunction();
    
    std::cout << "===== Testing Polynomial =====" << std::endl;
    
    // 测试整数多项式
    std::cout << "Testing polynomial with int:" << std::endl;
    testPolynomialFunction<int>({1, 2, 3}, 2);  // p(x) = 3x^2 + 2x + 1
    
    // 测试浮点多项式
    std::cout << "Testing polynomial with double:" << std::endl;
    testPolynomialFunction<double>({1.5, 2.5, 3.5, 4.5}, 2.0);  // p(x) = 4.5x^3 + 3.5x^2 + 2.5x + 1.5
    
    // 测试复数多项式
    std::cout << "Testing polynomial with std::complex<double>:" << std::endl;
    testPolynomialFunction<std::complex<double>>(
        {std::complex<double>(1.0, 1.0), std::complex<double>(2.0, -1.0), std::complex<double>(0.0, 3.0)},
        std::complex<double>(1.0, 1.0)
    );
    
#ifdef USE_GMP
    // 测试GMP库
    std::cout << "===== Testing with GMP =====" << std::endl;
    
    mpf_class a("2.5", 256);  // 256位精度
    mpf_class b("3.7", 256);
    mpf_class x("4.2", 256);
    
    LinearFunction<mpf_class> f(a, b);
    std::cout << "Linear function f(x) = " << a << " * x + " << b << std::endl;
    std::cout << "f(" << x << ") = " << f(x) << std::endl;
    std::cout << "Slope: " << f.getSlope() << ", Intercept: " << f.getIntercept() << std::endl;
    std::cout << std::endl;
    
    // 测试GMP多项式
    std::vector<mpf_class> coeffs = {
        mpf_class("1.5", 256),
        mpf_class("2.5", 256),
        mpf_class("3.5", 256)
    };
    
    Polynomial<mpf_class> p(coeffs);
    std::cout << "Polynomial function p(x) = 3.5x^2 + 2.5x + 1.5" << std::endl;
    std::cout << "p(" << x << ") = " << p(x) << std::endl;
    std::cout << "Degree: " << p.getDegree() << std::endl;
#endif
    
    return 0;
}

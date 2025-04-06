#ifndef FUNCTION_HPP
#define FUNCTION_HPP

// Function.hpp
template <typename T>
class Function {
public:
  virtual T operator()(T x) const = 0; // 纯虚函数，用来计算函数值
  virtual ~Function() = default; // 虚析构函数，确保派生类能正确析构
};

#endif // FUNCTION_HPP

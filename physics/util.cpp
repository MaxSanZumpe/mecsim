#include "util.hpp"

std::vector<double> operator*(const std::vector<float>& v, const std::vector<double>& w)
{
    size_t n = v.size();
    if (n != w.size()) {
        throw std::invalid_argument("ERROR::VECTOR_MULT::INCOMPATIBLE_DIMENSIONS");
    }

    std::vector<double> result {};
    result.reserve(n);
    for (auto [a, b] : std::views::zip(v, w)) {
        result.emplace_back(a * b);
    }

    return result;
}


std::vector<double> operator*(const std::vector<double>& v, const std::vector<double>& w)
{
    size_t n = v.size();
    if (n != w.size()) {
        throw std::invalid_argument("ERROR::VECTOR_MULT::INCOMPATIBLE_DIMENSIONS");
    }

    std::vector<double> result {};
    result.reserve(n);
    for (auto [a, b] : std::views::zip(v, w)) {
        result.emplace_back(a * b);
    }

    return result;
}


std::vector<double> operator*(double a, const std::vector<double>& v)
{
    std::vector<double> result {};
    result.reserve(v.size());
    for (auto e : v) {
        result.emplace_back(a * e);
    }

    return result;
}


std::vector<double> operator-(const std::vector<double>& v, const std::vector<double>& w) 
{
    size_t n = v.size();
    if (n != w.size()) {
        throw std::invalid_argument("ERROR::VECTOR_SUB::INCOMPATIBLE_DIMENSIONS");
    }

    std::vector<double> result {};
    result.reserve(n);
    for (auto [a, b] : std::views::zip(v, w)) {
        result.emplace_back(a - b);
    }

    return result;
}


std::vector<double>  operator+(const std::vector<double>& v, const std::vector<double>& w)
{
    size_t n = v.size();
    if (n != w.size()) {
        throw std::invalid_argument("ERROR::VECTOR_SUM::INCOMPATIBLE_DIMENSIONS");
    }

    std::vector<double> result {};
    result.reserve(n);
    for (auto [a, b] : std::views::zip(v, w)) {
        result.emplace_back(a + b);
    }

    return result;
}



std::vector<double>& operator-=(std::vector<double>& v, const std::vector<double>& w)
{
    size_t n = v.size();
    if (n != w.size()) {
        throw std::invalid_argument("ERROR::VECTOR_SUB::INCOMPATIBLE_DIMENSIONS");
    }

    for (size_t i = 0; i < n; ++i) {
        v[i] -= w[i] ;
    }

    return v;
}


std::vector<double>& operator+=(std::vector<double>& v, const std::vector<double>& w)
{
    size_t n = v.size();
    if (n != w.size()) {
        throw std::invalid_argument("ERROR::VECTOR_SUB::INCOMPATIBLE_DIMENSIONS");
    }

    for (size_t i = 0; i < n; ++i) {
        v[i] += w[i] ;
    }

    return v;   
}



double dot(const std::vector<double>& v, const std::vector<double>& w)
{
    double result = 0;

    for (auto [a, b] : std::views::zip(v, w)) {
        result += a * b;
    }

    return result;
}


double norm(const std::vector<double>& v)
{
    double result = 0;

    for (auto& a : v) {
        result += a * a;
    }

    return sqrt(result);
}
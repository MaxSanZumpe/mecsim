#ifndef LINEAR_SOLVER_HPP
#define LINEAR_SOLVER_HPP

#include <vector>
#include <cstdint>
#include <functional>

#include "util.hpp"

typedef std::function<std::vector<double>(const std::vector<double>&)> matrix_func;

class LinearSolver
{   
    /* Brief: Prototype class for solving linear equation Mx = b, where M is a (dim x dim) symetric
              positive-definite matrix. The matrix action on the solution vector x is user defined 
              via a function pointer. */

    protected: 
        size_t m_dim {};  
        matrix_func m_matrix {};
        std::vector<double> m_b {}; 

    public:
        LinearSolver(matrix_func matrix, std::vector<double>&& b, size_t dim) : m_dim(dim), m_matrix(matrix) 
        {
            m_b = std::move(b);
        }

        virtual std::vector<double> solve() = 0;
};


class ConjugateGradSleSolver : public LinearSolver
{   
    private:
        double m_tol {};
        size_t max_iter { 100 };
        std::vector<double> m_solution {};

    public: 
        ConjugateGradSleSolver(matrix_func matrix, std::vector<double>&& b,
                               size_t dim, double tolerance = 0.2) : 
                               LinearSolver(matrix, std::move(b), dim) 
        {
            m_solution.resize(dim, 0.0);
            m_tol = tolerance;
        }

        std::vector<double> solve() override;
};

#endif
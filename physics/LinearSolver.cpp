#include "LinearSolver.hpp"

std::vector<double> ConjugateGradSleSolver::solve()
{
    double alpha {};
    double beta {};

    std::vector<double> dirs (m_dim);
    std::vector<double> resi (m_dim);
    std::vector<double> resi_prev (m_dim);

    auto aux_b = m_matrix(m_solution);

    dirs = m_b - aux_b;
    resi_prev = dirs;

    double delta = norm(resi_prev);
    if (delta == 0.0) return m_solution;

    size_t i = 0;
    bool stop = false;
    while(!stop) {
        double tmp = dot(dirs, m_matrix(dirs));
        alpha = dot(resi_prev, resi_prev) / tmp;
        m_solution += alpha * dirs;

        resi = resi_prev - alpha * m_matrix(dirs);

        ++i;
        if (norm(resi) < m_tol * delta || i == max_iter) 
        {
            stop = true;
        } else { 
            beta = dot(resi, resi) / dot(resi_prev, resi_prev);
            
            dirs = resi + beta * dirs;
    
            resi_prev = resi;
        }
    }

    return m_solution;
}
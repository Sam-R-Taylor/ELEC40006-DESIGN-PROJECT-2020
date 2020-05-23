#include"leo_KCLSolver.hpp"

int main(int argc, char const *argv[])
{
    Circuit input_circuit = parse_input(argv[1]);
    
    Matrix_solver(input_circuit);
    return 0;
}

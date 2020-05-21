#include"Parser.hpp"
#include"Leo_Component.hpp"
#include<Eigen/Dense>



Eigen::MatrixXd return_Conductance_Matrix(const Circuit& input_circuit){}








/*
given a Circuit 

returns a Eigen::VectorXd of voltages (i.e. the solution to the circuit)
*/
Eigen::VectorXd Matrix_solver(const Circuit& input_circuit){}



int main(int argc, char const *argv[])
{   
    Circuit input_circuit = Parse_input(argv[1]);
}

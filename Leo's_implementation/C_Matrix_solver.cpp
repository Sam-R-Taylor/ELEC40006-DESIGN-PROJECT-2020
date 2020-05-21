#include"Parser.hpp"
#include"Leo_Component.hpp"
#include<Eigen/Dense>



Eigen::MatrixXd return_Conductance_Matrix(const Circuit& input_circuit)
{   
    //initializing a matrix of the right size
    int Mat_size = input_circuit.biggest_node_index();
    Eigen::MatrixXd Mat(Mat_size,Mat_size);

    //initializing each element of the matrix
    for (int row = 0; row < Mat_size; row++)
    {
        for (int column = 0; column < Mat_size; column++)
        {
            //find components attached to node "row" and add their conductance
        }
        
    }
    

}








/*
given a Circuit 

returns a Eigen::VectorXd of voltages (i.e. the solution to the circuit)
*/
Eigen::VectorXd Matrix_solver(const Circuit& input_circuit){}



int main(int argc, char const *argv[])
{   
    Circuit input_circuit = Parse_input(argv[1]);
}

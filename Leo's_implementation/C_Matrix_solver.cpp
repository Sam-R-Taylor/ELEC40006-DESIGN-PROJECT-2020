#include"Parser.hpp"
#include"Leo_Component.hpp"
#include<Eigen/Dense>



Eigen::MatrixXd return_Conductance_Matrix(const Circuit& input_circuit)
{   
    //initializing a matrix of the right size 
    //discards node 0
    int Mat_size = input_circuit.number_of_nodes() - 1;     
    Eigen::MatrixXd Mat(Mat_size,Mat_size);
    //Fills up the matrix with 0s
    Mat.Zero(Mat_size,Mat_size);

    std::vector<std::vector<Component&>> node_circuit = input_circuit.node_circuit_generator();

    //initializing each element of the matrix
    for (int row = 0; row < Mat_size; row++)
    {
        int current_node_index = row + 1;
        for(Component current_component: node_circuit[current_node_index])
            {
                if ()
                {
                    /* code */
                }
                
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

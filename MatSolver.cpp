#include "Circuit.hpp"
#include "Component.hpp"
#include "Parser.hpp"
#include<Eigen/Dense>



Eigen::MatrixXd return_Conductance_Matrix(const Circuit& input_circuit)
{   
    //initializing a matrix of the right size 
    //discards node 0
    int Mat_size = input_circuit.get_number_of_nodes() - 1;     
    Eigen::MatrixXd Mat(Mat_size,Mat_size);

    //Fills up the matrix with 0s
    Mat.Zero(Mat_size,Mat_size);


    //nodes needed to set up the KCL equations
    std::vector<Node> nodes = input_circuit.get_nodes();

    //initializing each element of the conductance matrix
    for (int row = 0; row < Mat_size; row++)
    {
        int current_node_index = row +1;
        
        for(Component current_componet: nodes[current_node_index].components_attached)  //vector<vector<components>> would be more natural
        {
            if(current_componet.get_type() == 'r')
            {
                if(current_componet.get_anode() == current_node_index)
                {
                    Mat(row,current_componet.get_anode())+=current_componet.get_con
                }

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
    Circuit input_circuit = parse_input(argv[1]);
}

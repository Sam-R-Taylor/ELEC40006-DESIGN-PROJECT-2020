#include "Circuit.hpp"
#include "Component.hpp"
#include "Parser.hpp"
#include<Eigen/Dense>






















Eigen::VectorXd Matrix_solver(const Circuit& input_circuit)
{   
    //initializing a matrix of the right size 
    int Mat_size = input_circuit.get_number_of_nodes();     
    Eigen::MatrixXd Mat(Mat_size,Mat_size);

    //initializing a vector of the right size
    Eigen::VectorXd Vec(Mat_size);

    //Fills up the matrix with 0s
    Mat.Zero(Mat_size,Mat_size);

    //Fills up the vector with 0s
    Vec.Zero();


    //nodes needed to set up the KCL equations
    std::vector<Node> nodes = input_circuit.get_nodes();

    //initializing each element of the conductance matrix
    //sum of currents out of a node = 0
    for (int row = 0; row < Mat_size; row++)
    {
        int node_index = row;

        //iterating through each componenet of the node considered
        for(Component* component_attached: nodes[node_index].components_attached)
        {
            //checking type of each component and act accordingly
            if(dynamic_cast<Resistor*>(component_attached))
            {
                int anode = component_attached->get_anode();
                int cathode = component_attached->get_cathode();
                Resistor* Rptr = dynamic_cast<Resistor*>(component_attached);

                if(anode == node_index)
                {
                    Mat(node_index,anode)+= Rptr->get_conductance();
                    Mat(node_index,cathode)-= Rptr->get_conductance();
                }
                else if (cathode == node_index)
                {
                    Mat(node_index,cathode)+= Rptr->get_conductance();
                    Mat(node_index,anode)-= Rptr->get_conductance();
                }
            }
            if(dynamic_cast<Current_source*>(component_attached))
            {
                int anode = component_attached->get_anode();
                int cathode = component_attached->get_cathode();
                Current_source* Currentptr = dynamic_cast<Current_source*>(component_attached);

                if(anode == node_index)
                {
                    Vec(node_index) += Currentptr->get_current();
                }
                else if (cathode == node_index)
                {
                    Vec(node_index) -= Currentptr->get_current();
                }

            }
        }
        
    }
    

}











int main(int argc, char const *argv[])
{   
    Circuit input_circuit = parse_input(argv[1]);
}
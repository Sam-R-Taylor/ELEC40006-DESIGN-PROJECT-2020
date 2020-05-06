#include <vector>
#include <iostream>
#include <Eigen/Dense>
using namespace std;
using Eigen::MatrixXd;

//takes a vector of an object (ie nodes)
//the object type named here as Data must have a method get_data that outputs the 
//constants of a line of a matrix in the form {a,b,c, ... ,d} such that
//ax1 + bx2 + cx3 .... + d = 0
//the matrix outputs the solution to the matrix equation in a vector float
template<typename Data>
vector<double> MatrixSolver(vector<Data> input){
    //create a matrix with the correct dimensions
    MatrixXd matrix(input.size(),input.size());
    //create vectors for the left and right hand side of the equation
    MatrixXd result(input.size(),1);
    MatrixXd constants(input.size(),1);
    //iterate through all the inputs
    for(int i=0; i < input.size(); i++){
        //for each input extract the row data
        vector<double> row = input[i].get_data();
        //fill the corresponding matrix row with the input data
        for(int j=0; j<input.size(); j++){

            matrix(i,j) = row[j];
        }
        //add the final input data value to the constants, make negative 
        //as moved to other side of equation
        constants(i) = -row[input.size()];
    }
    //invert matrix and solve equation
    matrix = matrix.inverse();
    result = matrix * constants;
    //convert the output matrix to vector format
    vector<double> output;
    for(int i=0; i < input.size(); i++){
        output.push_back(result(i));
    }
    //return output
    return output;
}


//Test Code
/*
struct TestData{
    vector<double> data;
    vector<double> get_data(){
        return data;
    }
};

int main(){
    TestData row1{{1,2,3,-4}};
    TestData row2{{3,2,5,-4}};
    TestData row3{{7,2,3,-4}};
    vector<TestData> vec{row1,row2,row3};
    vector<double> output = MatrixSolver(vec);
    cout << output[2];
    //MatrixXd m(2,2);
    //m(0,0) = 0;
    ///cout << m;
}
*/

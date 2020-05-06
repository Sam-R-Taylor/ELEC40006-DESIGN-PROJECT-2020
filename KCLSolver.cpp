#include <vector>
#include <iostream>
#include <Eigen/Dense>
#include <string>
using namespace std;
using Eigen::MatrixXd;
//to compile use g++ -I eigen3 KCLSolver.cpp -o ...

struct Component{
    int anode;
    int cathode;
    string name;
    double value;
    char type;
    char get_type() {
        return type;
    }
    int get_anode(){
        return anode;
    }
    int get_cathode(){
        return cathode;
    }
    double get_value(){
        return value;
    }
    string get_name(){
        return name;
    }
};
//template<typename Component>
struct Node{
        double voltage = 0;
        int index = 0;
        vector<Component> components;
        void set_index(int _index){index = _index;}
        int get_index(){return index;}
        double get_voltage(){return voltage;}
        void set_voltage(double _voltage){voltage = _voltage;}
        void add_component(Component component){
            components.push_back(component);
        }
        vector<Component> get_components(){
            return components;
        }
/*
        vector<double> coefficient_generator(vector<Node> nodes){
            vector<double> coeffiecients(nodes.size() + 1,0);
            for(Component component: components){
                vector<double> sub_coeffiecients(nodes.size() + 1,0);
                if(component.get_type() == 'R'){
                    if(component.get_anode() == index){
                        sub_coeffiecients[component.get_anode()] = 1/component.get_value();
                        sub_coeffiecients[component.get_cathode()] = -1/component.get_value();
                    }else{
                        sub_coeffiecients[component.get_anode()] = -1/component.get_value();
                        sub_coeffiecients[component.get_cathode()] = 1/component.get_value();
                    }
                }
                if(component.get_type() == 'C'){
                    sub_coeffiecients[nodes.size()] = component.value * component.get_anode() == index?1:-1;
                }
                if(component.get_type() == 'V'){
                    if(component.get_anode() == index){
                        sub_coeffiecients == 
                        nodes[component.get_cathode()].coefficient_generator(this, nodes);
                        sub_coeffiecients[index] = sub_coeffiecients[component.get_cathode()];
                        sub_coeffiecients[nodes.size()] = sub_coeffiecients[component.get_cathode()] * -component.value;
                        sub_coeffiecients[component.get_cathode()] = 0;
                    }else{
                        sub_coeffiecients == 
                        nodes[component.get_anode()].coefficient_generator(this, nodes);
                        sub_coeffiecients[index] = sub_coeffiecients[component.get_anode()];
                        sub_coeffiecients[nodes.size()] = sub_coeffiecients[component.get_anode()] * component.value;
                        sub_coeffiecients[component.get_anode()] = 0;
                    }
                }
                for(int i=0; i < coeffiecients.size(); i++){
                    coeffiecients[i] += sub_coeffiecients[i];
                }
                
            }
            coeffiecients[0] = 0;
            return coeffiecients;
        }
        vector<double> coefficient_generator(Node *source_node, vector<Node> nodes){
            vector<double> coeffiecients(nodes.size() + 1,0);
            for(Component component: components){
                if(component.get_anode() != source_node->get_index() && component.get_cathode() != source_node->get_index()){
                    vector<double> sub_coeffiecients(nodes.size() + 1,0);
                    if(component.get_type() == 'R'){
                        if(component.get_anode() == index){
                            sub_coeffiecients[component.get_anode()] = 1/component.get_value();
                            sub_coeffiecients[component.get_cathode()] = -1/component.get_value();
                        }else{
                            sub_coeffiecients[component.get_anode()] = -1/component.get_value();
                            sub_coeffiecients[component.get_cathode()] = 1/component.get_value();
                        }
                    }
                    if(component.get_type() == 'C'){
                        sub_coeffiecients[nodes.size()] = component.value * component.get_anode() == index?1:-1;
                    }
                    if(component.get_type() == 'V'){
                        if(component.get_anode() == index){
                            sub_coeffiecients == 
                            nodes[component.get_cathode()].coefficient_generator(this, nodes);
                            sub_coeffiecients[index] = sub_coeffiecients[component.get_cathode()];
                            sub_coeffiecients[nodes.size()] = sub_coeffiecients[component.get_cathode()] * -component.value;
                            sub_coeffiecients[component.get_cathode()] = 0;
                        }else{
                            sub_coeffiecients == 
                            nodes[component.get_anode()].coefficient_generator(this, nodes);
                            sub_coeffiecients[index] = sub_coeffiecients[component.get_anode()];
                            sub_coeffiecients[nodes.size()] = sub_coeffiecients[component.get_anode()] * component.value;
                            sub_coeffiecients[component.get_anode()] = 0;
                        }
                    }
                    for(int i=0; i < coeffiecients.size(); i++){
                        coeffiecients[i] += sub_coeffiecients[i];
                    }
                }
            }
            return coeffiecients;
        }*/
};

vector<double> coefficient_generator(Node *node, Component *source_component, vector<Node> nodes){
    //cout<<"voltage source " << node->get_index() << " " << source_node->get_index()<<endl;
    vector<double> coeffiecients(nodes.size() + 1,0);
    for(Component component: node->get_components()){
        if(component.get_name() != source_component->get_name()){
            vector<double> sub_coeffiecients(nodes.size() + 1,0);
            if(component.get_type() == 'R'){
                if(component.get_anode() == node->get_index()){
                    sub_coeffiecients[component.get_anode()] += 1/component.get_value();
                    sub_coeffiecients[component.get_cathode()] += -1/component.get_value();
                }
                if(component.get_cathode() == node->get_index()){
                    sub_coeffiecients[component.get_anode()] += -1/component.get_value();
                    sub_coeffiecients[component.get_cathode()] += 1/component.get_value();
                }
                //cout << "sub resistor" << sub_coeffiecients[node->get_index()]<< endl;
            }
            if(component.get_type() == 'C'){
                sub_coeffiecients[nodes.size()] = component.value * (component.get_anode() == node->get_index()?-1:1);
            }
            if(component.get_type() == 'V'){
                if(component.get_anode() == node->get_index()){
                    sub_coeffiecients = 
                    coefficient_generator(&nodes[component.get_cathode()],&component, nodes);
                    sub_coeffiecients[node->get_index()] += sub_coeffiecients[component.get_cathode()];
                    sub_coeffiecients[nodes.size()] = sub_coeffiecients[component.get_cathode()] * component.get_value() *-1;
                    sub_coeffiecients[component.get_cathode()] = 0;
                }else{
                    sub_coeffiecients = 
                    coefficient_generator(&nodes[component.get_anode()],&component, nodes);
                    sub_coeffiecients[node->get_index()] += sub_coeffiecients[component.get_anode()];
                    sub_coeffiecients[nodes.size()] = sub_coeffiecients[component.get_anode()] * component.get_value();
                    sub_coeffiecients[component.get_anode()] = 0;
                }

                //cout << "sub voltage" << sub_coeffiecients[node->get_index()]<< endl;
            }
            for(int i=0; i < coeffiecients.size(); i++){
                coeffiecients[i] += sub_coeffiecients[i];
            }
        }
    }
    //cout << "sub " << coeffiecients[node->get_index()]<< endl;
    //cout <<"supernode ";
    //for(double val: coeffiecients){
    //    cout <<val<<" ";
    //}
    cout << endl;
    return coeffiecients;
}
vector<double> coefficient_generator(Node *node, vector<Node> nodes){
            vector<double> coeffiecients(nodes.size() + 1,0);
            for(Component component: node->get_components()){
                vector<double> sub_coeffiecients(nodes.size() + 1,0);
                if(component.get_type() == 'R'){
                    //cout << "anode " << component.get_anode() << endl;
                //cout << "cathode " << component.get_cathode() << endl;
                //cout << "index " << node->get_index() << endl;
                    if(component.get_anode() == node->get_index()){
                        sub_coeffiecients[component.get_anode()] = 1/component.get_value();
                        sub_coeffiecients[component.get_cathode()] = -1/component.get_value();
                    }else{
                        sub_coeffiecients[component.get_anode()] = -1/component.get_value();
                        sub_coeffiecients[component.get_cathode()] = 1/component.get_value();
                    }
                }
                if(component.get_type() == 'C'){
                    sub_coeffiecients[nodes.size()] = component.value * (component.get_anode() == node->get_index()?-1:1);
                }
                if(component.get_type() == 'V'){
                    //cout << "Voltage" <<endl;
                    if(component.get_anode() == node->get_index()){
                        sub_coeffiecients = 
                        coefficient_generator(&nodes[component.get_cathode()],&component, nodes);
                        /*for(double c: sub_coeffiecients){
                            cout << c << " ";
                        }*/
                        //cout << "Voltage anode " << coefficient_generator(&nodes[component.get_cathode()],node, nodes)[component.get_cathode()] << " "<< node->get_index() <<endl;
                        sub_coeffiecients[node->get_index()] += sub_coeffiecients[component.get_cathode()];
                        sub_coeffiecients[nodes.size()] += sub_coeffiecients[component.get_cathode()] * component.get_value() * -1;
                        //cout << "Voltage anode " << sub_coeffiecients[component.get_cathode()] << " "<< node->get_index() <<endl;
                        //cout << "index1 "<<component.get_cathode() <<endl;
                        //cout << "index2 "<<nodes[component.get_cathode()].get_index() <<endl;
                        sub_coeffiecients[component.get_cathode()] = 0;
                        
                    }else{
                        sub_coeffiecients = 
                        coefficient_generator(&nodes[component.get_anode()],&component, nodes);
                        sub_coeffiecients[node->get_index()] += sub_coeffiecients[component.get_anode()];
                        sub_coeffiecients[nodes.size()] += sub_coeffiecients[component.get_anode()] * component.get_value();
                        sub_coeffiecients[component.get_anode()] = 0;
                        //cout << "Voltage cathode " << sub_coeffiecients[nodes.size()] << node->get_index() << endl;
                    }
                    //cout <<"voltage node ";
                    //for(double val: sub_coeffiecients){
                    //    cout <<val<<" ";
                    //}
                    //cout <<endl;
                }
                for(int i=0; i < coeffiecients.size(); i++){
                    //cout << sub_coeffiecients[i] << " " << i << component.get_type()<< endl;
                    coeffiecients[i] += sub_coeffiecients[i];
                }
                
            }
            //cout <<"voltage total ";
                    //for(double val: coeffiecients){
                    //    cout <<val<<" ";
                    //}
                    //cout <<endl;
            //coeffiecients[0] = 0;
            return coeffiecients;
}



//takes a vector of objects (should be components)
//outputs a vector of objects (should be nodes)
//each node has a correct vector of components
//template<typename Component> 
vector<Node> NodeGenerator(vector<Component> components){
    vector<Node> Nodes;
    //iterate through the components
    int index = 0;
    for(Component component: components){
        
        //add the components to the nodes it is attached to
        while(Nodes.size()<=component.get_cathode()){
            Node node;
            node.set_index(index);
            Nodes.push_back(node);
            index++;
        }
        while(Nodes.size()<=component.get_anode()){
            Node node;
            node.set_index(index);
            Nodes.push_back(node);
            index++;
        }
        //cout << component.get_type() << " " << component.get_cathode()<< " " << component.get_anode() << endl;
        Nodes[component.get_cathode()].add_component(component);
        Nodes[component.get_anode()].add_component(component);
    }
    //return the nodes vector
    /*for(Node node: Nodes){
        cout << node.get_index() << " index"<<endl;
    }*/
    return Nodes;
}

//takes a vector of an object (ie nodes)
//the object type named here as Data must have a method get_data that outputs the 
//constants of a line of a matrix in the form {a,b,c, ... ,d} such that
//ax1 + bx2 + cx3 .... + d = 0
//the matrix outputs the solution to the matrix equation in a vector float
//template<typename Node>
vector<double> MatrixSolver(vector<Node> input){
    int ground_node = 0;
    //create a matrix with the correct dimensions
    MatrixXd matrix(input.size()-1,input.size()-1);
    //create vectors for the left and right hand side of the equation
    MatrixXd result(input.size()-1,1);
    MatrixXd constants(input.size()-1,1);
    //iterate through all the inputs
    for(int i=0; i < input.size()-1; i++){
            //for each input extract the row data
            //cout << i << " "<< input[i+1].get_components().size() << "size" <<endl;
            vector<double> row = coefficient_generator(&input[i+1],input);
            //for(double val: row){
            //    cout << val << " ";
            //}cout<< " index" << i+1 <<endl;
            //fill the corresponding matrix row with the input data
            for(int j=1; j<input.size(); j++){
                matrix(i,j-1) = row[j];
            }
            //add the final input data value to the constants, make negative 
            //as moved to other side of equation
            constants(i) = -row[input.size()];
    }
    //invert matrix and solve equation
    cout << matrix << endl;
    matrix = matrix.inverse();
    result = matrix * constants;
    
    cout << constants << endl;
    cout << result << endl;
    //convert the output matrix to vector format
    vector<double> output;
    output.push_back(0);
    for(int i=0; i < input.size() -1; i++){
            output.push_back(result(i));
    }
    //return output
    return output;
}


//Test Code 1
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




int main(){
    //Component v{0,1,"V",3,'V'};
    //Component r1{1,2,"R1",2,'R'};
    //Component r2{2,0,"R2",1,'R'};
    Component v1{1,0,"V1",4,'V'};
    Component r1{1,0,"R1",1,'R'};
    Component v2{2,1,"V2",5,'V'};
    Component r2{2,3,"R2",2,'R'};
    Component r3{3,0,"R3",3,'R'};
    vector<Component> components{r1,r3,r2,v2,v1};
    //vector<double> values = MatrixSolver(NodeGenerator(components));

    Component c1{1,0,"C1",1,'C'};
    Component r4{2,0,"R4",1,'R'};
    Component v3{2,1,"V2",1,'V'};
    vector<Component> components2{c1,r4,v3};
    vector<double> values2 = MatrixSolver(NodeGenerator(components2));
    //cout << values[1];
    //cout << nodes.size();
}


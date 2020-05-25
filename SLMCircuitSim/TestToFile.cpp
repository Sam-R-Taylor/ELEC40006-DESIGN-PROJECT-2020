#include <vector> 
#include <fstream>
#include <iostream>
using namespace std; 

void NodeVoltagesToFile(vector<double> CKTIn2 , double CurrentTime){
    
  fstream myfile;
  myfile.open("output.txt",fstream::app);
  
  if (myfile.is_open())
  {
        myfile << CurrentTime << "," ;
    for(int i = 0; i < CKTIn2.size(); i++) {
        myfile << CKTIn2.at(i) << ",";
    }
    //COULD NOT REMOVE THE TRAILING , HOP
    myfile << "\n";
    myfile.close();
  }
  else cout << "Unable to open file";
    

}

int main(){
    vector<double> vect;
    vect.push_back(10); 
    vect.push_back(20); 
    vect.push_back(30); 
    vector<double> vect1;
    vect1.push_back(100); 
    vect1.push_back(200); 
    vect1.push_back(302); 

    remove("output.txt");
    fstream myfile ("output.txt");
    
    double CurrentTime = 0;
    double TimeStep = 1;
    double TimePeriod = 10;
    
    do {
        NodeVoltagesToFile(vect,CurrentTime);
        CurrentTime = CurrentTime + TimeStep ;

    }
    while (CurrentTime <= TimePeriod);

}
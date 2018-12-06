#include <sstream>
#include "Chromatin.h"

using namespace std;

void Chromatin::import_config(string config_file_name){
    
    map<string, double>::iterator it;
    string resume_file_name;
    ifstream read_config_file(config_file_name.c_str());
    bool resume=false;
    
    if (read_config_file.is_open()) {
        cout<<"'"<<config_file_name<<"' file opened successfully.\n";
        string line;
        int line_num=0;
        string comment="//";
        //        char delimiter=' ';
        while(getline(read_config_file, line)){
            line_num++;
            if(line.empty()){
                continue;
            }
            
            istringstream iss(line);
            vector<string> split(istream_iterator<string>{iss}, istream_iterator<string>());
            
            if (split[0] == comment || (split[0][0]=='/' && split[0][1]=='/')) {
                break;
            }
            
            param_map[split[0]]=stod(split[1]);
            
            if (split[0]=="Resume") {
        
                if (stoi(split[1])==0) {
                    cout<<"Resume flag off. The Chromatins will be initiated using the config parameters.\n";
                } else {
                    resume=true;
                    resume_file_name=split[2];
                    cout<<"Resume flag on. Chromatin will resume using the '"<<resume_file_name<<" file.\n";
                }
            } else {
                set_map_parameter(split[0], param_map[split[0]]);

            }
            
        }//End of while(getline(read_config_file, line))
        
        
    } else {//End of if (read_config_file.is_open())
        cout<<"Couldn't open the '"<<config_file_name<<"' file.\n";
        exit(EXIT_FAILURE);
    }
    if (resume) {
        import(resume_file_name);
    } else {
        if (Num_of_Nodes==0) {
            cout<< "Error. \nPlease specify the number of Chromatin chain nodes in the "<<config_file_name<< " file.\n";
            exit(EXIT_FAILURE);
        }
        initialise();
    }
//        else {
//            cout<<"Resume is off and no meshfile name is provided for initilisation. Please check the membrane config file.\n";
//        }
}


void Chromatin::set_map_parameter(string param_name, double param_value){
    
//    map<string, double>::iterator it;
    if (param_name=="Node_Mass") {
        Node_Mass=param_value;
    } else if (param_name=="Node_radius"){
        Node_radius=param_value;
    } else if (param_name=="spring_model"){
        spring_model=param_value;
    } else if (param_name=="Spring_coefficient"){
        Spring_coefficient=param_value;
    } else if (param_name=="Damping_coefficient"){
        Damping_coefficient=param_value;
    } else if (param_name=="Shift_in_X_direction"){
        Shift_in_X_direction=param_value;
    } else if (param_name=="Shift_in_Y_direction"){
        Shift_in_Y_direction=param_value;
    } else if (param_name=="Shift_in_Z_direction"){
        Shift_in_Z_direction=param_value;
    } else if (param_name=="Num_of_Nodes" && Num_of_Nodes==0){
        Num_of_Nodes=param_value;
    }
}

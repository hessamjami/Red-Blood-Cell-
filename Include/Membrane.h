/**
 * \class Membrane
 *
 * \brief The class 'Membrane' will build the required inviroment for simulating a triangular mesh of points in space bound by a potential suitable to simulate the behaviour of a cell membrane.
 *
 * This class is meant as an example.  It is not useful by itself
 * rather its usefulness is only a function of how much it helps
 * the reader.  It is in a sense defined by the person who reads it
 * and otherwise does not exist in any real form.
 *
 * \note Attempts at zen rarely work.
 *
 *
 * \version $Revision: 0.0 $
 *
 *
 * Contact: a.farnudi@gmail.com
 *
 */

#ifndef MEMBRANE_H
#define MEMBRANE_H
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <map>
#include <iomanip>
#include <iterator>

#include "General_functions.hpp"

using namespace std;


class Membrane
{
    
private:
    int index;
    /*variables*/
    int Num_of_Nodes;
    /*constants*/
    //This is the number of nodes on the membrane (Both the outer membrane and the Nucleus). This is the first number that appears in the 'membrane' file (once opend with a text editor)
    int Num_of_Triangles; //This is the number of triangles on the membrane (Both the outer membrane and the Nucleus). This is the number that appears in the 'membrane' file after the node position list is finished and before Gmesh lists the nodes that make a triangle.
    map<string, double> param_map;
    
    bool Relaxation=false;
    bool Relax_with_actin=false;
    
    double ECM_interaction_cut_off=0;
    
    double Node_Mass=1.0;//  also use in MD loop and should not be private unless we write some functions to get it outside the class
    double Total_Potential_Energy;
    double Total_Kinetic_Energy;
    double Radius=0;
    double Node_radius=1;
    double COM_velocity[3]={0};
    double COM_position[3]={0};
    
    int Num_of_Node_Pairs; //??? (This variable should be defined and explained)
    int Num_of_Triangle_Pairs;
    double X_in=0;
    double Y_in=0;
    double Z_in=0;
    double X_scale=0;
    double Y_scale=0;
    double Z_scale=0;
    
    double ECM_interaction_strength=1;
    
    
    double Average_Node_Distance();
    void read_gmesh_file (string gmesh_file);
    void read_membrabe_input(string input_file);
    void Triangle_pair_counter ();
    void Normal_direction_Identifier();
    //    void Normal_direction_Identifier(double x, double y, double z);
    
    void log_barrier (void);
    void Hookian (void);
    void FENE (void);
    void Relaxation_potential(void);
    void Node_Bonds_identifier(void);
    void Triangle_pair_identifier(void);
    void Bending_potetial(void);
    //    void Bending_potetial_2(void);
    void Bending_potetial_2(double theta_0);
    void check(void);
    void calculate_mesh_properties(void);
    void node_distance_correction(void);
    void export_relaxed(int MD_step);
    
public: //these are using in monte carlo flip function. for defining them as private variables, we have tow ways: defining monte_carlo_flip as a member of this class or writing some functions to make them accessible out of membrane class.
    
    
    
    
//    int membrane_counter;
    
    double min_radius_after_relaxation;
    
    string output_file_neme;
    string file_time;
    
    vector<vector<double> >Node_Position;
    vector<vector<int> > Triangle_list;
    vector<vector<int> > Triangle_pair_list;
    //vector<vector<int> > Membrane_Node_Pair_list;
    vector<vector<int> > Node_Bond_list;// this variable is  the same as Membrane_Node_pair_list. I think  the name "Membrane_Edges" is less confusing. and also we fill it in a different way.
    vector<vector<int> > Triangle_Pair_Nodes;
    vector<vector<double> > Node_Velocity;// also update in MD loop and should not be private unless we write some functions to get it outside the class
    vector<vector<double> > Node_Force;// also update in MD loop and should not be private unless we write some functions to get it outside the class
    vector<vector<int> > Node_neighbour_list;
    vector<vector<int> > ECM_Node_neighbour_list;
    
    
    void Triangle_Pair_and_Node_Bonds_Identifier(); //I guess this will use in MD loop and thus it should define as a public membere of class.
    //int Membrane_num_of_Node_Pair_Counter();// Hoda: no need to this function after modifying Membrane_Triangle_Pair_and_Edges_Identifier
    //void Membrane_num_of_Node_Pair_Counter_2();//Hoda: no need to this function after modifying Membrane_Triangle_Pair_and_Edges_Identifier
    void Elastic_Force_Calculator(double theta_0);
    void MD_Evolution_beginning (double MD_Time_Step);
    void MD_Evolution_end (double MD_Time_Step);
    void ConstantSurfaceForceLocalTriangles ();
    void Node_neighbour_list_constructor();
    void export_for_resume(int MD_step);
//    void initialise(string input_file_name , string Mesh_file_name);
    void initialise(string Mesh_file_name);
//    void initialise(string Mesh_file_name, double x, double y, double z);
    void import(string import_file_name);
    void import_config(string config_file_name);
    void set_map_parameter(string param_name, double param_value);
    void generate_report(void);
    void export_velocities(int MD_step);
    void Thermostat_2(double MD_KT);
    void Thermostat_N6(double MD_KT);
    void Thermostat_Bussi(double MD_KT);
    
    void relaxation_traj (void);
    void write_traj (string traj_name, string label);
    void write_parameters(int MD_Step);
    void omega_calculator(void);
    void omega_calculator_2(void);
    void equilibrate (void);
    void write_pov_traj(string traj_name, string label, int currentstep);
    double Average_velocity();
    double Omega[3]={0};
    
    
    
    int **Normal_direction; //??? (These 2 elements should be defined and explained)
    int spring_model=2;
    
    
//    vector <double> T_Kinetic_Energy;
    double Total_potential_Energy=0.0;
    double Spring_coefficient=10.0; // streching constant
    double Bending_coefficient=20.0*GenConst::MD_T*GenConst::K; // bending constant
    double Damping_coefficient=0.0; // Viscosity of the Mmmbrane. It is applied in Force calculation for the Membrane Node pairs. I have commented out these parts in the 'Membrane_Force_Calculator' because I think the current code does not need it (some energy consuming array calculations were invloved).
    double K_surfaceConstant_local=100.0;
    double Spring_force_cutt_off=1000.0;
    double Shift_in_X_direction=0.0; //???
    double Shift_in_Z_direction=0.0; //???
    double Shift_in_Y_direction=0.0; //???
    double Downward_speed=0.0; //???
    //bool =0;
    double com[3]; //center of mass
    double Min_node_pair_length, Max_node_pair_length, Average_node_pair_length;
    
    bool on_or_off_Spring_force_cutt_off=0; //??? I add it myself because virus should not have cut off
    void Relax(void);
    
    int return_num_of_nodes(void){
        return Num_of_Nodes;
    }
    void shift_position (double x, double y, double z){
        for (int i=0; i<Num_of_Nodes; i++) {
            Node_Position[i][0]+=x;
            Node_Position[i][1]+=y;
            Node_Position[i][2]+=z;
        }
    }
    void shift_velocity (double vx, double vy, double vz){
        for (int i=0; i<Num_of_Nodes; i++) {
            Node_Velocity[i][0]+=vx;
            Node_Velocity[i][1]+=vy;
            Node_Velocity[i][2]+=vz;
        }
    }
    int return_num_of_triangle(){
        return Num_of_Triangles;
    }
    
    double return_node_position(int node_number, int node_coordinate){
        return Node_Position[node_number][node_coordinate];
    }
    
    
    void calculate_average_force(void){
        double average_force_x=0, average_force_y=0, average_force_z=0;
        for(int j=0 ; j<Num_of_Nodes ; j++){
            average_force_x+=Node_Force[j][0];
            average_force_y+=Node_Force[j][1];
            average_force_z+=Node_Force[j][2];
            
        }
        cout<<"\n\naverage_force_x="<<average_force_x/Num_of_Nodes<<"\naverage_force_y="<<average_force_y/Num_of_Nodes<<"\naverage_force_z="<<average_force_z/Num_of_Nodes<<endl;
    }
    void set_file_time(char* buffer){
        file_time=buffer;
    }
    void set_index(int ind){
        index=ind;
    }
    double return_node_radius(void){
        return Node_radius;
    }
    double return_ECM_interaction_cut_off(void){
        return ECM_interaction_cut_off;
    }
    double return_ECM_interaction_strength(void){
        return ECM_interaction_strength;
    }
    
    void add_to_force(double force,int index, int coor){
        Node_Force[index][coor]+=force;
    }
    void update_COM_velocity(void){
        COM_velocity[0]=0;
        COM_velocity[1]=0;
        COM_velocity[2]=0;
        for (int i=0; i<Num_of_Nodes; i++) {
            COM_velocity[0]+=Node_Velocity[i][0];
            COM_velocity[1]+=Node_Velocity[i][1];
            COM_velocity[2]+=Node_Velocity[i][2];
        }
        COM_velocity[0]/=Num_of_Nodes;
        COM_velocity[2]/=Num_of_Nodes;
        COM_velocity[1]/=Num_of_Nodes;
    }
    void update_COM_position(void){
        COM_position[0]=0;
        COM_position[1]=0;
        COM_position[2]=0;
        for (int i=0; i<Num_of_Nodes; i++) {
            COM_position[0]+=Node_Position[i][0];
            COM_position[1]+=Node_Position[i][1];
            COM_position[2]+=Node_Position[i][2];
        }
        COM_position[0]/=Num_of_Nodes;
        COM_position[2]/=Num_of_Nodes;
        COM_position[1]/=Num_of_Nodes;
    }
    double return_min_radius_after_relaxation(void){
        return min_radius_after_relaxation;
    }
    bool return_relax_with_actin_flag(void){
        return Relax_with_actin;
    }
};

#endif // MEMBRANE_H


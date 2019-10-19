#include "Membrane.h"
#include "General_functions.hpp"
#include "OpenMM_structs.h"
#include "OpenMM_funcs.hpp"
#include <vector>

void myStepWithOpenMM(MyOpenMMData* omm,TimeDependantData* time_dependant_data, MyAtomInfo atoms[], int numSteps , int& total_step) {
    if(time_dependant_data->Kelvin_Voigt)
    {
        for (int i=0; i<numSteps; i++)
        {
            if(time_dependant_data->Kelvin_Voigt)
            {
                if (total_step % time_dependant_data->Kelvin_stepnum ==0)
                {
                    Cheap_GetOpenMMState(omm,atoms);
                    time_dependant_data->Kelvin_dist_calc(atoms);
                    if(time_dependant_data->Kelvin_Voigt_distInAng.size()>1)
                    {
                        //mys_state_update
                        Kelvin_Voigt_update(omm,time_dependant_data);
                        time_dependant_data->Kelvin_Voigt_distInAng.erase(time_dependant_data->Kelvin_Voigt_distInAng.begin());
                    }
                }
            }
            omm->integrator->step(1);
            total_step++;
        }
        
    }
    
    else
    {
        omm->integrator->step(numSteps);
        total_step += numSteps;
    }
    
}

void myTerminateOpenMM(MyOpenMMData* omm, TimeDependantData* time_dependant_data) {
    delete omm;
    delete time_dependant_data;
}

using OpenMM::Vec3;
void myGetOpenMMState(MyOpenMMData* omm,
                      bool wantEnergy,
                      bool wantForce,
                      double& timeInPs,
                      double& energyInKcal,
                      double& potential_energyInKcal,
                      MyAtomInfo atoms[])
{
    int infoMask = 0;
    infoMask = OpenMM::State::Positions;
    infoMask += OpenMM::State::Velocities;  // for kinetic energy (cheap)
    if (wantEnergy) {
        infoMask += OpenMM::State::Energy;     // for pot. energy (expensive)
    }
    if (wantForce) {
        infoMask += OpenMM::State::Forces;
    }
    // Forces are also available (and cheap).

    const OpenMM::State state = omm->context->getState(infoMask);
    timeInPs = state.getTime(); // OpenMM time is in ps already
    
    // Copy OpenMM positions into atoms array and change units from nm to Angstroms.
    const std::vector<Vec3>& positionsInNm = state.getPositions();
    const std::vector<Vec3>& velInAngperPs  = state.getVelocities();
    const std::vector<Vec3>& Forces        = state.getForces();
    for (int i=0; i < (int)positionsInNm.size(); ++i){
        for (int j=0; j < 3; ++j){
            atoms[i].posInAng[j] = positionsInNm[i][j] * OpenMM::AngstromsPerNm;
            atoms[i].velocityInAngperPs[j] = velInAngperPs[i][j];
            if (wantForce) {
                atoms[i].force[j]    = Forces[i][j] * OpenMM::KcalPerKJ * OpenMM::NmPerAngstrom;
            }
        }
    }
    // If energy has been requested, obtain it and convert from kJ to kcal.
    energyInKcal = 0;
    if (wantEnergy)
    energyInKcal = (state.getPotentialEnergy() + state.getKineticEnergy())
     * OpenMM::KcalPerKJ;
        potential_energyInKcal = (state.getPotentialEnergy())
        * OpenMM::KcalPerKJ;
}


OpenMM::State getCurrentState(MyOpenMMData* omm, bool wantEnergy,
                              double& timeInPs,  bool wantforce)
{
    int infoMask = 0;
    infoMask = OpenMM::State::Positions;
    if (wantEnergy) {
        infoMask += OpenMM::State::Velocities; // for kinetic energy (cheap)
        infoMask += OpenMM::State::Energy;     // for pot. energy (expensive)
    }
    // Forces are also available (and cheap).
    if (wantforce){
        infoMask+=OpenMM::State::Forces;
    }
    OpenMM::State currentstate = omm->context->getState(infoMask);
    timeInPs = currentstate.getTime(); // OpenMM time is in ps already
    return(currentstate);
}

void setNewState(MyOpenMMData* omm, bool wantEnergy,
                 double& energyInKcal,
                 MyAtomInfo atoms[], bool wantforce)
{   
    double timeInPs;
    const OpenMM::State newstate= getCurrentState(omm, wantEnergy,timeInPs,  wantforce);
    // Copy OpenMM positions into atoms array and change units from nm to Angstroms.
    const std::vector<Vec3>& positionsInNm = newstate.getPositions();
    for (int i=0; i < (int)positionsInNm.size(); ++i){
        for (int j=0; j < 3; ++j){
            atoms[i].posInAng[j] = positionsInNm[i][j] * OpenMM::AngstromsPerNm;}}
    
    // If energy has been requested, obtain it and convert from kJ to kcal.
    energyInKcal = 0;
    if (wantEnergy)
        energyInKcal = (newstate.getPotentialEnergy() + newstate.getKineticEnergy())
        * OpenMM::KcalPerKJ;
}

void          myreinitializeOpenMMState(MyOpenMMData* omm, Bonds* bonds, Dihedrals* dihedrals){
    bool preservestate=1;
    
    //OpenMM::HarmonicBondForce*   HarmonicBond = omm->system->getForce(0);
    vector<double> parameters;
    vector<double> bendingparameter ;
    vector<int>particles ={2 ,  1, 3, 0};
    parameters.resize(2);
    
    int bond_index=2;
    int particle1=1;
    int particle2=3;
    double r_rest= bonds[0].nominalLengthInAngstroms*OpenMM::NmPerAngstrom;
    double k_bond= bonds[0].stiffnessInKcalPerAngstrom4
    * OpenMM::KJPerKcal
    * OpenMM::AngstromsPerNm * OpenMM::AngstromsPerNm;
    double K_bend=dihedrals[0].bending_stiffness_value* OpenMM::KJPerKcal
    * OpenMM::AngstromsPerNm * OpenMM::AngstromsPerNm;
    parameters[0]=r_rest;
    parameters[1]=k_bond;
    bendingparameter.push_back(K_bend);
    //omm->x4harmonic->setBondParameters(bond_index,particle1, particle2, parameters);
    //Ali: talk to me before you uncomment.
//    omm->Dihedral->setBondParameters(0,particles, bendingparameter);
    double lenght= bonds[0].nominalLengthInAngstroms* OpenMM::NmPerAngstrom;
    double k= bonds[0].stiffnessInKcalPerAngstrom2* OpenMM::KJPerKcal* OpenMM::AngstromsPerNm * OpenMM::AngstromsPerNm;
    omm->harmonic->setBondParameters(bond_index,particle1, particle2, lenght, k );
    //bond_index=2;
    //particle1=1;
    //particle2=2;
    //omm->harmonic->setBondParameters(bond_index,particle1, particle2, lenght, k );
    omm->context->reinitialize(preservestate);
    
}

void Cheap_GetOpenMMState(MyOpenMMData* omm,
                          MyAtomInfo atoms[])
{
    int infoMask = 0;
    infoMask = OpenMM::State::Positions;
    const OpenMM::State state = omm->context->getState(infoMask);
    
    // Copy OpenMM positions into atoms array and change units from nm to Angstroms.
    const std::vector<Vec3>& positionsInNm = state.getPositions();
    
    for (int i=0; i < (int)positionsInNm.size(); ++i){
        for (int j=0; j < 3; ++j){
            atoms[i].posInAng[j] = positionsInNm[i][j] * OpenMM::AngstromsPerNm;
        }
    }
}

//                               PDB FILE WRITER
// Given state data, output a single frame (pdb "model") of the trajectory.
void myWritePDBFrame(int frameNum,
                     bool wantforce,
                     double timeInPs,
                     double energyInKcal,
                     const MyAtomInfo atoms[],
                     std::string traj_name)
{
    int EndOfList=-1;
    FILE* pFile;
    pFile = fopen (traj_name.c_str(),"a");
    fprintf(pFile,"MODEL     %d\n", frameNum);
    fprintf(pFile,"REMARK 250 time=%.3f ps; energy=%.3f kcal/mole\n",
            timeInPs, energyInKcal);
    int index=1;
    string hist = atoms[0].pdb;
    
    char chain[]={'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};
    
    //    double occ=1;
    for (int n=0; atoms[n].type != EndOfList; ++n){
        if (hist != atoms[n].pdb) {
            index++;
            hist = atoms[n].pdb;
        }
        fprintf(pFile,"ATOM  %5d %4s ETH %c   %4.0f %8.3f%8.3f%8.3f%6.2f%6.1f          %c\n",
                n+1,
                atoms[n].pdb,
                chain[index],
                double(index),
                atoms[n].posInAng[0],
                atoms[n].posInAng[1],
                atoms[n].posInAng[2],
                atoms[n].stretching_energy,
                atoms[n].energy,
                atoms[n].symbol);
    }
    fprintf(pFile,"ENDMDL\n");
    fclose (pFile);
}


void calc_energy(vector<Membrane>    mem,
                 MyAtomInfo          atoms[]){
    
    int node_A, node_B, node_C, node_D;
    
    double points[3][3];
    
    int mem_count=0;
    
    for (int i=0; i<mem.size(); i++) {
        for (int j=0; j<mem[i].get_num_of_nodes(); j++) {
            atoms[j+mem_count].energy = 0;
        }
        for (int j=0; j<mem[i].Triangle_Pair_Nodes.size(); j++) {
            
            double A, B, C, E, F, G;
            
            node_C=mem[i].Triangle_Pair_Nodes[j][0];
            node_A=mem[i].Triangle_Pair_Nodes[j][1];
            node_B=mem[i].Triangle_Pair_Nodes[j][2];
            node_D=mem[i].Triangle_Pair_Nodes[j][3];
            
            for (int k=0; k<3; k++) {
                points[0][k]=atoms[node_A+mem_count].posInAng[k];
                points[1][k]=atoms[node_B+mem_count].posInAng[k];
                points[2][k]=atoms[node_C+mem_count].posInAng[k];
            }
            calc_surface_coefficeints(points, A, B, C);
            
            for (int k=0; k<3; k++) {
                points[2][k]=atoms[node_D+mem_count].posInAng[k];
            }
            calc_surface_coefficeints(points, E, F, G);
            double cosine=0;
            double denominator = sqrt(A*A + B*B + C*C) * sqrt(E*E + F*F + G*G);
            if (denominator > 0.001) {
                cosine = ( A*E + B*F + C*G )/denominator;
            }
            
            
            //shift the value between 0 and 2 only for representation purposes.
            cosine += 1;
            //            double scale=5;
            atoms[node_A+mem_count].energy += cosine/2.0;
            atoms[node_B+mem_count].energy += cosine/2.0;
            //            atoms[node_C+mem_count].energy += scale*cosine;
            //            atoms[node_D+mem_count].energy += scale*cosine;
            
        }
        mem_count += mem[i].get_num_of_nodes();
    }
}

void calc_energy_2(vector<Membrane>    mem,
                   MyAtomInfo          atoms[]){
    
    int node_A, node_B, node_C, node_D;
    double deltax, deltay, deltaz, Node_distance;
    
    //    double points[3][3];
    
    int mem_count=0;
    
    for (int i=0; i<mem.size(); i++) {
        for (int j=0; j<mem[i].get_num_of_nodes(); j++) {
            atoms[j+mem_count].energy = 0;
            atoms[j+mem_count].stretching_energy = 0;
        }
        for (int j=0; j<mem[i].get_num_of_node_pairs(); j++) {
            node_B = mem[i].get_node_pair(j, 0);
            node_A = mem[i].get_node_pair(j, 1);
            
            deltax = atoms[node_A + mem_count].posInAng[0]-atoms[node_B + mem_count].posInAng[0];
            deltay = atoms[node_A + mem_count].posInAng[1]-atoms[node_B + mem_count].posInAng[1];
            deltaz = atoms[node_A + mem_count].posInAng[2]-atoms[node_B + mem_count].posInAng[2];
            
            Node_distance=deltax*deltax+deltay*deltay+deltaz*deltaz;
            
            double scale = 1;
            atoms[node_A + mem_count].stretching_energy += scale * 0.5*Node_distance;
            atoms[node_B + mem_count].stretching_energy += scale * 0.5*Node_distance;
            
        }
        
        
        for (int j=0; j<mem[i].Triangle_Pair_Nodes.size(); j++) {
            
            double AB[3], BA[3], AC[3], BD[3], ABxAC[3], BAxBD[3];
            
            node_C=mem[i].Triangle_Pair_Nodes[j][0];
            node_A=mem[i].Triangle_Pair_Nodes[j][1];
            node_B=mem[i].Triangle_Pair_Nodes[j][2];
            node_D=mem[i].Triangle_Pair_Nodes[j][3];
            
            for (int k=0; k<3; k++) {
                AB[k]=atoms[node_B+mem_count].posInAng[k]-atoms[node_A+mem_count].posInAng[k];
                BA[k]=-AB[k];
                AC[k]=atoms[node_C+mem_count].posInAng[k]-atoms[node_A+mem_count].posInAng[k];
                BD[k]=atoms[node_D+mem_count].posInAng[k]-atoms[node_B+mem_count].posInAng[k];
            }
            
            crossvector(ABxAC, AB, AC);
            crossvector(BAxBD, BA, BD);
            
            double dot =innerproduct(ABxAC, BAxBD), lABxACl=vector_length(ABxAC), lBAxBDl=vector_length(BAxBD);
            
            double cosine=(dot)/(lABxACl*lBAxBDl);
            double sine = (1.00001 - cosine)*0.5;
            
            sine = sqrt(sine);
            
            double scale=100;
            atoms[node_A+mem_count].energy += scale*sine/2;
            atoms[node_B+mem_count].energy += scale*sine/2;
            //            atoms[node_C+mem_count].energy += scale*sine/4;
            //            atoms[node_D+mem_count].energy += scale*sine/4;
        }
        mem_count += mem[i].get_num_of_nodes();
    }
}


void Kelvin_Voigt_update(MyOpenMMData* omm,
                         TimeDependantData* time_dependant_data)
{
    const int Num_Bonds = time_dependant_data->Kelvin_VoigtBond->getNumBonds();
    int atom1, atom2 ;
    double length, stiffness;
    
    for(int i=0; i<Num_Bonds ; i++)
    {
        time_dependant_data->Kelvin_VoigtBond->getBondParameters(i, atom1, atom2, length, stiffness);
        
        length = time_dependant_data->Kelvin_Voigt_initNominal_length_InNm[i] - (time_dependant_data->Kelvin_Voigt_distInAng[1][i] - time_dependant_data->Kelvin_Voigt_distInAng[0][i])*(OpenMM::NmPerAngstrom) * (time_dependant_data->Kelvin_Voigt_damp[i] * OpenMM::FsPerPs / stiffness)/(time_dependant_data->Kelvin_stepnum * GenConst::Step_Size_In_Fs) ;
        
        time_dependant_data->Kelvin_VoigtBond->setBondParameters(i, atom1, atom2, length, stiffness);
    }
    time_dependant_data->Kelvin_VoigtBond->updateParametersInContext(*omm->context);
    
}

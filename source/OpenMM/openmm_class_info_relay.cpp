#include "OpenMM_funcs.hpp"

void OpenMM_membrane_info_relay (vector<Membrane> membranes, vector<std::set<int> > &membrane_set, MyAtomInfo* all_atoms, Bonds* all_bonds, Dihedrals* all_dihedrals, int &atom_count, int &bond_count, int &dihe_count){
    for (int i=0; i<membranes.size(); i++) {
        
        //Create a set of the atom index to use for OpenMM's custom non bond interaction set.
        
        MyAtomInfo* atoms = convert_membrane_position_to_openmm(membranes[i]);
        for (int j=0;j<membranes[i].get_num_of_nodes(); j++) {
            all_atoms[j+atom_count]=atoms[j];
            membrane_set[i].insert(j+atom_count);
        }
        
        
        
        Bonds* bonds = convert_membrane_bond_info_to_openmm(membranes[i]);
        for (int j=0; j<membranes[i].get_num_of_node_pairs(); j++) {
            all_bonds[j+bond_count]=bonds[j];
            all_bonds[j+bond_count].atoms[0]=bonds[j].atoms[0]+atom_count;
            all_bonds[j+bond_count].atoms[1]=bonds[j].atoms[1]+atom_count;
            
        }
        
        
        Dihedrals* dihedrals = convert_membrane_dihedral_info_to_openmm(membranes[i]);
        for (int j=0; j<membranes[i].get_num_of_triangle_pairs(); j++) {
            all_dihedrals[j+dihe_count]=dihedrals[j];
            all_dihedrals[j+dihe_count].atoms[0]=dihedrals[j].atoms[0]+atom_count;
            all_dihedrals[j+dihe_count].atoms[1]=dihedrals[j].atoms[1]+atom_count;
            all_dihedrals[j+dihe_count].atoms[2]=dihedrals[j].atoms[2]+atom_count;
            all_dihedrals[j+dihe_count].atoms[3]=dihedrals[j].atoms[3]+atom_count;
        }
        
        //These parameters are used to shift the index of the atoms/bonds/dihedrals.
        atom_count += membranes[i].get_num_of_nodes();
        bond_count += membranes[i].get_num_of_node_pairs();
        dihe_count += membranes[i].get_num_of_triangle_pairs();
    }
}

void OpenMM_ECM_info_relay (vector<ECM> ecms, vector<std::set<int> > &ecm_set, MyAtomInfo* all_atoms, Bonds* all_bonds, Dihedrals* all_dihedrals, int &atom_count, int &bond_count, int &dihe_count){
    for (int i=0; i<ecms.size(); i++) {
        
        //Create a set of the atom index to use for OpenMM's custom non bond interaction set.
        
        MyAtomInfo* atoms = convert_ECM_position_to_openmm(ecms[i]);
        for (int j=0;j<ecms[i].get_num_of_nodes(); j++) {
            all_atoms[j+atom_count]=atoms[j];
            ecm_set[i].insert(j+atom_count);
        }
        
        
        
        Bonds* bonds = convert_ECM_bond_info_to_openmm(ecms[i]);
        for (int j=0; j<ecms[i].get_num_of_node_pairs(); j++) {
            all_bonds[j+bond_count]=bonds[j];
            all_bonds[j+bond_count].atoms[0]=bonds[j].atoms[0]+atom_count;
            all_bonds[j+bond_count].atoms[1]=bonds[j].atoms[1]+atom_count;
            
        }
        
        
//        Dihedrals* dihedrals = convert_membrane_dihedral_info_to_openmm(membranes[i]);
//        for (int j=0; j<membranes[i].get_num_of_triangle_pairs(); j++) {
//            all_dihedrals[j+dihe_count]=dihedrals[j];
//            all_dihedrals[j+dihe_count].atoms[0]=dihedrals[j].atoms[0]+atom_count;
//            all_dihedrals[j+dihe_count].atoms[1]=dihedrals[j].atoms[1]+atom_count;
//            all_dihedrals[j+dihe_count].atoms[2]=dihedrals[j].atoms[2]+atom_count;
//            all_dihedrals[j+dihe_count].atoms[3]=dihedrals[j].atoms[3]+atom_count;
//        }
        
        //These parameters are used to shift the index of the atoms/bonds/dihedrals.
        atom_count += ecms[i].get_num_of_nodes();
        bond_count += ecms[i].get_num_of_node_pairs();
//        dihe_count += membranes[i].get_num_of_triangle_pairs();
    }
}
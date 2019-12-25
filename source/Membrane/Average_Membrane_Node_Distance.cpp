#include "Membrane.h"
#include "General_functions.hpp"

double Membrane::Average_Node_Distance()
{
    double average_membrane_Node_distance=0.0;
    double temp[3];
    double length;
    int Node_A, Node_B;
    for (int i=0; i<Num_of_Triangle_Pairs; i++)
    {
        Node_A=Node_Bond_list[i][0];
        Node_B=Node_Bond_list[i][1];
        temp[0]=Node_Position[Node_A][0]-Node_Position[Node_B][0];
        temp[1]=Node_Position[Node_A][1]-Node_Position[Node_B][1];
        temp[2]=Node_Position[Node_A][2]-Node_Position[Node_B][2];
        length= vector_length(temp);
        average_membrane_Node_distance+=length;
    }
    average_membrane_Node_distance=average_membrane_Node_distance/Num_of_Triangle_Pairs;
    return(average_membrane_Node_distance);
    
}
 
void Membrane::calculate_volume(){
    volume = 0;
    
    update_COM_position();
    
    int node_A, node_B, node_C;
    double AB[3], AC[3], height[3], ABxAC[3];
    
    for(  int i=0;i<Triangle_list.size();i++)
    {
        node_A = Triangle_list[i][0];
        node_B = Triangle_list[i][1];
        node_C = Triangle_list[i][2];
        
        AB[0] = Node_Position[node_B][0] - Node_Position[node_A][0];
        AB[1] = Node_Position[node_B][1] - Node_Position[node_A][1];
        AB[2] = Node_Position[node_B][2] - Node_Position[node_A][2];
        
        AC[0] = Node_Position[node_C][0] - Node_Position[node_A][0];
        AC[1] = Node_Position[node_C][1] - Node_Position[node_A][1];
        AC[2] = Node_Position[node_C][2] - Node_Position[node_A][2];
        
        height[0] = COM_position[0] - Node_Position[node_A][0];
        height[1] = COM_position[1] - Node_Position[node_A][1];
        height[2] = COM_position[2] - Node_Position[node_A][2];
        
        crossvector(ABxAC, AB, AC);
        
        double area = vector_length(ABxAC);
        double H = abs(innerproduct(ABxAC, height)/area );
        
        volume += H*area/3.0;
        
    }
    
}

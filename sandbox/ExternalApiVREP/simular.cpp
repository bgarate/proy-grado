extern "C" {
     #include "extApi.h"
    /*#include "extApiCustom.h" if you wanna use custom remote API functions! */
}

#include <stdio.h>
#include <iostream>
using namespace std;
 
int main(int argc, char** argv)
{
   int portNb=19997;   //Se define el puerto de conexión

   //Conectar con V-REP
   int clientID = simxStart("127.0.0.1", portNb, true, true, 5000, 5) ;   

  //Si la conexión es exitosa iniciar la simulación
  if(clientID >-1)
  {
        //Iniciar siumulacion
        //simxStartSimulation(clientID,simx_opmode_oneshot);
        //getchar();
        //simxStopSimulation(clientID, simx_opmode_oneshot_wait);

        simxInt targetHandler, quadricopterHandler;
        
    
        //Obtener handler del targer
        simxInt res = simxGetObjectHandle(clientID, "Quadricopter_target",&targetHandler,simx_opmode_blocking);
        simxInt res2 = simxGetObjectHandle(clientID, "Quadricopter",&quadricopterHandler,simx_opmode_blocking);
        int i = 0;
        simxFloat delta = 0.5;
        int axis = 0;


        while (simxGetConnectionId(clientID)!=-1){

          //Cambio de direccion
          i++;
          if (i == 20){
            if (axis==0){
              axis=1;
              delta = -delta;
            } else{
              axis=0;
            }
            i=0;
          }


          //MOVIMIMIENTO 

          simxFloat * position = new simxFloat[3];
          //Obtener pos
          simxGetObjectPosition(clientID, quadricopterHandler, -1, position, simx_opmode_blocking);

          //cout << position[0] ; 
          //cout <<  "\n";
          

          //Cambiar pos
          position[axis] = position[axis] + delta;//Movimiento horizontal
          position[2] = position[2] + (delta/4);//Movimiento vertical
          simxSetObjectPosition(clientID, targetHandler, -1, position, simx_opmode_oneshot);


          //ROTACION 

          simxFloat * orientation = new simxFloat[3];
          //Obtener rot
          simxGetObjectOrientation(clientID, quadricopterHandler, -1, orientation, simx_opmode_blocking);

          //cout << orientation[0] ; 
          //cout <<  "\n";
          

          //Cambiar rot
          //orientation[2] = orientation[2] + delta;
          simxSetObjectOrientation(clientID, targetHandler, -1, orientation, simx_opmode_oneshot);



        }
  }
  simxFinish(clientID);
}
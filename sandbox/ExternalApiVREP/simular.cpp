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
        simxStartSimulation(clientID,simx_opmode_oneshot);
        //getchar();
        //simxStopSimulation(clientID, simx_opmode_oneshot_wait);

        simxInt targetHandler;
        
    
        //Obtener handler del targer
        simxInt res = simxGetObjectHandle(clientID, "Quadricopter_target",&targetHandler,simx_opmode_blocking);

        int i = 0;
        simxFloat delta = 0.05;
        int axis = 0;


        while (simxGetConnectionId(clientID)!=-1){

          //Cambio de direccion
          i++;
          if (i == 40){
            if (axis==0){
              axis=1;
              delta = -delta;
            } else{
              axis=0;
            }
            i=0;
          }


          simxFloat * position = new simxFloat[3];
          //Obtener pos
          simxGetObjectPosition(clientID, targetHandler, -1, position, simx_opmode_blocking);

          cout << position[0] ; 
          cout <<  "\n";
          

          //Cambiar pos
          position[axis] = position[axis] + delta;
          simxSetObjectPosition(clientID, targetHandler, -1, position, simx_opmode_oneshot);

        }
  }
  simxFinish(clientID);
}
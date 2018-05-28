//
// Created by santy on 03/11/17.
//

#ifndef PROY_GRADO_INTERCOMM_H
#define PROY_GRADO_INTERCOMM_H

#include "../config/Config.h"
#include "../messages/Broadcaster.h"
#include "../messages/MessageHandler.h"

class InterComm {

public:

    /**
     * Constructor
     */
    InterComm();

    /**
     * Inicializa la comunicación
     *
     * @param config conjunto de parametros de configuración del programa
     * @param active indica si difundir el estado o solo escuchar de forma pasiva
     */
    void setupInterComm(Config* config, bool active);

    /**
     * Paso de la comunicación que debe ser ejectuado en cada interación
     *
     * @param runningTime tiempo pasado desde el inicio de la ejecución
     * @param deltaTime tiempo pasado desde la anterior iteración
     */
    void interCommStep(long runningTime, long deltaTime);

    /**
     * Termina la comunicación
     */
    void shutdownInterComm();

    /**
     * Conjunto de estado de los drones mapeados por id, donde se puede setear los valores de mi estado para difundir y
     * consultar el estado de los demás drones
     */
    std::map<int, DroneState*> droneStates;

private:

    bool active;

    std::map<int, long> lastSeq;

    long seqNum;

    std::string name;
    unsigned int id;
    int broadcastPort;
    boost::asio::ip::address_v4 ip;

    MessageHandler messsageHandler;
    long lastStateSend = 0;
    Broadcaster broadcaster;
    int stateSendLapse;
    int stateExpireLapse;

    long runningTime;
    std::map<int, long> droneTimestamps;

    void copyDroneState(DroneState* orig, DroneState* copy);

    void stateHandler(Message &msg);

    void sendState();

    void handleMessages();


};


#endif //PROY_GRADO_INTERCOMM_H

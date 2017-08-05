//
// Created by santy on 25/07/17.
//

#include "MarkerLand.h"


MarkerLand::MarkerLand(){

    this->state = LandingState::Inactive;
}

LandMoveCommand MarkerLand::land(std::vector<cv::Point> points, cv::Point frameSize, double altitude){

    LandMoveCommand res;
    res.pitch = 0;
    res.roll = 0;
    res.yaw = 0;
    res.gaz = 0;
    res.land = false;

    if(abs(altitude - landAltitude) > alignmentTolerance*landAltitude){
        if(altitude > landAltitude){
            res.gaz = -this->gazPorcent;
        } else {
            res.gaz = this->gazPorcent;
        }
    }

    if(this->state == LandingState::Inactive){

        this->state = LandingState::Centring;
    } else if(this->state == LandingState::Centring){

        /*if(points.size() == 4){

            //this->state = LandingState::Rotating;
            this->state = LandingState::Centring;
        }else*/
        if (points.size() > 0){

            if(points.size()>=3){//Centrar punto medio

                //Calcular punto opuesto a [0]
                float d = 0;
                int b;
                for(int i = 0; i<3; i++){

                    cv::Point aux = points[0] - points[i];

                    if( (aux.x*aux.x+aux.y+aux.y) > d){
                        b = i;
                    }
                }

                cv::Point squareCenter( (points[0].x+points[b].x)/2, (points[0].y+points[b].y)/2);
                cv::Point frameCenter = cv::Point(frameSize.x / 2, frameSize.y / 2);

                //movimiento eje x
                res.roll = ( (squareCenter.x - frameCenter.x) / (frameSize.x/2) ) * (this->rollvelfactor);


                //movimiento eje y
                res.pitch = ( (frameCenter.y - squareCenter.y ) / (frameSize.y/2) ) * (this->pitchvelfactor);


                //movimiento de rotación
                int c = 1;
                int a = 0;
                if(b == 1)
                    c = 2;

                int ys[2];//Puntos agrupados por similitud de y de izquierda a derecha
                if(abs(points[c].y-points[a].y) < abs(points[c].y-points[b].y)){

                    if(points[a].x < points[c].x){
                        ys[0]=a;
                        ys[1]=c;
                    }else{
                        ys[0]=c;
                        ys[1]=a;
                    }
                } else {

                    if(points[b].x < points[c].x){
                        ys[0]=b;
                        ys[1]=c;
                    }else{
                        ys[0]=c;
                        ys[1]=b;
                    }
                }


                cv::Point auxpoit = points[ys[1]] - points[ys[0]];
                float l = std::sqrt(auxpoit.x*auxpoit.x + auxpoit.y*auxpoit.y);
                if(points[ys[0]].y<points[ys[1]].y){//ys[0] está mas arriba

                    //girar a la izquierda
                    res.yaw= -((points[ys[1]].y-points[ys[0]].y) / l) * (this->yawvelfactor);
                }else{//ys[1] está mas arriba

                    //girar a la derecha
                    res.yaw= ((points[ys[0]].y-points[ys[1]].y) / l) * (this->yawvelfactor);
                }


            } else if (points.size()==2){

                cv::Point frameCenter = cv::Point(frameSize.x / 2, frameSize.y / 2);

                ////Movimiento eje x
                if( points[0].x > frameCenter.x && points[1].x > frameCenter.x){

                    //Movera la derecha
                    int auxmax=0;
                    if(points[1].x>points[0].x)
                        auxmax=1;
                    res.roll = (points[auxmax].x/frameSize.x) * (this->rollvelfactor);

                } else if( points[0].x < frameCenter.x && points[1].x < frameCenter.x){

                    //mover a la izuierda
                    int auxmax=0;
                    if(points[1].x<points[0].x)
                        auxmax=1;
                    res.roll = -((frameSize.x - points[auxmax].x)/frameSize.x) * (this->rollvelfactor);
                }

                //movimiento eje y
                if( points[0].y > frameCenter.y && points[1].y > frameCenter.y){

                    //mover hacia atras
                    int auxmax=0;
                    if(points[1].y>points[0].y)
                        auxmax=1;
                    res.pitch = -(points[auxmax].y/frameSize.y) * (this->pitchvelfactor);

                } else if( points[0].y < frameCenter.y && points[1].y < frameCenter.y){

                    //mover hacia adelande
                    int auxmax=0;
                    if(points[1].y<points[0].y)
                        auxmax=1;
                    res.pitch = (frameSize.y - points[auxmax].y/frameSize.y) * (this->pitchvelfactor);
                }

            } else {//size 1

                cv::Point frameCenter = cv::Point(frameSize.x / 2,
                                                  frameSize.y / 2);

                //movimiento eje x
                if( points[0].x > frameCenter.x ){

                    //movera la derecha
                    res.roll = (points[0].x/frameSize.x) * (this->rollvelfactor);

                } else if( points[0].x < frameCenter.x ){

                    //mover a la izuierda
                    res.roll = -(frameSize.x - points[0].x/frameSize.x) * (this->rollvelfactor);
                }

                //movimiento eje y
                if( points[0].y > frameCenter.y){

                    //mover hacia atras
                    res.pitch = -(points[0].y/frameSize.y) * (this->pitchvelfactor);

                } else if( points[0].y < frameCenter.y){

                    //mover hacia adelande
                    res.pitch = (frameSize.y - points[0].y/frameSize.y) * (this->pitchvelfactor);
                }

            }


        }else {
            //Buscar Puntos in referencia: todo
        }

    } else if(this->state == LandingState::FinalPositioning){

        if(points.size() == 2){

            //ordenar por y
            int orderbyY[4] = {0,1,2,3};
            for(int i = 0; i < 4; i++){
                for (int k = i+1; k < 4; k++){
                    if(points[orderbyY[k]].y <  points[orderbyY[i]].y){
                        int aux = orderbyY[i];
                        orderbyY[i] = orderbyY[k];
                        orderbyY[k] = aux;
                    }
                }
            }

            if( abs(points[orderbyY[0]].y - (frameSize.y/2)) < frameSize.y*alignmentTolerance){

                this->state = LandingState::Inactive;
                //land
                res.land = true;
            } else {

                if(points[orderbyY[0]].y < (frameSize.y/2)){
                    //mover hacia adelante
                    res.pitch = this->pitchPorcent;
                } else {
                    //mover hacia atras
                    res.pitch = -this->pitchPorcent;
                }
            }

        }

    }
    res.state = this->state;
    return res;
}

bool MarkerLand::isLanding(){

    return  this->state != LandingState::Inactive;
}

void MarkerLand::stopLanding(){
    this->state = LandingState::Inactive;
}

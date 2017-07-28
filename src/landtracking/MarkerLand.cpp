//
// Created by santy on 25/07/17.
//

#include "MarkerLand.h"


MarkerLand::MarkerLand(){

}

void MarkerLand::land(std::vector<cv::Point> points, cv::Point frameSize){


    if(this->state == LandingState::Inactive){

        this->state = LandingState::Finding;
    } else if(this->state == LandingState::Finding){

        if(points.size() == 4){

            this->state = LandingState::Rotating;
        }else{
            //Buscar Puntos: todo
        }

    } else if(this->state == LandingState::Rotating){

        if(points.size() == 4){

            //ordenar por y
            int orderbyY[4] = {1,2,3,4};
            for(int i = 0; i < 4; i++){
                    for (int k = i+1; k < 4; k++){
                        if(points[orderbyY[k]].y <  points[orderbyY[i]].y){
                            int aux = orderbyY[i];
                            orderbyY[i] = orderbyY[k];
                            orderbyY[k] = aux;
                        }
                    }
            }

            if( (points[orderbyY[0]].y -points[orderbyY[1]].y) < frameSize.y*alignmentTolerance){//Estoy alineado

                this->state = LandingState::Centring;

            }else{//Tengo que alinear

                //order en x
                int a, b;
                if(points[orderbyY[0]].x > points[orderbyY[1]].x){
                    a=0;
                    b=1;
                }else{
                    a=1;
                    b=0;
                }

                if( points[orderbyY[a]].y > points[orderbyY[b]].y ){

                    //girar a la derecha todo
                } else {

                    // girar a la izquierda todo
                }
            }
        }

    } else if(this->state == LandingState::Centring){

        if(points.size() == 4){

            float d = 0;
            int b;
            for(int i = 0; i<4; i++){

                cv::Point aux = points[0] - points[i];

                if( (aux.x*aux.x+aux.y+aux.y) > d){
                    b = i;
                }
            }

            cv::Point squareCenter( (points[0].x+points[b].x)/2, (points[0].y+points[b].y)/2);
            cv::Point frameCenter = cv::Point(frameSize.x / 2,
                                              frameSize.y / 2);

            if( (squareCenter.x-frameCenter.x) < frameSize.x*alignmentTolerance
                    and (squareCenter.y-frameCenter.y) < frameSize.y*alignmentTolerance){

                this->state = LandingState::FinalPositioning;
            } else {

                //movimiento eje x
                if( squareCenter.x > frameCenter.x){

                    //movera la derecha todo
                } else {

                    //mover a la izuierda todo
                }

                //movimiento eje y
                if( squareCenter.y > frameCenter.x ){

                    //mover hacia adelante todo
                } else {

                    //mover hacia atras todo
                }
            }
        }

    } else if(this->state == LandingState::FinalPositioning){

        if(points.size() == 2){

            //ordenar por y
            int orderbyY[4] = {1,2,3,4};
            for(int i = 0; i < 4; i++){
                for (int k = i+1; k < 4; k++){
                    if(points[orderbyY[k]].y <  points[orderbyY[i]].y){
                        int aux = orderbyY[i];
                        orderbyY[i] = orderbyY[k];
                        orderbyY[k] = aux;
                    }
                }
            }

            if( points[orderbyY[0]].y - (frameSize.y/2) < alignmentTolerance){

                this->state = LandingState::Inactive;
                //land = ture todo
            } else {

                if(points[orderbyY[0]].y > (frameSize.y/2)){
                    //mover hacia adelante todo
                } else {
                    //mover hacia atras todo
                }
            }

        }

    }
}

bool MarkerLand::isLanding(){

    return  this->state != LandingState::Inactive;
}

void MarkerLand::stopLanding(){

}

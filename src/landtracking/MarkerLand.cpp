#include <zconf.h>
#include <src/config/ConfigKeys.h>
#include "MarkerLand.h"


MarkerLand::MarkerLand(Config* config){

    pitchvelfactor = config->Get(ConfigKeys::Land::PitchVelFactor);
    rollvelfactor = config->Get(ConfigKeys::Land::RollVelFactor);
    gazvelfactor = config->Get(ConfigKeys::Land::GazVelFactor);

    landAltitude = config->Get(ConfigKeys::Land::LandAltitude);

    pitchtolerance = config->Get(ConfigKeys::Land::PitchTolerance);
    rolltolerance = config->Get(ConfigKeys::Land::RollTolerance);

    gazpreland = config->Get(ConfigKeys::Land::GazAdjustment);
    pitchpreland = config->Get(ConfigKeys::Land::PitchAdjustment);
    rollpreland = config->Get(ConfigKeys::Land::RollAdjustment);

    withoutReferenceGaz = config->Get(ConfigKeys::Land::NoReferenceGazVel);

    moveWithoutReferenceTime = config->Get(ConfigKeys::Land::MoveWithoutRefereceTime) * 1000 * 1000;
    stabilisationTime = config->Get(ConfigKeys::Land::StabilizationTime) * 1000 * 1000;

    this->state = LandingState::Inactive;
    lastres.pitch = 0;
    lastres.roll = 0;
    lastres.yaw = 0;
    lastres.gaz = 0;
    lastres.land = false;
    preland = false;

    startTime = std::chrono::steady_clock::now();
    lastTime = startTime;
    newTime = startTime;
}

LandMoveCommand MarkerLand::land(std::vector<cv::Point> points, cv::Point frameSize, double altitude){

    lastTime = newTime;
    newTime = std::chrono::steady_clock::now();
    deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(newTime - lastTime).count();
    runningTime = std::chrono::duration_cast<std::chrono::microseconds>(newTime - startTime).count();

    LandMoveCommand res;
    res.pitch = 0;
    res.roll = 0;
    res.yaw = 0;
    res.gaz = 0;
    res.land = false;

    res.gaz = (float)(landAltitude - altitude)/landAltitude * (this->gazvelfactor);

    if(this->state == LandingState::Inactive){

        this->state = LandingState::Lost;

    } else if(this->state == LandingState::Landing){

        if(!this->preland){
            res.gaz = this->gazpreland;
            res.pitch= this->pitchpreland;
            res.roll= this->rollpreland;

            this->preland=true;
        } else{
            res.land = true;
        }

    } else {

        if (points.size() > 0){

            this->state = LandingState::Centring;
            lastReferenceTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - startTime).count();

            if(points.size()>=3){//Centrar punto medio

                //Calcular punto opuesto a [0]
                float dc01,dc02,dc12;
                cv::Point v01,v02,v12;
                int a,b,c;

                v01 = points[0] - points[1];
                dc01 = v01.x*v01.x+v01.y*v01.y;

                v02 = points[0] - points[2];
                dc02 = v02.x*v02.x+v02.y*v02.y;

                v12 = points[1] - points[2];
                dc12 = v12.x*v12.x+v12.y*v12.y;

                if(dc01> dc02 && dc01 > dc12){
                    a=0;
                    b=1;
                    c=2;
                } else if(dc02 > dc01 && dc02 > dc12){
                    a=0;
                    b=2;
                    c=1;
                } else {
                    a=1;
                    b=2;
                    c=0;
                }

                cv::Point squareCenter( (points[a].x+points[b].x)/2, (points[a].y+points[b].y)/2);
                cv::Point frameCenter = cv::Point(frameSize.x / 2, frameSize.y / 2);

                //movimiento eje x
                res.roll = ( ((float)squareCenter.x - (float)frameCenter.x) / ((float)frameSize.x/2) ) * (this->rollvelfactor);


                //movimiento eje y
                res.pitch = ( ((float)frameCenter.y - (float)squareCenter.y ) / ((float)frameSize.y/2) ) * (this->pitchvelfactor);

                //movimiento de rotación
                int ys[2];//Puntos agrupados por similitud de y de izquierda a derecha
                if(std::abs(points[c].y-points[a].y) < std::abs(points[c].y-points[b].y)){

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
                float l = (float) std::sqrt(auxpoit.x*auxpoit.x + auxpoit.y*auxpoit.y);
                if( points[ys[0]].y < points[ys[1]].y ){//ys[0] está mas arriba

                    //girar a la izquierda
                    if( (((float)points[ys[1]].y-(float)points[ys[0]].y) / l) > 0.5 ){
                        res.yaw= (((float)points[ys[1]].y-(float)points[ys[0]].y) / l) * (this->yawvelfactor);
                    } else {
                        res.yaw= -(((float)points[ys[1]].y-(float)points[ys[0]].y) / l) * (this->yawvelfactor);
                    }

                }else{//ys[1] está mas arriba

                    //girar a la derecha
                    res.yaw= (((float)points[ys[0]].y-(float)points[ys[1]].y) / l) * (this->yawvelfactor);
                }

                //NO GIRAR
                res.yaw=0.0;

                //Tengo que aterrizar?
                if(std::abs(res.pitch)<pitchtolerance &&std::abs(res.roll)<rolltolerance &&std::abs(res.yaw)<yawtolerance){

                    if(runningTime - firstAlignmentTime > stabilisationTime){
                        this->state = LandingState::Landing;
                        res.state = this->state;
                        return res;
                    }
                } else {
                    firstAlignmentTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - startTime).count();
                }


            } else if (points.size()==2){

                cv::Point frameCenter = cv::Point(frameSize.x / 2, frameSize.y / 2);

                ////Movimiento eje x
                if( points[0].x > frameCenter.x && points[1].x > frameCenter.x){

                    //Movera la derecha
                    int auxmax=0;
                    if(points[1].x>points[0].x)
                        auxmax=1;
                    res.roll = ((float)points[auxmax].x) / ((float)frameSize.x) * (this->rollvelfactor);

                } else if( points[0].x < frameCenter.x && points[1].x < frameCenter.x){

                    //mover a la izuierda
                    int auxmax=0;
                    if(points[1].x<points[0].x)
                        auxmax=1;
                    res.roll = -((float)frameSize.x - (float)points[auxmax].x) / ((float)frameSize.x) * (this->rollvelfactor);
                }

                //movimiento eje y
                if( points[0].y > frameCenter.y && points[1].y > frameCenter.y){

                    //mover hacia atras
                    int auxmax=0;
                    if(points[1].y>points[0].y)
                        auxmax=1;
                    res.pitch = -((float)points[auxmax].y)/ ((float)frameSize.y) * (this->pitchvelfactor);

                } else if( points[0].y < frameCenter.y && points[1].y < frameCenter.y){

                    //mover hacia adelande
                    int auxmax=0;
                    if(points[1].y<points[0].y)
                        auxmax=1;
                    res.pitch =  ((float)frameSize.y - (float)points[auxmax].y) / ((float)frameSize.y) * (this->pitchvelfactor);
                }

            } else {//size 1

                cv::Point frameCenter = cv::Point(frameSize.x / 2,
                                                  frameSize.y / 2);

                //movimiento eje x
                if( points[0].x > frameCenter.x ){

                    //movera la derecha
                    res.roll = ((float)points[0].x/(float)frameSize.x) * (this->rollvelfactor);

                } else if( points[0].x < frameCenter.x ){

                    //mover a la izuierda
                    res.roll = -((float)frameSize.x - (float)points[0].x )/ ((float)frameSize.x) * (this->rollvelfactor);
                }

                //movimiento eje y
                if( points[0].y > frameCenter.y){

                    //mover hacia atras
                    res.pitch = -((float)points[0].y/(float)frameSize.y) * (this->pitchvelfactor);

                } else if( points[0].y < frameCenter.y){

                    //mover hacia adelande
                    res.pitch = ((float)frameSize.y - (float)points[0].y ) / ((float)frameSize.y) * (this->pitchvelfactor);
                }

            }


        }else {
            //Buscar Puntos in referencia (sigo la ultima orden conocida)

            if(runningTime - lastReferenceTime < moveWithoutReferenceTime){
                res.pitch = lastres.pitch;
                res.roll = lastres.roll;
                res.yaw = lastres.yaw;
                res.gaz = withoutReferenceGaz;
            } else {
                this->state = LandingState::Lost;
            }

        }

    }
    res.state = this->state;
    lastres.pitch = res.pitch;
    lastres.roll = res.roll;
    lastres.yaw = res.yaw;
    lastres.gaz = res.gaz;
    return res;
}

bool MarkerLand::isLanding(){

    return  this->state != LandingState::Inactive;
}

void MarkerLand::stopLanding(){
    this->state = LandingState::Inactive;
    lastres.pitch = 0;
    lastres.roll = 0;
    lastres.yaw = 0;
    lastres.gaz = 0;
    lastres.land = false;
}

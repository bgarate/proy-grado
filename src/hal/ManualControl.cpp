//
// Created by santy on 27/05/17.
//

#include "ManualControl.h"
#include <curses.h>
#include <zconf.h>

ManualControl::ManualControl(Hal* hal){
    this->hal = hal;
    isstopped = false;
}

void ManualControl::deamon(){

    bool change = false;
    bool w = false;
    bool s = false;
    bool a = false;
    bool d = false;
    bool up = false;
    bool down = false;
    bool right = false;
    bool left = false;
    bool moving = false;

    initscr();
    clear();
    noecho();
    cbreak();
    while(!isstopped){

        timeout(500);
        int c = getch();
        //printw("You entered %d\n", c);

        w=false;
        s=false;
        a=false;
        d=false;
        up = false;
        down = false;
        right = false;
        left = false;

        switch ((char)c){
            case 'q':
                change=true;
                isstopped = true;

                break;
            case 'w':
                if(!w){
                    w=true;
                    moving = true;
                    change=true;
                }
                break;
            case 's':
                if(!s){
                    s=true;
                    moving = true;
                    change=true;
                }
                break;
            case 'a':
                if(!a){
                    a=true;
                    moving = true;
                    change=true;
                }
                break;
            case 'd':
                if(!d){
                    d=true;
                    moving = true;
                    change=true;
                }
                break;
            case '\033':
                getch(); // saltar [
                switch(getch()) { //valor real
                    case 'A':
                        //up
                        if(!up){
                            up = true;
                            moving = true;
                            change=true;
                        }

                        break;
                    case 'B':
                        //down
                        if(!down){
                            down = true;
                            moving = true;
                            change=true;
                        }

                        break;
                    case 'C':
                        //right
                        if(!right){
                            right = true;
                            moving = true;
                            change=true;
                        }

                        break;
                    case 'D':
                        //left
                        if(!left){
                            left = true;
                            moving = true;
                            change=true;
                        }

                        break;
                }
                break;
            default:
                if(moving){
                    change=true;
                }
                break;
        }

        if(change){
            change = false;
            int roll = 0;
            int pitch = 0;
            int yaw = 0;
            int gaz = 0;
            if(s){
                gaz = -50;
            }else if(w) {
                gaz = 50;
            }else if(d){
                roll = 50;
            }else if(a){
                roll = -50;
            }else if(up){
                pitch=50;
            }else if(down){
                pitch=-50;
            }else if(right){
                yaw=50;
            }else if(left){
                yaw=-50;
            }else{
                moving = false;
            }

            hal->move(roll,pitch,yaw,gaz);
        }

    }
}

void ManualControl::run(){
    this->t = new std::thread(&ManualControl::deamon, this);
}

bool ManualControl::stopped(){
    return this->isstopped;
}
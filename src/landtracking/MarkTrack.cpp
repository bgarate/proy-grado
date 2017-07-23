#include "MarkTrack.h"

MarkTrack::MarkTrack(){
    this->inicialized = false;
}

std::vector<std::vector<cv::Point>> MarkTrack::Track(std::shared_ptr<cv::Mat> frame){

    if(!this->inicialized){
        this->gray = new cv::Mat(frame->size(), CV_MAKETYPE(frame->depth(), 1));
        this->edges = new cv::Mat(frame->size(), CV_MAKETYPE(frame->depth(), 1));
    }

    cvtColor(*frame,*gray,CV_RGB2GRAY);
    Canny(*gray, *edges, 100 , 200, 3);
    findContours(*edges, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

    mark = 0;

    std::vector<cv::Moments> mu(contours.size());
    std::vector<cv::Point2f> mc(contours.size());

    for( int i = 0; i < contours.size(); i++ ){
        mu[i] = moments( contours[i], false );
        mc[i] = cv::Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );
    }

    for( int i = 0; i < contours.size(); i++ ){

        approxPolyDP(contours[i], pointsseq, arcLength(contours[i], true)*0.02, true);
        //bool isSquare = ( (pointsseq[0].x+pointsseq[1].x+pointsseq[2].x+pointsseq[3].x)/4-pointsseq[0].x ) < (pointsseq[0].x*squareTolerance);
        bool isSquare = (pointsseq[0].x-pointsseq[1].x) < (pointsseq[0].x*squareTolerance)
                        && (pointsseq[1].x-pointsseq[2].x) < (pointsseq[0].x*squareTolerance)
                        && (pointsseq[2].x-pointsseq[3].x) < (pointsseq[0].x*squareTolerance)
                        && (pointsseq[3].x-pointsseq[1].x) < (pointsseq[0].x*squareTolerance);

        if (pointsseq.size() == 4 && isSquare){

            int k=i;
            int c=0;

            while(hierarchy[k][2] != -1){

                k = hierarchy[k][2] ;
                c = c+1;
            }
            if(hierarchy[k][2] != -1)
                c = c+1;

            if (c >= 5){
                marklist[mark] = i;
                mark = mark + 1 ;
            }
        }
    }

    for( int k = 0; k < mark; k++ ){

        if(marklist[k] < contours.size() && contourArea(contours[marklist[k]]) > 10){
            //drawContours( *frame, contours, marklist[k] , cv::Scalar(255,200,0), 2, 8);//, hierarchy, 0 );

            squareContours.push_back(contours[marklist[k]]);
            drawContours( *frame, squareContours, squareContours.size()-1 , cv::Scalar(255,200,0), 2, 8);
        }


    }

    imshow ( "Image", *frame );
    cv::waitKey(1);

    return squareContours;
}
#include "MarkerTrack.h"

MarkerTrack::MarkerTrack(){
}

std::vector<cv::Point> MarkerTrack::Track(std::shared_ptr<cv::Mat> frame){

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    std::vector<cv::Point> pointsseq;
    cv::Mat *gray, *edges;

    gray = new cv::Mat(frame->size(), CV_MAKETYPE(frame->depth(), 1));
    edges = new cv::Mat(frame->size(), CV_MAKETYPE(frame->depth(), 1));

    cvtColor(*frame,*gray,CV_RGB2GRAY);
    //cv::threshold(*gray, *gray, 220,255,cv::THRESH_TOZERO);

    Canny(*gray, *edges, 100 , 200, 3);
    findContours(*edges, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

    std::vector<cv::Moments> mu(contours.size());
    std::vector<cv::Point2f> mc(contours.size());

    for( int i = 0; i < contours.size(); i++ ){
        mu[i] = moments( contours[i], false );
        mc[i] = cv::Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );
    }

    std::vector<cv::Point> squarePoints;

    for( int i = 0; i < contours.size(); i++ ){

        approxPolyDP(contours[i], pointsseq, arcLength(contours[i], true)*0.02, true);
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

                cv::Point p( (pointsseq[0].x+pointsseq[2].x)/2,
                                              (pointsseq[0].y+pointsseq[2].y)/2);

                cv::Point v = pointsseq[0] - pointsseq[2];
                float tolerance = v.x*v.x+v.y*v.y;


                bool isnew = true;
                for(int h = 0; h < squarePoints.size(); h++){

                    cv::Point vaux = p-squarePoints[h];
                    float d = vaux.x*vaux.x+vaux.y*vaux.y;

                    if(d<tolerance){
                        isnew = false;
                    }
                }

                if(isnew){
                    squarePoints.push_back(p);
                }

                //cv::circle( *frame, p, 10,  cv::Scalar(255,0,0), -1, 8, 0 );
                //cv::rectangle(*frame, pointsseq[0],pointsseq[2], cv::Scalar(0,0,255), 2, 8, 0);
                //break;
            }
        }
    }

    //imshow ( "Image", *gray );
    //cv::waitKey(1);

    delete gray;
    delete edges;

    return squarePoints;
}
#include <opencv2/opencv.hpp>

int main ( int argc, char **argv ){

    cv::VideoCapture capture(0);

    cv::Mat image;

    if(!capture.isOpened()) { std::cerr << " ERR: Unable find input Video source." << std::endl;
        return -1;
    }

    capture >> image;
    if(image.empty()){ std::cerr << "ERR: Unable to query image from capture device.\n" << std::endl;
        return -1;
    }

    cv::Mat gray(image.size(), CV_MAKETYPE(image.depth(), 1));
    cv::Mat edges(image.size(), CV_MAKETYPE(image.depth(), 1));

    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    std::vector<cv::Point> pointsseq;

    int mark;
    int marklist[4];

    int key = 0;
    while(key != 'q'){

        capture >> image;

        cvtColor(image,gray,CV_RGB2GRAY);
        Canny(gray, edges, 100 , 200, 3);
        findContours( edges, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

        mark = 0;


        std::vector<cv::Moments> mu(contours.size());
        std::vector<cv::Point2f> mc(contours.size());

        for( int i = 0; i < contours.size(); i++ ){
            mu[i] = moments( contours[i], false );
            mc[i] = cv::Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );
        }

        for( int i = 0; i < contours.size(); i++ ){

            approxPolyDP(contours[i], pointsseq, arcLength(contours[i], true)*0.02, true);
            if (pointsseq.size() == 4){

                //std::cout << pointsseq[0] << " || " << pointsseq[1] << " || " << pointsseq[2] << " || " << pointsseq[3] << std::endl;

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
                drawContours( image, contours, marklist[k] , cv::Scalar(255,200,0), 2, 8, hierarchy, 0 );

            }
        }

        imshow ( "Image", image );
        key = cv::waitKey(1);

    }

    return 0;
}

#include <cstdio>
#include <cstring>
#include <ctime>
#include <iostream>

#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <tracker/utility.hpp>

using namespace cv;
using namespace std;

// Display the help for the program
void help(const char* programName);

// parse the input command line arguments
bool parseArgs(int argc, char** argv, Size& boardSize, string& inputFilename, Pattern& pattern);



void calcChessboardCorners(Size boardSize, float squareSize, vector<Point3f>& corners){
    corners.clear();
    
    for( int i = 0; i < boardSize.height; ++i )
        for( int j = 0; j < boardSize.width; ++j )
            corners.push_back(Point3f(float( j*squareSize ), float( i*squareSize ), 0));

    }


int main(int argc, char** argv)
{
    /******************************************************************/
    /* CONSTANTS to use                                               */
    /******************************************************************/

    // the name of the window
    const string WINDOW_NAME = "Image View";

    // the name of the window
    const string WINDOW_RECTIFIED = "Rectified Image";

    /******************************************************************/
    /* VARIABLES to use                                               */
    /******************************************************************/
    // it will contain the detected corners on the chessboard
    vector<Point2f> pointbuf;

    // it will contain the 2D "arbitrary" chosen points on the chessboard
    vector<Point2f> objectPoints;

    // it will be true if a chessboard is found, false otherwise
    bool found{false};

    // it will contain the size in terms of corners (width X height) of the chessboard
    Size boardSize;

    // it will contain the filename of the image file
    string inputFilename;

    // Default pattern is chessboard
    Pattern pattern = CHESSBOARD;

    // Used to load the video and get the frames
    VideoCapture capture;

    /******************************************************************/
    /* READ THE INPUT PARAMETERS - DO NOT MODIFY                      */
    /******************************************************************/

    if(!parseArgs(argc, argv, boardSize, inputFilename, pattern))
    {
        cerr << "Aborting..." << endl;
        return EXIT_FAILURE;
    }

    /******************************************************************/
    /* PART TO DEVELOP                                                  */
    /******************************************************************/

    /******************************************************************/
    // create a window using WINDOW_NAME as name to display the image --> see namedWindow
    /******************************************************************/
    namedWindow(WINDOW_NAME,cv::WINDOW_AUTOSIZE);

    /******************************************************************/
    // create a second window using WINDOW_RECTIFIED as name to display the rectified image
    /******************************************************************/
    namedWindow(WINDOW_RECTIFIED,cv::WINDOW_AUTOSIZE);

    /******************************************************************/
    // read the input video with capture (same as before)
    /******************************************************************/
    capture.open(inputFilename);

    /******************************************************************/
    // check it is really opened
    /******************************************************************/

    if(!capture.isOpened())
        {
            cerr << "Unable to open the video file" << endl;
            return EXIT_FAILURE;
        }

    /******************************************************************/
    // create the set of 2D (arbitrary) points of the checkerboard, let's say the
    // size of the squares is 25
    // call to calcChessboardCorners
    /******************************************************************/
    calcChessboardCorners(boardSize, 25, objectPoints);

    // processing loop
    while(true)
    {
        Mat view, rectified;
        Mat frame;
        /******************************************************************/
        // get the new frame from capture and copy it to view
        /******************************************************************/
        
        auto t = (double)getTickCount();
        t = ((double)getTickCount() - t) / getTickFrequency();

        if( !capture.read(frame) ){
            cout<<"chessboard detection took"<<t*1000<<"ms"<<endl;
            break;
        }
        

        /******************************************************************/
        // if no more images to process exit the loop
        /******************************************************************/



        /******************************************************************/
        // call the function that detects the chessboard on the image
        /******************************************************************/
        found = detectChessboard(frame, pointbuf, boardSize, pattern);


        cout << ((!found) ? ("No ") : ("")) << "chessboard detected!" << endl;

        // if a chessboard is found estimate the homography and rectify the image
        if(found)
        {
            /******************************************************************/
            // estimate the homography
            // --> see findHomography
            // http://docs.opencv.org/modules/calib3d/doc/camera_calibration_and_3d_reconstruction.html?highlight=homography#findhomography
            /******************************************************************/
            Mat H = findHomography( objectCorners, pointbuf, CV_RANSAC);

            /******************************************************************/
            // use the estimated homography to rectify the image
            // --> see warpPerspective
            // http://docs.opencv.org/modules/imgproc/doc/geometric_transformations.html#void warpPerspective(InputArray src, OutputArray dst, InputArray M, Size dsize, int flags, int borderMode, const Scalar& borderValue)
            /******************************************************************/
            warpPerspective(view,rectified,H,rectified.size());
        }
        else
        {
            /******************************************************************/
            // otherwise copy the original image in rectified
            // Mat.copyTo()
            /******************************************************************/
            view.copyTo(rectified);
        }

        /******************************************************************/
        // if the chessboard is found draw the cornerns on top of it
        // --> see drawChessboardCorners
        /******************************************************************/
        if(found){
            drawChessboardCorners( frame, boardSize, Mat(pointbuf), found );
            }

        /******************************************************************/
        // show the image inside the window --> see imshow
        /******************************************************************/
        imshow(WINDOW_NAME, view);

        /******************************************************************/
        // show the rectified image inside the window --> see imshow
        /******************************************************************/
        imshow(WINDOW_RECTIFIED, rectified);

        // wait 20ms for user input before processing the next frame
        // Any user input will stop the execution
        if(waitKey(10) >= 0)
            break;
    }

    /******************************************************************/
    // release the video resource
    /******************************************************************/
    capture.release();
    destroyAllWindows();

    return EXIT_SUCCESS;
}

// Display the help for the programm

void help(const char* programName)
{
    cout << "Detect a chessboard in a given video and show the rectified image according to the estimated homography"
         << endl
         << "Usage: " << programName << endl
         << "     -w <board_width>                                  # the number of inner corners per one of board "
            "dimension"
         << endl
         << "     -h <board_height>                                 # the number of inner corners per another board "
            "dimension"
         << endl
         << "     [-pt <pattern=[circles|acircles|chess]>]          # the type of pattern: chessboard or circles' grid"
         << endl
         << "     <video file> " << endl
         << endl;
}

// parse the input command line arguments

bool parseArgs(int argc, char** argv, Size& boardSize, string& inputFilename, Pattern& pattern)
{
    // check the minimum number of arguments
    if(argc < 3)
    {
        help(argv[0]);
        return false;
    }

    // Read the input arguments
    for(int i = 1; i < argc; i++)
    {
        const char* s = argv[i];
        if(strcmp(s, "-w") == 0)
        {
            if(sscanf(argv[++i], "%u", &boardSize.width) != 1 || boardSize.width <= 0)
            {
                cerr << "Invalid board width" << endl;
                return false;
            }
        }
        else if(strcmp(s, "-h") == 0)
        {
            if(sscanf(argv[++i], "%u", &boardSize.height) != 1 || boardSize.height <= 0)
            {
                cerr << "Invalid board height" << endl;
                return false;
            }
        }
        else if(s[0] != '-')
        {
            inputFilename.assign(s);
        }
        else if(strcmp(s, "-pt") == 0)
        {
            i++;
            if(!strcmp(argv[i], "circles"))
                pattern = CIRCLES_GRID;
            else if(!strcmp(argv[i], "acircles"))
                pattern = ASYMMETRIC_CIRCLES_GRID;
            else if(!strcmp(argv[i], "chess"))
                pattern = CHESSBOARD;
            else
            {
                cerr << "Invalid pattern type: must be chessboard or circles" << endl;
                return false;
            }
        }
        else
        {
            cerr << "Unknown option " << s << endl;
            return false;
        }
    }

    return true;
}

#include <opencv2/opencv.hpp>
#include "findFingers.h"
using namespace cv;


int main() {
    // Open the default camera (usually 0 is the built-in webcam)
    utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);
    VideoCapture cap(1);
    
    // Check if camera opened successfully
    if (!cap.isOpened()) {
        std::cout << "Error opening video stream!" << std::endl;
        return -1;
    }

    // Create a window to show the video
    cv::namedWindow("Webcam", cv::WINDOW_AUTOSIZE);

    while (true) {
        cv::Mat frame, edges;
        
        // Capture frame-by-frame
        cap.read(frame);

        // If the frame is empty, break immediately
        if (frame.empty()) {
            std::cout << "Error: Blank frame grabbed" << std::endl;
            break;
        }

        flip(frame,frame,1);

        // Convert the frame to hsv
        cv::Mat hsv;
        cvtColor(frame, hsv, COLOR_BGR2HSV);

        // Apply GaussianBlur to reduce noise and improve edge detection
        cv::Mat blurred;
        GaussianBlur(hsv, blurred, Size(5, 5), 1.5);

        // Apply Canny edge detection
        int lowThreshold = 100;
        int highThreshold = 140;
        cv::Canny(blurred, edges, lowThreshold, highThreshold);

        // Show the edges
        //cv::imshow("Edges", edges);

        std::vector<cv::Mat> channels;
        cv::split(hsv, channels);
        // Calculate histogram for the Hue channel (H)
        int histSize = 180;
        float range[] = { 0, 180 }; // Hue range 
        const float* histRange = { range };
        cv::Mat hist;
        cv::calcHist(&channels[0], 1, 0, cv::Mat(), hist, 1, &histSize, &histRange, true, false);


        cv::normalize(hist, hist, 0, 400, cv::NORM_MINMAX);

        int histWidth = 512; // Width of the histogram image
        int histHeight = 400; // Height of the histogram image
        int binWidth = cvRound((double)histWidth / histSize);

        cv::Mat histImage(histHeight, histWidth, CV_8UC3, cv::Scalar(0, 0, 0));

        // Draw the histogram
        for (int i = 1; i < histSize; ++i) {
            cv::line(histImage,
                    cv::Point(binWidth * (i - 1), histHeight - cvRound(hist.at<float>(i - 1))),
                    cv::Point(binWidth * i, histHeight - cvRound(hist.at<float>(i))),
                    cv::Scalar(255, 0, 0), // Blue line
                    2, cv::LINE_AA);
        }

        // Show the histogram
        //cv::imshow("Hue Histogram", histImage);
        std::cout << "hiii";

        cv::Mat mask;
        cv::inRange(channels[0], 40, 180, mask);  // Mask pixels based on dynamic threshold

        // Show the result
        cv::imshow("Thresholded Mask", mask);

        cv::imshow("Webcam", frame);
        findFingersHelper(mask);
     
        // Break the loop if 'ESC' is pressed
        char c = (char)cv::waitKey(1);
        if (c == 27) {// ASCII value for 'ESC' key
            break;
        }
        else if (c == 's' || c == 'S') { // Save the current frame if 's' is pressed
            std::string filename = "saved_frame_" + std::string(".jpg");
            if (cv::imwrite(filename, mask)) {
                std::cout << "Frame saved as " << filename << std::endl;
            } else {
                std::cout << "Error saving frame!" << std::endl;
            }
        }

    }
    // Release the capture and destroy windows
    cap.release();
    cv::destroyAllWindows();
    
    return 0;
}
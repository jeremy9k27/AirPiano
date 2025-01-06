#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace cv;



void findFingers(const std::string& inputFile, const std::string& outputFile) {
    // Load the image
    Mat image = imread(inputFile, IMREAD_COLOR);
    if (image.empty()) {
        std::cerr << "Error: Could not load image " << inputFile << std::endl;
        return;
    }
    // Convert to grayscale
    cv::Mat gray;
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    
    // Convert to binary using thresholding
    cv::Mat binary;
    cv::threshold(gray, binary, 127, 255, cv::THRESH_BINARY);
    
    // Find contours
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(binary, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    
    // Find the largest contour (assuming it's the hand)
    int largest_contour_idx = 0;
    double largest_area = 0;
    for(int i = 0; i < contours.size(); i++) {
        double area = cv::contourArea(contours[i]);
        if(area > largest_area) {
            largest_area = area;
            largest_contour_idx = i;
        }
    }
    // Get convex hull and defects
    std::vector<int> hull_indices;
    cv::convexHull(contours[largest_contour_idx], hull_indices, true);
    std::vector<cv::Vec4i> defects;
    cv::convexityDefects(contours[largest_contour_idx], hull_indices, defects);
    
    // Create output image (black background)
    cv::Mat fingertips = image.clone();
    
    // Process defects to find fingertips
    std::vector<cv::Point> fingertip_points;
    for(const auto& defect : defects) {
        cv::Point start = contours[largest_contour_idx][defect[0]];
        // The point at maximum distance from convex hull is likely between two fingers
        // Points before and after these are likely fingertips
        fingertip_points.push_back(start);
    }

    // Sort points by y-coordinate (highest y value = lowest in image)
    std::sort(fingertip_points.begin(), fingertip_points.end(), 
        [](const cv::Point& a, const cv::Point& b) {
            return a.y > b.y;
        });

    // Get the 5 lowest points (first 5 after sorting)
    int i = 0;
    std::vector<cv::Point> lowest_points;
    while (lowest_points.size() < 5 && i < fingertip_points.size()) {
        bool far_enough = true;
        // Check distance to all already included points
        for (const auto& existing_point : lowest_points) {
            double dist = cv::norm(fingertip_points[i] - existing_point);
            if (dist < 30) {
                far_enough = false;
                break;
            }
        }
        
        if (far_enough) {
            lowest_points.push_back(fingertip_points[i]);
        }
        i++;
    }
    
    int radius = 2;  // Increase the radius to make it bigger
    for(const auto& point : lowest_points) {
        cv::circle(fingertips, point, radius, cv::Scalar(255), -1);  // -1 fills the circle
    }
    
    // Show results
    cv::imshow("Fingertips", fingertips);
    //cv::waitKey(0);

    

}

int main() {
    std::string inputFile = "saved_frame_.jpg";  // Input image
    std::string outputFile = "filtered_image.jpg"; // Output image

    findFingers(inputFile, outputFile);

    return 0;
}

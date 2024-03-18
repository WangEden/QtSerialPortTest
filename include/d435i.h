#ifndef d435i_H
#define d435i_H

#include <librealsense2/rs.hpp>
#include <opencv2/opencv.hpp>
#include <tuple>
#include <vector>
#include <thread>
#include <chrono>
#include <cstring>
#include <cassert>
#include <algorithm>
#include <random>
#include <stdexcept>

using namespace std;
using namespace cv;

class Camera
{
private:
    int height = 480;
    int width = 640;
    int fps = 30;
    rs2::pipeline pipe;
    rs2::config cfg;
    rs2_intrinsics color_intrinsics;
    rs2_intrinsics depth_intrinsics;
    Mat intrinsics_mat, distCoeffs;
    float depth_scale;
    float depth_offset;
    rs2::align align_to_color;
    rs2::frameset frameset;
    Mat color_mat;
    Mat depth_mat;
    int mat_width = 0;
    int mat_height = 0;
    int count = 0;
    string color_filename;
    string depth_filename;
    // calibration
    Mat gray_mat;
    vector<Point2f> corners_2f;
    vector<Point2f> corners_projection_2f;
    vector<Point3f> corners_3f;
    vector<Point2d> corners_2d;
    vector<Point3d> corners_3d;
    Mat V_Board2Cam, R_Board2Cam, T_Board2Cam;
    Size chessboard_size;
    double square_size = 0.015;
    bool pattern_found;
    // shelf grasp
    double projection_error;
    vector<Vec3f> circles;
    vector<Point3f> object_points_3f;
    vector<Point2d> centers, image_points, center_points_copy, center_points_5;
    vector<Point2f> projection_points;
    Vec3i circle;
    Point center;
    int radius;
    Mat V_Shelf2Cam, R_Shelf2Cam, T_Shelf2Cam;
    // **************************************************
    Mat V_QR2Cam, R_QR2Cam, T_QR2Cam;
public:
    Camera();
    tuple<Mat, Mat> connect();
    void disconnect();
    tuple<Mat, Mat> get_images();
    void save_images();
    void get_corners(bool save = false);
    tuple<Mat, Mat> get_R_board2cam(bool projection); 
    vector<Vec3f> get_circles(bool save);
    tuple<Mat, Mat, vector<Point2f>, string> get_shelf_pose(vector<vector<Point3d>> object_points, vector<Point2d> center_points);
    tuple<Mat, Mat, vector<Point2f>> get_projection_error(vector<Point3d> object_points, vector<Point2d> image_points);
    tuple<Mat, Mat, vector<Point2f>> get_QR_pose(vector<Point3d> object_points, vector<Point2d> image_points);
};


#endif
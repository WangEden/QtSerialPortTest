#include <d435i.h>
#include <thread>
#include <chrono>

Camera::Camera() : align_to_color(RS2_STREAM_COLOR), chessboard_size(Size(11, 8))
{
    for (int i = 0; i < this->chessboard_size.height; i++)
    {
        for (int j = 0; j < this->chessboard_size.width; j++)
        {
            this->corners_3d.push_back(Point3d(j * this->square_size, i * this->square_size, 0.0));
            this->corners_3f.push_back(Point3f(j * this->square_size, i * this->square_size, 0.0));
            // cout << "x:" << j * this->square_size << "\t" << "y:" << i * this->square_size << endl;
        }
        
    }
}

tuple<Mat, Mat> Camera::connect() {
    this->cfg.enable_stream(RS2_STREAM_COLOR, this->width, this->height, RS2_FORMAT_BGR8, this->fps);
    this->cfg.enable_stream(RS2_STREAM_DEPTH, this->width, this->height, RS2_FORMAT_Z16, this->fps);
    rs2::pipeline_profile profile = this->pipe.start(cfg);
    
    auto color_stream = profile.get_stream(RS2_STREAM_COLOR).as<rs2::video_stream_profile>();
    auto depth_stream = profile.get_stream(RS2_STREAM_DEPTH).as<rs2::video_stream_profile>();
    this->depth_intrinsics = depth_stream.get_intrinsics();
    this->color_intrinsics = color_stream.get_intrinsics();

    this->intrinsics_mat = (Mat_<double>(3, 3) << \
    this->color_intrinsics.fx, 0.0, this->color_intrinsics.ppx, \
    0.0, this->color_intrinsics.fy, this->color_intrinsics.ppy, \
    0.0, 0.0, 1.0);
    this->distCoeffs = (Mat_<double>(5, 1) << this->color_intrinsics.coeffs[0], this->color_intrinsics.coeffs[1], \
    this->color_intrinsics.coeffs[2], this->color_intrinsics.coeffs[3], this->color_intrinsics.coeffs[4]);
    // cout << this->color_intrinsics.width << this->color_intrinsics.height << endl;
    // cout << "intri: " << this->intrinsics_mat << endl;
    // for (size_t i = 0; i < 5; i++)
    // {
    //     cout << this->distCoeffs[i] << " " << endl;
    // }
    

    this->depth_scale = profile.get_device().first<rs2::depth_sensor>().get_depth_scale();
    this->depth_offset = profile.get_device().first<rs2::depth_sensor>().get_option(RS2_OPTION_DEPTH_UNITS);

    // cout << "Camera connected" << endl;
    this_thread::sleep_for(chrono::milliseconds(2000));

    return make_tuple(this->intrinsics_mat.clone(), this->distCoeffs.clone());
}

void Camera::disconnect() {
    this->pipe.stop();
    // cout << "Camera disconnected" << endl;
}


tuple<Mat, Mat> Camera::get_images() {
    this->frameset = this->pipe.wait_for_frames();
    this->frameset = this->align_to_color.process(this->frameset);

    rs2::video_frame color_frame = frameset.get_color_frame();
    rs2::depth_frame depth_frame = frameset.get_depth_frame();

    if (this->mat_height == 0) {
        this->mat_height = color_frame.get_height();
        this->mat_width = color_frame.get_width();
    }

    this->color_mat = Mat(Size(this->mat_width, this->mat_height), CV_8UC3, (void*)color_frame.get_data(), Mat::AUTO_STEP);
    this->depth_mat = Mat(Size(this->mat_width, this->mat_height), CV_16UC1, (void*)depth_frame.get_data(), Mat::AUTO_STEP);

    this->count++;
    return make_tuple(this->color_mat.clone(), this->depth_mat.clone());
}

void Camera::save_images(){
    this->color_filename = "../images/color_image" + to_string(this->count) + ".png";
    this->depth_filename = "../images/depth_image" + to_string(this->count) + ".png";

    imwrite(color_filename, this->color_mat);
    imwrite(depth_filename, this->depth_mat);

    cout << "Saved Image " << this->count << endl;
}

void Camera::get_corners(bool save) {
    cvtColor(this->color_mat, this->gray_mat, COLOR_BGR2GRAY);
    this->pattern_found = findChessboardCorners(this->gray_mat, this->chessboard_size, this->corners_2f);
    if(this->pattern_found){
         cornerSubPix(this->gray_mat, this->corners_2f, Size(11, 11), Size(-1, -1), \
         TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 30, 0.1));
    }
    if(save) {
        drawChessboardCorners(this->color_mat, this->chessboard_size, Mat(this->corners_2f), this->pattern_found);
        save_images();
    }
    this->corners_2d.clear();
    for (const auto& corner_2f : this->corners_2f)
    {
        Point2d point2d(corner_2f.x, corner_2f.y);
        this->corners_2d.push_back(point2d);
        // cout << "x:" << point2d.x << "\t" << "y:" << point2d.y << endl;
    }
    // cout << "corners saved " << endl;
}

tuple<Mat, Mat> Camera::get_R_board2cam(bool projection) {
    solvePnP(this->corners_3d, this->corners_2d, this->intrinsics_mat, this->distCoeffs, this->V_Board2Cam, this->T_Board2Cam);
    Rodrigues(V_Board2Cam, R_Board2Cam);
    // cout << "2d: " << endl;
    // for (const auto& corner_2d : this->corners_2d)
    // {
    //     cout << "x:" << corner_2d.x << "\t" << "y:" << corner_2d.y << endl;
    // }
    // cout << "3d: " << endl;
    // for (const auto& corner_3d : this->corners_3d)
    // {
    //     cout << "x:" << corner_3d.x << "\t" << "y:" << corner_3d.y << endl;
    // }
    // cout << "Rotation vector: " << this->V_Board2Cam << endl;
    // cout << "T_Board2Cam: " << this->T_Board2Cam << endl;
    // cout << "R_Board2Cam: " << this->R_Board2Cam << endl;
    if(projection) {
        projectPoints(this->corners_3f, this->V_Board2Cam, this->T_Board2Cam, this->intrinsics_mat, this->distCoeffs, this->corners_projection_2f);
        for (size_t i = 0; i < this->corners_projection_2f.size(); i++)
        {
            Point2f pt = this->corners_projection_2f[i];
            cv::circle(this->color_mat, pt, 4, Scalar(0, 0, 255), -1);
        }
        save_images();
        
    }
    return make_tuple(this->R_Board2Cam.clone(), this->T_Board2Cam.clone());
}

vector<Vec3f> Camera::get_circles(bool save) {
    this->centers.clear();
    cvtColor(this->color_mat, this->gray_mat, COLOR_BGR2GRAY);
    // GaussianBlur(this->gray_mat, this->gray_mat, Size(3, 3), 2, 2);
    HoughCircles(this->gray_mat, this->circles, HOUGH_GRADIENT, 0.3, this->gray_mat.rows / 30, 80, 12, 1, 10);

    // cout << this->circles.size() << endl;
    for (size_t i = 0; i < this->circles.size(); i++)
    {
        this->circle = this->circles[i];
        this->center = Point2d(circle[0], circle[1]);
        this->centers.push_back(this->center);
        this->radius = this->circle[2];

        cv::circle(this->color_mat, center, 1, Scalar(0, 100, 100), 3, LINE_AA);
        cv::circle(this->color_mat, center, radius, Scalar(255, 0, 255), 3, LINE_AA);

    }

    if(save) {
        save_images();
    }

    imshow("Circles: ", this->color_mat);
    waitKey(0);
    destroyAllWindows();
    
    return this->circles;
}

tuple<Mat, Mat, vector<Point2f>> Camera::get_projection_error(vector<Point3d> object_points, vector<Point2d> image_points){
    this->projection_error = 0;
    this->object_points_3f.clear();
    for (const auto& point3d : object_points)
    {
        Point3f point3f = Point3f(point3d);
        this->object_points_3f.push_back(point3f);
    }

    solvePnP(object_points, image_points, this->intrinsics_mat, this->distCoeffs, this->V_Shelf2Cam, this->T_Shelf2Cam);
    Rodrigues(this->V_Shelf2Cam, this->R_Shelf2Cam);
    projectPoints(this->object_points_3f, this->V_Shelf2Cam, this->T_Shelf2Cam, this->intrinsics_mat, this->distCoeffs, this->projection_points);
    

    for (size_t i = 0; i < image_points.size(); i++)
    {
        double dx = image_points[i].x - projection_points[i].x;
        double dy = image_points[i].y - projection_points[i].y;
        this->projection_error += sqrt(dx * dx + dy * dy);
    }
    return make_tuple(this->R_Shelf2Cam.clone(), this->T_Shelf2Cam.clone(), this->projection_points);
}

tuple<Mat, Mat, vector<Point2f>, string> Camera::get_shelf_pose(vector<vector<Point3d>> object_points, vector<Point2d> center_points) {
    random_device rd;
    mt19937 gen(rd());
    tuple<Mat, Mat, vector<Point2f>> shelf_pose;
    double min_x, max_x, min_y, max_y;
    for (size_t iteration = 0; iteration < 10000; iteration++)
    {
        do
        {
            this->center_points_5.clear();
            min_x = 1000;
            min_y = 1000;
            max_x = -1;
            max_y = -1;
            shuffle(center_points.begin(), center_points.end(), gen);
            for (size_t j = 0; j < 5; j++)
            {
                this->center_points_5.push_back(center_points[j]);
            }
            for (auto point : center_points_5)
            {
                min_x = min(min_x, point.x);
                min_y = min(min_y, point.y);
                max_x = max(max_x, point.x);
                max_y = max(max_y, point.y);
            }
        } while (max_x - min_x > 180 || max_y - min_y > 100);
        
        // sort by x
        sort(this->center_points_5.begin(), this->center_points_5.end(), [](const Point2d& p1, const Point2d& p2){
            return p1.x < p2.x;
        });
        // criterion up
        this->image_points.clear();
        this->center_points_copy = this->center_points_5;
        for (size_t i = 0; i < 3; i++)
        {
            auto minPoint = min_element(this->center_points_copy.begin(), this->center_points_copy.begin() + 3 - i, \
            [](const Point2d& p1, const Point2d& p2) {
                return p1.y < p2.y;
            });
            this->image_points.push_back(*minPoint);
            this->center_points_copy.erase(minPoint);
        }
        for (size_t i = 0; i < 2; i++)
        {
            auto minPoint = min_element(this->center_points_copy.begin(), this->center_points_copy.end(), \
            [](const Point2d& p1, const Point2d& p2) {
                return p1.y < p2.y;
            });
            this->image_points.push_back(*minPoint);
            this->center_points_copy.erase(minPoint);
        }
        shelf_pose = get_projection_error(object_points[0], this->image_points);
        if (this->projection_error < 2) {
            // for (auto point : object_points[0])
            // {
            //     cout << point.x << "\t" << point.y << "\t" << point.z << endl;
            // }
            // for (auto point : image_points)
            // {
            //     cout << point.x << "\t" << point.y << endl;
            // }
            
            cout << "projection error: " << this->projection_error << endl;
            return tuple_cat(shelf_pose, make_tuple("up"));
        }
        

        // criterion left
        this->image_points.clear();
        this->center_points_copy = this->center_points_5;
        this->image_points.push_back(this->center_points_copy[0]);
        this->center_points_copy.erase(center_points_copy.begin());
        for (size_t i = 0; i < 3; i++)
        {
            auto minPoint = min_element(this->center_points_copy.begin(), this->center_points_copy.begin() + 3 - i, \
            [](const Point2d& p1, const Point2d& p2) {
                return p1.y < p2.y;
            });
            this->image_points.push_back(*minPoint);
            this->center_points_copy.erase(minPoint);
        }
        this->image_points.push_back(this->center_points_copy[0]);
        shelf_pose = get_projection_error(object_points[1], this->image_points);
        if (this->projection_error < 2) {
            cout << "projection error: " << this->projection_error << endl;
            return tuple_cat(shelf_pose, make_tuple("left"));
        }
    
        // criterion right
        this->image_points.clear();
        this->center_points_copy = this->center_points_5;
        for (size_t i = 0; i < 2; i++)
        {
            auto minPoint = min_element(this->center_points_copy.begin(), this->center_points_copy.begin() + 2 - i, \
            [](const Point2d& p1, const Point2d& p2) {
                return p1.y < p2.y;
            });
            this->image_points.push_back(*minPoint);
            this->center_points_copy.erase(minPoint);
        }
        this->image_points.push_back(this->center_points_copy[0]);
        this->center_points_copy.erase(this->center_points_copy.begin());
        for (size_t i = 0; i < 2; i++)
        {
            auto minPoint = min_element(this->center_points_copy.begin(), this->center_points_copy.end(), \
            [](const Point2d& p1, const Point2d& p2) {
                return p1.y < p2.y;
            });
            this->image_points.push_back(*minPoint);
            this->center_points_copy.erase(minPoint);
        }
        shelf_pose = get_projection_error(object_points[2], this->image_points);
        if (this->projection_error < 2) {
            cout << "projection error: " << this->projection_error << endl;
            return tuple_cat(shelf_pose, make_tuple("right"));
        }

        // for (Point2d image_point : image_points)
        // {
        //     cout << image_point.x << "\t" << image_point.y << endl;
        // }
    }
    throw runtime_error("can't find projection !!!");
}

// ****************************************************
tuple<Mat, Mat, vector<Point2f>> Camera::get_QR_pose(vector<Point3d> object_points, vector<Point2d> image_points){
    this->object_points_3f.clear();
    for (const auto& point3d : object_points)
    {
        Point3f point3f = Point3f(point3d);
        this->object_points_3f.push_back(point3f);
    }
    
    solvePnP(object_points, image_points, this->intrinsics_mat, this->distCoeffs, this->V_QR2Cam, this->T_QR2Cam);
    Rodrigues(this->V_QR2Cam, this->R_QR2Cam);
    projectPoints(this->object_points_3f, this->V_QR2Cam, this->T_QR2Cam, this->intrinsics_mat, this->distCoeffs, this->projection_points);

    return make_tuple(this->R_QR2Cam.clone(), this->T_QR2Cam.clone(), this->projection_points);
}
#include <opencv2/core/operations.hpp>
#include <tools/Cam.h>
#include <cmath>

Cam::Cam(float f, Vector2D<float> size, Transform3D<float> posCam) {
    // Construct transition matrix from image to camera
    matK_C2I = (cv::Mat_<float>(3, 3) << f, 0, size.x,
            0, f, size.y,
            0, 0, 1);

    // Construct the transition matrix from camera to image
    matK_C2I = matK_I2C.inv();

    // construct the rotation matrix from robot to camera
    float crx = cos(posCam.rx);
    float srx = sin(posCam.rx);
    float cry = cos(posCam.ry);
    float sry = sin(posCam.ry);
    float crz = cos(posCam.rz);
    float srz = sin(posCam.rz);
    cv::Mat rotX_R2C = (cv::Mat_<float>(3, 3) << 1, 0, 0,
            0, crx, srx,
            0, -srx, crx);
    cv::Mat rotY_R2C = (cv::Mat_<float>(3, 3) << cry, 0, -sry,
            0, 1, 0,
            sry, 0, cry);
    cv::Mat rotZ_R2C = (cv::Mat_<float>(3, 3) << crz, srz, 0,
            -srz, crz, 0,
            0, 0, 1);
    cv::Mat rot_R2C = rotX_R2C * rotY_R2C * rotZ_R2C;

    // Construct the translation matrix from robot to camera
    cv::Mat trsl_R2C = (cv::Mat_<float>(3, 1) << posCam.x,
            posCam.y,
            posCam.z);

    // Construct the transition matrix from robot 2 camera
    matTransi_R2C = cv::Mat_<float>(4, 4);
    cv::Mat Z = (cv::Mat_<float>(1, 3) << 0, 0, 0);
    cv::Mat One = (cv::Mat_<float>(1, 1) << 1);
    rot_R2C.copyTo(matTransi_R2C(cv::Rect(0, 0, rot_R2C.cols, rot_R2C.rows)));
    trsl_R2C.copyTo(matTransi_R2C(cv::Rect(3, 0, trsl_R2C.cols, trsl_R2C.rows)));
    Z.copyTo(matTransi_R2C(cv::Rect(0, 3, Z.cols, Z.rows)));
    One.copyTo(matTransi_R2C(cv::Rect(3, 3, One.cols, One.rows)));

    // Construct the transition matrix from camera to robot
    matTransi_C2R = matTransi_R2C.inv();
}


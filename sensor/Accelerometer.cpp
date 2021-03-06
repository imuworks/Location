//
// Created by yangcheng on 2018/11/28.
//

#include "Accelerometer.h"
#include "../math/Optimizer.h"

Vector3d Accelerometer::Normalise(Vector3d &a) const {
    Vector3d normA;
    double norm2 = a(0) * a(0) + a(1) * a(1) + a(2) * a(2);
    // 如果四元数各项足够接近单位四元数, 则不做任何处理.
    if (norm2 != 0.0) {
        double norm = sqrt(norm2);
        normA(0) = a(0) / norm;
        normA(1) = a(1) / norm;
        normA(2) = a(2) / norm;
    } else {
        normA = a;
    }
    return normA;
}

// 通过旋转获取 地理坐标系下重力加速度 转 机体坐标系下的重力加速度.
Vector3d Accelerometer::RotateG(Matrix3d &n2b) const {
    // b系下坐标
    Vector3d bg;
    // 地理坐标系下重力加速度
    Vector3d g(0, 0, 1);
    bg = n2b * g;
    return bg;
}

// 加速计向量(originA)叉乘地理重力转b系(rotatedG)误差，用于较正陀螺仪.
Vector3d Accelerometer::GetAccError(Vector3d &originA, Vector3d &rotatedG) const {
    Vector3d accErr;

    accErr(0) = originA(1) * rotatedG(2) - originA(2) * rotatedG(1);
    accErr(1) = originA(2) * rotatedG(0) - originA(0) * rotatedG(2);
    accErr(2) = originA(0) * rotatedG(1) - originA(1) * rotatedG(0);

    return accErr;
}

void Accelerometer::AccCalibration(MatrixXd &input_data, Status *status) {
    double gamma = (*status).parameters.gamma;
    double epsilon = (*status).parameters.epsilon;
    int max_step = (*status).parameters.max_step;
    VectorXd *coef = &(*status).parameters.acc_coef;
    Optimizer optimizer;
    MatrixXd input_data_format = input_data / (*status).parameters.g;
    optimizer.LevenbergMarquardt(input_data_format, coef, gamma, epsilon, max_step);
}


void Accelerometer::PositionIntegral(Status *status, Vector3d &acc, double t) const {

    // 更新位置
    (*status).position.x = (*status).velocity.v_x * t + 0.5 * acc(0) * t * t;
    (*status).position.y = (*status).velocity.v_y * t + 0.5 * acc(1) * t * t;
    (*status).position.z = (*status).velocity.v_z * t + 0.5 * acc(2) * t * t;
    // 更新速度
    (*status).velocity.v_x = (*status).velocity.v_x + acc(0) * t;
    (*status).velocity.v_y = (*status).velocity.v_y + acc(1) * t;
    (*status).velocity.v_z = (*status).velocity.v_z + acc(2) * t;

}

Accelerometer::Accelerometer() {}

Accelerometer::~Accelerometer() = default;

/**
* Implementation of KalmanFilter class. Modified by phamngtuananh.
*
* @author: Hayk Martirosyan
* @date: 2014.11.15
*/

#include <iostream>
#include <stdexcept>

#include "kalman.h"

namespace PoseOptimizer
{
PoseKalmanFilter::PoseKalmanFilter(double dt,
                                   const Eigen::MatrixXd& A,
                                   const Eigen::MatrixXd& C,
                                   const Eigen::MatrixXd& Q,
                                   const Eigen::MatrixXd& R,
                                   const Eigen::MatrixXd& P)
  : A(A), C(C), Q(Q), R(R), P0(P),
    m(C.rows()), n(A.rows()), dt(dt), initialized(false),
    I(n, n), x_hat(n), x_hat_new(n)
{
  I.setIdentity();
}

PoseKalmanFilter::PoseKalmanFilter() {}

void PoseKalmanFilter::init(double t0, const Eigen::VectorXd& x0)
{
  x_hat = x0;
  P = P0;
  this->t0 = t0;
  t = t0;
  initialized = true;
}

void PoseKalmanFilter::init(double t0, const tf::Transform& tf_x0)
{
  
}


void PoseKalmanFilter::init()
{
  x_hat.setZero();
  P = P0;
  t0 = 0;
  t = t0;
  initialized = true;
}

void PoseKalmanFilter::update(const Eigen::VectorXd& y)
{
  if(!initialized)
    throw std::runtime_error("Filter is not initialized!");

  x_hat_new = A * x_hat;
  P = A*P*A.transpose() + Q;
  K = P*C.transpose()*(C*P*C.transpose() + R).inverse();
  x_hat_new += K * (y - C*x_hat_new);
  P = (I - K*C)*P;
  x_hat = x_hat_new;

  t += dt;
}

void PoseKalmanFilter::update(const Eigen::VectorXd& y, double dt, const Eigen::MatrixXd A)
{
  this->A = A;
  this->dt = dt;
  update(y);
}

void fromTFTransformToEigen(const tf::Transform& tf, Eigen::VectorXd& eigenvec);
{
  eigenvec = Eigen::VectorXd(6);
  tf::Vector3 tf_xyz = tf.getOrigin();
  eigenvec[0] = tf_xyz.x();
  eigenvec[1] = tf_xyz.y();
  eigenvec[2] = tf_xyz.z();
  tf.getBasis().getRPY(eigenvec[3], eigenvec[4], eigenvec[5], 1);
}
} // namespace PoseOptimizer
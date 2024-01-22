/* 
 * File:   UTLN_KalmanFilter.h
 * Author: valen
 *
 * Created on 2 mai 2023, 12:29
 */

#ifndef UTLN_KALMANFILTER_H
#define	UTLN_KALMANFILTER_H


void UpdateThetaUsingGyro();
void UpdateVxVyUsingAccelAndOdometry();
void UpdateXYUsingKalmanSpeed();
void UpdateXYOnGNSS(float xGNSS, float yGNSS);
void UpdateThetaOnGNSS(float thetaGNSS);
void SetAlphaTheta(float value);
void SetAlphaSpeed(float value);
void SetAlphaPosition(float value);

#endif	/* UTLN_KALMANFILTER_H */


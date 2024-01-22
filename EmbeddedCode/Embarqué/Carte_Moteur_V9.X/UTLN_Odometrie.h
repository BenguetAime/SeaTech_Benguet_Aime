/* 
 * File:   UTLN_Odometrie.h
 * Author: Barchasz
 *
 * Created on 28 janvier 2020, 16:45
 */

#ifndef UTLN_ODOMETRIE_H
#define	UTLN_ODOMETRIE_H

typedef enum{
    TWO_WHEELS,
    FOUR_WHEELS,
} PropulsionMode;

void QEIUpdateRawData(void);
void ComputePolarSpeed4WheelsFromQEI();
void ComputePolarSpeed2WheelsFromQEI();
void ComputeSpeedAuxiliaryMotorsFromQEI(void);
void Init4WheelsToPolarMatrix(double mx1, double mx2, double mx3, double mx4,
                              double my1, double my2, double my3, double my4,
                              double mtheta1, double mtheta2, double mtheta3, double mtheta4);
void InitPolarTo4WheelsMatrix(double m1x, double m1y, double m1t,
                              double m2x, double m2y, double m2t,
                              double m3x, double m3y, double m3t,
                              double m4x, double m4y, double m4t);
void Init2WheelsToPolarMatrix(double mx1, double mx2,
                              double mtheta1, double mtheta2);
void TransformSpeed4WheelsToPolar();
double ConvertSpeedFromPolaireToIndependant(double vitesseX, double vitesseY, double vitesseTheta, int motorNumber); 
void TransformSpeed2Wheels();
void ComputePolarSpeed4WheelsFromC620(void);
void SetPointToPositionSingleMotor(uint8 nbMotor, float value);

#endif	/* UTLN_ODOMETRIE_H */


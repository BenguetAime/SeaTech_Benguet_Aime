#include "Utilities.h"
#include "math.h"
#include "Define.h"
#include <xc.h>

float getFloat(unsigned char *p, int index) {
    float *result_ptr = (float*) (p + index);
    float result = *result_ptr;
    return result;
}

float getFloatSafe(unsigned char *p, int index) {
    unsigned char tmp[4];
    tmp[0] = p[0 + index];
    tmp[1] = p[1 + index];
    tmp[2] = p[2 + index];
    tmp[3] = p[3 + index];
    float *result_ptr = (float*) (tmp);
    float result = *result_ptr;
    return result;
}

double getDouble(unsigned char *p, int index) {
    double *result_ptr = (double*) (p + index);
    return *result_ptr;
}

void getBytesFromFloat(unsigned char *p, int index, float f) {
    int i;
    unsigned char *f_ptr = (unsigned char*) &f;
    for (i = 0; i < 4; i++)
        p[index + i] = f_ptr[i];
}

void getBytesFromInt32(unsigned char *p, int index, int32_t in) {
    int i;
    unsigned char *f_ptr = (unsigned char*) &in;
    for (i = 0; i < 4; i++)
        p[index + i] = f_ptr[3 - i];
}

void getBytesFromDouble(unsigned char *p, int index, double d) {
    int i;
    unsigned char *f_ptr = (unsigned char*) &d;
    for (i = 0; i < 8; i++)
        p[index + i] = f_ptr[i];
}

double LimitToInterval(double value, double min, double max) {
    if (value < min)
        return min;
    else if (value > max)
        return max;
    else
        return value;
}

double Modulo2PIAngleRadian(double angleRadian) {
    double angleTemp = fmod(angleRadian - M_PI, M_2PI) + M_PI;
    return fmod(angleTemp + M_PI, M_2PI) - M_PI;
}

float Modulo2PiAroundAngle(float angleToCenterAround, float angleToCorrect) {
    // On corrige l'angle obtenu pour le moduloter autour de l'angle Kalman
    int decalageNbTours = (int) UTLN_roundf((angleToCorrect - angleToCenterAround) / (M_2PI));
    float thetaDest = angleToCorrect - decalageNbTours * M_2PI;
    return thetaDest;
}

int UTLN_roundf(float number)
{
    return (number >= 0) ? (int)(number + 0.5) : (int)(number - 0.5);
}
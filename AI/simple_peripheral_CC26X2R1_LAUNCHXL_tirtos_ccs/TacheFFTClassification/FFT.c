/*
 * FFT.c
 *
 *  Created on: 20 nov. 2019
 *      Author: sebas
 */



#include <TacheFFTClassification/FFT.h>
#include "stdio.h"
#include "stdlib.h"
#include "math.h"


#define  swap(a,b)  norm=(a); (a)=(b); (b)=norm

//reel[] et imag[i] sont la liste des réelles et des imaginaires
// sign = 1 donne la transformée de Fourier
// sign = -1 donne la transformée de Fourier inverse

void fft(float *reel, float *imag, int log2n, int sign) {

  int n, m, m2, i, j, k, l;
  float  c1, c2, norm, norm2, cphi, sphi;

  n = 1<<log2n;

  /* Inversement des bits */
  for(i=0; i<n; i++) {

    for(j=log2n-1, m=0, k=i; j>=0; j--, k>>=1) m += (k&1)<<j;

    if(m>i) {
      swap(reel[i],reel[m]);
      swap(imag[i],imag[m]);
    }
  }

  /* normalisation de la transformée de Fourier */
  norm = 1.0/sqrtf((float)n);
  for(i=0; i<n ;i++) {
    reel[i] *= norm;
    imag[i] *= norm;
  }

  /* calcul de la FFT */
  for(j=0; j < log2n; j++)
  {
      m = 1<<j;  m2 = 2*m;
      c1 = 1.0;
      c2 = 0.0;
      cphi = cos(sign*2.0*M_PI/((float)m2));
      sphi = sin(sign*2.0*M_PI/((float)m2));
      for(k=0; k<m; k++)
      {
          for(i=k; i<n; i+=m2)
          {
              l = i + m;
              norm  = c1*reel[l] - c2*imag[l];
              norm2 = c1*imag[l] + c2*reel[l];
              reel[l] = reel[i] - norm;
              imag[l] = imag[i] - norm2;
              reel[i] += norm;
              imag[i] += norm2;
          }
          norm  = c1*cphi - c2*sphi; // Calcul de exp(2 pi k/m) avec        norm2 = c1*sphi + c2*cphi; // le théorème d'addition
          c1 = norm;  c2 = norm2;
      }
  }
}

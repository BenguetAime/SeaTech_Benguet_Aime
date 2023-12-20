/*
 * Filter.h
 *
 *  Created on: 11 janv. 2022
 *      Author: Valentin
 */

#ifndef TACHEFE_ALGORITHMS_FILTER_H_
#define TACHEFE_ALGORITHMS_FILTER_H_

enum typeFiltre{
    HP,
    LP,
    BP,
};

typedef struct
{
  float alpha1;
  float beta0;
  float beta1;

  float s1;
  float e1;

  enum typeFiltre type;

  unsigned char reset;
} Order1Filter; // Filtre d'ordre 1

void InitOrder1LPFilterEuler(Order1Filter* filter, float freqCoupure, float freqSampling);
void InitOrder1HPFilterEuler(Order1Filter* filter, float freqCoupure, float freqSampling);
float ComputeOrder1Filter(Order1Filter* filter, float input);

#endif /* TACHEFE_ALGORITHMS_FILTER_H_ */

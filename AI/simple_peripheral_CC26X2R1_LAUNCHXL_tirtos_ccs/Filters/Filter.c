/*
 * Filter.c
 *
 *  Created on: 11 janv. 2022
 *      Author: Valentin
 */

#include "Filter.h"

void InitOrder1LPFilterEuler(Order1Filter* filter, float freqCoupure, float freqSampling)
{
    //S(n) = alpha1 * S(n-1) + beta0 * E(n) + beta1 * E(n-1)
    float Tau = 1/freqCoupure;
    float Te = 1/freqSampling;
    filter->alpha1 = Tau/(Te+Tau);
    filter->beta0 = Te/(Te+Tau);
    filter->beta1 = 0;

    filter->s1 = 0;
    filter->reset = 1; //Demande un reset de l'entrée à la prochaine itération
}

void InitOrder1HPFilterEuler(Order1Filter* filter, float freqCoupure, float freqSampling)
{
    //S(n) = alpha1 * S(n-1) + beta0 * E(n) + beta1 * E(n-1)
    float Tau = 1/freqCoupure;
    float Te = 1/freqSampling;
    filter->alpha1 = 1-(Te/Tau);
    filter->beta0 = 1-(Te/Tau);
    filter->beta1 = -(1-(Te/Tau));

    filter->s1 = 0;
    filter->reset = 1; //Demande un reset de l'entrée à la prochaine itération
}

float ComputeOrder1Filter(Order1Filter* filter, float input)
{
    //En cas de reset demandé, on initialise l'état interne du filtre
    if(filter->reset==1)
    {
        switch(filter->type)
        {
            case LP:
                filter->s1 = input;
            break;

            case HP:
                filter->s1 = 0;
            break;

            default:
                filter->s1 = 0;
            break;
        }
        filter->reset = 0;
    }

    //On calcule la sortie du filtre
    float output = filter->alpha1 * filter->s1 + filter->beta0 * input + filter->beta1 * filter->e1;

    //On met à jour les variables internes pour la fois suivante
    filter->s1 = output;
    filter->e1 = input;

    return output;
}



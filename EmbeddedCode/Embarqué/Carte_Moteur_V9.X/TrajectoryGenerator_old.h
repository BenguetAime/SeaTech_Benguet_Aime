#ifndef TRAJECTORYGENERATOR_H
#define	TRAJECTORYGENERATOR_H

/******************************************************************************/
/* PUBLIC ATTRIBUES                                                           */
/******************************************************************************/

/**
 * Used by trajectory generator
 */
typedef struct TrajectoryParametersBITS{
    union {
        struct {
            float xDistanceCouranteTrajectoireTheorique;
            float yDistanceCouranteTrajectoireTheorique;
            float thetaAngleCourantTrajectoireTheorique;
            float xPosTheorique;
            float yPosTheorique;
            float xVitesseTrajectoireTheorique;
            float yVitesseTrajectoireTheorique;
            float thetaVitesseTrajectoireTheorique;
                        
            int isMovementFinishing;
            int isMovementFinished;  
        };
    };
} TrajectoryParameters;

/******************************************************************************/
/* PUBLIC DECLARATION                                                         */
/******************************************************************************/

void TrajectoryGeneratorSetParameters(float xMaxSpeed, float yMaxSpeed, float thetaMaxSpeed, float xAccel, float yAccel, float thetaAccel);
void TrajectoryGeneratorSetTrajectoryType(int chainedMode);
void TrajectoryGeneratorInitNewTrajectory(float distanceX, float distanceY, float angle);
void TrajectoryGeneratorGetNextPosition();
void TrajectoryGeneratorSetTrajectoryType(int chainedMode);
void TrajectoryGeneratorSetPositionToCurrentPosition();
void SendTrajectoryData();

#endif	/* TRAJECTORYGENERATOR_H */


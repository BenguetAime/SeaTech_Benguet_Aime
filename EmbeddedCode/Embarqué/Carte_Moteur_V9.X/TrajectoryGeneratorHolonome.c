#include "TrajectoryGeneratorHolonome.h"
#include "Asservissement.h"
#include "Define.h"
#include "RobotObj.h"
#include "UTLN_Communication.h"

float accelLineaireMax = 0; //en m.s-2
float accelLineaireFreinageMax = 0; //en m.s-2
float accelRotationCapVitesseMax = 0; //en rad.s-2
float accelRotationOrientationRobotMax = 0; //en rad.s-2

float vitesseLineaireMax = 0; //en m.s-1
float vitesseRotationCapVitesseMax = 0; //en rad.s-1
float vitesseRotationOrientationRobotMax = 0; //en rad.s-1

float capVitesseRefTerrain = 0;
float vitesseRotationCapVitesse = 0;

volatile PidCorrector PID_X;
volatile PidCorrector PID_Y;
volatile PidCorrector PID_Theta;

//volatile Location currentLocationRefTerrain;


void SetTrajectoryAccelerationSpeedConstants(float accelLineaire, float accelRotationCap, float accelRotationOrientation,
        float vitesseLineaire, float vitesseRotationCap, float vitesseRotationOrientation, float accelLineaireFreinage) {
    accelLineaireMax = accelLineaire; //en m.s-2
    accelRotationCapVitesseMax = accelRotationCap; //en rad.s-2
    accelLineaireFreinageMax=accelLineaireFreinage;
    accelRotationOrientationRobotMax = accelRotationOrientation; //en rad.s-2

    vitesseLineaireMax = vitesseLineaire; //en m.s-1
    vitesseRotationCapVitesseMax = vitesseRotationCap; //en rad.s-1
    vitesseRotationOrientationRobotMax = vitesseRotationOrientation; //en rad.s-1
}

void InitRobotPosition(float x, float y, float theta) {

    robotState.xPositionRefTerrain = x;
    robotState.yPositionRefTerrain = y;
    robotState.thetaPositionRefTerrain = theta;
    robotState.xGhost = x;
    robotState.yGhost = y;
    robotState.thetaGhost = theta;
    robotState.vxGhost = 0;
    robotState.vyGhost = 0;
    robotState.vthetaGhost = 0;

    PIDSpeedReset();
    SetSpeedConsigneHolonome(0, 0, 0);
    PIDPositionReset();
}

void ResetGhostToCurrentPosition()
{
    robotState.xGhost = robotState.xPositionRefTerrain;
    robotState.yGhost = robotState.yPositionRefTerrain;
    robotState.thetaGhost = robotState.thetaPositionRefTerrain;
    robotState.vxGhost = 0;
    robotState.vyGhost = 0;
    robotState.vthetaGhost = 0;
    PIDSpeedReset();
    SetSpeedConsigneHolonome(0, 0, 0);
    PIDPositionReset();
}

void SetWaypointLocation(float x, float y, float theta) {

    robotState.xWaypoint = x;
    robotState.yWaypoint = y;
    robotState.thetaWaypoint = theta;
}

void PIDSpeedReset() {
    ResetCorrecteur(&(robotState.PidSpeedX), 0);
    ResetCorrecteur(&(robotState.PidSpeedY), 0);
    ResetCorrecteur(&(robotState.PidSpeedTheta), 0);
}

void PIDPositionReset() {
    ResetCorrecteur(&PID_X, 0);
    ResetCorrecteur(&PID_Y, 0);
    ResetCorrecteur(&PID_Theta, 0);
}

void PIDSetup( PidCorrector* p_pid, float kp, float ki, float kd, float kpLimit, float kiLimit, float kdLimit)
{
    p_pid->Kp = kp;
    p_pid->Ki = ki;
    p_pid->Kd = kd;
    p_pid->erreurProportionelleMax = kpLimit;
    p_pid->erreurIntegraleMax = kiLimit;
    p_pid->erreurDeriveeMax = kdLimit;
}

void CalculateGhostPosition() {
    /************************* D�but du calcul pr�liminaire des infos utilis�es ensuite ****************************/

    //Calcul du cap du Waypoint dans les r�f�rentiel terrain et robot
    float CapWayPointRefTerrain;
    if (robotState.xWaypoint - robotState.xGhost != 0)
        CapWayPointRefTerrain = atan2(robotState.yWaypoint - robotState.yGhost, robotState.xWaypoint - robotState.xGhost);
    else
        CapWayPointRefTerrain = atan2(robotState.yWaypoint - robotState.yGhost, 0.0001);

    //double CapWayPointRefRobot = CapWayPointRefTerrain - ghostLocation.Theta;
    CapWayPointRefTerrain = Modulo2PiAroundAngle(capVitesseRefTerrain, CapWayPointRefTerrain);

    //Calcul de l'�cart de cap
    float ecartCapVitesse = CapWayPointRefTerrain - capVitesseRefTerrain;

    robotState.thetaGhost = Modulo2PiAroundAngle(robotState.thetaWaypoint, robotState.thetaGhost);
    float ecartOrientationRobot = robotState.thetaWaypoint - robotState.thetaGhost;

    //Calcul de la distance au WayPoint
    float distanceWayPoint = sqrt(pow(robotState.yWaypoint - robotState.yGhost, 2) + pow(robotState.xWaypoint - robotState.xGhost, 2));

    //Calcul de la vitesse lin�aire du robot
    float vitesseLineaireRobot = sqrt(pow(robotState.vxGhost, 2) + pow(robotState.vyGhost, 2));

    //Calcul de la distance de freinage 
    float distanceFreinageLineaire = pow(vitesseLineaireRobot, 2) / (2 * accelLineaireFreinageMax);

    /* Fin du calcul des vari�ables interm�diaires */
    /************************ Ajustement de la vitesse lin�aire du robot *******************************/
    // Si le robot a un cap vitesse � peu pr�s align� sur son Waypoint ou une vitesse presque nulle 
    // et que la distance au Waypoint est sup�rieure � la distance de freinage : on acc�l�re en lin�aire
    // sinon on freine
    float nouvelleVitesseLineaire;

    if (ABS(ecartCapVitesse) < M_PI_DIV2) //Le WayPoint est devant
    {
        if (distanceWayPoint > distanceFreinageLineaire)
            nouvelleVitesseLineaire = MIN(vitesseLineaireMax, vitesseLineaireRobot + accelLineaireMax * T_ECH_QEI); //On acc�l�re
        else
            //On d�termine la valeur du freinage en fonction des conditions
            nouvelleVitesseLineaire = MAX(0, vitesseLineaireRobot - accelLineaireFreinageMax * T_ECH_QEI); //On freine
    } else //Le WayPoint est derri�re
    {
        if (distanceWayPoint > distanceFreinageLineaire)
            nouvelleVitesseLineaire = MAX(-vitesseLineaireMax, vitesseLineaireRobot - accelLineaireMax * T_ECH_QEI); //On acc�l�re
        else
            //On d�termine la valeur du freinage en fonction des conditions
            nouvelleVitesseLineaire = MIN(0, vitesseLineaireRobot + accelLineaireFreinageMax * T_ECH_QEI); //On freine
    }

    float ecartCapModuloPi = Modulo2PIAngleRadian(ecartCapVitesse);

    /************************ Rotation du vecteur vitesse lin�aire du robot *******************************/
    //Si le robot a un �cart de cap vitesse sup�rieur � l'angle de freinage en rotation de cap vitesse, on acc�l�re la rotation, sinon on freine

    float accelRotationCapVitesse = accelRotationCapVitesseMax / pow(MAX(0.01, nouvelleVitesseLineaire / vitesseLineaireMax), 2);

    float angleArretRotationCapVitesse = pow(vitesseRotationCapVitesse, 2) / (2 * accelRotationCapVitesse);
    if (ecartCapModuloPi > 0) {
        if (ecartCapModuloPi > angleArretRotationCapVitesse)
            vitesseRotationCapVitesse = MIN(vitesseRotationCapVitesseMax, vitesseRotationCapVitesse + accelRotationCapVitesse * T_ECH_QEI); //on acc�l�re
        else
            vitesseRotationCapVitesse = MAX(0, vitesseRotationCapVitesse - accelRotationCapVitesse * T_ECH_QEI); //On freine
    } else {
        if (ecartCapModuloPi < -angleArretRotationCapVitesse)
            vitesseRotationCapVitesse = MAX(-vitesseRotationCapVitesseMax, vitesseRotationCapVitesse - accelRotationCapVitesse * T_ECH_QEI); //On acc�l�re en n�gatif
        else
            vitesseRotationCapVitesse = MIN(0, vitesseRotationCapVitesse + accelRotationCapVitesse * T_ECH_QEI); //On freine en n�gatif
    }

    //On regarde si la vitesse lin�aire est �lev�e ou pas. 
    //Si c'est le cas, on update le cap vitesse normalement en rampe
    //Sinon, on set le capvitesse � la valeur du cap WayPoint directement
    //if (vitesseLineaireRobot > 0.0) //0.5 avant
    capVitesseRefTerrain += vitesseRotationCapVitesse * T_ECH_QEI;
    //else
    //{
    //    capVitesseRefTerrain = CapWayPointRefTerrain; //Si la vitesse lin�aire est faible, on tourne instantan�ment
    //    vitesseRotationCapVitesse = 0;
    //}

    /************************ Orientation angulaire du robot *******************************/
    float angleArretRotationOrientationRobot = pow(robotState.vthetaGhost, 2) / (2 * accelRotationOrientationRobotMax);
    float nouvelleVitesseRotationOrientationRobot = 0;
    if (ecartOrientationRobot > 0) {
        if (ecartOrientationRobot > angleArretRotationOrientationRobot)
            nouvelleVitesseRotationOrientationRobot = MIN(vitesseRotationOrientationRobotMax, robotState.vthetaGhost + accelRotationOrientationRobotMax * T_ECH_QEI); //on acc�l�re
        else
            nouvelleVitesseRotationOrientationRobot = MAX(0, robotState.vthetaGhost - accelRotationOrientationRobotMax * T_ECH_QEI); //On freine
    } else {
        if (ecartOrientationRobot < -angleArretRotationOrientationRobot)
            nouvelleVitesseRotationOrientationRobot = MAX(-vitesseRotationOrientationRobotMax, robotState.vthetaGhost - accelRotationOrientationRobotMax * T_ECH_QEI); //On acc�l�re en n�gatif
        else
            nouvelleVitesseRotationOrientationRobot = MIN(0, robotState.vthetaGhost + accelRotationOrientationRobotMax * T_ECH_QEI); //On freine en n�gatif
    }

    /// Gestion des ordres d'arr�t global des robots 
    robotState.vxGhost = nouvelleVitesseLineaire * cos(capVitesseRefTerrain);
    robotState.vyGhost = nouvelleVitesseLineaire * sin(capVitesseRefTerrain);
    robotState.vthetaGhost = nouvelleVitesseRotationOrientationRobot;
    
    //Nouvelle orientation du robot
    robotState.xGhost += robotState.vxGhost * T_ECH_QEI;
    robotState.yGhost += robotState.vyGhost * T_ECH_QEI;
    robotState.thetaGhost += robotState.vthetaGhost * T_ECH_QEI;

    //OnGhostLocation(robotId, ghostLocationRefTerrain);
}

void UpdateAsservissementPosition()
{
    float erreurXRefTerrain = robotState.xGhost - robotState.xPositionRefTerrain;
    float erreurYRefTerrain = robotState.yGhost - robotState.yPositionRefTerrain;
    
    ///Attention, ne pas toucher la currentLocationRefTerrain -> Mod Ghost
    float currentLocationRefTerrainModGhost = Modulo2PiAroundAngle(robotState.thetaGhost, robotState.thetaPositionRefTerrain);
    float erreurTheta = robotState.thetaGhost - currentLocationRefTerrainModGhost;
    
    //Changement de rep�re car les asservissements se font dans le r�f�rentiel du robot
    float erreurXRefRobot = erreurXRefTerrain * cos(robotState.thetaPositionRefTerrain) + erreurYRefTerrain * sin(robotState.thetaPositionRefTerrain);
    float erreurYRefRobot = -erreurXRefTerrain * sin(robotState.thetaPositionRefTerrain) + erreurYRefTerrain * cos(robotState.thetaPositionRefTerrain);

    float xSpeedConsigne = Correcteur(&PID_X, erreurXRefRobot);
    float ySpeedConsigne = Correcteur(&PID_Y, erreurYRefRobot);
    float thetaSpeedConsigne = Correcteur(&PID_Theta, erreurTheta);
    
    //Ajout du feed forward
    xSpeedConsigne += robotState.vxGhost * cos(robotState.thetaPositionRefTerrain) + robotState.vyGhost * sin(robotState.thetaPositionRefTerrain); 
    ySpeedConsigne += -robotState.vxGhost * sin(robotState.thetaPositionRefTerrain) + robotState.vyGhost * cos(robotState.thetaPositionRefTerrain); 
    thetaSpeedConsigne += robotState.vthetaGhost;
    
    //S�curit� en cas de blocage
    //On regarde si la position du robot est proche de la position du ghost
    float seuilToleranceEcartLineaireGhost = 10;
    float seuilToleranceEcartAngulaireGhost = DegreeToRadian(360);
    
    if ((sqrt(pow(erreurXRefTerrain, 2) + pow(erreurYRefTerrain, 2)) < seuilToleranceEcartLineaireGhost) && 
            (ABS(erreurTheta) < seuilToleranceEcartAngulaireGhost))
    {
        //Si c'est le cas, le robot n'a pas rencontr� de probl�me, on envoie les vitesses consigne.                
        SetSpeedConsigneHolonome(xSpeedConsigne, ySpeedConsigne, thetaSpeedConsigne);   
        //OnLocalWorldMapDisplayTrigger(robotId);     
    }
    else
    {
        //Sinon, le robot a rencontr� un obstacle ou eu un probl�me, on arr�te le robot et on r�initialise les correcteurs et la ghostLocation
        /// OnCollision(robotId, currentLocationRefTerrain); TODO : modif pour valider Kalman, � r�int�grer peut �tre
        SetSpeedConsigneHolonome(0,0,0);
        //OnLocalWorldMapDisplayTrigger(robotId);
        
        robotState.xGhost = robotState.xPositionRefTerrain;
        robotState.yGhost = robotState.yPositionRefTerrain;
        robotState.thetaGhost = robotState.thetaPositionRefTerrain;
        
        PIDPositionReset();
        PIDSpeedReset();
    }
}



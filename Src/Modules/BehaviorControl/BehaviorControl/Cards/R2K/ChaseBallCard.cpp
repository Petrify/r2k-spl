/**
 * @file ChaseBallCard.cpp
 * @author Niklas Schmidts, Adrian Müller   
 * @brief Allows Offenseplayer to chase the Ball and kick to goal
 * @version 1.2
 * @date 2023-01-06
 * 
 * Functions, values, side effects: 
 * OffensePlayer tries to catch the ball (ie ´walks in this direction) if
 * - ball is nearer to opponent goal as his x postion (minus threshold)
 * - player is in range from middle line - threshold, or closer to opp. goal
 * - ignores thePlayerRole.playsTheBall()
 * 
 * 
 * Details
 * if bot is closest to ball (playsTheBall()) card ShootAtGoalCard will take over
 * * 
 * 
 * v1.1. avoid that our  offense bots struggle for ball. loop over buddies -> 
 *      if BehaviorStatus::chaseBallCard or clearOwnHalfCard or clearOwnHalfCardGoalie exit this card
 * 
 * v.1.2 card now checks wether there is an passing event active (OffenseForwardPassCard, OffenseReceivePassCard)
    
 * - Check: GoalShot has higher priority and takes over close to opp.goal
 * v 1.3 DEFENSE only x < 0 - threshold
 */

// Skills - Must be included BEFORE Card Base
#include "Representations/BehaviorControl/Skills.h"

// Card Base
#include "Tools/BehaviorControl/Framework/Card/Card.h"
#include "Tools/BehaviorControl/Framework/Card/CabslCard.h"

// Representations
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Configuration/FieldDimensions.h"
#include "Representations/Communication/RobotInfo.h"
#include "Representations/BehaviorControl/FieldBall.h"
#include "Representations/BehaviorControl/PlayerRole.h"
#include "Representations/Communication/TeamData.h"
#include "Representations/BehaviorControl/TeammateRoles.h"
#include "Representations/Communication/GameInfo.h"



CARD(ChaseBallCard,
     {
        ,
        CALLS(Activity),
        CALLS(LookForward),
        CALLS(GoToBallAndDribble),
        CALLS(WalkAtRelativeSpeed),
        USES(GameInfo),
        REQUIRES(ObstacleModel),
        REQUIRES(TeamBehaviorStatus),
        REQUIRES(RobotPose),
        REQUIRES(RobotInfo),
        REQUIRES(FieldBall),
        REQUIRES(FieldDimensions),
        REQUIRES(TeamData),   // check behavior
        REQUIRES(TeammateRoles),

        DEFINES_PARAMETERS(
             {,
                //Define Params here
                (float)(0.8f) walkSpeed,
                (int)(7000) ballNotSeenTimeout,
                (int)(1000) threshold,
             }),

     });

class ChaseBallCard : public ChaseBallCardBase
{

  bool preconditions() const override
  {  
    //Abfragen Spielerposition
   
    //Vergleich ob die Spielerposition in der Opponentside liegt
    //mit einem threshold damit Stürmer noch teils ins eigene Feld darf
  
    return
      (
        //!aBuddyIsChasingOrClearing() && // prevent bots to cluster at ball
        theTeammateRoles.isTacticalOffense(theRobotInfo.number) && // OFFENSE_RIGHT, OFFENSE_MIDDLE, OFFENSE_LEFT
        theFieldBall.positionOnField.x() > (0 - threshold)
        // theFieldBall.positionOnField.x() >= theRobotPose.translation.x() - threshold;
        )
      ||
      (theGameInfo.setPlay == SET_PLAY_NONE &&
        // !aBuddyIsClearingOwnHalf() &&
        //theTeammateRoles.playsTheBall(theRobotInfo.number) &&  // I am the striker
        theObstacleModel.opponentIsClose(800) &&  // see LongShotCard, !opponentIsTooClose()
        theTeammateRoles.isTacticalDefense(theRobotInfo.number) && // my recent role
        theFieldBall.endPositionOnField.x() < (0 - threshold) &&
        !(theTeamBehaviorStatus.teamActivity == TeamBehaviorStatus::R2K_SPARSE_GAME));
  }

  bool postconditions() const override
  {
    return !preconditions();
  }

  option
  {
    theActivitySkill(BehaviorStatus::chaseBallCard);

   initial_state(goToBallAndDribble)
    {
      transition
      {
        if(!theFieldBall.ballWasSeen(ballNotSeenTimeout))
          goto searchForBall;
      }

        action
      {
        // theGoToBallAndKickSkill(calcAngleToGoal(), KickInfo::walkForwardsLeft);
        // SKILL_INTERFACE(GoToBallAndDribble, (Angle) targetDirection, (bool)(false) alignPrecisely, (float)(1.f) kickPower, (bool)(true) preStepAllowed, (bool)(true) turnKickAllowed, (const Rangea&)(Rangea(0_deg, 0_deg)) directionPrecision);

        theGoToBallAndDribbleSkill(calcAngleToGoal(),true);
      }
    }

    state(searchForBall)
    {
      transition
      {
        if(theFieldBall.ballWasSeen())
          goto goToBallAndDribble;
      }

      action
      {
        theLookForwardSkill();
        theWalkAtRelativeSpeedSkill(Pose2f(walkSpeed, 0.f, 0.f));
      }
    }
  }

    Angle calcAngleToGoal() const
  {
    return (theRobotPose.inversePose * Vector2f(theFieldDimensions.xPosOpponentGroundLine, 0.f)).angle();
  }

    Angle calcAngleToBall() const
  {
    return (theRobotPose.inversePose * Vector2f(theFieldBall.endPositionOnField.x(), theFieldBall.endPositionOnField.y())).angle();
  }

    bool aBuddyIsChasingOrClearing() const
    {
      for (const auto& buddy : theTeamData.teammates) 
      {
        if (buddy.theBehaviorStatus.activity == BehaviorStatus::chaseBallCard ||
          buddy.theBehaviorStatus.activity == BehaviorStatus::clearOwnHalfCard ||
          buddy.theBehaviorStatus.activity == BehaviorStatus::clearOwnHalfCardGoalie ||
          buddy.theBehaviorStatus.activity == BehaviorStatus::offenseForwardPassCard ||
          buddy.theBehaviorStatus.activity == BehaviorStatus::offenseReceivePassCard)
          return true;
      }
      return false;
    }
};

MAKE_CARD(ChaseBallCard);

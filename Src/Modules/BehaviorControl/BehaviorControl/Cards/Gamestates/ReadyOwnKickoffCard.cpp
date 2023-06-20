/**
 * @file ReadyOwnKickoffCard.cpp
 * @author Andy Hobelsberger
 * @brief Set individual ready behavior for the own Kickoff
 * @version 1.2
 * @date 2022-03-16
 * 
 * Behavior:
 * Sets up Robot 5 Behind the Ball so that it may be kicked towards the opponents Field. 
 * 
 * v1.1: Card dynamically select robot instead hardcoding number  (Adrian)
 * v1.2. Card migrated (Adrian
 * v1.3. using offenseIndex for qualifiying (Asrar)
 */

#include "Representations/BehaviorControl/DefaultPose.h"
#include "Representations/BehaviorControl/Skills.h"
#include "Representations/Communication/GameInfo.h"
#include "Representations/Communication/TeamInfo.h"
#include "Representations/Configuration/GlobalOptions.h"
#include "Representations/Communication/RobotInfo.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/BehaviorControl/TeamBehaviorStatus.h" 
#include "Representations/BehaviorControl/TeammateRoles.h"

#include "Tools/Math/Geometry.h"
#include "Tools/BehaviorControl/Framework/Card/Card.h"
#include "Tools/BehaviorControl/Framework/Card/CabslCard.h"


CARD(ReadyOwnKickoffCard,
  {,
    CALLS(Activity),
    CALLS(LookActive),
    CALLS(WalkToPoint),
    REQUIRES(DefaultPose),
    REQUIRES(GameInfo),
    REQUIRES(OwnTeamInfo),
    REQUIRES(RobotPose),
  REQUIRES(RobotInfo),
  REQUIRES(TeammateRoles),
  });

class ReadyOwnKickoffCard : public ReadyOwnKickoffCardBase
{
  /**
   * @brief walk to default position and exit to DefaultCards
   */
  bool preconditions() const override
  {
    return  theGameInfo.kickingTeam == theOwnTeamInfo.teamNumber 
      && theGameInfo.state == STATE_READY;
  }

  /**
   * @brief The condition that needs to be met to exit the this card
   */
  bool postconditions() const override
  {
    return !preconditions();
  }

  void execute() override
  {

    theActivitySkill(BehaviorStatus::defaultBehavior);
    Vector2f targetAbsolute = theDefaultPose.ownDefaultPose.translation + Vector2f(+450.f, 0);

    int nOffenseFound = 0;

    switch (theTeammateRoles.offenseRoleIndex(theRobotInfo.number)) {
    case 0: // right-most offense
      targetAbsolute = Vector2f(-500, 0);
      break;
    case 1: // 
      targetAbsolute = Vector2f(-700, -2000);
      break;
    case 2: // OFFENSIVE MODE - we have 3 offense
      targetAbsolute = Vector2f(-700, 2000);
      break;
    }

    Vector2f targetRelative = theRobotPose.toRelative(targetAbsolute);

    theLookActiveSkill(); // Head Motion Request
    theWalkToPointSkill(Pose2f(theDefaultPose.ownDefaultPose.rotation - theRobotPose.rotation, targetRelative), 1.0f, true);

  }
  
};

MAKE_CARD(ReadyOwnKickoffCard);
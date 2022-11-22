/**
 * @file OppKickoffCard.cpp
 * @author Andy Hobelsberger
 * @brief Covers Opponent Free Kick
 * @version 1.1
 * @date 2022-11-22
 *  
 * Makes the Offensive and Defensive robots wait until the ball has been Kicked after Kickoff
 * 
 * V1.1 Card migrated (Nicholas)
 */

#include "Tools/BehaviorControl/Framework/Card/Card.h"
#include "Tools/BehaviorControl/Framework/Card/CabslCard.h"

#include "Representations/BehaviorControl/Skills.h"
#include "Representations/BehaviorControl/FieldBall.h"
#include "Representations/BehaviorControl/TeamBehaviorStatus.h"

#include "Representations/Configuration/FieldDimensions.h"

#include "Representations/Communication/GameInfo.h"
#include "Representations/Communication/TeamInfo.h"
#include "Representations/Communication/RobotInfo.h"

#include "Representations/Modeling/RobotPose.h"

#include "Tools/Math/Geometry.h"


CARD(OppKickoffCard,
{,
  CALLS(Stand),
  CALLS(Activity),
  CALLS(LookForward),
  CALLS(GoToBallAndKick),

  REQUIRES(FieldBall),
  REQUIRES(RobotPose),
  REQUIRES(RobotInfo),
  REQUIRES(FieldDimensions),
  REQUIRES(OwnTeamInfo),
  REQUIRES(GameInfo),
  REQUIRES(TeamBehaviorStatus),
  REQUIRES(TeammateRoles),

  DEFINES_PARAMETERS(
  {,
    (float)(200.0f) ballKickedThreshold,
  }),
});

class OppKickoffCard : public OppKickoffCardBase
{
  /**
   * @brief 
   * IF the other Team is kicking 
   * AND It's the first 10 seconds of the game
   * AND we're in the playing state
   * AND the ball is still near the cneter point
   * AND the Robot is not the Goalie
   */
  bool preconditions() const override
  {
    return theGameInfo.kickingTeam != theOwnTeamInfo.teamNumber
      && theGameInfo.secsRemaining >= 590
      && theGameInfo.state == STATE_PLAYING
      && theFieldBall.positionOnField.norm() < ballKickedThreshold
      && !theTeammateRoles.isTacticalGoalKeeper(theRobotInfo.number);
  }

  /**
   * @brief Not Precondition
   */
  bool postconditions() const override
  {
    return !preconditions();
  }

  option
  {
    theActivitySkill(BehaviorStatus::oppKickoff);
    
    initial_state(init)
    {
      transition
      {
        if (theTeammateRoles.isTacticalOffense(theRobotInfo.number)) {
          goto forward;
        } else if (theTeammateRoles.isTacticalDefense(theRobotInfo.number)) {
          goto back;
        }
      }

      action
      {
        theLookForwardSkill();
        theStandSkill();
      }
    }

    state(forward)
    {
      transition
      {}

      action
      {
        theLookForwardSkill();
        theStandSkill();
      }
    }

    state(back)
    {
      transition
      {}

      action
      {
        theLookForwardSkill();
        theStandSkill();
      }
    }
  }
};

MAKE_CARD(OppKickoffCard);
#ifndef AIKIDO_PLANNER_SNAP_PLANNER_HPP_
#define AIKIDO_PLANNER_SNAP_PLANNER_HPP_

#include <memory>
#include <exception>
#include <stdexcept>
#include <vector>
#include "PlanningResult.hpp"
#include "../constraint/TestableConstraint.hpp"
#include "../distance/DistanceMetric.hpp"
#include "../statespace/StateSpace.hpp"
#include "../statespace/Interpolator.hpp"
#include "../util/VanDerCorput.hpp"

namespace aikido
{
namespace planner
{
using Trajectory = std::vector<aikido::statespace::StateSpace::ScopedState>;

static Trajectory planSnap(
    const aikido::statespace::StateSpace::State *startState,
    const aikido::statespace::StateSpace::State *goalState,
    const std::shared_ptr<aikido::statespace::StateSpace> stateSpace,
    const std::shared_ptr<aikido::constraint::TestableConstraint> constraint,
    const std::shared_ptr<aikido::statespace::Interpolator> interpolator,
    aikido::planner::PlanningResult* planningResult)
{
  if (stateSpace != constraint->getStateSpace()) {
    throw std::invalid_argument(
        "StateSpace of constraint not equal to StateSpace of planning space");
  }
  aikido::util::VanDerCorput vdc{1, true, 0.02};  // TODO junk resolution
  Trajectory trajectory;
  auto testState = stateSpace->createState();

  for (double alpha : vdc) {
    interpolator->interpolate(startState, goalState, alpha, testState);
    if (!constraint->isSatisfied(testState)) {
      planningResult->message = "Collision detected";
      return trajectory;
    }
  }

  auto startCopy = stateSpace->createState();
  auto goalCopy = stateSpace->createState();
  stateSpace->copyState(startCopy, startState);
  stateSpace->copyState(goalCopy, goalState);

  trajectory.emplace_back(std::move(startCopy));
  trajectory.emplace_back(std::move(goalCopy));
  return trajectory;
}

}  // planner
}  // aikido

#endif  // AIKIDO_PLANNER_SNAP_PLANNER_HPP_
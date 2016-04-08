#ifndef AIKIDO_STATESPACE_STATESPACE_H
#define AIKIDO_STATESPACE_STATESPACE_H
#include <memory>
#include "ScopedState.hpp"
#include "../util/RNG.hpp"

namespace aikido {
namespace constraint {

class SampleableConstraint;

} // namespace constraint

namespace statespace {

/// Base class for all StateSpaces.
class StateSpace {
public:
  /// Base class for all States.
  class State
  {
  protected:
    State() = default;

    /// It is unsafe to call this, since it is a non-virtual destructor.  Having
    /// any virtual function in this class, including this destructor, adds
    /// sizeof(pointer) overhead for a vtable.
    ~State() = default;
  };

  using StateHandle = statespace::StateHandle<StateSpace, State>;
  using StateHandleConst = statespace::StateHandle<StateSpace, const State>;

  using ScopedState = statespace::ScopedState<StateHandle>;
  using ScopedStateConst = statespace::ScopedState<StateHandleConst>;

  using SampleableConstraintPtr
    = std::shared_ptr<constraint::SampleableConstraint>;

  virtual ~StateSpace() = default;

  /// Gets the size of a State, in bytes.
  virtual size_t getStateSizeInBytes() const = 0;

  /// Allocate a new state. This must be deleted with freeState.
  virtual StateSpace::State* allocateState() const
  {
    return allocateStateInBuffer(new char[getStateSizeInBytes()]);
  }

  /// Create a new state in a pre-allocated buffer. The input argument must
  /// contain at least getStateSizeInBytes() bytes of memory.
  virtual StateSpace::State* allocateStateInBuffer(void* _buffer) const = 0;

  /// Free a state previously created by allocateState.
  virtual void freeState(StateSpace::State* _state) const
  {
    delete[] reinterpret_cast<char*>(_state);
  }

  /// Free a state previously created by allocateStateInBuffer.
  virtual void freeStateInBuffer(StateSpace::State* _state) const = 0;

  /// Sample uniformly at random from this state space.
  virtual SampleableConstraintPtr createSampleableConstraint(
    std::unique_ptr<util::RNG> _rng) const = 0;

  /// Lie group operation for this StateSpace.
  virtual void compose(
    const State* _state1, const State* _state2, State* _out) const = 0;
};

using StateSpacePtr = std::shared_ptr<StateSpace>;

} // namespace statespace
} // namespace aikido

#endif

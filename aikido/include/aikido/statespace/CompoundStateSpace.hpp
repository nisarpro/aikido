#ifndef AIKIDO_STATESPACE_COMPOUNDSTATESPACE_H
#define AIKIDO_STATESPACE_COMPOUNDSTATESPACE_H
#include <vector>
#include "StateSpace.hpp"
#include "ScopedState.hpp"

namespace aikido {
namespace statespace {

/// Represents the Cartesian product of other StateSpaces.
class CompoundStateSpace : public StateSpace
{
public:
  /// A tuple of states where the i-th state is from the i-th subspace.
  class State : public StateSpace::State
  {
  protected:
    State() = default;
    ~State() = default;

    friend class CompoundStateSpace;
  };

  template <class _QualifiedState>
  class TemplateStateHandle
    : public statespace::StateHandle<CompoundStateSpace, _QualifiedState>
  {
  public:
    using typename statespace::StateHandle<
      CompoundStateSpace, _QualifiedState>::State;
    using typename statespace::StateHandle<
      CompoundStateSpace, _QualifiedState>::StateSpace;
    using typename statespace::StateHandle<
      CompoundStateSpace, _QualifiedState>::QualifiedState;


    TemplateStateHandle()
    {
    }

    TemplateStateHandle(const StateSpace* _space, State* _state)
      : statespace::StateHandle<CompoundStateSpace, QualifiedState>(
          _space, _state)
    {
    }

    /// Gets state of type by subspace index.
    template <class Space = StateSpace>
    typename Space::State& getSubState(size_t _index) const
    {
      return this->getStateSpace()->getSubState(
        *this->getState(), _index);
    }

    /// Gets state of type by subspace index.
    template <class Space = StateSpace>
    statespace::StateHandle<typename Space::State, _QualifiedState>
      getSubStateHandle(size_t _index) const
    {
      return this->getStateSpace()->getSubStateHandle(
        *this->getState(), _index);
    }
  };

  using StateHandle = TemplateStateHandle<State>;
  using StateHandleConst = TemplateStateHandle<const State>;

  using ScopedState = statespace::ScopedState<
    CompoundStateSpace, State>;
  using ScopedStateConst = statespace::ScopedState<
    CompoundStateSpace, const State>;

  /// Construct the Cartesian product of a vector of subspaces.
  explicit CompoundStateSpace(const std::vector<StateSpacePtr>& _subspaces);

  /// Helper function to create a ScopedState.
  ScopedState createState()
  {
    return ScopedState(this);
  }

  /// Gets number of subspaces.
  size_t getNumStates() const;

  /// Gets subspace by index.
  template <class Space = StateSpace>
  const Space& getSubSpace(size_t _index) const;

  /// Gets state of type by subspace index.
  template <class Space = StateSpace>
  typename Space::State& getSubState(
    StateSpace::State& _state, size_t _index) const;

  /// Gets state of type by subspace index.
  template <class Space = StateSpace>
  const typename Space::State& getSubState(
    const StateSpace::State& _state, size_t _index) const;

  // Documentation inherited.
  size_t getStateSizeInBytes() const override;

  // Documentation inherited.
  StateSpace::State* allocateStateInBuffer(void* _buffer) const override;

  // Documentation inherited.
  void freeStateInBuffer(StateSpace::State* _state) const override;

  // Documentation inherited.
  void compose(
    const StateSpace::State& _state1, const StateSpace::State& _state2,
    StateSpace::State& _out) const override;

private:
  std::vector<StateSpacePtr> mSubspaces;
  std::vector<std::size_t> mOffsets;
  size_t mSizeInBytes;
};

} // namespace statespace
} // namespace aikido

#include "detail/CompoundStateSpace.hpp"

#endif

#include <gtest/gtest.h>
#include <aikido/constraint/uniform/SO2UniformSampler.hpp>
#include <dart/common/StlHelpers.h>
#include <aikido/distance/AngularDistanceMetric.hpp>
#include "SampleGeneratorCoverage.hpp"

using aikido::statespace::SO2StateSpace;
using aikido::statespace::SO2StateSpaceSampleableConstraint;
using aikido::constraint::SampleGenerator;
using aikido::util::RNG;
using aikido::util::RNGWrapper;
using aikido::distance::AngularDistanceMetric;
using dart::common::make_unique;

class SO2UniformSamplerTests : public ::testing::Test
{
protected:
  static constexpr size_t NUM_SAMPLES = 10000;
  static constexpr size_t NUM_TARGETS = 20;
  static constexpr double DISTANCE_THRESHOLD = M_PI / NUM_TARGETS;

  void SetUp() override
  {
    mStateSpace = std::make_shared<SO2StateSpace>();
    mDistance = std::make_shared<AngularDistanceMetric>(mStateSpace);
    mRng = make_unique<RNGWrapper<std::default_random_engine>>(0);

    mTargets.clear();
    for (int i = 0; i < NUM_TARGETS; ++i)
    {
      const double angle = (2 * M_PI * i) / NUM_TARGETS;

      mTargets.emplace_back(mStateSpace->createState());
      mTargets.back().setAngle(angle);
    }
  }

  std::unique_ptr<RNG> mRng;
  std::shared_ptr<SO2StateSpace> mStateSpace;
  std::shared_ptr<AngularDistanceMetric> mDistance;
  std::vector<SO2StateSpace::ScopedState> mTargets;
};

TEST_F(SO2UniformSamplerTests, constructor_StateSpaceIsNull_Throws)
{
  EXPECT_THROW({
    SO2StateSpaceSampleableConstraint(nullptr, mRng->clone());
  }, std::invalid_argument);
}

TEST_F(SO2UniformSamplerTests, constructor_RNGIsNull_Throws)
{
  EXPECT_THROW({
    SO2StateSpaceSampleableConstraint(mStateSpace, nullptr);
  }, std::invalid_argument);
}

TEST_F(SO2UniformSamplerTests, getStateSpace)
{
  SO2StateSpaceSampleableConstraint constraint(mStateSpace, mRng->clone());
  EXPECT_EQ(mStateSpace, constraint.getStateSpace());
}

TEST_F(SO2UniformSamplerTests, createSampleGenerator)
{
  SO2StateSpaceSampleableConstraint constraint(mStateSpace, mRng->clone());
  auto generator = constraint.createSampleGenerator();

  ASSERT_TRUE(!!generator);
  EXPECT_EQ(mStateSpace, generator->getStateSpace());

  auto result = SampleGeneratorCoverage(*generator, *mDistance,
    std::begin(mTargets), std::end(mTargets), DISTANCE_THRESHOLD, NUM_SAMPLES);
  ASSERT_TRUE(result);
}

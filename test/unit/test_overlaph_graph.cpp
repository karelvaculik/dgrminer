#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "common_utils.h"

#include "OverlapGraph.hpp"


TEST(OveralGraphSupport, testSupportCalculationSimple)
{
	// 		   4
	//		   |
  	// 0 ----- 1 ----- 2
  	// 		   |
  	//		   3
  	dgrminer::OverlapGraph overlapGraph(false);
  	for (int i = 0; i < 5; ++i) {
	  	overlapGraph.addVertex();
  	}

  	overlapGraph.addEdge(0, 1);
  	overlapGraph.addEdge(1, 2);
  	overlapGraph.addEdge(1, 4);
  	overlapGraph.addEdge(1, 3);

  	EXPECT_TRUE(equal(overlapGraph.computeSupport(), 4));
}


TEST(OveralGraphSupport, testSupportCalculationAdvanced)
{
	// 0 ----- 1 ----- 5 ----- 6
	// |	   |	   |
	// 2 ----- 3 ----- 4
	dgrminer::OverlapGraph overlapGraph(false);
  	for (int i = 0; i < 7; ++i) {
		overlapGraph.addVertex();
	}
	overlapGraph.addEdge(0, 1);
	overlapGraph.addEdge(0, 2);
	overlapGraph.addEdge(1, 3);
	overlapGraph.addEdge(2, 3);
	overlapGraph.addEdge(4, 3);
  	overlapGraph.addEdge(1, 5);
  	overlapGraph.addEdge(5, 4);
  	overlapGraph.addEdge(5, 6);

	EXPECT_TRUE(equal(overlapGraph.computeSupport(), 4));
}

TEST(OveralGraphSupport, testEmpty)
{

  dgrminer::OverlapGraph overlapGraph(false);
  for (int i = 0; i < 4; ++i) {
	  overlapGraph.addVertex();
  }

  EXPECT_TRUE(equal(overlapGraph.computeSupport(), 4));
}

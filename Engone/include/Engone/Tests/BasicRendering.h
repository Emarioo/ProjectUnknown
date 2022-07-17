#pragma once

namespace engone {
	// Do one test at a time. The tests share resources.
	namespace test {
		// This function will draw a simple 2D triangle.
		void Test2D(RenderInfo& info);
		// This function will draw a simple 3D triangle which should be in front of the camera assuming you haven't moved it. (triangle's z position is -4).
		void Test3D(RenderInfo& info);
	}
}
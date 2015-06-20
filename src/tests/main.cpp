#include <gtest/gtest.h>
#include <gst/gst.h>

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	gst_init(&argc, &argv);

	return RUN_ALL_TESTS();
}

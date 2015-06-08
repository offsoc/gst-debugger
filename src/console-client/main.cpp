#include "protocol/gstdebugger.pb.h"

int main(int argc, char **argv)
{
	PadWatch watch;
	watch.set_pad_path("videotestsrc:src");

	return 0;
}

// Force-included into clean-core's own C++ sources on Linux (see CMakeLists.txt).
//
// Workaround for shaped-core 8c853df: clean-core/thread/thread.cc uses an
// unqualified `isize` in its Linux thread-name code (the macOS arm was fixed in
// cd0f58591, the Linux arm wasn't). To be reported upstream; delete this file and its
// CMake hook once the pin moves past the fix.
#pragma once
#include <clean-core/fwd.hh>
using cc::isize;

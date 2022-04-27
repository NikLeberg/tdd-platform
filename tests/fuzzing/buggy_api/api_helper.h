extern "C" {
#include "buggy_api/buggy_api.h"
#include "fuzzing.h"
}

struct GlobalContext {
  public:
    GlobalContext() {
    }
    void _run(void) {
        buggy_api_run();
    }
    void _setState(int state) {
        buggy_api_setState(state);
    }
};
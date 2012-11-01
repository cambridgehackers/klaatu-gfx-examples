
#include "common/gfx.h"
#include "klaatu_api.h"

#define templateAppInit KlaatuAPI::init
#define templateAppDraw KlaatuAPI::draw
#define templateAppToucheBegan KlaatuAPI::touchStart
#define templateAppToucheMoved KlaatuAPI::touchMove
#define templateAppToucheEnded KlaatuAPI::touchEnd
#define templateAppToucheCancelled KlaatuAPI::touchCancel
#define templateAppAccelerometer KlaatuAPI::accel
#define templateAppExit KlaatuAPI::finish
#define klaatu_main(A, B) current_methods.main((A), (B))

LOCAL_PATH := $(call my-dir)
TEST_DIRECTORY := 12-4

include $(CLEAR_VARS)
LOCAL_ARM_MODE := arm
LOCAL_MODULE := libopenal
OPENALTOP := $(LOCAL_PATH)/../klaatu-openal-soft
SRC_FILES := $(OPENALTOP)/Alc/ALc.c $(OPENALTOP)/Alc/alcConfig.c \
    $(OPENALTOP)/Alc/alcEcho.c $(OPENALTOP)/Alc/alcModulator.c \
    $(OPENALTOP)/Alc/alcReverb.c $(OPENALTOP)/Alc/alcRing.c \
    $(OPENALTOP)/Alc/alcThread.c $(OPENALTOP)/Alc/ALu.c \
    $(OPENALTOP)/Alc/bs2b.c $(OPENALTOP)/Alc/null.c \
    $(OPENALTOP)/OpenAL32/alAuxEffectSlot.c $(OPENALTOP)/OpenAL32/alBuffer.c \
    $(OPENALTOP)/OpenAL32/alDatabuffer.c $(OPENALTOP)/OpenAL32/alEffect.c \
    $(OPENALTOP)/OpenAL32/alError.c $(OPENALTOP)/OpenAL32/alExtension.c \
    $(OPENALTOP)/OpenAL32/alFilter.c $(OPENALTOP)/OpenAL32/alListener.c \
    $(OPENALTOP)/OpenAL32/alSource.c $(OPENALTOP)/OpenAL32/alState.c \
    $(OPENALTOP)/OpenAL32/alThunk.c
LOCAL_C_INCLUDES += external/klaatu-openal-soft/android/jni
LOCAL_C_INCLUDES += external/klaatu-openal-soft/include external/klaatu-openal-soft/OpenAL32/Include
LOCAL_SRC_FILES := $(SRC_FILES:$(LOCAL_PATH)/%=%) ../klaatu-api/klaatu_audio.cpp ../klaatu-api/openalandroid.c
LOCAL_LDLIBS = -llog
LOCAL_CFLAGS := -O3 -mno-thumb -DAL_ALEXT_PROTOTYPES
LOCAL_CPPFLAGS := -O3 -mno-thumb
LOCAL_MODULE_TAGS := optional
LOCAL_SHARED_LIBRARIES := liblog libmedia libbinder libcutils libutils

# in 2.3, the path in 'build/core/combo/TARGET_linux-arm.mk' mistakenly includes 'arch/' !!!
LOCAL_C_INCLUDES += bionic/libm/include/arm external/klaatu-api
LOCAL_PRELINK_MODULE = false
include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_ARM_MODE := arm
LOCAL_MODULE := foobar
LOCAL_C_INCLUDES = external/klaatu-openal-soft/include
LOCAL_C_INCLUDES += external/stlport/stlport bionic frameworks/native/include frameworks/base/services $(LOCAL_PATH)
LOCAL_C_INCLUDES += frameworks/base/include/surfaceflinger frameworks/native/include/gui \
    frameworks/base/services/input frameworks/base/include/ui external/klaatu-api external/klaatu-gfx external/klaatu-gfx-extra
LOCAL_C_INCLUDES += external/libpng external/klaatu-recastnavigation/Detour/Include external/klaatu-recastnavigation/Recast/Include
LOCAL_C_INCLUDES += external/klaatu-recastnavigation/Detour/Include external/klaatu-recastnavigation/Recast/Include external/klaatu-recastnavigation/DebugUtils/Include
SRC_FILES := $(wildcard $(LOCAL_PATH)/../klaatu-gfx/*.cpp)
BULLETSOURCE := $(LOCAL_PATH)/../klaatu-bullet
SRC_FILES += $(BULLETSOURCE)/Extras/Serialize/BulletFileLoader/bChunk.cpp \
    $(BULLETSOURCE)/Extras/Serialize/BulletFileLoader/bDNA.cpp \
    $(BULLETSOURCE)/Extras/Serialize/BulletFileLoader/bFile.cpp \
    $(BULLETSOURCE)/Extras/Serialize/BulletFileLoader/btBulletFile.cpp \
    $(BULLETSOURCE)/Extras/Serialize/BulletWorldImporter/btBulletWorldImporter.cpp \
    $(BULLETSOURCE)/src/BulletCollision/BroadphaseCollision/btAxisSweep3.cpp \
    $(BULLETSOURCE)/src/BulletCollision/BroadphaseCollision/btBroadphaseProxy.cpp \
    $(BULLETSOURCE)/src/BulletCollision/BroadphaseCollision/btCollisionAlgorithm.cpp \
    $(BULLETSOURCE)/src/BulletCollision/BroadphaseCollision/btDbvtBroadphase.cpp \
    $(BULLETSOURCE)/src/BulletCollision/BroadphaseCollision/btDbvt.cpp \
    $(BULLETSOURCE)/src/BulletCollision/BroadphaseCollision/btDispatcher.cpp \
    $(BULLETSOURCE)/src/BulletCollision/BroadphaseCollision/btMultiSapBroadphase.cpp \
    $(BULLETSOURCE)/src/BulletCollision/BroadphaseCollision/btOverlappingPairCache.cpp \
    $(BULLETSOURCE)/src/BulletCollision/BroadphaseCollision/btQuantizedBvh.cpp \
    $(BULLETSOURCE)/src/BulletCollision/BroadphaseCollision/btSimpleBroadphase.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionDispatch/btActivatingCollisionAlgorithm.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionDispatch/btBox2dBox2dCollisionAlgorithm.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionDispatch/btBoxBoxCollisionAlgorithm.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionDispatch/btBoxBoxDetector.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionDispatch/btCollisionDispatcher.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionDispatch/btCollisionObject.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionDispatch/btCollisionWorld.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionDispatch/btCompoundCollisionAlgorithm.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionDispatch/btConvex2dConvex2dAlgorithm.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionDispatch/btConvexConcaveCollisionAlgorithm.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionDispatch/btConvexConvexAlgorithm.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionDispatch/btConvexPlaneCollisionAlgorithm.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionDispatch/btEmptyCollisionAlgorithm.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionDispatch/btGhostObject.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionDispatch/btInternalEdgeUtility.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionDispatch/btManifoldResult.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionDispatch/btSimulationIslandManager.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionDispatch/btSphereBoxCollisionAlgorithm.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionDispatch/btSphereSphereCollisionAlgorithm.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionDispatch/btSphereTriangleCollisionAlgorithm.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionDispatch/btUnionFind.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionDispatch/SphereTriangleDetector.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionShapes/btBox2dShape.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionShapes/btBoxShape.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionShapes/btBvhTriangleMeshShape.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionShapes/btCapsuleShape.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionShapes/btCollisionShape.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionShapes/btCompoundShape.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionShapes/btConcaveShape.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionShapes/btConeShape.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionShapes/btConvex2dShape.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionShapes/btConvexHullShape.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionShapes/btConvexInternalShape.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionShapes/btConvexPointCloudShape.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionShapes/btConvexPolyhedron.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionShapes/btConvexShape.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionShapes/btConvexTriangleMeshShape.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionShapes/btCylinderShape.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionShapes/btEmptyShape.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionShapes/btMinkowskiSumShape.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionShapes/btMultimaterialTriangleMeshShape.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionShapes/btMultiSphereShape.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionShapes/btOptimizedBvh.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionShapes/btPolyhedralConvexShape.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionShapes/btScaledBvhTriangleMeshShape.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionShapes/btShapeHull.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionShapes/btSphereShape.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionShapes/btStaticPlaneShape.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionShapes/btStridingMeshInterface.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionShapes/btTetrahedronShape.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionShapes/btTriangleBuffer.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionShapes/btTriangleCallback.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionShapes/btTriangleIndexVertexArray.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionShapes/btTriangleIndexVertexMaterialArray.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionShapes/btTriangleMesh.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionShapes/btTriangleMeshShape.cpp \
    $(BULLETSOURCE)/src/BulletCollision/CollisionShapes/btUniformScalingShape.cpp \
    $(BULLETSOURCE)/src/BulletCollision/Gimpact/btContactProcessing.cpp \
    $(BULLETSOURCE)/src/BulletCollision/Gimpact/btGenericPoolAllocator.cpp \
    $(BULLETSOURCE)/src/BulletCollision/Gimpact/btGImpactBvh.cpp \
    $(BULLETSOURCE)/src/BulletCollision/Gimpact/btGImpactCollisionAlgorithm.cpp \
    $(BULLETSOURCE)/src/BulletCollision/Gimpact/btGImpactQuantizedBvh.cpp \
    $(BULLETSOURCE)/src/BulletCollision/Gimpact/btGImpactShape.cpp \
    $(BULLETSOURCE)/src/BulletCollision/Gimpact/btTriangleShapeEx.cpp \
    $(BULLETSOURCE)/src/BulletCollision/Gimpact/gim_box_set.cpp \
    $(BULLETSOURCE)/src/BulletCollision/Gimpact/gim_contact.cpp \
    $(BULLETSOURCE)/src/BulletCollision/Gimpact/gim_memory.cpp \
    $(BULLETSOURCE)/src/BulletCollision/Gimpact/gim_tri_collision.cpp \
    $(BULLETSOURCE)/src/BulletCollision/NarrowPhaseCollision/btContinuousConvexCollision.cpp \
    $(BULLETSOURCE)/src/BulletCollision/NarrowPhaseCollision/btConvexCast.cpp \
    $(BULLETSOURCE)/src/BulletCollision/NarrowPhaseCollision/btGjkConvexCast.cpp \
    $(BULLETSOURCE)/src/BulletCollision/NarrowPhaseCollision/btGjkEpa2.cpp \
    $(BULLETSOURCE)/src/BulletCollision/NarrowPhaseCollision/btGjkEpaPenetrationDepthSolver.cpp \
    $(BULLETSOURCE)/src/BulletCollision/NarrowPhaseCollision/btGjkPairDetector.cpp \
    $(BULLETSOURCE)/src/BulletCollision/NarrowPhaseCollision/btMinkowskiPenetrationDepthSolver.cpp \
    $(BULLETSOURCE)/src/BulletCollision/NarrowPhaseCollision/btPersistentManifold.cpp \
    $(BULLETSOURCE)/src/BulletCollision/NarrowPhaseCollision/btPolyhedralContactClipping.cpp \
    $(BULLETSOURCE)/src/BulletCollision/NarrowPhaseCollision/btRaycastCallback.cpp \
    $(BULLETSOURCE)/src/BulletCollision/NarrowPhaseCollision/btSubSimplexConvexCast.cpp \
    $(BULLETSOURCE)/src/BulletCollision/NarrowPhaseCollision/btVoronoiSimplexSolver.cpp \
    $(BULLETSOURCE)/src/BulletDynamics/Character/btKinematicCharacterController.cpp \
    $(BULLETSOURCE)/src/BulletDynamics/ConstraintSolver/btConeTwistConstraint.cpp \
    $(BULLETSOURCE)/src/BulletDynamics/ConstraintSolver/btContactConstraint.cpp \
    $(BULLETSOURCE)/src/BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.cpp \
    $(BULLETSOURCE)/src/BulletDynamics/ConstraintSolver/btGeneric6DofSpringConstraint.cpp \
    $(BULLETSOURCE)/src/BulletDynamics/ConstraintSolver/btHinge2Constraint.cpp \
    $(BULLETSOURCE)/src/BulletDynamics/ConstraintSolver/btHingeConstraint.cpp \
    $(BULLETSOURCE)/src/BulletDynamics/ConstraintSolver/btPoint2PointConstraint.cpp \
    $(BULLETSOURCE)/src/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.cpp \
    $(BULLETSOURCE)/src/BulletDynamics/ConstraintSolver/btSliderConstraint.cpp \
    $(BULLETSOURCE)/src/BulletDynamics/ConstraintSolver/btSolve2LinearConstraint.cpp \
    $(BULLETSOURCE)/src/BulletDynamics/ConstraintSolver/btTypedConstraint.cpp \
    $(BULLETSOURCE)/src/BulletDynamics/ConstraintSolver/btUniversalConstraint.cpp \
    $(BULLETSOURCE)/src/BulletDynamics/Dynamics/btContinuousDynamicsWorld.cpp \
    $(BULLETSOURCE)/src/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.cpp \
    $(BULLETSOURCE)/src/BulletDynamics/Dynamics/btRigidBody.cpp \
    $(BULLETSOURCE)/src/BulletDynamics/Dynamics/btSimpleDynamicsWorld.cpp \
    $(BULLETSOURCE)/src/BulletDynamics/Vehicle/btRaycastVehicle.cpp \
    $(BULLETSOURCE)/src/BulletDynamics/Vehicle/btWheelInfo.cpp \
    $(BULLETSOURCE)/src/BulletSoftBody/btDefaultSoftBodySolver.cpp \
    $(BULLETSOURCE)/src/BulletSoftBody/btSoftBodyConcaveCollisionAlgorithm.cpp \
    $(BULLETSOURCE)/src/BulletSoftBody/btSoftBody.cpp \
    $(BULLETSOURCE)/src/BulletSoftBody/btSoftBodyHelpers.cpp \
    $(BULLETSOURCE)/src/BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.cpp \
    $(BULLETSOURCE)/src/BulletSoftBody/btSoftRigidCollisionAlgorithm.cpp \
    $(BULLETSOURCE)/src/BulletSoftBody/btSoftRigidDynamicsWorld.cpp \
    $(BULLETSOURCE)/src/BulletSoftBody/btSoftSoftCollisionAlgorithm.cpp \
    $(BULLETSOURCE)/src/LinearMath/btAlignedAllocator.cpp \
    $(BULLETSOURCE)/src/LinearMath/btConvexHullComputer.cpp \
    $(BULLETSOURCE)/src/LinearMath/btConvexHull.cpp \
    $(BULLETSOURCE)/src/LinearMath/btGeometryUtil.cpp \
    $(BULLETSOURCE)/src/LinearMath/btQuickprof.cpp \
    $(BULLETSOURCE)/src/LinearMath/btSerializer.cpp
LOCAL_C_INCLUDES += external/klaatu-bullet/src \
    external/klaatu-bullet/Extras/Serialize/BulletFileLoader \
    external/klaatu-bullet/Extras/Serialize/BulletWorldImporter
SRC_FILES += $(wildcard $(LOCAL_PATH)/../klaatu-gfx-extra/ttf/*.cpp)
SRC_FILES += $(wildcard $(LOCAL_PATH)/../klaatu-gfx-extra/nvtristrip/*.cpp)
LOCAL_C_INCLUDES += external/klaatu-gfx-extra/zlib
SRC_FILES += $(wildcard $(LOCAL_PATH)/../klaatu-gfx-extra/zlib/*.c)
SRC_FILES += $(wildcard $(LOCAL_PATH)/../klaatu-recastnavigation/Detour/Source/*.cpp)
SRC_FILES += $(wildcard $(LOCAL_PATH)/../klaatu-recastnavigation/Recast/Source/*.cpp)
SRC_FILES += $(wildcard $(LOCAL_PATH)/../klaatu-gfx-extra/vorbis/*.c)
SRC_FILES := $(SRC_FILES:$(LOCAL_PATH)/%=%)
LOCAL_SRC_FILES := $(SRC_FILES) sdk/$(TEST_DIRECTORY)/templateApp.cpp \
    ../klaatu-api/klaatu_api.cpp ../klaatu-api/klaatu_touch.cpp \
    ../klaatu-api/klaatu_sensor.cpp ../klaatu-api/klaatu_audio.cpp
LOCAL_SHARED_LIBRARIES := libopenal libstlport libEGL libGLESv2 liblog \
        libutils libbinder libgui
ifeq ($(PLATFORM_VERSION),2.3.3)
LOCAL_SHARED_LIBRARIES += libsurfaceflinger_client libui
else
LOCAL_SHARED_LIBRARIES += libinput
endif
LOCAL_SHARED_LIBRARIES += libandroid_runtime
LOCAL_SHARED_LIBRARIES += libmedia libcutils
LOCAL_STATIC_LIBRARIES += libpng

LOCAL_LDLIBS := -L$(LOCAL_PATH)/../libs/armeabi 
LOCAL_LDLIBS += -lz -lm -ldl -lGLESv2 -lEGL -llog -lopenal
LOCAL_CFLAGS := -O3 -mno-thumb -Wno-write-strings
SVERSION:=$(subst ., ,$(PLATFORM_VERSION))
LOCAL_CFLAGS += -DSHORT_PLATFORM_VERSION=$(word 1,$(SVERSION))$(word 2,$(SVERSION))
LOCAL_CPPFLAGS := $(LOCAL_CFLAGS)
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)

#
# now compile asset files into .apk for use by test program
#
PRIVATE_ASSET_DIR := $(LOCAL_PATH)/sdk/$(TEST_DIRECTORY)/assets
PRIVATE_RESOURCE_DIR := $(LOCAL_PATH)/res
PRIVATE_ANDROID_MANIFEST := $(LOCAL_PATH)/AndroidManifest.xml

all_assets := $(addprefix $(PRIVATE_ASSET_DIR)/,$(patsubst assets/%,%,$(call find-subdir-assets,$(PRIVATE_ASSET_DIR))))
all_resources := $(addprefix $(PRIVATE_RESOURCE_DIR)/, $(patsubst res/%,%, $(call find-subdir-assets,$(PRIVATE_RESOURCE_DIR))))
#THISAPK := $(intermediates)/package.apk
THISAPK := $(LOCAL_INSTALLED_MODULE).apk

$(THISAPK): $(all_assets) $(all_resources) $(PRIVATE_ANDROID_MANIFEST) $(AAPT) $(LOCAL_PATH)/Android.mk
	echo "target APKPackage: $(PRIVATE_MODULE) ($@)"
	$(create-empty-package)
	$(add-assets-to-package)

$(LOCAL_BUILT_MODULE): $(THISAPK)

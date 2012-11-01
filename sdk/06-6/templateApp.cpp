/* see the file COPYRIGHT for copyright notice for this file */
#include "templateApp.h"

class KlaatuAPI : public KlaatuAPITemplate {
public:
    void init(int width, int height);
    void draw(void);
    void finish(void);
};
static KlaatuAPI current_methods;
#define OBJ_FILE ( char * )"pinball.obj"
#define PHYSIC_FILE ( char * )"pinball.bullet"
#define VERTEX_SHADER ( char * )"vertex.glsl"
#define FRAGMENT_SHADER ( char * )"fragment.glsl"
OBJ *obj = NULL;
PROGRAM *program = NULL;

btSoftBodyRigidBodyCollisionConfiguration *collisionconfiguration = NULL;
btCollisionDispatcher *dispatcher = NULL;
btBroadphaseInterface *broadphase = NULL;
btConstraintSolver *solver = NULL;
btSoftRigidDynamicsWorld *dynamicsworld = NULL;
unsigned char ball_index = 0, restart_game = 0;
OBJMESH *ball = NULL;
void get_next_ball(void)
{
    char tmp[MAX_CHAR] = { "" };
    ++ball_index;
    sprintf(tmp, "ball%d", ball_index);
    ball = OBJ_get_mesh(obj, tmp, 0);
}

void near_callback(btBroadphasePair & btbroadphasepair, btCollisionDispatcher & btdispatcher, const btDispatcherInfo & btdispatcherinfo)
{
    OBJMESH *objmesh0 = (OBJMESH *) ((btRigidBody *)
				     (btbroadphasepair.m_pProxy0->m_clientObject))->getUserPointer();
    OBJMESH *objmesh1 = (OBJMESH *) ((btRigidBody *)
				     (btbroadphasepair.m_pProxy1->m_clientObject))->getUserPointer();
    char tmp[MAX_CHAR] = { "" };
    sprintf(tmp, "ball%d", ball_index);
    if (ball && (strstr(objmesh0->name, "out_of_bound") || strstr(objmesh0->name, tmp))
	&& (strstr(objmesh1->name, "out_of_bound") || strstr(objmesh1->name, tmp))) {
	get_next_ball();
	--ball_index;
	if (!ball)
	    OBJ_get_mesh(obj, "game_over", 0)->visible = 1;
	ball = NULL;
    }
    btdispatcher.defaultNearCallback(btbroadphasepair, btdispatcher, btdispatcherinfo);
}

void init_physic_world(void)
{
    collisionconfiguration = new btSoftBodyRigidBodyCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionconfiguration);
    broadphase = new btDbvtBroadphase();
    solver = new btSequentialImpulseConstraintSolver();
    dynamicsworld = new btSoftRigidDynamicsWorld(dispatcher, broadphase, solver, collisionconfiguration);
    dynamicsworld->setGravity(btVector3(0.0f, 0.0f, -9.8f));
}

void load_physic_world(void)
{
    btBulletWorldImporter *btbulletworldimporter = new btBulletWorldImporter(dynamicsworld);
    MEMORY *memory = mopen(PHYSIC_FILE, 1);
    btbulletworldimporter->loadFileFromMemory((char *)memory->buffer, memory->size);
    mclose(memory);
    unsigned int i = 0;
    while (i != btbulletworldimporter->getNumRigidBodies()) {
	OBJMESH *objmesh = OBJ_get_mesh(obj,
					btbulletworldimporter->getNameForPointer(btbulletworldimporter->getRigidBodyByIndex(i)), 0);
	if (objmesh) {
	    objmesh->btrigidbody = (btRigidBody *) btbulletworldimporter->getRigidBodyByIndex(i);
	    objmesh->btrigidbody->setUserPointer(objmesh);
	    objmesh->btrigidbody->setRestitution(0.75f);
	}
	++i;
    }
    delete btbulletworldimporter;
}

void free_physic_world(void)
{
    while (dynamicsworld->getNumCollisionObjects()) {
	btCollisionObject *btcollisionobject = dynamicsworld->getCollisionObjectArray()[0];
	btRigidBody *btrigidbody = btRigidBody::upcast(btcollisionobject);
	if (btrigidbody) {
	    delete btrigidbody->getCollisionShape();
	    delete btrigidbody->getMotionState();
	    dynamicsworld->removeRigidBody(btrigidbody);
	    dynamicsworld->removeCollisionObject(btcollisionobject);
	    delete btrigidbody;
	}
    }
    delete collisionconfiguration;
    collisionconfiguration = NULL;
    delete dispatcher;
    dispatcher = NULL;
    delete broadphase;
    broadphase = NULL;
    delete solver;
    solver = NULL;
    delete dynamicsworld;
    dynamicsworld = NULL;
}

void program_bind_attrib_location(void *ptr)
{
    PROGRAM *program = (PROGRAM *) ptr;
    glBindAttribLocation(program->pid, 0, "POSITION");
    glBindAttribLocation(program->pid, 2, "TEXCOORD0");
}

void load_game(void)
{
    obj = OBJ_load(OBJ_FILE, 1);
    unsigned int i = 0;
    while (i != obj->n_objmesh) {
	OBJ_build_mesh(obj, i);
	OBJ_free_mesh_vertex_data(obj, i);
	++i;
    }
    init_physic_world();
    dispatcher->setNearCallback(near_callback);
    load_physic_world();
    OBJ_get_mesh(obj, "game_over", 0)->visible = 0;
    i = 0;
    while (i != obj->n_texture) {
	OBJ_build_texture(obj, i, obj->texture_path, TEXTURE_MIPMAP | TEXTURE_16_BITS, TEXTURE_FILTER_2X, 0.0f);
	++i;
    }
    i = 0;
    while (i != obj->n_objmaterial) {
	OBJ_build_material(obj, i, NULL);
	++i;
    }
    program = PROGRAM_create((char *)"default", VERTEX_SHADER, FRAGMENT_SHADER, 1, 0, program_bind_attrib_location, NULL);
    PROGRAM_draw(program);
    glUniform1i(PROGRAM_get_uniform_location(program, (char *)"DIFFUSE"), 1);
}

void templateAppInit(int width, int height)
{
    GFX_start();
    glViewport(0.0f, 0.0f, width, height);
    GFX_set_matrix_mode(PROJECTION_MATRIX);
    GFX_load_identity();
    GFX_set_perspective(45.0f, (float)width / (float)height, 1.0f, 100.0f, 0.0f);
    load_game();
}

void templateAppDraw(void)
{
    if (restart_game) {
	templateAppExit();
	load_game();
	ball_index = 0;
	restart_game = 0;
    }
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    GFX_set_matrix_mode(MODELVIEW_MATRIX);
    GFX_load_identity(); {
	vec3 e = { 0.0f, -21.36f, 19.64f }
	, c = {
	0.0f, -20.36f, 19.22f}
	, u = {
	0.0f, 0.0f, 1.0f};
	GFX_look_at(&e, &c, &u);
    }
    unsigned int i = 0;
    while (i != obj->n_objmesh) {
	OBJMESH *objmesh = &obj->objmesh[i];
	GFX_push_matrix();
	if (objmesh->btrigidbody) {
	    mat4 mat;
	    objmesh->btrigidbody->getWorldTransform().getOpenGLMatrix((float *)&mat);
	    GFX_multiply_matrix(&mat);
	} else
	    GFX_translate(objmesh->location.x, objmesh->location.y, objmesh->location.z);
	glUniformMatrix4fv(program->uniform_array[0].location, 1, GL_FALSE, (float *)GFX_get_modelview_projection_matrix());
	OBJ_draw_mesh(obj, i);
	GFX_pop_matrix();
	++i;
    }
    dynamicsworld->stepSimulation(1.0f / 60.0f);
}

void templateAppToucheBegan(float x, float y, unsigned int tap_count)
{
    if (OBJ_get_mesh(obj, "game_over", 0)->visible) {
	restart_game = 1;
	return;
    }
    if (!ball) {
	get_next_ball();
	if (ball) {
	    ball->btrigidbody->setActivationState(ACTIVE_TAG);
	    ball->btrigidbody->setLinearVelocity(btVector3(0.0f, 30.0f, 0.0f));
	}
    } else {
	OBJMESH *objmesh = OBJ_get_mesh(obj, "flipper1", 0);
	objmesh->btrigidbody->setActivationState(ACTIVE_TAG);
	objmesh->btrigidbody->setAngularVelocity(btVector3(0.0f, 0.0f, -30.0f));
	objmesh = OBJ_get_mesh(obj, "flipper2", 0);
	objmesh->btrigidbody->setActivationState(ACTIVE_TAG);
	objmesh->btrigidbody->setAngularVelocity(btVector3(0.0f, 0.0f, 30.0f));
    }
}

void templateAppExit(void)
{
    free_physic_world();
    OBJ_free(obj);
}
int main(int argc, char *argv[])
{
    return klaatu_main(argc, argv);
}

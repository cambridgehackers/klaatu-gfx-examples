/* see the file COPYRIGHT for copyright notice for this file */
#include "templateApp.h"

class KlaatuAPI : public KlaatuAPITemplate {
public:
    void init(int width, int height);
    void draw(void);
    void finish(void);
};
static KlaatuAPI current_methods;
#define OBJ_FILE ( char * )"Scene.obj"
#define VERTEX_SHADER ( char * )"vertex.glsl"
#define FRAGMENT_SHADER ( char * )"fragment.glsl"
OBJ *obj = NULL;
PROGRAM *program = NULL;

btSoftBodyRigidBodyCollisionConfiguration *collisionconfiguration = NULL;
btCollisionDispatcher *dispatcher = NULL;
btBroadphaseInterface *broadphase = NULL;
btConstraintSolver *solver = NULL;
btSoftRigidDynamicsWorld *dynamicsworld = NULL;
void init_physic_world(void)
{
    collisionconfiguration = new btSoftBodyRigidBodyCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionconfiguration);
    broadphase = new btDbvtBroadphase();
    solver = new btSequentialImpulseConstraintSolver();
    dynamicsworld = new btSoftRigidDynamicsWorld(dispatcher, broadphase, solver, collisionconfiguration);
    dynamicsworld->setGravity(btVector3(0.0f, 0.0f, -9.8f));
}

void add_rigid_body(OBJMESH * objmesh, float mass)
{
    btCollisionShape *btcollisionshape = new btBoxShape(btVector3(objmesh->dimension.x * 0.5f,
								  objmesh->dimension.y * 0.5f,
								  objmesh->dimension.z * 0.5f));
    btTransform bttransform;
    mat4 mat;
    vec4 rotx = { 1.0f, 0.0f, 0.0f, objmesh->rotation.x }, roty = {
    0.0f, 1.0f, 0.0f, objmesh->rotation.y}, rotz = {
    0.0f, 0.0f, 1.0f, objmesh->rotation.z};
    mat4_identity(&mat);
    mat4_translate(&mat, &mat, &objmesh->location);
    mat4_rotate(&mat, &mat, &rotz);
    mat4_rotate(&mat, &mat, &roty);
    mat4_rotate(&mat, &mat, &rotx);
    bttransform.setFromOpenGLMatrix((float *)&mat);
    btDefaultMotionState *btdefaultmotionstate = NULL;
    btdefaultmotionstate = new btDefaultMotionState(bttransform);
    btVector3 localinertia(0.0f, 0.0f, 0.0f);
    if (mass)
	btcollisionshape->calculateLocalInertia(mass, localinertia);
    objmesh->btrigidbody = new btRigidBody(mass, btdefaultmotionstate, btcollisionshape, localinertia);
    objmesh->btrigidbody->setUserPointer(objmesh);
    dynamicsworld->addRigidBody(objmesh->btrigidbody);
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
    glBindAttribLocation(program->pid, 1, "NORMAL");
}

void templateAppInit(int width, int height)
{
    GFX_start();
    init_physic_world();
    glViewport(0.0f, 0.0f, width, height);
    GFX_set_matrix_mode(PROJECTION_MATRIX);
    GFX_load_identity();
    GFX_set_perspective(45.0f, (float)width / (float)height, 0.1f, 100.0f, -90.0f);
    obj = OBJ_load(OBJ_FILE, 1);
    unsigned int i = 0;
    while (i != obj->n_objmesh) {
	OBJ_build_mesh(obj, i);
	OBJMESH *objmesh = &obj->objmesh[i];
	if (!strcmp(objmesh->name, "Cube")) {
	    objmesh->rotation.x = objmesh->rotation.y = objmesh->rotation.z = 35.0f;
	    add_rigid_body(objmesh, 1.0f);
	} else
	    add_rigid_body(objmesh, 0.0f);
	OBJ_free_mesh_vertex_data(obj, i);
	++i;
    }
    program = PROGRAM_create((char *)"default", VERTEX_SHADER, FRAGMENT_SHADER, 1, 0, program_bind_attrib_location, NULL);
    PROGRAM_draw(program);
}

void templateAppDraw(void)
{
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    GFX_set_matrix_mode(MODELVIEW_MATRIX);
    GFX_load_identity(); {
	vec3 e = { 10.4f, -9.8f, 5.5f }, c = {
	-3.4f, 2.8f, 0.0f}, u = {
	0.0f, 0.0f, 1.0f};
	GFX_look_at(&e, &c, &u);
    }
    unsigned int i = 0;
    while (i != obj->n_objmesh) {
	OBJMESH *objmesh = &obj->objmesh[i];
	GFX_push_matrix();
	mat4 mat;
	objmesh->btrigidbody->getWorldTransform().getOpenGLMatrix((float *)&mat);
	GFX_multiply_matrix(&mat);
	glUniformMatrix4fv(program->uniform_array[0].location, 1, GL_FALSE, (float *)GFX_get_modelview_projection_matrix());
	OBJ_draw_mesh(obj, i);
	GFX_pop_matrix();
	++i;
    }
    dynamicsworld->stepSimulation(1.0f / 60.0f);
    unsigned int n_manifolds = dynamicsworld->getDispatcher()->getNumManifolds();
    i = 0;
    while (i != n_manifolds) {
	btPersistentManifold *manifold = dynamicsworld->getDispatcher()->getManifoldByIndexInternal(i);
	OBJMESH *objmesh0 = (OBJMESH *) ((btRigidBody *) manifold->getBody0())->getUserPointer();
	OBJMESH *objmesh1 = (OBJMESH *) ((btRigidBody *) manifold->getBody1())->getUserPointer();
	unsigned int j = 0, n_contacts = manifold->getNumContacts();
	while (j != n_contacts) {
	    btManifoldPoint & contact = manifold->getContactPoint(j);
	    console_print("Manifold : %d\n", i);
	    console_print("Contact  : %d\n", j);
	    console_print("Object #0: %s\n", objmesh0->name);
	    console_print("Point  #0: %.3f %.3f %.3f\n", contact.getPositionWorldOnA().x(), contact.getPositionWorldOnA().y(), contact.getPositionWorldOnA().z());
	    console_print("Object #1: %s\n", objmesh1->name);
	    console_print("Point  #1: %.3f %.3f %.3f\n", contact.getPositionWorldOnB().x(), contact.getPositionWorldOnB().y(), contact.getPositionWorldOnB().z());
	    console_print("Distance : %.3f\n", contact.getDistance());
	    console_print("Lifetime : %d\n", contact.getLifeTime());
	    console_print("Normal   : %.3f %.3f %.3f\n", contact.m_normalWorldOnB.x(), contact.m_normalWorldOnB.y(), contact.m_normalWorldOnB.z());
	    console_print("%d\n\n", get_milli_time());
	    ++j;
	}
	++i;
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

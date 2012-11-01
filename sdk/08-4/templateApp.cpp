/* see the file COPYRIGHT for copyright notice for this file */
#include "templateApp.h"

class KlaatuAPI : public KlaatuAPITemplate {
public:
    void init(int width, int height);
    void draw(void);
    void finish(void);
};
static KlaatuAPI current_methods;
#define OBJ_FILE ( char * )"maze.obj"
#define PHYSIC_FILE ( char * )"maze.bullet"
#define VERTEX_SHADER ( char * )"vertex.glsl"
#define FRAGMENT_SHADER ( char * )"fragment.glsl"
OBJ *obj = NULL;
PROGRAM *program = NULL;
PROGRAM *path_point = NULL;
vec2 view_location, view_delta = { 0.0f, 0.0f };
vec3 eye, next_eye, center = { 0.0f, 0.0f, 0.0f }, up = {
0.0f, 0.0f, 1.0f};

float rotx = 45.0f, next_rotx = 0.0f, rotz = 0.0f, next_rotz = -45.0f, distance = 30.0f;
OBJMESH *player = NULL;
OBJMESH *maze = NULL;
NAVIGATION *navigation = NULL;
unsigned char double_tap = 0;
NAVIGATIONPATH navigationpath_player;
NAVIGATIONPATHDATA navigationpathdata_player;
int viewport_matrix[4];
int player_next_point = -1;

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

void templateAppInit(int width, int height)
{
    GFX_start();
    glViewport(0.0f, 0.0f, width, height);
    glGetIntegerv(GL_VIEWPORT, viewport_matrix);
    GFX_set_matrix_mode(PROJECTION_MATRIX);
    GFX_load_identity();
    GFX_set_perspective(80.0f, (float)width / (float)height, 1.0f, 1000.0f, -90.0f);
    obj = OBJ_load(OBJ_FILE, 1);
    unsigned int i = 0;
    while (i != obj->n_objmesh) {
	if (strstr(obj->objmesh[i].name, "maze")) {
	    navigation = NAVIGATION_init((char *)"maze");
	    navigation->navigationconfiguration.agent_height = 2.0f;
	    navigation->navigationconfiguration.agent_radius = 0.4f;
	    if (NAVIGATION_build(navigation, obj, i)) {
		console_print("Navigation generated.\n");
	    } else {
		console_print("Unable to create the navigation mesh.");
	    }
	}
	OBJ_optimize_mesh(obj, i, 128);
	OBJ_build_mesh2(obj, i);
	OBJ_free_mesh_vertex_data(obj, i);
	++i;
    }
    init_physic_world();
    load_physic_world();
    player = OBJ_get_mesh(obj, "player", 0);
    player->btrigidbody->setAngularFactor(0.0f);
    maze = OBJ_get_mesh(obj, "maze", 0);
    distance = maze->radius * 2.0f;
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
}

void draw_navigation_points(NAVIGATIONPATHDATA * navigationpathdata, vec3 * color)
{
    unsigned int i = 0;
    while (i != navigationpathdata->path_point_count + 1) {
	navigationpathdata->path_point_array[i].z = 1.0f;
	++i;
    }
    glBindVertexArrayOES(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (!path_point) {
	path_point = PROGRAM_create((char *)"path_point", (char *)"point_vert.glsl", (char *)"point_frag.glsl", 1, 0, program_bind_attrib_location, NULL);
    }
    PROGRAM_draw(path_point);
    glUniformMatrix4fv(PROGRAM_get_uniform_location(path_point, (char *)"MODELVIEWPROJECTIONMATRIX"), 1, GL_FALSE, (float *)GFX_get_modelview_projection_matrix());
    glUniform3fv(PROGRAM_get_uniform_location(path_point, (char *)"COLOR"), 1, (float *)color);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, navigationpathdata->path_point_array);
    glDrawArrays(GL_POINTS, 0, navigationpathdata->path_point_count + 1);
    glDrawArrays(GL_LINE_STRIP, 0, navigationpathdata->path_point_count + 1);
    glDisable(GL_BLEND);
}

void move_entity(OBJMESH * objmesh, NAVIGATIONPATHDATA * navigationpathdata, int *next_point, float speed)
{
    objmesh->location.z = navigationpathdata->path_point_array[*next_point].z = 0.0f;
    float distance = vec3_dist(&objmesh->location,
			       &navigationpathdata->path_point_array[*next_point]);
    if (distance < 0.1f) {
	++*next_point;
	if (*next_point == (navigationpathdata->path_point_count + 1)) {
	    *next_point = -1;
	}
    }
    if (*next_point != -1) {
	vec3 direction;
	vec3_diff(&direction, &navigationpathdata->path_point_array[*next_point], &objmesh->location);
	vec3_normalize(&direction, &direction);
	objmesh->btrigidbody->setLinearVelocity(btVector3(direction.x * speed, direction.y * speed, 0.0f));
	objmesh->btrigidbody->setActivationState(ACTIVE_TAG);
    } else {
	objmesh->btrigidbody->setActivationState(WANTS_DEACTIVATION);
	navigationpathdata->path_point_count = 0;
    }
}

void templateAppDraw(void)
{
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    GFX_set_matrix_mode(MODELVIEW_MATRIX);
    GFX_load_identity();
    if (view_delta.x || view_delta.y) {
	if (view_delta.y)
	    next_rotz -= view_delta.y;
	if (view_delta.x) {
	    next_rotx -= view_delta.x;
	    next_rotx = CLAMP(next_rotx, 0.0f, 90.0f);
	}
	view_delta.x = view_delta.y = 0.0f;
    }
    rotx = rotx * 0.9f + next_rotx * 0.1f;
    rotz = rotz * 0.9f + next_rotz * 0.1f;
    eye.x = center.x + distance * cosf(rotx * DEG_TO_RAD) * sinf(rotz * DEG_TO_RAD);
    eye.y = center.y - distance * cosf(rotx * DEG_TO_RAD) * cosf(rotz * DEG_TO_RAD);
    eye.z = center.z + distance * sinf(rotx * DEG_TO_RAD);
    rotx = rotx * 0.9f + next_rotx * 0.1f;
    rotz = rotz * 0.9f + next_rotz * 0.1f;
    center.x = maze->location.x;
    center.y = maze->location.y;
    center.z = maze->location.z;
    GFX_look_at(&eye, &center, &up);
    if (double_tap) {
	vec3 location;
	if (GFX_unproject(view_location.x, viewport_matrix[3] - view_location.y, 1.0f, GFX_get_modelview_matrix(), GFX_get_projection_matrix(), viewport_matrix, &location.x, &location.y, &location.z)) {
	    btVector3 ray_from(eye.x, eye.y, eye.z), ray_to(location.x + eye.x, location.y + eye.y, location.z + eye.z);
	    btCollisionWorld::ClosestRayResultCallback collision_ray(ray_from, ray_to);
	    dynamicsworld->rayTest(ray_from, ray_to, collision_ray);
	    if (collision_ray.hasHit() && collision_ray.m_collisionObject == maze->btrigidbody) {
		collision_ray.m_hitNormalWorld.normalize();
		if (collision_ray.m_hitNormalWorld.z() == 1.0f) {
		    navigationpath_player.start_location.x = player->location.x;
		    navigationpath_player.start_location.y = player->location.y;
		    navigationpath_player.start_location.z = player->location.z;
		    navigationpath_player.end_location.x = collision_ray.m_hitPointWorld.x();
		    navigationpath_player.end_location.y = collision_ray.m_hitPointWorld.y();
		    navigationpath_player.end_location.z = collision_ray.m_hitPointWorld.z();
		    if (NAVIGATION_get_path(navigation, &navigationpath_player, &navigationpathdata_player)) {
			player_next_point = 1;
			unsigned int i = 0;
			while (i != navigationpathdata_player.path_point_count + 1) {
			    console_print("%d: %f %f %f\n",
					  i, navigationpathdata_player.path_point_array[i].x, navigationpathdata_player.path_point_array[i].y, navigationpathdata_player.path_point_array[i].z);
			    ++i;
			}
			printf("\n");
		    }
		}
	    }
	}
	double_tap = 0;
    }
    if (navigationpathdata_player.path_point_count) {
	vec3 color = { 0.0f, 0.0f, 1.0f };
	draw_navigation_points(&navigationpathdata_player, &color);
	move_entity(player, &navigationpathdata_player, &player_next_point, 3.0f);
    }
    PROGRAM_draw(program);
    glUniform1i(PROGRAM_get_uniform_location(program, (char *)"DIFFUSE"), 1);
    unsigned int i = 0;
    while (i != obj->n_objmesh) {
	OBJMESH *objmesh = &obj->objmesh[i];
	GFX_push_matrix();
	mat4 mat;
	objmesh->btrigidbody->getWorldTransform().getOpenGLMatrix((float *)&mat);
	memcpy(&objmesh->location, (vec3 *) & mat.m[3], sizeof(vec3));
	GFX_multiply_matrix(&mat);
	glUniformMatrix4fv(PROGRAM_get_uniform_location(program, (char *)"MODELVIEWPROJECTIONMATRIX"), 1, GL_FALSE, (float *)GFX_get_modelview_projection_matrix());
	OBJ_draw_mesh(obj, i);
	GFX_pop_matrix();
	++i;
    }
    NAVIGATION_draw(navigation);
    dynamicsworld->stepSimulation(1.0f / 60.0f);
}

void templateAppToucheBegan(float x, float y, unsigned int tap_count)
{
    if (tap_count == 2)
	double_tap = 1;
    view_location.x = x;
    view_location.y = y;
}

void templateAppToucheMoved(float x, float y, unsigned int tap_count)
{
    view_delta.x = view_delta.x * 0.75f + (x - view_location.x) * 0.25f;
    view_delta.y = view_delta.y * 0.75f + (y - view_location.y) * 0.25f;
    view_location.x = x;
    view_location.y = y;
}

void templateAppExit(void)
{
    if (path_point) {
	SHADER_free(path_point->vertex_shader);
	SHADER_free(path_point->fragment_shader);
	PROGRAM_free(path_point);
    }
    NAVIGATION_free(navigation);
    free_physic_world();
    OBJ_free(obj);
}
int main(int argc, char *argv[])
{
    return klaatu_main(argc, argv);
}

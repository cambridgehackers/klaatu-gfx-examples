/* see the file COPYRIGHT for copyright notice for this file */
#include "templateApp.h"

class KlaatuAPI : public KlaatuAPITemplate {
public:
    void init(int width, int height);
    void draw(void);
    void finish(void);
};
static KlaatuAPI current_methods;
#define OBJ_FILE ( char * )"model.obj"
#define VERTEX_SHADER ( char * )"vertex.glsl"
#define FRAGMENT_SHADER ( char * )"fragment.glsl"
#define DEBUG_SHADERS 1
OBJ *obj = NULL;
OBJMESH *objmesh = NULL;
PROGRAM *program = NULL;
unsigned char auto_rotate = 0;
vec2 touche = { 0.0f, 0.0f };
vec3 rot_angle = { 0.0f, 0.0f, 0.0f };

void program_draw_callback(void *ptr)
{
    PROGRAM *curr_program = (PROGRAM *) ptr;
    unsigned int i = 0;
    while (i != curr_program->uniform_count) {
	if (!strcmp(curr_program->uniform_array[i].name, "MODELVIEWPROJECTIONMATRIX")) {
	    glUniformMatrix4fv(curr_program->uniform_array[i].location, 1, GL_FALSE, (float *)GFX_get_modelview_projection_matrix());
	}
	++i;
    }
}

void templateAppInit(int width, int height)
{
    GFX_start();
    glViewport(0.0f, 0.0f, width, height);
    GFX_set_matrix_mode(PROJECTION_MATRIX);
    GFX_load_identity();
    GFX_set_perspective(45.0f, (float)width / (float)height, 0.1f, 100.0f, 0.0f);
    program = PROGRAM_create((char *)"default", VERTEX_SHADER, FRAGMENT_SHADER, 1, DEBUG_SHADERS, NULL, program_draw_callback);
    obj = OBJ_load(OBJ_FILE, 1);
    unsigned char *vertex_array = NULL, *vertex_start = NULL;
    unsigned int i = 0, index = 0, stride = 0, size = 0;
    objmesh = &obj->objmesh[0];
    size = objmesh->n_objvertexdata * sizeof(vec3) * sizeof(vec3);
    vertex_array = (unsigned char *)malloc(size);
    vertex_start = vertex_array;
    while (i != objmesh->n_objvertexdata) {
	index = objmesh->objvertexdata[i].vertex_index;
	memcpy(vertex_array, &obj->indexed_vertex[index], sizeof(vec3));
	vertex_array += sizeof(vec3);
	memcpy(vertex_array, &obj->indexed_normal[index], sizeof(vec3));
	vertex_array += sizeof(vec3);
	++i;
    }
    glGenBuffers(1, &objmesh->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, objmesh->vbo);
    glBufferData(GL_ARRAY_BUFFER, size, vertex_start, GL_STATIC_DRAW);
    free(vertex_start);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glGenBuffers(1, &objmesh->objtrianglelist[0].vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, objmesh->objtrianglelist[0].vbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, objmesh->objtrianglelist[0].n_indice_array * sizeof(unsigned short), objmesh->objtrianglelist[0].indice_array, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    unsigned char attribute;
    stride = sizeof(vec3) + sizeof(vec3);
    glGenVertexArraysOES(1, &objmesh->vao);
    glBindVertexArrayOES(objmesh->vao);
    glBindBuffer(GL_ARRAY_BUFFER, objmesh->vbo);
    attribute = PROGRAM_get_vertex_attrib_location(program, (char *)"POSITION");
    glEnableVertexAttribArray(attribute);
    glVertexAttribPointer(attribute, 3, GL_FLOAT, GL_FALSE, stride, (void *)NULL);
    attribute = PROGRAM_get_vertex_attrib_location(program, (char *)"NORMAL");
    glEnableVertexAttribArray(attribute);
    glVertexAttribPointer(attribute, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(sizeof(vec3)));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, objmesh->objtrianglelist[0].vbo);
    glBindVertexArrayOES(0);
}

void templateAppDraw(void)
{
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    GFX_set_matrix_mode(MODELVIEW_MATRIX);
    GFX_load_identity(); {
	vec3 e = { 0.0f, -4.0f, 0.0f }, c = {
	0.0f, 0.0f, 0.0f}, u = {
	0.0f, 0.0f, 1.0f};
	GFX_look_at(&e, &c, &u);
    }
    glBindVertexArrayOES(objmesh->vao);
    if (auto_rotate)
	rot_angle.z += 2.0f;
    GFX_rotate(rot_angle.x, 1.0f, 0.0f, 0.0f);
    GFX_rotate(rot_angle.z, 0.0f, 0.0f, 1.0f);
    PROGRAM_draw(program);
    glDrawElements(GL_TRIANGLES, objmesh->objtrianglelist[0].n_indice_array, GL_UNSIGNED_SHORT, (void *)NULL);
}

void templateAppToucheBegan(float x, float y, unsigned int tap_count)
{
    if (tap_count == 2)
	auto_rotate = !auto_rotate;
    touche.x = x;
    touche.y = y;
}

void templateAppToucheMoved(float x, float y, unsigned int tap_count)
{
    auto_rotate = 0;
    rot_angle.z += -(touche.x - x);
    rot_angle.x += -(touche.y - y);
    touche.x = x;
    touche.y = y;
}

void templateAppExit(void)
{
    //OBJ_free(obj);
}
int main(int argc, char *argv[])
{
    return klaatu_main(argc, argv);
}

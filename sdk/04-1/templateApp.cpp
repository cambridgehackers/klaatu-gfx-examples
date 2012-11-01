/* see the file COPYRIGHT for copyright notice for this file */
#include "templateApp.h"

class KlaatuAPI : public KlaatuAPITemplate {
public:
    void init(int width, int height);
    void draw(void);
    void finish(void);
};
static KlaatuAPI current_methods;
#define OBJ_FILE ( char * )"scene.obj"
OBJ *obj = NULL;

void program_bind_attrib_location(void *ptr)
{
    PROGRAM *program = (PROGRAM *) ptr;
    glBindAttribLocation(program->pid, 0, "POSITION");
    glBindAttribLocation(program->pid, 2, "TEXCOORD0");
}

void material_draw_callback(void *ptr)
{
    OBJMATERIAL *objmaterial = (OBJMATERIAL *) ptr;
    PROGRAM *program = objmaterial->program;
    unsigned int i = 0;
    while (i != program->uniform_count) {
	if (!strcmp(program->uniform_array[i].name, "DIFFUSE")) {
	    glUniform1i(program->uniform_array[i].location, 1);
	} else if (!strcmp(program->uniform_array[i].name, "MODELVIEWPROJECTIONMATRIX")) {
	    glUniformMatrix4fv(program->uniform_array[i].location, 1, GL_FALSE, (float *)GFX_get_modelview_projection_matrix());
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
    GFX_set_perspective(45.0f, (float)width / (float)height, 0.1f, 100.0f, -90.0f);
    obj = OBJ_load(OBJ_FILE, 1);
    unsigned int i = 0;
    while (i != obj->n_objmesh) {
	OBJ_build_mesh(obj, i);
	OBJ_free_mesh_vertex_data(obj, i);
	++i;
    }
    i = 0;
    while (i != obj->n_texture) {
	OBJ_build_texture(obj, i, obj->texture_path, TEXTURE_MIPMAP, TEXTURE_FILTER_2X, 0.0f);
	++i;
    }
    i = 0;
    while (i != obj->n_objmaterial) {
	OBJ_build_material(obj, i, NULL);
	obj->objmaterial[i].program = PROGRAM_create((char *)"default", (char *)"vertex.glsl", (char *)"fragment.glsl", 1, 1, program_bind_attrib_location, NULL);
	OBJ_set_draw_callback_material(obj, i, material_draw_callback);
	++i;
    }
}

void templateAppDraw(void)
{
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    GFX_set_matrix_mode(MODELVIEW_MATRIX);
    GFX_load_identity();
    vec3 e = { 0.0f, -6.0f, 1.35f }, c = {
    0.0f, -5.0f, 1.35f}, u = {
    0.0f, 0.0f, 1.0f};
    GFX_look_at(&e, &c, &u);
    unsigned int i = 0;
    while (i != obj->n_objmesh) {
	GFX_push_matrix();
	GFX_translate(obj->objmesh[i].location.x, obj->objmesh[i].location.y, obj->objmesh[i].location.z);
	OBJ_draw_mesh(obj, i);
	GFX_pop_matrix();
	++i;
    }
}

void templateAppExit(void)
{
    unsigned i = 0;
    while (i != obj->n_objmaterial) {
	SHADER_free(obj->objmaterial[i].program->vertex_shader);
	SHADER_free(obj->objmaterial[i].program->fragment_shader);
	PROGRAM_free(obj->objmaterial[i].program);
	++i;
    }
    OBJ_free(obj);
}
int main(int argc, char *argv[])
{
    return klaatu_main(argc, argv);
}

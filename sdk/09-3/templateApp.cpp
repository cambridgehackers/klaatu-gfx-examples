/* see the file COPYRIGHT for copyright notice for this file */
#include "templateApp.h"

class KlaatuAPI : public KlaatuAPITemplate {
public:
    void init(int width, int height);
    void draw(void);
    void finish(void);
    void touchStart(float x, float y, unsigned int tap_count=0);
};
static KlaatuAPI current_methods;
#define OBJ_FILE ( char * )"piano.obj"
#define VERTEX_SHADER ( char * )"vertex.glsl"
#define FRAGMENT_SHADER ( char * )"fragment.glsl"
#define MAX_PIANO_KEY 13
SOUNDBUFFER *soundbuffer[MAX_PIANO_KEY];
SOUND *soundsource[MAX_PIANO_KEY];
SOUNDBUFFER *wrongbuffer;
SOUND *wrong;
SOUNDBUFFER *ambientbuffer;
SOUND *ambient;
THREAD *thread = NULL;
OBJ *obj = NULL;
PROGRAM *program = NULL;
int viewport_matrix[4];
vec2 touche;
unsigned char pick = 0;
vec4 color;
unsigned int sound_index;
#define MAX_LEVEL 50
unsigned char game_over = 0;
unsigned int cur_level = 0, cur_level_sound, level[MAX_LEVEL], cur_player_sound;
FONT *font_small = NULL, *font_big = NULL;

void program_bind_attrib_location(void *ptr)
{
    PROGRAM *program = (PROGRAM *) ptr;
    glBindAttribLocation(program->pid, 0, "POSITION");
    glBindAttribLocation(program->pid, 2, "TEXCOORD0");
}

void decompress_stream(void *ptr)
{
    SOUND_update_queue(ambient);
}

void next_level(void)
{
    unsigned int i = 0;
    ++cur_level;
    while (i != cur_level) {
	level[i] = rand() % MAX_PIANO_KEY;
	++i;
    }
    cur_level_sound = 0;
}

void templateAppInit(int width, int height)
{
    GFX_start();
    AUDIO_start();
    glViewport(0.0f, 0.0f, width, height);
    glGetIntegerv(GL_VIEWPORT, &viewport_matrix[0]);
    obj = OBJ_load(OBJ_FILE, 1);
    unsigned int i = 0;
    while (i != obj->n_objmesh) {
	OBJ_optimize_mesh(obj, i, 128);
	OBJ_build_mesh2(obj, i);
	OBJ_free_mesh_vertex_data(obj, i);
	++i;
    }
    MEMORY *memory = NULL;
    i = 0;
    while (i != MAX_PIANO_KEY) {
	char soundfile[MAX_CHAR] = { "" };
	sprintf(soundfile, "%02d.ogg", i);
	memory = mopen(soundfile, 1);
	soundbuffer[i] = SOUNDBUFFER_load(soundfile, memory);
	mclose(memory);
	soundsource[i] = SOUND_add(obj->objmesh[i].name, soundbuffer[i]);
	++i;
    }
    memory = mopen((char *)"wrong.ogg", 1);
    wrongbuffer = SOUNDBUFFER_load((char *)"wrong", memory);
    mclose(memory);
    wrong = SOUND_add((char *)"wrong", wrongbuffer);
    memory = mopen((char *)"lounge.ogg", 1);
    ambientbuffer = SOUNDBUFFER_load_stream((char *)"lounge", memory);
    ambient = SOUND_add((char *)"lounge", ambientbuffer);
    thread = THREAD_create(decompress_stream, NULL, THREAD_PRIORITY_NORMAL, 1);
    THREAD_play(thread);
    SOUND_set_volume(ambient, 0.5f);
    SOUND_play(ambient, 1);
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
    srandom(get_milli_time());
    next_level();
    font_small = FONT_init((char *)"foo.ttf");
    FONT_load(font_small, font_small->name, 1, 32.0f, 512, 512, 32, 96);
    font_big = FONT_init((char *)"foo.ttf");
    FONT_load(font_big, font_big->name, 1, 64.0f, 512, 512, 32, 96);
}

void templateAppDraw(void)
{
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    GFX_set_matrix_mode(PROJECTION_MATRIX);
    GFX_load_identity();
    GFX_set_perspective(50.0f, (float)viewport_matrix[2] / (float)viewport_matrix[3], 1.0f, 100.0f, -90.0f);
    GFX_set_matrix_mode(MODELVIEW_MATRIX);
    GFX_load_identity();
    vec3 e = { 0.0f, -8.0f, 7.5f }, c = {
    0.0f, 2.0f, 0.0f}, u = {
    0.0f, 0.0f, 1.0f};
    GFX_look_at(&e, &c, &u);
    PROGRAM_draw(program);
    if (cur_level_sound != cur_level) {
	static unsigned int start = get_milli_time();
	if (get_milli_time() - start > 1000) {
	    if (SOUND_get_state(soundsource[level[cur_level_sound]]) != AL_PLAYING) {
		SOUND_set_volume(soundsource[level[cur_level_sound]], 1.0f);
		SOUND_play(soundsource[level[cur_level_sound]], 0);
		++cur_level_sound;
	    }
	    start = get_milli_time();
	}
	cur_player_sound = 0;
    } else if (pick) {
	glUniform1i(PROGRAM_get_uniform_location(program, (char *)"DIFFUSE"), 7);
	unsigned int i = 0;
	while (i != obj->n_objmesh) {
	    OBJMESH *objmesh = &obj->objmesh[i];
	    GFX_push_matrix();
	    GFX_translate(objmesh->location.x, objmesh->location.y, objmesh->location.z);
	    glUniformMatrix4fv(PROGRAM_get_uniform_location(program, (char *)"MODELVIEWPROJECTIONMATRIX"), 1, GL_FALSE, (float *)GFX_get_modelview_projection_matrix());
	    generate_color_from_index(i, &color);
	    glUniform4fv(PROGRAM_get_uniform_location(program, (char *)"COLOR"), 1, (float *)&color);
	    OBJ_draw_mesh(obj, i);
	    GFX_pop_matrix();
	    ++i;
	}
	ucol4 ucolor;
	glReadPixels(touche.x, viewport_matrix[3] - touche.y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &ucolor);
	if (ucolor.b < MAX_PIANO_KEY) {
	    sscanf(obj->objmesh[ucolor.b].name, "%d", &sound_index);
	    if (level[cur_player_sound] != sound_index) {
		SOUND_set_volume(wrong, 1.0f);
		SOUND_play(wrong, 0);
		game_over = 1;
	    } else {
		SOUND_set_volume(soundsource[sound_index], 1.0f);
		SOUND_play(soundsource[sound_index], 0);
		++cur_player_sound;
	    }
	}
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    }
    pick = 0;
    unsigned char source_playing = 0;
    glUniform1i(PROGRAM_get_uniform_location(program, (char *)"DIFFUSE"), 1);
    unsigned int i = 0;
    while (i != obj->n_objmesh) {
	OBJMESH *objmesh = &obj->objmesh[i];
	GFX_push_matrix();
	GFX_translate(objmesh->location.x, objmesh->location.y, objmesh->location.z);
	glUniformMatrix4fv(PROGRAM_get_uniform_location(program, (char *)"MODELVIEWPROJECTIONMATRIX"), 1, GL_FALSE, (float *)GFX_get_modelview_projection_matrix());
	sscanf(objmesh->name, "%d", &sound_index);
	if (!strstr(objmesh->name, "curtain") && SOUND_get_state(soundsource[sound_index]) == AL_PLAYING) {
	    source_playing = 1;
	    color.x = 1.0f;
	    color.y = 1.0f;
	    color.z = 1.0f;
	    color.w = 1.0f;
	} else {
	    color.x = color.y = color.z = 0.0f;
	    color.w = 1.0f;
	}
	glUniform4fv(PROGRAM_get_uniform_location(program, (char *)"COLOR"), 1, (float *)&color);
	OBJ_draw_mesh(obj, i);
	GFX_pop_matrix();
	++i;
    }
    if (cur_player_sound == cur_level && !source_playing)
	next_level();
    GFX_set_matrix_mode(PROJECTION_MATRIX);
    GFX_load_identity();
    float half_width = (float)viewport_matrix[2] * 0.5f, half_height = (float)viewport_matrix[3] * 0.5f;
    GFX_set_orthographic_2d(-half_width, half_width, -half_height, half_height);
    GFX_rotate(-90.0f, 0.0f, 0.0f, 1.0f);
    GFX_translate(-half_height, -half_width, 0.0f);
    GFX_set_matrix_mode(MODELVIEW_MATRIX);
    GFX_load_identity();
    char str[MAX_CHAR] = { "" };
    if (game_over) {
	strcpy(str, "GAME OVER");
	/* Yellow. */
	color.x = 1.0f;
	color.y = 1.0f;
	color.z = 0.0f;
	color.w = 1.0f;
	FONT_print(font_big, viewport_matrix[3] * 0.5f - FONT_length(font_big, str) * 0.5f, viewport_matrix[2] - font_big->font_size * 1.5f, str, &color);
    }
    sprintf(str, "Level:%d", cur_level);
    /* Green. */
    color.x = 0.0f;
    color.y = 1.0f;
    color.z = 0.0f;
    color.w = 1.0f;
    FONT_print(font_small, 5.0f, viewport_matrix[2] - font_small->font_size, str, &color);
}

void templateAppToucheBegan(float x, float y, unsigned int tap_count)
{
    if (!game_over)
	pick = 1;
    else if (game_over && tap_count >= 2) {
	game_over = cur_level = 0;
	next_level();
    }
    touche.x = x;
    touche.y = y;
}

void templateAppExit(void)
{
    FONT_free(font_small);
    FONT_free(font_big);
    THREAD_free(thread);
    unsigned int i = 0;
    while (i != MAX_PIANO_KEY) {
	SOUND_free(soundsource[i]);
	SOUNDBUFFER_free(soundbuffer[i]);
	++i;
    }
    SOUND_free(wrong);
    SOUNDBUFFER_free(wrongbuffer);
    SOUND_free(ambient);
    mclose(ambientbuffer->memory);
    SOUNDBUFFER_free(ambientbuffer);
    AUDIO_stop();
    OBJ_free(obj);
}
int main(int argc, char *argv[])
{
    return klaatu_main(argc, argv);
}

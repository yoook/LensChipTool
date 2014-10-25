#include <gphoto2/gphoto2.h>

#ifndef __HELPER_H__
#define __HELPER_H__



/*
 * some global settings
 */
extern int use_DOF;         // move only the aperture via DOF preview instead of shutter
extern int aperture_speed;  // how fast to move the aperture. milliseconds
extern int check_success;   //if 0 don't stop on error if possible to continue
extern int verbosity;       // 0: only errors, 1: progress, 2: additionsal information
extern const char *version;

extern int SET_AF;           // 0: MF, 1: AF, 2: toggle
extern int SET_MAX_APERTURE; // x10. set_max_aperture = 56 <--> f/5.6
extern int SET_MICROADJUST;  //
extern int SET_FOCALLENGTH;  // millimeter
extern char CHIP[30];        // type of chip, that will be programmed
extern char MODE;            // auto exposure mode: M: Manuel, A: Av, n: nothing useful




int unmount_camera();
GPContext* create_context();
void print_whole_config(Camera *camera, GPContext *context);


int  has_value_listwidget(Camera *camera, char const *widgetName, char const* value, GPContext *context);
void set_aperture(Camera *camera, int val, GPContext *context);
int  get_aperture(Camera *camera, GPContext *context);
int get_max_aperture(Camera *camera, GPContext *context);
void set_depthoffield(Camera *camera, int mode, GPContext *context);
void set_shutterspeed(Camera *camera, char const *c_ss, GPContext *context);
void get_shutterspeed(Camera *camera, char *c_ss, int buf_len, GPContext *context);
void set_imageformat(Camera *camera, char const *c_if, GPContext *context);
void get_imageformat(Camera *camera, char *c_if, int buf_len, GPContext *context);
void get_lensname(Camera *camera, char *c_ln, int buf_len, GPContext *context);
char get_autoexposuremode(Camera *camera, GPContext *context);

void msleep(int milliseconds);
void DOF_or_shutter(Camera *camera, int overwrite_use_DOF, GPContext *context);

void error_handler(int errcode, const char* message);
void error_handler_exit(int errcode, const char* message);

void print_usage();



#endif

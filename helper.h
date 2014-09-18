#include <gphoto2/gphoto2.h>

#ifndef __helper_h__
#define __helper_h__



struct InterestingApertures
{
    int current;
    int first;
    int last;
    int second_last;
};


int unmount_camera();
GPContext* create_context();
int get_config_value_string (Camera *camera, const char *key, char **str, GPContext *context);
int set_config_value_string (Camera *camera, const char *key, const char *val, GPContext *context);
void print_whole_config(Camera *camera, GPContext *context);
void print_whole_config_linear(Camera *camera, GPContext *context);


void set_aperture(Camera *camera, int val, GPContext *context);
int get_aperture(Camera *camera, struct InterestingApertures *int_ap, GPContext *context);
int get_max_aperture(Camera *camera, GPContext *context);
void set_shutterspeed(Camera *camera, char const *c_ss, GPContext *context);
void get_shutterspeed(Camera *camera, char c_ss_return[], GPContext *context);

void shutter(Camera *camera, GPContext *context);

void error_handler(int errcode, const char* message);
void error_handler_exit(int errcode, const char* message);





#endif

#ifndef __CHECKS_H__
#define __CHECKS_H__

int check_DOF_available( Camera *camera, GPContext *context);
int check_chip_attached( Camera *camera, GPContext *context);
int check_autoexposuremode( Camera *camera, int require_M, GPContext *context);
int check_1_3_steps( Camera *camera, GPContext *context);

#endif

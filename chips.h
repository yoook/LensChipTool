#include <gphoto2/gphoto2.h>




#ifndef __CHIPS_H__
#define __CHIPS_h__


// chips actually handled by this program
#define LCT_CHIP_UNSUPPORTED    -1
#define LCT_CHIP_UNKNOWN    0
#define LCT_CHIP_EMF        1
#define LCT_CHIP_DANDELION  2
#define LCT_CHIP_OPTIX      3



//int emf_open_programmingmode(Camera *ca, GPContext *cxt);
//int emf_close_programmingmode(Camera *ca, GPContext *cxt);

void emf_set_maxaperture(Camera *ca, int maxaperture, GPContext *cxt);
void emf_set_focallength(Camera *ca, int focallength, GPContext *cxt);
void emf_set_microadjust(Camera *ca, int microadjust, GPContext *cxt);

#endif

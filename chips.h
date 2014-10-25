#ifndef __CHIPS_H__
#define __CHIPS_H__


// chips actually handled by this program
#define LCT_CHIP_UNSUPPORTED    -1
#define LCT_CHIP_UNKNOWN    0
#define LCT_CHIP_EMF        1
#define LCT_CHIP_DANDELION  2
#define LCT_CHIP_OPTIX      3



//int emf_open_programmingmode(Camera *ca, GPContext *cxt);
//int emf_close_programmingmode(Camera *ca, GPContext *cxt);

void prepare(Camera *ca, GPContext *ctx);
int write_af(Camera *ca, int af, GPContext *ctx);
int write_max_aperture(Camera *ca, int maxaperture, GPContext *cxt);
int write_microadjust(Camera *ca, int microadjust, GPContext *cxt);
int write_focallength(Camera *ca, int focallength, GPContext *cxt);
void finish(Camera *ca, GPContext *ctx);

#endif

#include <gphoto2/gphoto2.h>

#include "helper.h"
/** get digit and convert to aperture number
 *
 * for emf_chip digits are coded as special aperture values.
 * this function does this convertion
 **/
int _digit_to_ap(int number, int digit)       // digit=1 is last digit, =2 secondlast etc.
{
    int i;
    for(i=1; i<digit; i++)
        number = number/10;

    switch (number % 10)
    {
        case 0: return 20;
        case 1: return 22;
        case 2: return 25;
        case 3: return 28;
        case 4: return 32;
        case 5: return 35;
        case 6: return 40;
        case 7: return 45;
        case 8: return 50;
        case 9: return 56;
        default: return -1;
    }
}


int _emf_open_programmingmode(Camera *ca, GPContext *cxt)
{
    set_aperture(ca, 640, cxt); shutter(ca, cxt);
    set_aperture(ca, 570, cxt); shutter(ca, cxt);
    set_aperture(ca, 640, cxt); shutter(ca, cxt);
    //TODO: check, if successfull

    return 0;
}


int _emf_close_programmingmode(Camera *ca, GPContext *cxt)
{
    set_aperture(ca, 570, cxt); shutter(ca, cxt);
    set_aperture(ca, 640, cxt); shutter(ca, cxt);
    set_aperture(ca, 570, cxt); shutter(ca, cxt);
    //TODO: check, if successfull
    return 0;
}


void emf_set_maxaperture(Camera *ca, int maxaperture, GPContext *cxt)
{
    _emf_open_programmingmode(ca, cxt);

    set_aperture(ca, 20, cxt); shutter(ca, cxt);            // choose max_aperture_mode

    set_aperture(ca, maxaperture, cxt); shutter(ca, cxt);   //set max_aperture

    _emf_close_programmingmode(ca, cxt);
}


void emf_set_focallength(Camera *ca, int focallength, GPContext *cxt)
{
    _emf_open_programmingmode(ca, cxt);

    set_aperture(ca, 22, cxt); shutter(ca, cxt);    // choose focallength_mode

    set_aperture(ca, _digit_to_ap( focallength, 5), cxt); shutter(ca, cxt);   // set focallength digit by digit
    set_aperture(ca, _digit_to_ap( focallength, 4), cxt); shutter(ca, cxt);
    set_aperture(ca, _digit_to_ap( focallength, 3), cxt); shutter(ca, cxt);
    set_aperture(ca, _digit_to_ap( focallength, 2), cxt); shutter(ca, cxt);
    set_aperture(ca, _digit_to_ap( focallength, 1), cxt); shutter(ca, cxt);

    _emf_close_programmingmode(ca, cxt);
}


void emf_set_microadjust(Camera *ca, int microadjust, GPContext *cxt)
{
    _emf_open_programmingmode(ca, cxt);

    set_aperture(ca, 25, cxt); shutter(ca, cxt);    // choose microadjust_mode

    set_aperture(ca, _digit_to_ap( microadjust, 2), cxt); shutter(ca, cxt);   // set microadjust digit by digit
    set_aperture(ca, _digit_to_ap( microadjust, 1), cxt); shutter(ca, cxt);

    _emf_close_programmingmode(ca, cxt);
}



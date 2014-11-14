#include <string.h>
#include <stdio.h>

#include "helper.h"
#include "chips.h"


///////////////// chip specific worker functions

/////// emf chip

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
    if (verbosity>=2) printf( "start programming mode...\n");
    set_aperture(ca, 640, cxt); DOF_or_shutter(ca, -1, cxt);
    set_aperture(ca, 570, cxt); DOF_or_shutter(ca, -1, cxt);
    set_aperture(ca, 640, cxt); DOF_or_shutter(ca, -1, cxt);

    return 0;
}



int _emf_close_programmingmode(Camera *ca, GPContext *cxt)
{
    if (verbosity>=2) printf( "close programming mode...\n");
    set_aperture(ca, 570, cxt); DOF_or_shutter(ca, -1, cxt);
    set_aperture(ca, 640, cxt); DOF_or_shutter(ca, -1, cxt);
    set_aperture(ca, 570, cxt); DOF_or_shutter(ca, -1, cxt);

    return 0;
}



int _emf_write_max_aperture(Camera *ca, int maxaperture, GPContext *cxt)
{
    _emf_open_programmingmode(ca, cxt);
    if (get_max_aperture(ca, cxt) != 10) { fprintf(stderr, "open programming mode not successful\n"); return 1; }

    if (verbosity>=2) printf( "select max aperture\n");
    set_aperture(ca, 20, cxt); DOF_or_shutter(ca, -1, cxt);            // choose max_aperture_mode
    if (get_max_aperture(ca, cxt) != 10) { fprintf(stderr, "programming mode was lost\n"); return 1; }

    if (verbosity>=2) printf( "set max aperture\n");
    set_aperture(ca, maxaperture, cxt); DOF_or_shutter(ca, -1, cxt);   //set max_aperture

    _emf_close_programmingmode(ca, cxt);
    if (get_max_aperture(ca, cxt) == 10)
    {
        fprintf(stderr, "close programming mode not successful, retry\n");
        msleep(500);
        _emf_close_programmingmode(ca, cxt);
        if (get_max_aperture(ca, cxt) == 10) { fprintf(stderr, "can't close programming mode\n"); return 1; }
        if (verbosity >= 1) printf( "closed programming mode successfully at second try.\n");
    }
    return 0;
}



int _emf_write_microadjust(Camera *ca, int microadjust, GPContext *cxt)
{
    _emf_open_programmingmode(ca, cxt);
    if (get_max_aperture(ca, cxt) != 10) { fprintf(stderr, "open programming mode not successful\n"); return 1; }

    if (verbosity>=2) printf( "select microadjust\n");
    set_aperture(ca, 25, cxt); DOF_or_shutter(ca, -1, cxt);    // choose microadjust_mode
    if (get_max_aperture(ca, cxt) != 10) { fprintf(stderr, "programming mode was lost\n"); return 1; }

    if (verbosity>=2) printf( "set microadjust digit by digit\n");
    set_aperture(ca, _digit_to_ap( microadjust, 2), cxt); DOF_or_shutter(ca, -1, cxt);   // set microadjust digit by digit
    set_aperture(ca, _digit_to_ap( microadjust, 1), cxt); DOF_or_shutter(ca, -1, cxt);
    if (get_max_aperture(ca, cxt) != 10) { fprintf(stderr, "programming mode was lost\n"); return 1; }

    _emf_close_programmingmode(ca, cxt);
    if (get_max_aperture(ca, cxt) == 10)
    {
        fprintf(stderr, "close programming mode not successful, retry\n");
        msleep(500);
        _emf_close_programmingmode(ca, cxt);
        if (get_max_aperture(ca, cxt) == 10) { fprintf(stderr, "can't close programming mode\n"); return 1; }
        if (verbosity >= 1) printf( "closed programming mode successfully at second try.\n");
    }
    return 0;
}



int _emf_write_focallength(Camera *ca, int focallength, GPContext *cxt)
{
    _emf_open_programmingmode(ca, cxt);
    if (get_max_aperture(ca, cxt) != 10) { fprintf(stderr, "open programming mode not successful\n"); return 1; }

    if (verbosity>=2) printf( "select focallength\n");
    set_aperture(ca, 22, cxt); DOF_or_shutter(ca, -1, cxt);    // choose focallength_mode
    if (get_max_aperture(ca, cxt) != 10) { fprintf(stderr, "programming mode was lost\n"); return 1; }

    if (verbosity>=2) printf( "set focallength digit by digit\n");
    set_aperture(ca, _digit_to_ap( focallength, 5), cxt); DOF_or_shutter(ca, -1, cxt);   // set focallength digit by digit
    set_aperture(ca, _digit_to_ap( focallength, 4), cxt); DOF_or_shutter(ca, -1, cxt);
    set_aperture(ca, _digit_to_ap( focallength, 3), cxt); DOF_or_shutter(ca, -1, cxt);
    set_aperture(ca, _digit_to_ap( focallength, 2), cxt); DOF_or_shutter(ca, -1, cxt);
    set_aperture(ca, _digit_to_ap( focallength, 1), cxt); DOF_or_shutter(ca, -1, cxt);
    if (get_max_aperture(ca, cxt) != 10) { fprintf(stderr, "programming mode was lost\n"); return 1; }

    _emf_close_programmingmode(ca, cxt);
    if (get_max_aperture(ca, cxt) == 10)
    {
        fprintf(stderr, "close programming mode not successful, retry\n");
        msleep(500);
        _emf_close_programmingmode(ca, cxt);
        if (get_max_aperture(ca, cxt) == 10) { fprintf(stderr, "can't close programming mode\n"); return 1; }
        if (verbosity >= 1) printf( "closed programming mode successfully at second try.\n");
    }
    return 0;
}




/////////// wrapper functions, that destribute work to the function for the right chip

void prepare(Camera *ca, GPContext *cxt)
{
    if (verbosity >= 1) printf("\nPerparing camera...\n");
    if (MODE == 'M') set_shutterspeed(ca, "1/60", cxt);
    set_imageformat(ca, "Tiny JPEG (S3)", cxt);
}



int write_af(Camera *ca, int af, GPContext *cxt)
{
    int errors = 1;
    if (!strcmp(CHIP, "emf"))
        fprintf(stderr, "set AF is not supported by EMF chip\n");
    else
        printf( "this chip is not yet supported");
    return errors;
}



int write_max_aperture(Camera *ca, int aperture, GPContext *cxt)
{
    int errors = 1;
    if (!strcmp(CHIP, "emf"))
    {
        if (verbosity>=1) printf( "\nwrite max aperture now:\n");
        errors = _emf_write_max_aperture(ca, aperture, cxt);
    }
    else
        printf( "this chip is not yet supported");
    return errors;
}



int write_microadjust(Camera *ca, int microadjust, GPContext *cxt)
{
    int errors = 1;
    if (!strcmp(CHIP, "emf"))
    {
        if (verbosity>=1) printf( "\nwrite microadjust now:\n");
        errors = _emf_write_microadjust(ca, microadjust, cxt);
    }
    else
        printf( "this chip is not yet supported");
    return errors;
}



int write_focallength(Camera *ca, int focallength, GPContext *cxt)
{
    int errors = 1;
    if (!strcmp(CHIP, "emf"))
    {
        if (verbosity>=1) printf( "\nwrite focallength now:\n");
        errors = _emf_write_focallength(ca, focallength, cxt);
    }
    else
        printf( "this chip is not yet supported");
    return errors;
}



void finish(Camera *ca, GPContext *cxt)
{       // TODO: restore old values, check, if chip was written successfully

}

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <gphoto2/gphoto2-port-version.h>
#include <gphoto2/gphoto2-version.h>

#include "helper.h"
#include "chips.h"
#include "checks.h"


/*
 * some global settings
 */
int use_DOF = 0;                    // move only the aperture via DOF preview instead of shutter
int aperture_speed = 16;           // how fast to move the aperture, if using DOF. milliseconds
int omit_checks = 0;                // if 1 don't stop on error if possible to continue
int verbosity = 2;                  // 0: only errors, 1: progress, 2: additional information, 3: excessive information
char const *version = "0.0.0.1";

// values, that will be set on the chip. default == do nothing == -1
int SET_AF = -1;                    // 0: MF, 1: AF, 2: toggle
int SET_MAX_APERTURE = -1;          // x10. set_max_aperture = 56 <--> f/5.6
int SET_MICROADJUST = -1;           //
int SET_FOCALLENGTH = -1;           // millimeter
char CHIP[30] = "emf";              // type of chip, that will be programmed
char MODE = 'n';                    // auto exposure mode: M: Manuel, A: Av, n: nothing useful



int main(int argc, char **argv)
{
    // parse command line options and arguments

    int c;
    int i = 0;
    float f_set_max_aperture;

    while ((c = getopt (argc, argv, "v:fSDF:a:m:l:")) != -1)
    {
        i = 1;
        switch (c)
        {
        case 'v':   // set level of verbosity
            if (!strcmp(optarg, "0"))       verbosity = 0;
            else if (!strcmp(optarg, "1"))  verbosity = 1;
            else if (!strcmp(optarg, "2"))  verbosity = 2;
            else if (!strcmp(optarg, "3"))  verbosity = 3;
            else print_usage();
            break;

        case 'f':   // force == omit checks
            omit_checks = 1;
            break;

        case 'S':   // use shutter instead of DOF preview
            use_DOF = 0;
            break;

        case 'D':   // use DOF preview instead of shutter. Might fallback to shutte, if necessary
            use_DOF = 1;
            break;

        case 'F':   // autofocus.
            if (!strcmp(optarg, "0"))   SET_AF = 0;
            else if (!strcmp(optarg, "1"))   SET_AF = 1;
            else if (!strcmp(optarg, "2"))   SET_AF = 2;     // toggle AF
            else print_usage();
            break;

        case 'a':   // max aperture
            sscanf( optarg, "%f", &f_set_max_aperture);
            SET_MAX_APERTURE = (int) (f_set_max_aperture*10);
            break;

        case 'm':   // micro adjust
            sscanf( optarg, "%d", &SET_MICROADJUST);
            break;

        case 'l':   // focal length
            sscanf( optarg, "%d", &SET_FOCALLENGTH);
            break;

        case '?':
            print_usage();
        }
    }
    if ( i == 0 ) print_usage();




    // print some information:
    // versions
    if (verbosity >= 2)
    {
        printf( "LensChipTool version    %s\n", version);
        char const **vers=NULL;
        vers = gp_library_version(1);
        printf( "libgphoto2 version      %s\n", *vers);
        vers = gp_port_library_version(1);
        printf( "libgphoto2_port version %s\n\n", *vers);
    }

    // recognised parameters
    if (verbosity >= 2)
    {
        printf( "set this parameters:\n");
        if (SET_AF           != -1) printf( "  AF:           %d\n",   SET_AF);
        if (SET_MAX_APERTURE != -1) printf( "  max aperture: %g\n",   SET_MAX_APERTURE/10.);
        if (SET_MICROADJUST  != -1) printf( "  microadjust:  %d\n",   SET_MICROADJUST);
        if (SET_FOCALLENGTH  != -1) printf( "  focallength:  %d\n\n", SET_FOCALLENGTH);
    }




    // initialize libgphoto and camera

    Camera *camera = NULL;
    GPContext *context = gp_context_new();
    int retval=0;

    retval = gp_camera_new( &camera);
    error_handler_exit(retval, "gp_camera_new");

    retval = gp_camera_init(camera, context);
    if (retval == GP_ERROR_IO_USB_CLAIM || retval == GP_ERROR_IO_LOCK)
    {
        if (verbosity >= 1) printf("camera not initialised. Error %d. Unmounting and retry...\n", retval);
        retval = unmount_camera();
        error_handler(retval, "unmount_camera");

        retval = gp_camera_init(camera, context);
    }
    error_handler_exit(retval, "gp_camera_init");




    // print some more information, if very verbose

    if (verbosity >= 3)
    {
        CameraText summary;
        retval = gp_camera_get_summary(camera, &summary, context);
        error_handler(retval, "gp_camera_get_summary");
        printf("%s\n\n", summary.text);

        print_whole_config(camera, context);
        printf( "\n\n");
    }




    // check, if all settings on the camera are set as required

    int checks_failed = 0;
    int require_M = 0;
    if (SET_MAX_APERTURE > 0 && SET_MAX_APERTURE <= 10)     // there might be other reasons to require M,
        require_M = 1;                                      // add here later

    check_DOF_available(camera, context);
    checks_failed += check_chip_attached(camera, context);
    checks_failed += check_autoexposuremode(camera, require_M, context);
    checks_failed += check_1_3_steps(camera, context);

    if (checks_failed && !omit_checks)
    {
        printf( "    -> abort\n");
        return 1;
    }




    // save settings, that we will change, to restore them later
    MODE = get_autoexposuremode(camera, context);
    int old_ap = get_aperture(camera, context);
    int const buf_len_ss = 10;
    char old_ss[buf_len_ss + 1];
    if (MODE == 'M') get_shutterspeed(camera, old_ss, buf_len_ss, context);
    int const buf_len_if = 40;
    char old_if[buf_len_if + 1];
    get_imageformat(camera, old_if, buf_len_if, context);





    // do, what is ecessary to program the chip
    int errors = 0;
    if (SET_AF != -1 || SET_MAX_APERTURE != -1  || SET_MICROADJUST != -1 || SET_FOCALLENGTH != -1)  // do anything at all
        prepare(camera, context);

    if (SET_AF != -1)
        errors = write_af(camera, SET_AF, context);

    if (!errors && SET_MAX_APERTURE != -1)
        errors += write_max_aperture(camera, SET_MAX_APERTURE, context);

    if (!errors && SET_MICROADJUST != -1)
        errors += write_microadjust(camera, SET_MICROADJUST, context);

    if (!errors && SET_FOCALLENGTH != -1)
        errors += write_focallength(camera, SET_FOCALLENGTH, context);

    if (SET_AF != -1 || SET_MAX_APERTURE != -1  || SET_MICROADJUST != -1 || SET_FOCALLENGTH != -1)
        finish(camera, context);




    // restore old values, if possible
    if (verbosity >= 1) printf( "\nrestore old values\n");
    int max_ap = get_max_aperture(camera, context);
    set_aperture(camera, max_ap > old_ap? max_ap : old_ap, context);
    if (MODE == 'M') set_shutterspeed(camera, old_ss, context);
    set_imageformat(camera, old_if, context);

    //retval = gp_camera_exit(camera, context);     // this kind of blocks the camera :-/
    //error_handler(retval, "gp_camera_exit");

    if (errors)
    {
        printf("all done, but errors did occur\n");
        return 1;
    }
    if (verbosity >= 1) printf("all done successfully\n");
    return 0;
}





/*  Elements to care about:
 *
 *
 *
 * perhaps focusmode (won't shoot, if in one shot and can't focus, but SHOULD be in MF anyway)
 *
 *
 */

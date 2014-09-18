#include <stdio.h>
// for sleep();
#include <unistd.h>

#include <gphoto2/gphoto2.h>

#include "helper.h"
#include "chips.h"


int main()
{
    Camera *camera=NULL;
    int retval=0;
    GPContext *context=create_context();

    retval = gp_camera_new( &camera);
    error_handler_exit(retval, "gp_camera_new");

    retval = gp_camera_init(camera, context);
    if (retval == GP_ERROR_IO_USB_CLAIM)
    {
        printf("camera not initialised. Error %d. Unmounting and retry...\n", retval);
        retval = unmount_camera();
        error_handler(retval, "unmount_camera");

        retval = gp_camera_init(camera, context);
    }
    error_handler_exit(retval, "gp_camera_init");


/*  CameraText summary;
    retval = gp_camera_get_summary(camera, &summary, context);
    error_handler(retval, "gp_camera_get_summary");
    printf("%s\n", summary.text);
*/

/*    char *val=NULL;
    retval = get_config_value_string(camera,"iso",&val,context);
    error_handler(retval, "get_config_value_string");
    printf("iso: %s\n", val);
*/
//  print_whole_config_linear(camera, context);
//  printf("\n\n\n\n");
    print_whole_config(camera, context);

    struct InterestingApertures int_ap;
    get_aperture(camera, &int_ap, context);
    printf("first %i, last %i, second_last %i, current %i\n", int_ap.first, int_ap.last, int_ap.second_last, int_ap.current);
    int old_ap = int_ap.current;
    if (int_ap.last!=640)
    {
        printf("Are you shure you properly installed your lens chip?? -> abort\n");
        return 0;
    }
    if (int_ap.second_last!=570)
    {
        printf("Set Exposure Level Increments to 1/3 on your camera! -> abort\n");
        return 0;
    }
    char old_ss[7];
    get_shutterspeed(camera, old_ss, context);
// todo: enshure, camera is set to M
    set_shutterspeed(camera, "1/60", context);

//    emf_set_maxaperture(camera, 28, context);
//    emf_set_focallength(camera, 29, context);
    emf_set_microadjust(camera, 25, context);


// todo: enshure, don't restore old aperture, if new max aperture is smaller
//       set to maxaperture instead


/*  Elements to care about:
 *
 * aperture (get min / max is essential!)
 * lensname (check, if chip is attached)
 * shutterspeed
 *
 * what, if apperture increment != 1/3?
 *
 *
 * perhaps focusmode (won't shoot, if in one shot and can't focus, but SHOULD be in MF anyway)
 *
 *
 */


    sleep(0);
    set_aperture(camera, old_ap, context);
    set_shutterspeed(camera, old_ss, context);
    //retval = gp_camera_exit(camera, context);     // this kind of blocks the camera :-/
    //error_handler(retval, "gp_camera_exit");
    printf("survived to the end \n");
    return 0;
}

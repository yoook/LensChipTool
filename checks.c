#include <stdio.h>
#include <string.h>

#include "helper.h"
#include "checks.h"


// all checks in this file return 1 on error and 0 on success

int check_DOF_available( Camera *camera, GPContext *context)
{
    CameraWidget *configWidget = NULL;
    int retval;

    retval = gp_camera_get_config(camera, &configWidget, context);
    error_handler_exit(retval, "gp_camera_get_config");
    retval = gp_widget_get_child_by_name(configWidget, "depthoffield", NULL);
    gp_widget_free( configWidget);
    if (!retval)
    {
        use_DOF = 0;
        if (verbosity >= 1)
        {
            printf( "DOF preview not available. Shutter is used instead. Either your "
            "Camera does not support DOF preview or you should get a newer version "
            "of libgphoto2 (newer than 2.5.5.2 or at least svn release 15212)\n\n");
        }
        return 1;
    }
    if (verbosity >= 1) printf("DOF preview is available.\n");
    return 0;
}



int check_chip_attached( Camera *camera, GPContext *context)
{
    int const buf_len = 50;
    char lensname[buf_len + 1];
    get_lensname(camera, lensname, buf_len, context);

    if (strcmp( lensname, "1-65535mm"))
    {
        printf("Are you shure you properly installed your lens chip??\n");
        return 1;
    }
    if (verbosity >= 1) printf("EMF chip recognised -> OK\n");
    return 0;
}



int check_autoexposuremode( Camera *camera, int require_M, GPContext *context)
{
    char aem = get_autoexposuremode(camera, context);
    if (use_DOF && !require_M)
    {
        if (aem != 'A' && aem != 'M')
        {
            printf("Set auto exposure mode to Av or Manual!\n");
            return 1;
        }
    }
    else
    {
        if (aem != 'M')
        {
            printf("Set auto exposure mode Manual!\n");
            return 1;
        }
    }
    if (verbosity >= 1) printf ("mode dail set to %s -> OK\n", aem=='M' ? "M" : "Av");
    return 0;
}



int check_1_3_steps( Camera *camera, GPContext *context)
{
    int val=0;
    switch(get_autoexposuremode(camera, context))
    {
    case 'M':
        val = has_value_listwidget(camera, "shutterspeed", "5", context);
        break;
    case 'A':
        val = has_value_listwidget(camera, "aperture", "99", context)
           || has_value_listwidget(camera, "aperture", "20", context)
           || has_value_listwidget(camera, "aperture", "57", context);
        if (!val)
            if (!has_value_listwidget(camera, "aperture", "8", context)
                && !has_value_listwidget(camera, "aperture", "22", context)
                && !has_value_listwidget(camera, "aperture", "64", context))
            {
                printf("couldn't determine step size. something is wrong\n");
                return 1;
            }

        break;

    default:
        printf("I will check the steps only in 'M' or 'Av', so set your mode dail acordingly!\n");
        return 1;
    }

    if (val)
    {
        if (verbosity >= 1) printf("step size = 1/3 -> OK\n");
        return 0;
    }
    printf("Set the step size to 1/3!\n");
    return 1;
}

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


#include "helper.h"


////////// Functions for communication with camera

void _set_value_textwidget(Camera *camera, char const *widgetName, char const *c_value, GPContext *context)
{
    CameraWidget *configWidget, *interestingWidget;
    int retval;

    retval = gp_camera_get_config(camera, &configWidget, context);
    error_handler_exit(retval, "gp_camera_get_config");
    retval = gp_widget_get_child_by_name(configWidget, widgetName, &interestingWidget);
    error_handler_exit(retval, "gp_widget_get_child_by_name");

    retval = gp_widget_set_value(interestingWidget, c_value);
    error_handler_exit(retval, "gp_widget_set_value");
    retval = gp_camera_set_config(camera, configWidget, context);
    error_handler_exit(retval, "gp_camera_set_config");

    gp_widget_free(configWidget);
}



void _get_value_textwidget(Camera *camera, char const *widgetName, char *retstring, int buf_len, GPContext *context)
{
    CameraWidget *configWidget, *interestingWidget;
    int retval;

    char const *c_value;

    retval = gp_camera_get_config(camera, &configWidget, context);
    error_handler_exit(retval, "gp_camera_get_config");
    retval = gp_widget_get_child_by_name(configWidget, widgetName, &interestingWidget);
    error_handler_exit(retval, "gp_widget_get_child_by_name");

    retval = gp_widget_get_value(interestingWidget, &c_value);
    error_handler_exit(retval, "gp_widget_get_value");

    strncpy(retstring, c_value, buf_len);

    gp_widget_free(configWidget);
}



int has_value_listwidget(Camera *camera, char const *widgetName, char const* value, GPContext *context)
{
    CameraWidget *configWidget, *interestingWidget;
    int retval;

    retval = gp_camera_get_config(camera, &configWidget, context);
    error_handler_exit(retval, "gp_camera_get_config");
    retval = gp_widget_get_child_by_name(configWidget, widgetName, &interestingWidget);
    error_handler_exit(retval, "gp_widget_get_child_by_name");

    retval = 0;
    int i=0;
    char const *c_val=NULL;

    while(!retval)
    {
        retval = gp_widget_get_choice(interestingWidget, i, &c_val);
        if (!retval)
        {
            // printf("%s  ", c_val);
            if (!strcmp(c_val, value))      // found the desired value!
            {
                gp_widget_free( configWidget);
                return 1;
            }
        }
        i++;
    }

    gp_widget_free( configWidget);
    return 0;
}



const char* _get_type_string(CameraWidgetType type)
{
    char const *ret=NULL;

    switch(type)
    {
        case GP_WIDGET_WINDOW:  ret = "window"; break;
        case GP_WIDGET_SECTION: ret = "section"; break;
        case GP_WIDGET_TEXT:    ret = "text"; break;
        case GP_WIDGET_RANGE:   ret = "range"; break;
        case GP_WIDGET_TOGGLE:  ret = "toggle"; break;
        case GP_WIDGET_RADIO:   ret = "radio"; break;
        case GP_WIDGET_MENU:    ret = "menu"; break;
        case GP_WIDGET_BUTTON:  ret = "button"; break;
        case GP_WIDGET_DATE:    ret = "date"; break;
        default: ret="";
    }
    return ret;
}



void set_aperture(Camera *camera, int i_ap, GPContext *context)
{
    if (verbosity >= 2) printf( "set aperture to %g\n", i_ap/10.);
    char c_ap[4];
    sprintf(c_ap, "%.2g", i_ap/10.0);

    _set_value_textwidget(camera, "aperture", c_ap, context);
}



int get_aperture(Camera *camera, GPContext *context)
{
    int const buf_len=10;
    char  c_ap[buf_len+1];
    c_ap[buf_len] = '\0';
    float f_ap;
    int   i_ap;

    _get_value_textwidget(camera, "aperture", c_ap, buf_len, context);
    sscanf(c_ap, "%f", &f_ap);
    i_ap = (int) (f_ap*10);

    return i_ap;
}



/** just return max_aperture as int, -1 if error
 */
int get_max_aperture(Camera *camera, GPContext *context)
{
    CameraWidget *configWidget, *apertureWidget;
    int retval;

    retval = gp_camera_get_config(camera, &configWidget, context);
    error_handler_exit(retval, "gp_camera_get_config");
    retval = gp_widget_get_child_by_name(configWidget, "aperture", &apertureWidget);
    error_handler_exit(retval, "gp_widget_get_child_by_name");

    retval = 0;
    float f_ap;
    int   i_ap;
    char const *c_ap=NULL;

    retval = gp_widget_get_choice(apertureWidget, 0, &c_ap);
    if (!retval)
    {
        sscanf(c_ap, "%f", &f_ap);
        i_ap = (int) (f_ap*10);
        gp_widget_free(configWidget);
        return i_ap;
    }
    else
    {
        gp_widget_free(configWidget);
        return -1;
    }
}



void set_depthoffield(Camera *camera, int mode, GPContext *context)
{
    char cmode[2];
    cmode[0] = mode? '1' : '0';
    cmode[1] = '\0';

    _set_value_textwidget(camera, "depthoffield", cmode, context);
}



void set_shutterspeed(Camera *camera, char const *c_ss, GPContext *context)
{
    if (verbosity >= 2) printf( "set shutterspeed to %s\n", c_ss);
    _set_value_textwidget(camera, "shutterspeed", c_ss, context);
}



void get_shutterspeed(Camera *camera, char *c_ss, int buf_len, GPContext *context)
{
    _get_value_textwidget(camera, "shutterspeed", c_ss, buf_len, context);
    c_ss[buf_len] = '\0';
}



void set_imageformat(Camera *camera, char const *c_if, GPContext *context)
{
    if (verbosity >= 2) printf( "set imageformat so %s\n", c_if);
    _set_value_textwidget(camera, "imageformat", c_if, context);
}



void get_imageformat(Camera *camera, char *c_if, int buf_len, GPContext *context)
{
    _get_value_textwidget(camera, "imageformat", c_if, buf_len, context);
    c_if[buf_len] = '\n';
}



void get_lensname(Camera *camera, char *c_ln, int buf_len, GPContext *context)
{
    _get_value_textwidget(camera, "lensname", c_ln, buf_len, context);
    c_ln[buf_len] = '\0';
}



char get_autoexposuremode(Camera *camera, GPContext *context)
{
    int const buf_len=10;
    char c_aem[buf_len+1];
    c_aem[buf_len] = '\0';
    _get_value_textwidget(camera, "autoexposuremode", c_aem, buf_len, context);

    if (!strcmp(c_aem, "AV")) return 'A';
    if (!strcmp(c_aem, "Manual"))  return 'M';
    return 'n';

}



void msleep(int milliseconds)
{
    struct timespec tim;
    tim.tv_sec  = milliseconds / 1000;
    tim.tv_nsec = (milliseconds % 1000) * 1000000;

    nanosleep(&tim, NULL);
}



void _shutter(Camera *camera, GPContext *context)
{
    int retval;
    CameraFilePath camera_file_path;

	strcpy(camera_file_path.folder, "/");
	strcpy(camera_file_path.name, "foo.jpg");

    retval = gp_camera_capture(camera, GP_CAPTURE_IMAGE, &camera_file_path, context);
    error_handler(retval, "gp_camera_capture");

    retval = gp_camera_file_delete(camera, camera_file_path.folder, camera_file_path.name, context);
    error_handler(retval, "gp_camera_file_delete");
}



void _DOF(Camera *camera, int msec,  GPContext *context)
{
    set_depthoffield(camera, 1, context);
    msleep(msec);
    set_depthoffield(camera, 0, context);
}



void DOF_or_shutter(Camera *camera, int overwrite_use_DOF, GPContext *context)
{
        msleep(50);
    if (overwrite_use_DOF < 0) overwrite_use_DOF = use_DOF;
    if (overwrite_use_DOF == 0) _shutter(camera, context);
    else _DOF(camera, aperture_speed, context);
    if (verbosity >= 3) printf("max ap: %g\n", get_max_aperture(camera, context) / 10.);
    msleep(100);
}



/** Print information available about this widget
 *
 **/
void _print_this_widget(CameraWidget *widget)
{
    int id, ival;
    char const *name, *label, *typestring;
    char* cval=NULL;
    CameraWidgetType type;

    gp_widget_get_id(widget, &id);
    gp_widget_get_name(widget, &name);
    gp_widget_get_label(widget, &label);
    gp_widget_get_type(widget, &type);
    typestring = _get_type_string(type);

    printf("%d[%s]: %s(%s):", id, typestring, name, label);
    switch(type)
    {
        case GP_WIDGET_RADIO:
        case GP_WIDGET_MENU:
        case GP_WIDGET_TEXT:
            gp_widget_get_value(widget, &cval);
            printf(" -- \t%s\n", cval);
            break;

        case GP_WIDGET_TOGGLE:
            gp_widget_get_value(widget, &ival);
            printf(" -- \t%d\n", ival);
            break;

        default:
           printf("\n");
    }
}



void _print_whole_config_rec(CameraWidget *widget, int level, int bro)
{
    CameraWidget *child = NULL;
    int retval = 0, i=0, j=0;

    /* print who i am */
    for(j=0; j<level; j++) printf("   ");   // proper indentations
    _print_this_widget(widget);


    while (retval==GP_OK)
    {
        retval = gp_widget_get_child(widget, i, &child);
        if (retval==GP_OK)
            _print_whole_config_rec(child, level+1, i);
        i++;
    }
}



void print_whole_config(Camera *camera, GPContext *context)
{
    CameraWidget *widget = NULL;
    int retval;

    retval = gp_camera_get_config (camera, &widget, context);
    error_handler(retval, "gp_camera_get_config");
    if (retval!=GP_OK) return;

    _print_whole_config_rec(widget, 0, 0);
    gp_widget_free(widget);
}




////////// END Functions for communication with camera

/**          unmount the camera
 *
 *          TODO: do it without throwing an error message
 * \param   void    nothing
 * \return          error code
 *
 **/
int unmount_camera()
{
    int ret;
    ret = system("gvfs-mount -s gphoto2 2> /dev/null");
    sleep(1);

    return ret;
}



/**         error handling
 *
 *          handle the errors, that might occur in the gp_... functions
 *
 * \param   int errcode             the error code
 *          const char* message     the message, that shall be displayed in
 *                                  the case of an error
 *
 **/
void error_handler(int errcode, const char* message)
{
    if (errcode==0 && verbosity <=3)
        return;
    char const *errstring=NULL;

    errstring = gp_result_as_string(errcode);
//  errstring = gp_camera_get_result_as_string(errcode);

    fprintf(stderr, "%s", message);
    fprintf(stderr, " ERRORCODE: %d, %s\n", errcode, errstring);
}



void error_handler_exit(int errcode, const char* message)
{
    error_handler(errcode, message);
    if (errcode)
        exit(errcode);
}



void print_usage()
{       // TODO: change text to useful
    printf("Usage:\n"
            " one day this might be a useful usage message\n\n");
    exit(0);
}

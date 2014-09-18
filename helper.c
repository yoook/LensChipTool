#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>



#include "helper.h"

#if 0
#define verbose
#endif


////////// Functions for communication with camera
/*static void errordumper(GPLogLevel level, const char *domain, const char *str, void *data)        // Don't know, what it is good for
{
  fprintf(stdout, "%s\n", str);
}

static void ctx_error_func (GPContext *context, const char *str, void *data)
{
        fprintf  (stderr, "\n*** Contexterror ***              \n%s\n",str);
        fflush   (stderr);
}

static void ctx_status_func (GPContext *context, const char *str, void *data)
{
        fprintf  (stderr, "%s\n", str);
        fflush   (stderr);
}
*/
GPContext* create_context()
{
    GPContext * context=NULL;

    context = gp_context_new();
//    gp_context_set_error_func (context, ctx_error_func, NULL);        // Don't know, what it is good for
//    gp_context_set_status_func (context, ctx_status_func, NULL);

    return context;
}

/*
 * This function looks up a label or key entry of
 * a configuration widget.
 * The functions descend recursively, so you can just
 * specify the last component.
 */
static int _lookup_widget(CameraWidget*widget, const char *key, CameraWidget **child)
{
	int retval;
	retval = gp_widget_get_child_by_name (widget, key, child);
	return retval;
}

/* Gets a string configuration value.
 * This can be:
 *  - A Text widget
 *  - The current selection of a Radio Button choice
 *  - The current selection of a Menu choice
 *
 * Sample (for Canons eg):
 *   get_config_value_string (camera, "owner", &ownerstr, context);
 */
int get_config_value_string (Camera *camera, const char *key, char **str, GPContext *context)
{
	CameraWidget		*widget = NULL, *child = NULL;
	CameraWidgetType	type;
	int	  ret;
	char *val;

	ret = gp_camera_get_config (camera, &widget, context);
	if (ret < GP_OK)
    {
		fprintf (stderr, "camera_get_config failed: %d\n", ret);
		return ret;
	}
	ret = _lookup_widget (widget, key, &child);
	if (ret < GP_OK)
    {
		fprintf (stderr, "lookup widget failed: %d\n", ret);
		goto out;
	}
	/* This type check is optional, if you know what type the label
	 * has already. If you are not sure, better check. */

	ret = gp_widget_get_type (child, &type);
	if (ret < GP_OK)
    {
		fprintf (stderr, "widget get type failed: %d\n", ret);
		goto out;
	}
	switch (type)
    {
    case GP_WIDGET_MENU:
    case GP_WIDGET_RADIO:
    case GP_WIDGET_TEXT:
		break;
	default:
		fprintf (stderr, "widget has bad type %d\n", type);
		ret = GP_ERROR_BAD_PARAMETERS;
		goto out;
	}

	/* This is the actual query call. Note that we just
	 * a pointer reference to the string, not a copy... */
	ret = gp_widget_get_value (child, &val);
	if (ret < GP_OK)
    {
		fprintf (stderr, "could not query widget value: %d\n", ret);
		goto out;
	}
	/* Create a new copy for our caller. */
	*str = strdup (val);
out:
	gp_widget_free (widget);
	return ret;
}

/* Sets a string configuration value.
 * This can set for:
 *  - A Text widget
 *  - The current selection of a Radio Button choice
 *  - The current selection of a Menu choice
 *
 * Sample (for Canons eg):
 *   get_config_value_string (camera, "owner", &ownerstr, context);
 */
int set_config_value_string (Camera *camera, const char *key, const char *val, GPContext *context)
{
	CameraWidget		*widget = NULL, *child = NULL;
	CameraWidgetType	type;
	int			ret;

	ret = gp_camera_get_config (camera, &widget, context);
	if (ret < GP_OK) {
		fprintf (stderr, "camera_get_config failed: %d\n", ret);
		return ret;
	}
	ret = _lookup_widget (widget, key, &child);
	if (ret < GP_OK) {
		fprintf (stderr, "lookup widget failed: %d\n", ret);
		goto out;
	}

	/* This type check is optional, if you know what type the label
	 * has already. If you are not sure, better check. */
	ret = gp_widget_get_type (child, &type);
	if (ret < GP_OK) {
		fprintf (stderr, "widget get type failed: %d\n", ret);
		goto out;
	}
	switch (type)
    {
    case GP_WIDGET_MENU:
    case GP_WIDGET_RADIO:
    case GP_WIDGET_TEXT:
		/* This is the actual set call. Note that we keep
		 * ownership of the string and have to free it if necessary.
		 */
		ret = gp_widget_set_value (child, val);
		if (ret < GP_OK)
        {
			fprintf (stderr, "could not set widget value: %d\n", ret);
			goto out;
		}
		break;
    case GP_WIDGET_TOGGLE:
        {
            int ival;

            sscanf(val,"%d",&ival);
            ret = gp_widget_set_value (child, &ival);
            if (ret < GP_OK)
            {
                fprintf (stderr, "could not set widget value: %d\n", ret);
                goto out;
            }
            break;
        }
	default:
		fprintf (stderr, "widget has bad type %d\n", type);
		ret = GP_ERROR_BAD_PARAMETERS;
		goto out;
	}

	/* This stores it on the camera again */
	ret = gp_camera_set_config (camera, widget, context);
	if (ret < GP_OK)
    {
		fprintf (stderr, "camera_set_config failed: %d\n", ret);
		return ret;
	}
out:
	gp_widget_free (widget);
	return ret;
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
    CameraWidget *configWidget, *apertureWidget;
    int retval;

    retval = gp_camera_get_config(camera, &configWidget, context);
    error_handler_exit(retval, "gp_camera_get_config");
    retval = gp_widget_get_child_by_name(configWidget, "aperture", &apertureWidget);
    error_handler_exit(retval, "gp_widget_get_child_by_name");

    char c_ap[4];
    sprintf(c_ap, "%.2g", i_ap/10.0);
    retval = gp_widget_set_value(apertureWidget, c_ap);
    error_handler_exit(retval, "gp_widget_set_text");
    retval = gp_camera_set_config(camera, configWidget, context);
    error_handler_exit(retval, "gp_camera_set_config");

    gp_widget_free(configWidget);

}
int get_aperture(Camera *camera, struct InterestingApertures *int_ap, GPContext *context)
{
    CameraWidget *configWidget, *apertureWidget;
    int retval;

    retval = gp_camera_get_config(camera, &configWidget, context);
    error_handler_exit(retval, "gp_camera_get_config");
    retval = gp_widget_get_child_by_name(configWidget, "aperture", &apertureWidget);
    error_handler_exit(retval, "gp_widget_get_child_by_name");

    retval = 0;
    int i=0;
    float f_ap;
    int   i_ap;
    char const *c_ap=NULL;
    int_ap->last = 0;

    printf("possible apertures: ");
    while(!retval)
    {
        retval = gp_widget_get_choice(apertureWidget, i, &c_ap);
        if (!retval)
        {
            sscanf(c_ap, "%f", &f_ap);
            i_ap = (int) (f_ap*10);
            printf("%s  ", c_ap);
            if(i==0)  int_ap->first = i_ap;
            int_ap->second_last = int_ap->last;
            int_ap->last = i_ap;
        }
        i++;
    }
    printf("\n");

    retval = gp_widget_get_value(apertureWidget, &c_ap);
    sscanf(c_ap, "%f", &f_ap);
    i_ap = (int) (f_ap*10);
    int_ap->current = i_ap;

    gp_widget_free(configWidget);
    return 0;
}


/** just return max_aperture as int, -i if error
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



void set_shutterspeed(Camera *camera, char const *c_ss, GPContext *context)
{
    CameraWidget *configWidget, *shutterspeedWidget;
    int retval;

    retval = gp_camera_get_config(camera, &configWidget, context);
    error_handler_exit(retval, "gp_camera_get_config");
    retval = gp_widget_get_child_by_name(configWidget, "shutterspeed", &shutterspeedWidget);
    error_handler_exit(retval, "gp_widget_get_child_by_name");

    retval = gp_widget_set_value(shutterspeedWidget, c_ss);
    error_handler_exit(retval, "gp_widget_set_value");
    retval = gp_camera_set_config(camera, configWidget, context);
    error_handler_exit(retval, "gp_camera_set_config");

    gp_widget_free(configWidget);
}
void get_shutterspeed(Camera *camera, char c_ss_return[], GPContext *context)
{
    CameraWidget *configWidget, *shutterspeedWidget;
    int retval;

    char const *c_ss=NULL;

    retval = gp_camera_get_config(camera, &configWidget, context);
    error_handler_exit(retval, "gp_camera_get_config");
    retval = gp_widget_get_child_by_name(configWidget, "shutterspeed", &shutterspeedWidget);
    error_handler_exit(retval, "gp_widget_get_child_by_name");

    retval = gp_widget_get_value(shutterspeedWidget, &c_ss);
    error_handler_exit(retval, "gp_widget_get_value");

    //printf("shutterspeed: %s\n", c_ss);

    strcpy(c_ss_return, c_ss);
    c_ss_return[6] = '\0';
    gp_widget_free(configWidget);
}
void shutter(Camera *camera, GPContext *context)
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
void DOF(Camera *camera, GPContext *context)
{
    shutter(camera, context);      // TODO: emulate press(DOF) for time=shutterspeed
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

void print_whole_config_linear(Camera *camera, GPContext *context)
{
    CameraWidget *widget = NULL,
                 *child  = NULL;
    int retval;

    retval = gp_camera_get_config (camera, &widget, context);
    error_handler(retval, "gp_camera_get_config");
    if (retval) return;

    int id;
    gp_widget_get_child(widget, 0, &child);     // find first id
    gp_widget_get_id(child, &id);               // find first id

    while (retval==GP_OK)
    {
        retval = gp_widget_get_child_by_id(widget, id, &child);
        if (retval==GP_OK)
            _print_this_widget(child);
        id++;
    }

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
#ifndef verbose
    if (errcode==0)
        return;
#endif
    char *errstring=NULL;
    switch (errcode)
    {
        case GP_OK:                      errstring = "GP_OK"; break;
        case GP_ERROR_BAD_PARAMETERS:    errstring = "GP_ERROR_BAD_PARAMETERS"; break;
        default: errstring = "";
    }

    printf("%s", message);          // Note: no new line at the end is inserted
    printf(" ERRORCODE: %d, %s\n", errcode, errstring);
}

void error_handler_exit(int errcode, const char* message)
{
    error_handler(errcode, message);
    if (errcode)
        exit(errcode);
}

#include "ei_widget_configure.h"
#include "ei_implementation.h"
#include "ei_widget_all.h"
#include "ei_utils.h"
#include "ei_utils_util.h"
#include "ei_draw_util.h"

void	ei_button_configure(ei_widget_t		widget,
							 ei_size_t*		requested_size,
							 const ei_color_t*	color,
							 int*			border_width,
							 int*			corner_radius,
							 ei_relief_t*		relief,
							 ei_string_t*		text,
							 ei_font_t*		text_font,
							 ei_color_t*		text_color,
							 ei_anchor_t*		text_anchor,
							 ei_surface_t*		img,
							 ei_rect_ptr_t*		img_rect,
							 ei_anchor_t*		img_anchor,
							 ei_callback_t*		callback,
							 ei_user_param_t*	user_param) {
    ei_impl_button_t* widget_specific = (ei_impl_button_t*) widget;
    if (color != NULL) {
        widget_specific->widget.color = *color;
    }
    if (border_width != NULL) {
        widget_specific->widget.border_width = *border_width;
    }
    if (corner_radius != NULL) {
        widget_specific->corner_radius = *corner_radius;
        *(widget->min_size) = ei_size(2 * (*corner_radius), 2 * (*corner_radius));
    }
    if (relief != NULL) {
        widget_specific->relief = *relief;
    }
    if (text != NULL) {
        if(*text == NULL){
            if(widget_specific->text) free(widget_specific->text);
            widget_specific->text = NULL;
        }
        else {
            size_t text_length = strlen(*text);
            if (widget_specific->text) free(widget_specific->text);
            widget_specific->text = calloc(text_length + 1, sizeof(char));
            strcpy(widget_specific->text, *text);

            //recalculating the minimal size required for the button to fit the label, as well as the text rect size
            ei_surface_t text_surf = hw_text_create_surface(widget_specific->text, widget_specific->text_font,
                                                            widget_specific->text_color);
            ei_size_t natural = hw_surface_get_size(text_surf);
            hw_surface_free(text_surf);
            if (!widget_specific->text_rect_sz) widget_specific->text_rect_sz = malloc(sizeof(ei_size_t));
            *(widget_specific->text_rect_sz) = natural;
            natural.width += 2 * widget_specific->corner_radius;
            natural.height += 2 * widget_specific->corner_radius;
            if (!widget_specific->widget.min_size) widget_specific->widget.min_size = malloc(sizeof(ei_size_t));
            *(widget_specific->widget.min_size) = natural;
        }
    }
    if (text_font != NULL) {
        widget_specific->text_font = *text_font;
    }
    if (text_color != NULL) {
        widget_specific->text_color = *text_color;
    }
    if (text_anchor != NULL) {
        widget_specific->text_anchor = *text_anchor;
    }
    if (img != NULL) {
        widget_specific->img = hw_surface_create(*img, hw_surface_get_size(*img), true);
        hw_surface_lock(widget_specific->img);
        ei_copy_surface(widget_specific->img, NULL, *img, NULL, true);
        hw_surface_unlock(widget_specific->img);
    }
    if (img_rect != NULL) {
        widget_specific->img_rect = calloc(1, sizeof(ei_rect_t));
        ei_size_t img_size = hw_surface_get_size(widget_specific->img);
        ei_rect_t img_whole = ei_rect(ei_point_zero(), img_size);
        *(widget_specific->img_rect) = ei_get_rect_intersection(*img_rect, &img_whole);
        //recalculating the minimal size required for the button to fit the label
        ei_size_t  natural = (*img_rect)->size;
        natural.width += 2 * widget_specific->corner_radius;
        natural.height += 2 * widget_specific->corner_radius;
        *(widget_specific->widget.min_size) = natural;
    }
    else if(img){
        //recalculating the minimal size required for the button to fit the label
        if(!widget_specific->img_rect) widget_specific->img_rect = malloc(sizeof(ei_rect_t));
        ei_size_t  natural = hw_surface_get_size(*img);
        *(widget_specific->img_rect) = ei_rect(ei_point_zero(), natural);
        natural.width += 2 * widget_specific->corner_radius;
        natural.height += 2 * widget_specific->corner_radius;
        *(widget_specific->widget.min_size) = natural;
    }
    if (img_anchor != NULL) {
        widget_specific->img_anchor = *img_anchor;
    }
    if (callback != NULL) {
        widget_specific->callback = *callback;
    }
    if (user_param != NULL) {
        widget_specific->user_param = *user_param;
    }
    if (requested_size != NULL) {
        widget_specific->widget.requested_size = *requested_size;
        widget_specific->widget.screen_location.size = fit_to_label_size(*requested_size, *(widget_specific->widget.min_size));
    }
}

void	ei_frame_configure(ei_widget_t		widget,
							 ei_size_t*		requested_size,
							 const ei_color_t*	color,
							 int*			border_width,
							 ei_relief_t*		relief,
							 ei_string_t*		text,
							 ei_font_t*		text_font,
							 ei_color_t*		text_color,
							 ei_anchor_t*		text_anchor,
							 ei_surface_t*		img,
							 ei_rect_ptr_t*		img_rect,
							 ei_anchor_t*		img_anchor) {
    ei_impl_frame_t* widget_specific = (ei_impl_frame_t*) widget;
    if (color != NULL) {
        widget_specific->widget.color = *color;
    }
    if (border_width != NULL) {
        widget_specific->widget.border_width = *border_width;
    }
    if (relief != NULL) {
        widget_specific->relief = *relief;
    }

    if (text != NULL) {
        if(*text == NULL){
            if(widget_specific->text) free(widget_specific->text);
            widget_specific->text = NULL;
        }
        else{
            size_t text_length = strlen(*text);
            if(widget_specific->text) free(widget_specific->text);
            widget_specific->text = calloc(text_length+1, sizeof(char));
            strcpy(widget_specific->text,*text);

            //recalculating the minimal size required for the button to fit the label, as well as the text rect size
            ei_size_t  natural = hw_surface_get_size(hw_text_create_surface(widget_specific->text, widget_specific->text_font, widget_specific->text_color));
            if(!widget_specific->text_rect_sz) widget_specific->text_rect_sz = malloc(sizeof(ei_size_t));
            *(widget_specific->text_rect_sz) = natural;
            if(!widget_specific->widget.min_size) widget_specific->widget.min_size = malloc(sizeof(ei_size_t));
            *(widget_specific->widget.min_size) = natural;
        }
    }
    if (text_font != NULL) {
        widget_specific->text_font = *text_font;
    }
    if (text_color != NULL) {
        widget_specific->text_color = *text_color;
    }
    if (text_anchor != NULL) {
        widget_specific->text_anchor = *text_anchor;
    }
    if (img != NULL) {
        widget_specific->img = hw_surface_create(*img, hw_surface_get_size(*img), true);
        hw_surface_lock(widget_specific->img);
        ei_copy_surface(widget_specific->img, NULL, *img, NULL, true);
        hw_surface_unlock(widget_specific->img);
    }
    if (img_rect != NULL) {
        widget_specific->img_rect = calloc(1, sizeof(ei_rect_t));
        *(widget_specific->img_rect) = *(*img_rect);
        //recalculating the minimal size required for the button to fit the label
        ei_size_t  natural = (*img_rect)->size;
        *(widget_specific->widget.min_size) = natural;
    }
    else if(img){
        //recalculating the minimal size required for the button to fit the label
        if(!widget_specific->img_rect) widget_specific->img_rect = malloc(sizeof(ei_rect_t));
        ei_size_t  natural = hw_surface_get_size(*img);
        *(widget_specific->img_rect) = ei_rect(ei_point_zero(), natural);
        *(widget_specific->widget.min_size) = natural;
    }
    if (img_anchor != NULL) {
        widget_specific->img_anchor = *img_anchor;
    }

    if (requested_size != NULL) {
        widget_specific->widget.requested_size = *requested_size;
        widget_specific->widget.screen_location.size = fit_to_label_size(*requested_size, *(widget_specific->widget.min_size));
    }
}


static ei_size_t toplevel_default_min_size = (ei_size_t){160,120};
//static ei_size_t toplevel_default_requested_size = (ei_size_t){320, 240};

void	ei_toplevel_configure(ei_widget_t		widget,
							 ei_size_t*		requested_size,
							 ei_color_t*		color,
							 int*			border_width,
							 ei_string_t*		title,
							 bool*			closable,
							 ei_axis_set_t*		resizable,
						 	 ei_size_ptr_t*		min_size) {
    ei_impl_toplevel_t* widget_specific = (ei_impl_toplevel_t*) widget;
    if (requested_size != NULL) {
        requested_size->height += toplevel_bar_height;
        widget_specific->widget.requested_size = *requested_size;
        widget_specific->widget.screen_location.size = *requested_size;
    }
    if (color != NULL) {
        widget_specific->widget.color = *color;
    }
    if (border_width != NULL) {
        widget_specific->widget.border_width = *border_width;
    }
    if (title != NULL) {
        size_t text_length = strlen(*title);
        widget_specific->title = calloc(text_length+1, sizeof(char));
        strcpy(widget_specific->title,*title);
    }
    if (closable != NULL) {
        widget_specific->closable = *closable;
    }
    if(widget_specific->closable){
        //creating the close button
        ei_widget_t close_button = ei_widget_create("button", widget, NULL, NULL);
        ei_callback_t call_close = &close_toplevel;
        ei_button_configure(close_button,
                            &(ei_size_t){0,0},
                            &(ei_color_t){255,0,0,255},
                            &close_button_border_width,
                            &close_button_radius,
                            &(ei_relief_t){ei_relief_raised},
                            NULL,NULL,NULL,NULL,NULL,NULL,NULL,
                            &call_close ,NULL);
        ei_place_xy(close_button, close_button_pos_x, close_button_pos_y);
    }
    if (resizable != NULL) {
        widget_specific->resizable = *resizable;
    }
    if(widget_specific->resizable != ei_axis_none){
        //creating the resize button
        ei_widget_t resize_button = ei_widget_create("button", widget, NULL, NULL);
        ei_button_configure(resize_button,
                            &(ei_size_t){resize_button_size,resize_button_size},
                            &grey,
                            &(int){0},
                            &(int){0},
                            &(ei_relief_t){ei_relief_none},
                            NULL,NULL,NULL,NULL,NULL,NULL,NULL,
                            NULL,NULL);
        ei_place(resize_button,
                 &(ei_anchor_t){ei_anc_southeast},
                 &(int){0},&(int){0},
                 NULL,NULL,
                 &(float){1.},&(float){1.},
                 NULL,NULL);
    }
    if (min_size != NULL) {
        if (*min_size != NULL) {
            widget_specific->widget.min_size = *min_size;
        } else {
            widget_specific->widget.min_size = &toplevel_default_min_size;
        }
    }
}

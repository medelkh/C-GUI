#include "ei_widgetclass.h"
#include "ei_widget_all.h"
#include "ei_implementation.h"
#include "ei_utils.h"
#include "ei_draw_util.h"

ei_widget_t ei_frame_allocfunc(void) {
    ei_impl_frame_t* widget = calloc(1, sizeof(ei_impl_frame_t));
    return &(widget->widget);
}

void ei_frame_releasefunc(ei_widget_t widget) {
    ei_impl_frame_t* widget_full = (ei_impl_frame_t*) widget;
    if(widget_full->text) free(widget_full->text);
    if(widget_full->text_rect_sz) free(widget_full->text_rect_sz);
    if(widget_full->img_rect) free(widget_full->img_rect);
    if(widget_full->img) hw_surface_free(widget_full->img);
    ei_widget_release(widget);
    free(widget_full);
}

void ei_frame_setdefaultsfunc(ei_widget_t widget) {
    widget->requested_size = ei_size(0,0);
    widget->color = ei_default_background_color;
    widget->border_width = 0;
    ei_impl_frame_t* widget_spec = (ei_impl_frame_t*) widget;
    widget_spec->relief = ei_relief_none;
    widget_spec-> text = NULL;
    widget_spec-> text_font = ei_default_font;
    widget_spec->text_color = ei_font_default_color;
    widget_spec->text_anchor = ei_anc_center;
    widget_spec->img = NULL;
    widget_spec->img_rect = NULL;
    widget_spec->img_anchor = ei_anc_center;
    widget_spec->widget.min_size = malloc(sizeof (ei_size_t));
    *(widget_spec->widget.min_size) = ei_size_zero();
}


bool ei_frame_handlefunc(ei_widget_t widget, struct ei_event_t* event){
    return true; //c'est tout ce qu'il faut faire je crois, cf page 25
}


void ei_frame_geomnotifyfunc(ei_widget_t widget){
    
}

//attention: quand on dessine un widget avec drawfunc ça doit aussi dessiner les enfants du widget
void ei_frame_drawfunc(ei_widget_t widget,
                    ei_surface_t surface, ei_surface_t pick_surface,
                    ei_rect_t* clipper) {
    // Cast the widget to a toplevel widget
    ei_impl_frame_t* frame = (ei_impl_frame_t*)widget;

    //draw the frame in the display
    ei_draw_button(surface,
                   frame->widget.screen_location.top_left,
                   frame->widget.screen_location.size,
                   frame->widget.color,
                   frame->widget.border_width,
                   0,
                   frame->relief,
                   frame->text,
                   frame->text_font,
                   &(frame->text_color),
                   frame->text_anchor,
                   frame->text_rect_sz,
                   frame->img,
                   frame->img_rect,
                   frame->img_anchor,
                   clipper);
    
    //draw the frame in the pick surface
    ei_draw_button(pick_surface,
                   frame->widget.screen_location.top_left,
                   frame->widget.screen_location.size,
                   *(frame->widget.pick_color),
                   0,
                   0,
                   ei_relief_none,
                   NULL,
                   NULL,
                   NULL,
                   ei_anc_none,
                   NULL,
                   NULL,
                   NULL,
                   ei_anc_none,
                   clipper);
    
    ei_impl_widget_draw_children(widget, surface, pick_surface, clipper);
}


//ei_widgetclass_t frame;
//...
//frame.allocfunc = &ei_frame_allocfunc;
//...
//ei_widgetclass_register(&frame);
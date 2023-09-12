#ifndef EI_IMPL_APPLICATION_H
#define EI_IMPL_APPLICATION_H

#include "ei_application.h"
#include "ei_widget_all.h"


typedef struct ei_widget_vec{
    ei_widget_t* widgets;
    size_t size;
    size_t capacity;
} ei_widget_vec;


/**
 * @brief Initializes the global widget list
*/
void ei_init_widget_vec(void);

/**
 * @brief Inserts a widget to the back of the widget list
 * @param widget The widget to be inserted
 */
void ei_append_widget(ei_widget_t widget);

/**
 * @brief Get the i-th widget in the widget list
 * @param position position of the widget in the widget list
 * @return A pointer to the widget
 */
ei_widget_t ei_widget_at(size_t position);

/**
 * @brief Returns the size of the widget list
 */
size_t ei_widget_list_size(void);

/**
 * @brief Frees the space allocated for the widget list
 */
void ei_widget_vec_free(void);


/**
 * @return Returns the pick surface
 */
ei_surface_t ei_app_pick_surface(void);

#endif
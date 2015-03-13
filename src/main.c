/*
// PebbleOverflow
// Copyright (c) 2015, Adrian Shatte
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
*/

#include <pebble.h>
  
#define KEY_USERNAME 0
#define KEY_TOTALREP 1
#define KEY_REPGAINED 2
  
// reference to the main window
static Window *s_main_window;

// references to all of the text layer
static TextLayer *s_time_layer, *s_temperature_layer, *s_date_layer, *s_username_layer, *s_total_rep_layer, *s_rep_gained_layer;

// reference to the background
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static void update_time() {
    // Get a tm structure
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
  
    // Create a long-lived buffer
    static char buffer[] = "00:00";
  
    // Write the current hours and minutes into the buffer
    if (clock_is_24h_style() == true) {
        // Use 24 hour format
        strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
    } else {
        // Use 12 hour format
        strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
    }
  
    // Display this time on the TextLayer
    text_layer_set_text(s_time_layer, buffer);
}

static void update_date() {
    // Get a tm structure
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
  
    // Create a long-lived buffer
    static char buffer[] = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
  
    // Write the date into the buffer
    strftime(buffer, sizeof("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"), "%b %d", tick_time);
  
    // Display this time on the date TextLayer
    text_layer_set_text(s_date_layer, buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();
    update_date();
  
    // Get updated user info update every 10 minutes
    if (tick_time->tm_min % 10 == 0) {
        // Begin dictionary
        DictionaryIterator *iter;
        app_message_outbox_begin(&iter);
		
        // Add a key-value pair
        dict_write_uint8(iter, 0, 0);
		
        // Send the message
        app_message_outbox_send();
    }
}

static void main_window_load(Window *window) {
  
    // Set window background to black
    window_set_background_color(s_main_window, GColorBlack);
  
    // Create time TextLayer
    s_time_layer = text_layer_create(GRect(0, 15, 144, 50));
    text_layer_set_background_color(s_time_layer, GColorBlack);
    text_layer_set_text_color(s_time_layer, GColorWhite);
    text_layer_set_text(s_time_layer, "00:00");
    text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  
    // Create date TextLayer
    s_date_layer = text_layer_create(GRect(0, 60, 122, 50));
    text_layer_set_background_color(s_date_layer, GColorBlack);
    text_layer_set_text_color(s_date_layer, GColorWhite);
    text_layer_set_text(s_date_layer, "");
    text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
    text_layer_set_text_alignment(s_date_layer, GTextAlignmentRight);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
  
    // Create username layer
    s_username_layer = text_layer_create(GRect(44, 86, 80, 32));
    text_layer_set_background_color(s_username_layer, GColorBlack);
    text_layer_set_text_color(s_username_layer, GColorWhite);
    text_layer_set_font(s_username_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
    text_layer_set_text_alignment(s_username_layer, GTextAlignmentRight);
    text_layer_set_text(s_username_layer, "");
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_username_layer));
  
    // Create total_rep layer
    s_total_rep_layer = text_layer_create(GRect(0, 110, 125, 50));
    text_layer_set_background_color(s_total_rep_layer, GColorBlack);
    text_layer_set_text_color(s_total_rep_layer, GColorWhite);
    text_layer_set_font(s_total_rep_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
    text_layer_set_text_alignment(s_total_rep_layer, GTextAlignmentRight);
    text_layer_set_text(s_total_rep_layer, "");
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_total_rep_layer));
  
    // Create rep_gained layer
    s_rep_gained_layer = text_layer_create(GRect(0, 138, 126, 50));
    text_layer_set_background_color(s_rep_gained_layer, GColorBlack);
    text_layer_set_text_color(s_rep_gained_layer, GColorWhite);
    text_layer_set_font(s_rep_gained_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
    text_layer_set_text_alignment(s_rep_gained_layer, GTextAlignmentRight);
    text_layer_set_text(s_rep_gained_layer, "Add userID in settings");
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_rep_gained_layer));
  
    // Create GBitmap, then set to created BitmapLayer
    s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMG_SOII);
    s_background_layer = bitmap_layer_create(GRect(0, 48, 54, 128));
    bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
}

static void main_window_unload(Window *window) {
    // Destroy TextLayers
    text_layer_destroy(s_time_layer);
    text_layer_destroy(s_temperature_layer);
    text_layer_destroy(s_date_layer);
    text_layer_destroy(s_username_layer);
    text_layer_destroy(s_total_rep_layer);
    text_layer_destroy(s_rep_gained_layer);
  
    // Destroy GBitmap
    gbitmap_destroy(s_background_bitmap);
  
    // Destroy BitmapLayer
    bitmap_layer_destroy(s_background_layer);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  
    // Store incoming information
    static char username_buffer[32];
    static char totalrep_buffer[8];
    static char repgained_buffer[32];
  
    // Read first item
    Tuple *t = dict_read_first(iterator);
  
    // For all items
    while(t != NULL) {
        switch(t->key) {
            case KEY_USERNAME:
                snprintf(username_buffer, sizeof(username_buffer), "%s", t->value->cstring);
                break;
            case KEY_TOTALREP:
                snprintf(totalrep_buffer, sizeof(totalrep_buffer), "%s", t->value->cstring);
                break;
            case KEY_REPGAINED:
                snprintf(repgained_buffer, sizeof(repgained_buffer), "%s", t->value->cstring);
                break;
            default:
                APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
                break;
        }
		
        // proceed to next item
        t = dict_read_next(iterator);
    }
  
    // Display strings in TextLayers
    text_layer_set_text(s_username_layer, username_buffer);
    text_layer_set_text(s_total_rep_layer, totalrep_buffer);
    text_layer_set_text(s_rep_gained_layer, repgained_buffer);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void init() {
    // Create main Window element and assign to pointer
    s_main_window = window_create();
  
    // Set handlers to manage the elements inside the Window
    window_set_window_handlers(s_main_window, (WindowHandlers) {
      .load = main_window_load,
      .unload = main_window_unload
    });
	
    // Show the Window on the watch, with animated=true
    window_stack_push(s_main_window, true);
  
    // Register with TickTimerService
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
    // Display the current time
    update_time();
  
    // Display the current date
    update_date();
  
    // Register callbacks
    app_message_register_inbox_received(inbox_received_callback);
    app_message_register_inbox_dropped(inbox_dropped_callback);
    app_message_register_outbox_failed(outbox_failed_callback);
    app_message_register_outbox_sent(outbox_sent_callback);
  
    // Open AppMessage
    app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

static void deinit() {
    // Destroy Window
    window_destroy(s_main_window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}
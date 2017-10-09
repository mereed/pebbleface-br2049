/*

Copyright (C) 2017 Mark Reed

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

-------------------------------------------------------------------

*/

#include <pebble.h>
#include "main.h"


//WINDOW
Window* window;

TextLayer *layer_time_text;
TextLayer *layer_date_text1;
TextLayer *layer_date_text2;

static GFont mini;
static GFont mini2;

static GBitmap *bluetooth_image;
static BitmapLayer *bluetooth_layer;


// A struct for our specific settings (see main.h)
ClaySettings settings;


int stringToInt(char *str);


// Initialize the default settings
static void prv_default_settings() {	

  settings.digtime = 0;

}


void tick(struct tm *tick_time, TimeUnits units_changed) {		
	
	static char time_buffer[] = "0000";
    static char date_text[] = "xx.xx.xxxx";
    static char date_text2[] = "xx.xx.xxxx";
	
	//get digital time
    if (clock_is_24h_style()) {
		   strftime(time_buffer, 8, "%H%M", tick_time);
     } else {
		   strftime(time_buffer, 8, "%I%M", tick_time);
    }	 
  text_layer_set_text(layer_time_text, time_buffer);	
	
//date - US
	  strftime(date_text, sizeof(date_text), "%m.%d.%Y", tick_time);
	  text_layer_set_text(layer_date_text1, date_text);

//date - INT	
	  strftime(date_text2, sizeof(date_text2), "%d.%m.%Y", tick_time);
	  text_layer_set_text(layer_date_text2, date_text2);
	
  }


// Read settings from persistent storage
static void prv_load_settings() {
  // Load the default settings
  prv_default_settings();
  // Read settings from persistent storage, if they exist
  persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
}

// Save the settings to persistent storage
static void prv_save_settings() {
  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
  // Update the display based on new settings
  prv_update_display();
}

// Update the display elements
static void prv_update_display() {

	switch(settings.digtime) {
		
		case 1: // US date
		layer_set_hidden(text_layer_get_layer(layer_date_text2), true); 
		layer_set_hidden(text_layer_get_layer(layer_date_text1), false); 		
		break;		
		
		case 2: // INT date	
		layer_set_hidden(text_layer_get_layer(layer_date_text2), false); 
		layer_set_hidden(text_layer_get_layer(layer_date_text1), true); 
	    break;		
	}

}

int stringToInt(char *str){
    int i=0,sum=0;
    while(str[i]!='\0'){
         if(str[i]< 48 || str[i] > 57){
            // if (DEBUG) APP_LOG(APP_LOG_LEVEL_ERROR, "Unable to convert it into integer.");
          //   return 0;
         }
         else{
             sum = sum*10 + (str[i] - 48);
             i++;
         }
    }
    return sum;
}

// Handle the response from AppMessage
static void prv_inbox_received_handler(DictionaryIterator *iter, void *context) {

// date format
  Tuple *dt_t = dict_find(iter, MESSAGE_KEY_digtime);
  if (dt_t) {
    settings.digtime = stringToInt((char*) dt_t->value->data);
  }

	// Save the new settings to persistent storage
  prv_save_settings();
}


static void toggle_bluetooth_icon(bool connected) {

  layer_set_hidden(bitmap_layer_get_layer(bluetooth_layer), !connected);	  
}

void bluetooth_connection_callback(bool connected) {
  toggle_bluetooth_icon(connected);
}

void init() {
	
  prv_load_settings();

// Listen for AppMessages
  app_message_register_inbox_received(prv_inbox_received_handler);
  app_message_open(256, 256);

// international support
  setlocale(LC_ALL, "");
	
//create window
  window = window_create();
  window_set_background_color(window,GColorBlack);
  window_stack_push(window, true);
  Layer* window_layer = window_get_root_layer(window);	

  bluetooth_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
  GRect bitmap_bounds_bt_on = gbitmap_get_bounds(bluetooth_image);
#if PBL_PLATFORM_CHALK	
  GRect frame_bt = GRect(0, 0, bitmap_bounds_bt_on.size.w, bitmap_bounds_bt_on.size.h);
#else
  GRect frame_bt = GRect(0, 0, bitmap_bounds_bt_on.size.w, bitmap_bounds_bt_on.size.h);
#endif
  bluetooth_layer = bitmap_layer_create(frame_bt);
  bitmap_layer_set_bitmap(bluetooth_layer, bluetooth_image);
  layer_add_child(window_layer, bitmap_layer_get_layer(bluetooth_layer));	
		

  mini = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_MINI_20));
  mini2 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_MINI_30));
	

#if PBL_PLATFORM_CHALK	
  layer_time_text = text_layer_create(GRect(0, 55, 180, 40));
  text_layer_set_font(layer_time_text,mini2);
#else
  layer_time_text = text_layer_create(GRect(0, 12, 144, 40));
  text_layer_set_font(layer_time_text,mini2);
#endif 
  text_layer_set_background_color(layer_time_text, GColorClear);
  text_layer_set_text_color(layer_time_text, GColorWhite);
  text_layer_set_text_alignment(layer_time_text, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(layer_time_text));
	
#if PBL_PLATFORM_CHALK	
  layer_date_text1 = text_layer_create(GRect(0, 130, 180, 30));
#else
  layer_date_text1 = text_layer_create(GRect(0, 145, 144, 30));
#endif  
  text_layer_set_background_color(layer_date_text1, GColorClear);
  text_layer_set_text_color(layer_date_text1, GColorWhite);
  text_layer_set_font(layer_date_text1,mini);
  text_layer_set_text_alignment(layer_date_text1, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(layer_date_text1));
	
#if PBL_PLATFORM_CHALK	
  layer_date_text2 = text_layer_create(GRect(0, 130, 180, 30));
#else
  layer_date_text2 = text_layer_create(GRect(0, 145, 144, 30));
#endif 
  text_layer_set_background_color(layer_date_text2, GColorClear);
  text_layer_set_text_color(layer_date_text2, GColorWhite);
  text_layer_set_font(layer_date_text2,mini);
  text_layer_set_text_alignment(layer_date_text2, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(layer_date_text2));
	

  prv_update_display();

  toggle_bluetooth_icon(bluetooth_connection_service_peek());
	
	
// Avoids a blank screen on watch start.
  time_t now = time(NULL);
  struct tm *tick_time = localtime(&now);  
  tick(tick_time, DAY_UNIT + HOUR_UNIT + MINUTE_UNIT);
	
	//subscribe to events
  tick_timer_service_subscribe(MINUTE_UNIT, tick);
  bluetooth_connection_service_subscribe(bluetooth_connection_callback);

}

static void deinit(void) {

  tick_timer_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();

  text_layer_destroy(layer_time_text);
  text_layer_destroy(layer_date_text1);
  text_layer_destroy(layer_date_text2);
	
  fonts_unload_custom_font(mini);
  fonts_unload_custom_font(mini2);
	
  layer_remove_from_parent(bitmap_layer_get_layer(bluetooth_layer));
  bitmap_layer_destroy(bluetooth_layer);
  gbitmap_destroy(bluetooth_image);
  bluetooth_image = NULL;

	window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
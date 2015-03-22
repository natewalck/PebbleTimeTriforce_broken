/*
 * The original source image is from:
 *   <http://openclipart.org/detail/26728/aiga-litter-disposal-by-anonymous>
 *
 * The source image was converted from an SVG into a RGB bitmap using
 * Inkscape. It has no transparency and uses only black and white as
 * colors.
 */

#include "pebble.h"

static Window *s_main_window;
static Layer *s_battery_layer;
static Layer *s_time_layer;
static GBitmap *s_battery_image;

static GFont s_time_font;

static void layer_update_callback(Layer *layer, GContext* ctx) {
  // We make sure the dimensions of the GRect to draw into
  // are equal to the size of the bitmap--otherwise the image
  // will automatically tile. Which might be what *you* want.

#ifdef PBL_PLATFORM_BASALT
  GSize image_size = gbitmap_get_bounds(s_battery_image).size;
#else 
  GSize image_size = s_battery_image->bounds.size;
#endif

  graphics_draw_bitmap_in_rect(ctx, s_battery_image, GRect(5, 5, image_size.w, image_size.h));
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(s_main_window);
  GRect bounds = layer_get_frame(window_layer);

  s_battery_layer = layer_create(bounds);
  layer_set_update_proc(s_battery_layer, layer_update_callback);
  layer_add_child(window_layer, s_battery_layer);

  s_battery_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERYTEST);

  s_time_layer = layer_create(bounds);
  text_layer_set_background_color(s_time_layer, GColorBlack);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text(s_time_layer, "00:00");
  // Create GFont for Time Layer
  s_time_font = fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD);
    //Apply to TextLayer
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  layer_add_child(window_layer, s_time_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();

  // // Get weather update every 30 minutes
  // if(tick_time->tm_min % 30 == 0) {
  //   // Begin dictionary
  //   DictionaryIterator *iter;
  //   app_message_outbox_begin(&iter);

  //   // Add a key-value pair
  //   dict_write_uint8(iter, 0, 0);

  //   // Send the message!
  //   app_message_outbox_send();
  // }
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    //Use 2h hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    //Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);

  // // Create buffer and write date into buffer
  // static char date_text[] = "Xxxxxxxxx 00";
  // strftime(date_text, sizeof(date_text), "%B %e", tick_time);
  // // Set date text
  // text_layer_set_text(s_date_layer, date_text);
}


static void main_window_unload(Window *window) {
  gbitmap_destroy(s_battery_image);
  layer_destroy(s_battery_layer);
  text_layer_destroy(s_time_layer);
}


static void init() {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);

  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}

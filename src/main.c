#include <pebble.h>
 
static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_time_layer_back;
static TextLayer *s_second_layer;
static TextLayer *s_second_layer_back;
static GFont s_time_font;
static Layer *s_canvas;

static GPath *triangles[8];

static const int colors[8] = {
  0xAAFFFF, 
  0x00FFFF, 
  0xAAAAFF, 
  0x5555AA, 
  0xAAFF55, 
  0x00AA55, 
  0xFF55FF, 
  0xAA00FF
};

int selected;
int selected_color;
int selected_background;

static const GPathInfo TRIAGLE_1_PATH_INFO = {
  .num_points = 3,
  .points = (GPoint []) {{72, 0}, {144, 0}, {72, 84}}
};
static const GPathInfo TRIAGLE_2_PATH_INFO = {
  .num_points = 3,
  .points = (GPoint []) {{144, 0}, {144, 84}, {72, 84}}
};
static const GPathInfo TRIAGLE_3_PATH_INFO = {
  .num_points = 3,
  .points = (GPoint []) {{144, 84}, {144, 168}, {72, 84}}
};
static const GPathInfo TRIAGLE_4_PATH_INFO = {
  .num_points = 3,
  .points = (GPoint []) {{72, 168}, {144, 168}, {72, 84}}
};
static const GPathInfo TRIAGLE_5_PATH_INFO = {
  .num_points = 3,
  .points = (GPoint []) {{72, 168}, {0, 168}, {72, 84}}
};
static const GPathInfo TRIAGLE_6_PATH_INFO = {
  .num_points = 3,
  .points = (GPoint []) {{0, 168}, {0, 84}, {72, 84}}
};
static const GPathInfo TRIAGLE_7_PATH_INFO = {
  .num_points = 3,
  .points = (GPoint []) {{0, 0}, {0, 84}, {72, 84}}
};
static const GPathInfo TRIAGLE_8_PATH_INFO = {
  .num_points = 3,
  .points = (GPoint []) {{0, 0}, {72, 0}, {72, 84}}
};

static void draw_triangle(GPath *triangle, GContext *ctx, GColor8 color) {
  graphics_context_set_fill_color(ctx, color);
  gpath_draw_filled(ctx, triangle);
  graphics_context_set_stroke_color(ctx, color);
  gpath_draw_outline(ctx, triangle);
}

static void update_canvas(Layer *this_layer, GContext *ctx) {
  graphics_context_set_antialiased(ctx, false);
  graphics_context_set_stroke_width(ctx, 8);
  
  draw_triangle(triangles[selected], ctx, GColorFromHEX(colors[selected_color]));
}

static void update_time(int force) {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char sbuffer[] = "00";

  strftime(sbuffer, sizeof("00"), "%S", tick_time);
    
  // Display this time on the TextLayer
  text_layer_set_text(s_second_layer, sbuffer);
  text_layer_set_text(s_second_layer_back, sbuffer);
  
  int result = atoi(sbuffer);
  
  double d_res = (result) / 7.5;
  selected = d_res;
  
  if (result <= 0 || force) {
    static char buffer[] = "00:00";
    // Write the current hours and minutes into the buffer
    if(clock_is_24h_style() == true) {
      // Use 24 hour format
      strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
    } else {
      // Use 12 hour format
      strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
    }
    
    text_layer_set_text(s_time_layer, buffer);
    text_layer_set_text(s_time_layer_back, buffer);

    selected_background = (selected_background + 2) % 8;
    selected_color = selected_background + 1;
    window_set_background_color(s_main_window, GColorFromHEX(colors[selected_background]));
  }

  if (result % 8 == 0) {
    layer_mark_dirty(s_canvas);
  }
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time(0);
}

static void main_window_load(Window *window) {
  selected = 0;
  selected_color = 1;
  selected_background = 0;
  
  window_set_background_color(s_main_window, GColorFromHEX(colors[selected_background]));

  s_canvas = layer_create(GRect(0, 0, 144, 168));
  
  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(0, 30, 144, 50));
  s_time_layer_back = text_layer_create(GRect(1, 31, 143, 51));
  s_second_layer = text_layer_create(GRect(6, 85, 139, 50));
  s_second_layer_back = text_layer_create(GRect(7, 86, 138, 49));
  
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text_color(s_time_layer_back, GColorWhite);
  text_layer_set_text_color(s_second_layer, GColorBlack);
  text_layer_set_text_color(s_second_layer_back, GColorWhite);
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_background_color(s_time_layer_back, GColorClear);
  text_layer_set_background_color(s_second_layer, GColorClear);
  text_layer_set_background_color(s_second_layer_back, GColorClear);
  
  // Create GFont
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTICON_42));
  
  // Apply to TextLayer
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_font(s_time_layer_back, s_time_font);
  text_layer_set_font(s_second_layer, s_time_font);
  text_layer_set_font(s_second_layer_back, s_time_font);
  
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_time_layer_back, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_second_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_second_layer_back, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), s_canvas);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer_back));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_second_layer_back));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_second_layer));
}

static void main_window_unload(Window *window) {
  int i;
  
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_time_layer_back);
  text_layer_destroy(s_second_layer);
  text_layer_destroy(s_second_layer_back);
  
  for (i = 0; i < 8; i++) {
    gpath_destroy(triangles[i]);
  }
  
  // Unload GFont
  fonts_unload_custom_font(s_time_font);
}

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  window_set_fullscreen(s_main_window, 1);
  
  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  update_time(1);
  
  triangles[0] = gpath_create(&TRIAGLE_1_PATH_INFO);
  triangles[1] = gpath_create(&TRIAGLE_2_PATH_INFO);
  triangles[2] = gpath_create(&TRIAGLE_3_PATH_INFO);
  triangles[3] = gpath_create(&TRIAGLE_4_PATH_INFO);
  triangles[4] = gpath_create(&TRIAGLE_5_PATH_INFO);
  triangles[5] = gpath_create(&TRIAGLE_6_PATH_INFO);
  triangles[6] = gpath_create(&TRIAGLE_7_PATH_INFO);
  triangles[7] = gpath_create(&TRIAGLE_8_PATH_INFO);
  
  layer_set_update_proc(s_canvas, update_canvas);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
}

static void deinit() {
  tick_timer_service_unsubscribe();
  
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
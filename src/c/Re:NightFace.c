#include <pebble.h>

/// Re:NightFace
/// 
/// My take on a battery-saving watchface

static Window *s_main_window;
static TextLayer *s_time_layer;

// It's not called update_time for a reason ;)
static void get_time() {
	// Get a tm structure
	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);

	// Write the current hours and minutes into a buffer
	// Yes, I am new and this is straight from the tutorial
	static char s_buffer[8];
	strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style()?
			"%H:%M" : "%I:%M", tick_time);
	// Display the text on the TextLayer
	text_layer_set_text(s_time_layer, s_buffer);
}

// Here's the battery saving magic: we hide the text after a dozen seconds
// I learned this from u/Northeastpaw on the r/PebbleDevelopers subreddit
static void app_timer_callback(void *date) {
	layer_set_hidden(text_layer_get_layer(s_time_layer), true);
}

static void main_window_load(Window *window) {
	// Get information about the Window
	Layer *window_layer = window_get_root_layer(window);
	// Extra fancy Quick View aware bounds
	GRect unobstructed_bounds = layer_get_unobstructed_bounds(window_layer);
	s_time_layer = text_layer_create(GRect(0, unobstructed_bounds.size.h / 4, unobstructed_bounds.size.w, 50));

	// Improve the layout to be more like a watchface
	text_layer_set_background_color(s_time_layer, GColorClear);
	// The original NighWatch used light gray instead of pure white, maybe I'll make it configurable
	text_layer_set_text_color(s_time_layer, GColorWhite);
	// We'll be using the LECO font
	text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS));
	// Let's get our font nicely centered
	text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

	// Add the text layer as a child layer to the Window's root layer
	layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
}

static void main_window_unload(Window *window) {
	// Destroy TextLayer
	text_layer_destroy(s_time_layer);
}

static void init() {
	// Create main Window element and assign to pointer
	s_main_window = window_create();

	// Set the window background color
	window_set_background_color(s_main_window, GColorBlack);

	// Set handlers to manage the elements inside the Window
	window_set_window_handlers(s_main_window, (WindowHandlers) {
			.load = main_window_load,
			.unload = main_window_unload
		});
	
	// Show the Window on the watch, with anumated=true
	window_stack_push(s_main_window, true);

	// Set the time
	get_time();

	// app_timer is responsible for the time showing timeout
	app_timer_register(10000, app_timer_callback, NULL);
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


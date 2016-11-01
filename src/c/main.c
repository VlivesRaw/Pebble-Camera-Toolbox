#include <pebble.h>
//TODO :
// Support pebble OG
// Support Round
// Persistent
// Companion app / window for settings
// Design
// Logos
static Window *s_main_window;

static StatusBarLayer *s_status_bar;;

static TextLayer *s_focal_layer;
static TextLayer *s_aperture_layer;
static TextLayer *s_hyperfocal_layer;
static TextLayer *s_coc_layer;

static TextLayer *s_yourFocal_layer;
static TextLayer *s_yourAperture_layer;
static TextLayer *s_yourHyperfocal_layer;
static TextLayer *s_yourCoc_layer;

static char s_focal_text[10];
static char s_aperture_text[10];
static char s_hyperfocal_text[10];
static char s_coc_text[8];

const char yourFocal_text[6] = "Focal\0";
const char yourAperture_text[3] = "f/\0";
const char yourCoc_text[20] = "Circle of Confusion\0";
const char yourHyperfocal_text[11] = "Hyperfocal\0";

static int s_focal = 30;
static int s_aperture = 80;
static int s_coc = 19;
static float s_hyperfocal;

static bool cocEditable = 1;
static bool focalEditable = 0;
static bool apertureEditable = 0;

char* floatToString(char* buffer, int bufferSize, double number)
	{
		char decimalBuffer[5];

		snprintf(buffer, bufferSize, "%d", (int)number);
		if ((int)number < 10) {
		strcat(buffer, ".");
		snprintf(decimalBuffer, 5, "%02d", (int)((float)(number - (int)number) * (float)100));
		strcat(buffer, decimalBuffer);
		
		}
		strcat(buffer, "m");
		return buffer;
	}

char* cocToString(char* buffer, int bufferSize, int number) 
{
	char decimalBuffer[5];
	snprintf(buffer, bufferSize, "%d", number/1000);	
	strcat(buffer,".");
	snprintf(decimalBuffer, 5, "%03d", number);
	strcat(buffer,decimalBuffer);

	return buffer;
}

char* focalToString(char* buffer, int bufferSize, int number) 
{
	snprintf(buffer, bufferSize, "%d", number);	
	return buffer;
}

char* apertureToString(char* buffer, int bufferSize, int number) 
{
	char decimalBuffer[3];
	snprintf(buffer, bufferSize, "%d", number/10);	
	if (number < 100) {
			strcat(buffer, ".");
			snprintf(decimalBuffer, 3, "%d", number % 10);
			strcat(buffer, decimalBuffer);
		}
	return buffer;
}

static void update() {
	if (cocEditable == 1) {
		text_layer_set_background_color(s_coc_layer, GColorElectricBlue);
		text_layer_set_background_color(s_yourCoc_layer, GColorElectricBlue);
		text_layer_set_background_color(s_focal_layer, GColorClear);
		text_layer_set_background_color(s_yourFocal_layer, GColorClear);
		text_layer_set_background_color(s_aperture_layer, GColorClear);
		text_layer_set_background_color(s_yourAperture_layer, GColorClear);
	}
	else if (focalEditable == 1) {
		text_layer_set_background_color(s_coc_layer, GColorClear);
		text_layer_set_background_color(s_yourCoc_layer, GColorClear);
		text_layer_set_background_color(s_focal_layer, GColorElectricBlue);
		text_layer_set_background_color(s_yourFocal_layer, GColorElectricBlue);
		text_layer_set_background_color(s_aperture_layer, GColorClear);
		text_layer_set_background_color(s_yourAperture_layer, GColorClear);
	}
	else if (apertureEditable == 1) {
		text_layer_set_background_color(s_coc_layer, GColorClear);
		text_layer_set_background_color(s_yourCoc_layer, GColorClear);
		text_layer_set_background_color(s_focal_layer, GColorClear);
		text_layer_set_background_color(s_yourFocal_layer, GColorClear);
		text_layer_set_background_color(s_aperture_layer, GColorElectricBlue);
		text_layer_set_background_color(s_yourAperture_layer, GColorElectricBlue);
	}
	
	s_hyperfocal = ((s_focal*s_focal)/(((float)s_aperture/10)*((float)s_coc/1000))+s_focal)/1000;	
	
  text_layer_set_text(s_hyperfocal_layer, floatToString(s_hyperfocal_text,sizeof(s_hyperfocal_text),s_hyperfocal));
	text_layer_set_text(s_coc_layer, cocToString(s_coc_text,sizeof(s_coc_text),s_coc));
	text_layer_set_text(s_aperture_layer, apertureToString(s_aperture_text, sizeof (s_aperture_text), s_aperture));
	text_layer_set_text(s_focal_layer, focalToString(s_focal_text, sizeof(s_focal_text), s_focal));
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context){
	if (cocEditable == 1) {
		cocEditable = 0;
		focalEditable = 1;
		apertureEditable = 0;
	}
	else	{
		if (focalEditable == 1) {
			cocEditable = 0;
			focalEditable = 0;
			apertureEditable = 1;
		}
		else {
			if (apertureEditable == 1) {
				cocEditable = 1;
				focalEditable = 0;
				apertureEditable = 0;
				
			}
		}
	}
	
	update();
}
static void up_click_handler(ClickRecognizerRef recognizer, void *context){
	if (cocEditable == 1 && s_coc < 1000) {
		s_coc = s_coc + 1;
	}
	if (focalEditable == 1 && s_focal < 1000) {
		if (s_focal >= 300) {
			s_focal = s_focal + 10;
		}
		else if (s_focal >= 100){
			s_focal = s_focal + 5;
		}
		else {
			s_focal = s_focal + 1;
		}
	}
	if (apertureEditable == 1  && s_aperture < 640) {
		if (s_aperture >= 100) {
			s_aperture = s_aperture + 10;
		}
		else {
			s_aperture = s_aperture + 1;
		}
	}
	update();
}
static void down_click_handler(ClickRecognizerRef recognizer, void *context){
	if (cocEditable == 1 && s_coc > 1) {
		s_coc = s_coc - 1;
	}
	if (focalEditable == 1 && s_focal > 1) {
		if (s_focal >= 310) {
			s_focal = s_focal - 10;
		}
		else if (s_focal >= 105){
			s_focal = s_focal - 5;
		}
		else {
			s_focal = s_focal - 1;
		}
	}
	if (apertureEditable == 1  && s_aperture > 10) {
		if (s_aperture >= 110) {
			s_aperture = s_aperture - 10;
		}
		else {
			s_aperture = s_aperture - 1;
		}
	}
	update();
}
static void click_config_provider(void *context) {
  // Subcribe to button click events here
	ButtonId b_select = BUTTON_ID_SELECT;
	ButtonId b_up = BUTTON_ID_UP;
	ButtonId b_down = BUTTON_ID_DOWN;
	
	uint16_t repeat_interval_ms = 100;
	
	window_single_click_subscribe(b_select, select_click_handler);
	window_single_click_subscribe(b_up, up_click_handler);
	window_single_click_subscribe(b_down, down_click_handler);
	window_single_repeating_click_subscribe(b_up, repeat_interval_ms, up_click_handler);
	window_single_repeating_click_subscribe(b_down, repeat_interval_ms, down_click_handler);
}

static void main_window_load(Window *window) {
	// Get information about the Window
	Layer *window_layer = window_get_root_layer(window);
	
	s_status_bar = status_bar_layer_create();
	
	
	// Create TextLayer with specific bounds
	s_yourCoc_layer = text_layer_create (GRect(0, 10, 144, 24));
	s_coc_layer = text_layer_create (GRect(0, 30, 144, 28));
	
	s_yourFocal_layer = text_layer_create (GRect(0, 60, 72, 30));
	s_focal_layer = text_layer_create (GRect(0, 80, 72, 30));
	
	s_yourAperture_layer = text_layer_create (GRect(72, 60, 72, 30));
	s_aperture_layer = text_layer_create (GRect(72, 80, 72, 30));
	
	s_yourHyperfocal_layer = text_layer_create (GRect(0, 110, 144, 38));
	s_hyperfocal_layer = text_layer_create (GRect(0, 133, 144, 28));
		
	text_layer_set_background_color(s_coc_layer, GColorClear);
	text_layer_set_text_color(s_coc_layer, GColorBlack);
	text_layer_set_text(s_coc_layer, cocToString(s_coc_text, sizeof (s_coc_text), s_coc));
	text_layer_set_font(s_coc_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(s_coc_layer, GTextAlignmentCenter);
	
	text_layer_set_background_color(s_yourCoc_layer, GColorClear);
	text_layer_set_text_color(s_yourCoc_layer, GColorBlack);
	text_layer_set_text(s_yourCoc_layer, yourCoc_text);
	text_layer_set_font(s_yourCoc_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_text_alignment(s_yourCoc_layer, GTextAlignmentCenter);
	
	text_layer_set_background_color(s_focal_layer, GColorClear);
	text_layer_set_text_color(s_focal_layer, GColorBlack);
	text_layer_set_text(s_focal_layer, focalToString(s_focal_text, sizeof(s_focal_text), s_focal));
	text_layer_set_font(s_focal_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(s_focal_layer, GTextAlignmentCenter);
	
	text_layer_set_background_color(s_yourFocal_layer, GColorClear);
	text_layer_set_text_color(s_yourFocal_layer, GColorBlack);
	text_layer_set_text(s_yourFocal_layer, yourFocal_text);
	text_layer_set_font(s_yourFocal_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_text_alignment(s_yourFocal_layer, GTextAlignmentCenter);
	
	text_layer_set_background_color(s_aperture_layer, GColorClear);
	text_layer_set_text_color(s_aperture_layer, GColorBlack);
	text_layer_set_text(s_aperture_layer, apertureToString(s_aperture_text, sizeof(s_aperture_text), s_aperture));
	text_layer_set_font(s_aperture_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(s_aperture_layer, GTextAlignmentCenter);
	
	text_layer_set_background_color(s_yourAperture_layer, GColorClear);
	text_layer_set_text_color(s_yourAperture_layer, GColorBlack);
	text_layer_set_text(s_yourAperture_layer, yourAperture_text);
	text_layer_set_font(s_yourAperture_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_text_alignment(s_yourAperture_layer, GTextAlignmentCenter);
	
	text_layer_set_background_color(s_hyperfocal_layer, GColorClear);
	text_layer_set_text_color(s_hyperfocal_layer, GColorBlack);
	text_layer_set_text(s_hyperfocal_layer, s_hyperfocal_text);
	text_layer_set_font(s_hyperfocal_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(s_hyperfocal_layer, GTextAlignmentCenter);
	
	text_layer_set_background_color(s_yourHyperfocal_layer, GColorClear);
	text_layer_set_text_color(s_yourHyperfocal_layer, GColorBlack);
	text_layer_set_text(s_yourHyperfocal_layer, yourHyperfocal_text);
	text_layer_set_font(s_yourHyperfocal_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_text_alignment(s_yourHyperfocal_layer, GTextAlignmentCenter);
	
	//add it as a child layer to the window's root layer
	
	layer_add_child(window_layer, text_layer_get_layer(s_coc_layer));
	layer_add_child(window_layer, text_layer_get_layer(s_yourCoc_layer));
	layer_add_child(window_layer, text_layer_get_layer(s_focal_layer));
	layer_add_child(window_layer, text_layer_get_layer(s_yourFocal_layer));
	layer_add_child(window_layer, text_layer_get_layer(s_aperture_layer));
	layer_add_child(window_layer, text_layer_get_layer(s_yourAperture_layer));
	layer_add_child(window_layer, text_layer_get_layer(s_hyperfocal_layer));
	layer_add_child(window_layer, text_layer_get_layer(s_yourHyperfocal_layer));
	
	layer_add_child(window_layer, status_bar_layer_get_layer(s_status_bar));
}

static void main_window_unload(Window *window) {
	status_bar_layer_destroy(s_status_bar);
	text_layer_destroy(s_focal_layer);
	text_layer_destroy(s_aperture_layer);
	text_layer_destroy(s_hyperfocal_layer);
	text_layer_destroy(s_coc_layer);
	text_layer_destroy(s_yourCoc_layer);
	text_layer_destroy(s_yourFocal_layer);
	text_layer_destroy(s_yourAperture_layer);
	text_layer_destroy(s_yourHyperfocal_layer);
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
	// Use this provider to add button click subscriptions
	window_set_click_config_provider(s_main_window, click_config_provider);
	
	update();
}

static void deinit() {
	window_destroy(s_main_window);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}
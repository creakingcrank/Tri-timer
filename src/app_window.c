#include <pebble.h>
#include "app_window.h"

#define NUMBER_OF_TIMERS 6 //number of timers used by app
#define START_TIME 0  // for code readability in timer array
#define ELAPSED_TIME 1 // for code readability in timer array
#define RESET_DISPLAY_TO_CURRENT_TIMER_AFTER_MS 10000 // timer display resets to the current running timer after this many ms

// values for the persistent sotrage of timer  and display info

#define PERSIST_DATA_VERSION_KEY 101
#define PERSIST_DATA_CURRENT_VERSION 1
#define PERSIST_DATA_TIMER_KEY 1
#define PERSIST_DATA_SHOWING_KEY 2

int timer [2][NUMBER_OF_TIMERS]; // the array to store the timers [0=START_TIME,1=ELAPSED TIME][TIMER NUMBER]
int timer_showing; // the number of the timer currently showing on the screen

AppTimer *display_reset_timer;

/*

THINGS TO DO
------------

- tidy up header file


*/

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GFont s_res_bitham_42_bold;
static GFont s_res_bitham_30_black;
static GFont s_res_bitham_42_light;
static TextLayer *top_time_layer;
static TextLayer *top_text_layer;
static TextLayer *bot_time_layer;
static TextLayer *bot_button_layer;
static TextLayer *bot_left_layer;

static void initialise_ui(void) {
  s_window = window_create();
  #ifndef PBL_SDK_3
    window_set_fullscreen(s_window, true);
  #endif
  
  s_res_bitham_42_bold = fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD);
  s_res_bitham_30_black = fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK);
  s_res_bitham_42_light = fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT);
  // top_time_layer
  top_time_layer = text_layer_create(GRect(0, 43, 144, 44));
  text_layer_set_background_color(top_time_layer, GColorClear);
  text_layer_set_text(top_time_layer, "00:00");
  text_layer_set_text_alignment(top_time_layer, GTextAlignmentCenter);
  text_layer_set_font(top_time_layer, s_res_bitham_42_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)top_time_layer);
  
  // top_text_layer
  top_text_layer = text_layer_create(GRect(9, 17, 125, 42));
  text_layer_set_background_color(top_text_layer, GColorClear);
  text_layer_set_text(top_text_layer, "SWIM");
  text_layer_set_text_alignment(top_text_layer, GTextAlignmentCenter);
  text_layer_set_font(top_text_layer, s_res_bitham_30_black);
  layer_add_child(window_get_root_layer(s_window), (Layer *)top_text_layer);
  
  // bot_time_layer
  bot_time_layer = text_layer_create(GRect(66, 79, 78, 42));
  text_layer_set_background_color(bot_time_layer, GColorClear);
  text_layer_set_text(bot_time_layer, ":00");
  text_layer_set_font(bot_time_layer, s_res_bitham_42_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)bot_time_layer);
  
  // bot_button_layer
  bot_button_layer = text_layer_create(GRect(70, 118, 71, 41));
  text_layer_set_background_color(bot_button_layer, GColorClear);
  text_layer_set_text(bot_button_layer, ">");
  text_layer_set_text_alignment(bot_button_layer, GTextAlignmentRight);
  text_layer_set_font(bot_button_layer, s_res_bitham_42_light);
  layer_add_child(window_get_root_layer(s_window), (Layer *)bot_button_layer);
  
  // bot_left_layer
  bot_left_layer = text_layer_create(GRect(2, 127, 69, 42));
  text_layer_set_background_color(bot_left_layer, GColorClear);
  text_layer_set_text(bot_left_layer, "OK");
  text_layer_set_font(bot_left_layer, s_res_bitham_30_black);
  layer_add_child(window_get_root_layer(s_window), (Layer *)bot_left_layer);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  text_layer_destroy(top_time_layer);
  text_layer_destroy(top_text_layer);
  text_layer_destroy(bot_time_layer);
  text_layer_destroy(bot_button_layer);
  text_layer_destroy(bot_left_layer);
}
// END AUTO-GENERATED UI CODE

static int get_timer_showing(void) {
  return timer_showing;
}

static void set_timer_showing(int timer_to_show) {
  timer_showing=timer_to_show;
  set_timer_description(timer_showing);
  show_time(timer[ELAPSED_TIME][timer_showing]);
  increment_timers(NULL);
  
}
  
static int get_current_running_timer(void)  {
  
  //returns the number of the highest running timer. If no timer running, returns -1
  
  int i;
  int timer_running_now = -1;
  
  for (i = 0; i < NUMBER_OF_TIMERS; i++) { 
    if (timer[START_TIME][i]>0) timer_running_now = i;
  }
  return timer_running_now;

}

 static void reset_all_timers(void) {
   
 int i;
  
  for (i = 0; i < NUMBER_OF_TIMERS; i++) { 
    timer[START_TIME] [i] = 0;
    timer[ELAPSED_TIME][i] = 0;
  }
 }
   
 static int get_last_timer_run(void)  {
  
  //returns the number of the last timer with any elapsed time recorded
  
  int i;
  int last_timer_run = -1;
  
  for (i = 0; i < NUMBER_OF_TIMERS; i++) { 
    if (timer[ELAPSED_TIME][i]>0) last_timer_run = i;
  }
  return last_timer_run;
} 
  
static void next_timer(void) {
  
  // Moves on to the next timer
  
  int i;
  int timer_running_now;
  time_t current_time;
  
  current_time = time(NULL);
  
  timer_running_now = get_current_running_timer();
  
  vibes_double_pulse();
  
  //if no timer running - start the total timer (0) and the 1st timer (1)
  if (timer_running_now == -1) {
    timer[START_TIME][0] = current_time; 
    timer[START_TIME][1] = current_time;
    set_timer_showing(1);
    return;
  }
  
  // if the last timer is running stop all timers
  if (timer_running_now == NUMBER_OF_TIMERS-1) {
    for (i = 0; i < NUMBER_OF_TIMERS; i++) {
      timer[START_TIME][i] = 0;
    } 
    set_timer_showing(0); // show the grand total at the end
   return;
  }
  
  //else move to the next timer
  timer[START_TIME][timer_running_now] = 0;
  timer[START_TIME][timer_running_now+1] = current_time;
  if (get_timer_showing() != 0 ) set_timer_showing(timer_running_now+1); // show the current running timer, unless you are watching total time
 
}

 void increment_timers(void *data) {
   
   // This is the main timer callback loop 
  
  int i; //counter
  time_t current_time;
 
  
  current_time = time(NULL);
  
  for (i = 0; i < NUMBER_OF_TIMERS; i++) {
    if (timer[START_TIME][i]>0) {
      timer[ELAPSED_TIME][i] = timer[ELAPSED_TIME][i] + (current_time - timer[START_TIME][i]);
      timer[START_TIME][i] = current_time;
    }
  }
   
   show_time(timer[ELAPSED_TIME][timer_showing]);
  
 // If we have any running timers, come back here
  if (get_current_running_timer() != -1) {
   app_timer_register(1000, increment_timers, NULL);
  }
}

static void pause_resume(void)  { //middle button action
  
  int i;
  int timer_running_now;
  int last_timer_run;
  time_t current_time;
  
  current_time = time(NULL);
  
  vibes_double_pulse();
  
  timer_running_now = get_current_running_timer();
  
  if (timer_running_now == -1) { //no timers running so restart the last running timer
    last_timer_run = get_last_timer_run();
    if (last_timer_run > -1) timer[START_TIME][last_timer_run] = current_time;
    else next_timer();   
    increment_timers(NULL);
  }
  else {
    for (i = 0; i < NUMBER_OF_TIMERS; i++)  timer[START_TIME][i] = 0; // stop all the timers
  }
  
}
 void set_timer_description(int timer_number) { // tells you about the current timer
   
  
  switch(timer_number) {
    case 0 :
    text_layer_set_text(top_text_layer, "TOTAL");
    break;
    case 1 :
    text_layer_set_text(top_text_layer, "SWIM");
    break;
    case 2 :
    text_layer_set_text(top_text_layer, "T1");
    break;
    case 3 :
    text_layer_set_text(top_text_layer, "BIKE");
    break;
    case 4 :
    text_layer_set_text(top_text_layer, "T2");
    break;
    case 5 :
    text_layer_set_text(top_text_layer, "RUN");
    break;
    default :
    text_layer_set_text(top_text_layer, "BAD T#");
  }
    
}

 void show_time(int time) { //displays a time in the two time windows
  
  static char top_text[6];
  static char bot_text[6];
  
  
  if (time < 3600) {
    snprintf(top_text,sizeof(top_text),"%02d:%02d", (time / 60) % 60, time  % 60 );
    text_layer_set_text(top_time_layer, top_text);
    text_layer_set_text(bot_time_layer, " ");
  }
  else {
    snprintf(top_text,sizeof(top_text),"%02d:%02d",time / 3600, (time / 60) % 60 );
    snprintf(bot_text,sizeof(bot_text),":%02d",time  % 60 );
    text_layer_set_text(top_time_layer, top_text);
    text_layer_set_text(bot_time_layer, bot_text);
  }

}

void show_current_timer(void *data) {
  // resets display to show the current running timer
  
  int current_timer=get_current_running_timer();
  
  if (current_timer != -1) { //check there is a current running timer
    timer_showing = current_timer;
    set_timer_description(timer_showing);
    show_time(timer[ELAPSED_TIME][timer_showing]);
  }
}

 void show_next_timer(void) { //top button action
   
   app_timer_cancel(display_reset_timer);
   
  timer_showing++;
  if (timer_showing > NUMBER_OF_TIMERS-1) timer_showing = 0;
  set_timer_description(timer_showing);
  show_time(timer[ELAPSED_TIME][timer_showing]);
   
  if ((get_timer_showing() != get_current_running_timer()) && (get_timer_showing() !=0 )) { // this automatically resets to the current runnign timer after a while, unless you are watching total time
     display_reset_timer=app_timer_register(RESET_DISPLAY_TO_CURRENT_TIMER_AFTER_MS,show_current_timer, NULL);
  }
}

 void update_bot_left_display(void *data) {
   // callback function for the bottom left status display 
   // If you are not watching the current running timer, this puts a blinking indicator at the bottom of the screen
   
   int current_running_timer = get_current_running_timer();
   
   
   if (timer_showing != current_running_timer) {
     switch (current_running_timer)  {
    case -1 :
    text_layer_set_text(bot_left_layer, " ");
    break;
    case 1 :
    text_layer_set_text(bot_left_layer, "S");
    break;
    case 2 :
    case 4 :   
    text_layer_set_text(bot_left_layer, "T");
    break;
    case 3 :
    text_layer_set_text(bot_left_layer, "B");
    break;
    case 5 :
    text_layer_set_text(bot_left_layer, "R");
    break;
    default :
    text_layer_set_text(bot_left_layer, "#");
     }
  }
  else text_layer_set_text(bot_left_layer, " ");
 
// flash the text every second
   if (time(NULL) % 2) text_layer_set_text(bot_left_layer, " ");
 

app_timer_register(1000, update_bot_left_display, NULL);
  

  }

static void load_timers_from_storage(void) {
  
  int data_version = 0;
  
  if (persist_exists(PERSIST_DATA_VERSION_KEY)) data_version = persist_read_int(PERSIST_DATA_VERSION_KEY);
                                                                                
  if (data_version == PERSIST_DATA_CURRENT_VERSION) {
    persist_read_data(PERSIST_DATA_TIMER_KEY,*timer,sizeof(timer));
    set_timer_showing(persist_read_int(PERSIST_DATA_SHOWING_KEY));
  }
  else {
    reset_all_timers();
    set_timer_showing(1);
  }
  }
                                                                                
                                                                                                                                                             
                                                                                
static void push_timers_to_storage(void) {
  
  persist_write_int(PERSIST_DATA_VERSION_KEY,PERSIST_DATA_CURRENT_VERSION);
  persist_write_data(PERSIST_DATA_TIMER_KEY,*timer,sizeof(timer));
  persist_write_int(PERSIST_DATA_SHOWING_KEY,get_timer_showing());
  

}

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void show_app_window(void) {
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_app_window(void) {
  window_stack_remove(s_window, true);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  pause_resume();
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  show_next_timer();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  next_timer();
}

static void long_select_down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (get_current_running_timer() == -1) { // only allow reset if no timers running
    text_layer_set_text(top_text_layer, " ");
    text_layer_set_text(top_time_layer, "RESET"); 
    text_layer_set_text(bot_time_layer, " ");
    vibes_long_pulse();
}
  
}

static void long_select_up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (get_current_running_timer() == -1) { // only allow reset if no timers running
  reset_all_timers();
  timer_showing=(0);
  set_timer_description(timer_showing);
  increment_timers(NULL); 
  
  }
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
  window_long_click_subscribe(BUTTON_ID_SELECT, 3000, long_select_down_click_handler, long_select_up_click_handler);
}

static void init(void) {
  
  
  show_app_window();
  window_set_click_config_provider(s_window, click_config_provider);
  load_timers_from_storage();
  set_timer_description(timer_showing);
  show_time(timer[ELAPSED_TIME][timer_showing]);
  update_bot_left_display(NULL);
  
}

static void deinit(void) {
  push_timers_to_storage();
  hide_app_window();
}

  
int main(void) {
  init();
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", s_window);
  app_event_loop();
  deinit();

}
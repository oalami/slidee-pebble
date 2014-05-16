#include <pebble.h>

static Window *window;
static TextLayer *text_layer;
static AppSync sync;
static uint8_t sync_buffer[32];

void out_sent_handler(DictionaryIterator *sent, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "outgoing message was delivered");
   // outgoing message was delivered
 }


 void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "outgoing message failed");
   // outgoing message failed
 }


 void in_received_handler(DictionaryIterator *received, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "incoming message received");
   // incoming message received
 }


 void in_dropped_handler(AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "incoming message dropped");

   // incoming message dropped
 }

 static void init_messaging(void) {   
  APP_LOG(APP_LOG_LEVEL_DEBUG, "init start");
   app_message_register_inbox_received(in_received_handler);
   app_message_register_inbox_dropped(in_dropped_handler);
   app_message_register_outbox_sent(out_sent_handler);
   app_message_register_outbox_failed(out_failed_handler);

   const uint32_t inbound_size = 64;
   const uint32_t outbound_size = 64;
   app_message_open(inbound_size, outbound_size);

   APP_LOG(APP_LOG_LEVEL_DEBUG, "init done");
 }

static void send_message(int i) {
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  Tuplet value = TupletInteger(0, i);
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Sent %i", i);  
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Select");
  APP_LOG(APP_LOG_LEVEL_DEBUG, "sending Select");
  send_message(1);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Up");
  send_message(0);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Down");
  send_message(2);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(text_layer, "Press a button");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  init_messaging();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}

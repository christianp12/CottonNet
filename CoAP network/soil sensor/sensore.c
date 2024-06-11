#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "coap-engine.h"
#include "coap-blocking-api.h"
#include "os/dev/button-hal.h"
#include "../cJSON/cJSON.h"
#include "global_variables.h"
#if PLATFORM_SUPPORTS_BUTTON_HAL
#include "dev/button-hal.h"
#else
#include "dev/button-sensor.h"
#endif
/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP
#define SERVER_EP "coap://[fd00::1]:5683" // localhost ip6
#define GOOD_ACK 65
int sensor_reg = 0;
extern int turnoff;
void client_chunk_handler(coap_message_t *response)
{
  const uint8_t *chunk;
  if (response == NULL)
  {
    printf("Request timed out\n");
    return;
  }
  int len = coap_get_payload(response, &chunk);
  char payload[len + 1];
  memcpy(payload, chunk, len);
  payload[len] = '\0'; // Ensure null-terminated string
  printf("Response: %i\n", response->code);
  if (response->code == GOOD_ACK)
  {
    printf("Registration successful\n");
    sensor_reg = 1;
  }
  else
  {
    printf("Registration failed\n");
  }
}
extern coap_resource_t res_soil, res_sampling , res_turnoff;
static struct etimer et;
extern int sampling;

PROCESS(soil_sensor_process, "Soil sensor Server");
AUTOSTART_PROCESSES(&soil_sensor_process);

PROCESS_THREAD(soil_sensor_process, ev, data)
{
  static coap_endpoint_t server_ep;
  static coap_message_t request[1];

  PROCESS_BEGIN();
  while (ev != button_hal_press_event && turnoff == 0)
  {
    PROCESS_YIELD();
    while (sensor_reg == 0)
    {
      coap_endpoint_parse(SERVER_EP, strlen(SERVER_EP), &server_ep);
      // Registration Process
      printf("REGISTRATION TO THE SERVER...\n");
      coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);
      coap_set_header_uri_path(request, "/registration");
      printf("MESSAGGIO INIZIALIZZATO\n");
      cJSON *package = cJSON_CreateObject();
      cJSON_AddStringToObject(package, "s", "sensor1");
      cJSON_AddStringToObject(package, "t", "sensor");
      cJSON_AddNumberToObject(package, "c", sampling);
      char *payload = cJSON_PrintUnformatted(package);
      if (payload == NULL)
      {
        LOG_ERR("Failed to print JSON object\n");
        cJSON_Delete(package);
        PROCESS_EXIT();
      }
 
      coap_set_payload(request, (uint8_t *)payload, strlen(payload));
      COAP_BLOCKING_REQUEST(&server_ep, request, client_chunk_handler);
    }
    printf("REGISTRATION TO THE SERVER COMPLETED\n");

    coap_activate_resource(&res_sampling, "sampling");
    coap_activate_resource(&res_soil, "soil");
    coap_activate_resource(&res_turnoff, "turn_off");

    etimer_set(&et, CLOCK_SECOND * sampling);

    while (turnoff != 1)
    {
      PROCESS_WAIT_EVENT();

      if (ev == PROCESS_EVENT_TIMER && data == &et)
      {
        printf("sampling soil\n");
        res_soil.trigger();
        etimer_set(&et, CLOCK_SECOND * sampling);
      }
    }
  }
  PROCESS_END();
}

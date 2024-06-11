#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "coap-engine.h"
#include "coap-blocking-api.h"
#include "os/dev/button-hal.h"
#include "../cJSON/cJSON.h"
#include "global_variables.h"

// Include button-hal or button-sensor based on platform support
#if PLATFORM_SUPPORTS_BUTTON_HAL
#include "dev/button-hal.h"
#else
#include "dev/button-sensor.h"
#endif

// Log configuration
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP

// Server endpoint (localhost IPv6)
#define SERVER_EP "coap://[fd00::1]:5683"

// Good acknowledgment code
#define GOOD_ACK 65

// Sensor registration status
int sensor_reg = 0;

// Turnoff flag
extern int turnoff;

// Client chunk handler function
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

// CoAP resources
extern coap_resource_t res_co2, res_light, res_phase, res_sampling, res_observation, res_turnoff;
//sampling time
extern int sampling;
// Timer
static struct etimer et;

// Illumination server process
PROCESS(illumination_server, "Illumination Server");

// Autostart processes
AUTOSTART_PROCESSES(&illumination_server);

// Illumination server process thread
PROCESS_THREAD(illumination_server, ev, data)
{

    // Initialize server endpoint and request message
    static coap_endpoint_t server_ep;
    static coap_message_t request[1];

    PROCESS_BEGIN();
    while (ev!= button_hal_press_event && turnoff == 0)
    {
        PROCESS_YIELD();
        // Registration Process
        while (sensor_reg == 0)
        {
            coap_endpoint_parse(SERVER_EP, strlen(SERVER_EP), &server_ep);
            printf("REGISTRATION TO THE SERVER...\n");
            coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);
            coap_set_header_uri_path(request, "/registration");
            printf("MESSAGGIO INIZIALIZZATO\n");
            cJSON *package = cJSON_CreateObject();

            cJSON_AddStringToObject(package, "s", "sensor0");
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
             printf("sending msg\n");
            COAP_BLOCKING_REQUEST(&server_ep, request, client_chunk_handler);
        }
        printf("REGISTRATION TO THE SERVER COMPLETED\n");
        coap_activate_resource(&res_co2, "co2");
        coap_activate_resource(&res_light, "light");
        coap_activate_resource(&res_phase, "phase");
        coap_activate_resource(&res_sampling, "sampling");
        coap_activate_resource(&res_observation, "observation");
        coap_activate_resource(&res_turnoff, "turn_off");

        etimer_set(&et, CLOCK_SECOND * sampling);
        while (turnoff!= 1)
        {
            PROCESS_WAIT_EVENT();

            if (ev == PROCESS_EVENT_TIMER && data == &et)
            {
                printf("Sampling resources\n");
                res_co2.trigger();
                res_light.trigger();
                res_phase.trigger();
                res_observation.trigger();
                etimer_set(&et, CLOCK_SECOND * sampling);
            }
        }
    }
    PROCESS_END();
}
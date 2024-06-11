#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"
#include "coap-engine.h"
#include "coap-blocking-api.h"
#include "irrigation_model.h" // emlearn generated model
#include "eml_trees.h"        // Correct header for tree model functions
#include "eml_common.h"       // Common header for emlearn
#include "../cJSON/cJSON.h"

#if PLATFORM_SUPPORTS_BUTTON_HAL
#include "dev/button-hal.h"
#else
#include "dev/button-sensor.h"
#endif

/* Log configuration */
#include "coap-log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP

#define SERVER_EP "coap://[fd00::1]:5683" // localhost ip6
#define GOOD_ACK 69
#define TOGGLE_INTERVAL 10
char *service_ip;
char *service_url = "/soil";
PROCESS(sprinkler_actuator_process, "Sprinkler Client");
AUTOSTART_PROCESSES(&sprinkler_actuator_process);

float features[] = {1, 0, 0};
int moisture = 0;
int temperature = 0;
static int actuator_reg = 0;
extern int turnoff;
void client_chunk_handler_registration(coap_message_t *response)
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
    service_ip = (char *)malloc(len + 1);
    strcpy(service_ip, payload); // Copia la stringa da payload a service_ip
    printf("Indirizzo IP del sensore di riferimento: %s\n", service_ip);
    if (len != 0)
    {
        printf("Registration successful\n");
        actuator_reg = 1;
    }
    else
    {
        printf("Registration failed\n");
    }
}
void client_chunk_handler(coap_message_t *response)
{
    const uint8_t *chunk;

    if (response == NULL)
    {
        puts("Request timed out");
        return;
    }

    coap_get_payload(response, &chunk);
    sscanf((char *)chunk, "{\"m\": %d, \"t\": %d}", &moisture, &temperature);
    printf("moisture: %d , temperature: %d\n", moisture, temperature);
    features[1] = moisture;
    features[2] = temperature;
    int class_idx = eml_trees_predict(&irrigation_model, features, 3);

    printf("Irrigation needed: %d\n", class_idx);
}

void handle_notification(struct coap_observee_s *observee, void *notification, coap_notification_flag_t flag)
{
    coap_message_t *msg = (coap_message_t *)notification;
    if (msg)
    {
        printf("Received notification\n");
        client_chunk_handler(msg);
    }
    else
    {
        printf("No notification received\n");
    }
}
extern coap_resource_t res_turnoff;
PROCESS_THREAD(sprinkler_actuator_process, ev, data)
{
    static coap_endpoint_t server_ep;
    static coap_message_t request[1];

    PROCESS_BEGIN();
    while (ev != button_hal_press_event && turnoff == 0)
    {
        PROCESS_YIELD();
        while (actuator_reg == 0)
        {
            coap_endpoint_parse(SERVER_EP, strlen(SERVER_EP), &server_ep);
            // Registration Process
            printf("REGISTRATION TO THE SERVER...\n");
            coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);
            coap_set_header_uri_path(request, "/registration");
            printf("MESSAGGIO INIZIALIZZATO\n");
            cJSON *package = cJSON_CreateObject();
            cJSON_AddStringToObject(package, "s", "sprinkler");
            cJSON_AddStringToObject(package, "t", "actuator");
            cJSON_AddNumberToObject(package, "c", 0);
            char *payload = cJSON_PrintUnformatted(package);
            if (payload == NULL)
            {
                LOG_ERR("Failed to print JSON object\n");
                cJSON_Delete(package);
                PROCESS_EXIT();
            }
            
            coap_set_payload(request, (uint8_t *)payload, strlen(payload));
            COAP_BLOCKING_REQUEST(&server_ep, request, client_chunk_handler_registration);
        }
        printf("REGISTRATION TO THE SERVER COMPLETED\n");

        // starting observing to the resource
        coap_endpoint_parse(service_ip, strlen(service_ip), &server_ep);
        coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);
        coap_set_header_uri_path(request, service_url);
        coap_obs_request_registration(&server_ep, service_url, handle_notification, NULL);
        coap_activate_resource(&res_turnoff, "turn_off");
        while (turnoff != 1)
        {
            PROCESS_YIELD();
        }
    }
    PROCESS_END();
}

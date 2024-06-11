#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"
#include "coap-engine.h"
#include "coap-blocking-api.h"
#include "os/dev/leds.h"
#include "../cJSON/cJSON.h"
#include "coap-log.h"
#include "global_variables.h"
#if PLATFORM_SUPPORTS_BUTTON_HAL
#include "dev/button-hal.h"
#else
#include "dev/button-sensor.h"
#endif
/* Log configuration */
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP
#define TOGGLE_INTERVAL 10
#define SERVER_EP "coap://[fd00::1]:5683" // localhost ip6
#define GOOD_ACK 69

char *service_ip;
char *service_url_co2 = "/co2";
char *service_url_light = "/light";
char *service_url_phase = "/phase";
extern int turnoff;
static int actuator_reg = 0;
PROCESS(illumination_client, "Illumination Client");
AUTOSTART_PROCESSES(&illumination_client);

static void update_led_state();
 int light_attuatore = 0;
 int co2 = 0;
 int fase = 0;

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

    if (len != 0 )
    {
        printf("Registration successful\n");
        actuator_reg = 1;
    }
    else
    {
        printf("Registration failed\n");
    }
}

void client_chunk_handler_co2(coap_message_t *response)
{
    const uint8_t *chunk;

    if (response == NULL)
    {
        puts("Request timed out");
        return;
    }

    coap_get_payload(response, &chunk);

    // Directly parse the JSON response to extract the CO2 level
    sscanf((const char *)chunk, "{\"co2_level\": %d}", &co2);
    printf("CO2 level: %d\n", co2);
    update_led_state();
}

void client_chunk_handler_light(coap_message_t *response)
{
    const uint8_t *chunk;

    if (response == NULL)
    {
        puts("Request timed out");
        return;
    }

    coap_get_payload(response, &chunk);

    // Directly parse the JSON response to extract the light status
    sscanf((const char *)chunk, "{\"light\": %d}" , &light_attuatore);
    printf("light status: %d\n", light_attuatore);
    update_led_state();
}

void client_chunk_handler_phase(coap_message_t *response)
{
    const uint8_t *chunk;

    if (response == NULL)
    {
        puts("Request timed out");
        return;
    }

    coap_get_payload(response, &chunk);

    
    sscanf((const char *)chunk, "{\"phase\": %d}" , &fase);
    printf("Phase: %d\n", fase);
    update_led_state();
}
void handle_notification_co2(struct coap_observee_s *observee, void *notification, coap_notification_flag_t flag)
{
    coap_message_t *msg = (coap_message_t *)notification;
    if (msg)
    {
        printf("Received notification ");
        client_chunk_handler_co2(msg);
    }
    else
    {
        printf("No notification received\n");
    }
}
void handle_notification_light(struct coap_observee_s *observee, void *notification, coap_notification_flag_t flag)
{
    coap_message_t *msg = (coap_message_t *)notification;
    if (msg)
    {
        printf("Received notification ");
        client_chunk_handler_light(msg);
    }
    else
    {
        printf("No notification received\n");
    }
}
void handle_notification_phase(struct coap_observee_s *observee, void *notification, coap_notification_flag_t flag)
{
    coap_message_t *msg = (coap_message_t *)notification;
    if (msg)
    {
        printf("Received notification ");
        client_chunk_handler_phase(msg);
    }
    else
    {
        printf("No notification received\n");
    }
}
void update_led_state() {
    leds_off(LEDS_ALL); // Turn off all LEDs initially

    if (light_attuatore == 1 && co2 > 400 && co2 < 1500 && fase == 0) {
        // LED OFF
    }
    else if (light_attuatore == 1 && (co2 < 400 || co2 > 1500) && fase == 0) {
        leds_single_on(LEDS_YELLOW);
    }
    else if (light_attuatore == 0 && co2 > 400 && co2 < 1500 && fase == 0) {
        leds_on(LEDS_RED);
    }
    else if (light_attuatore == 0 && (co2 < 400 || co2 > 1500) && fase == 0) {
        leds_on(LEDS_RED);
    }
    else if (light_attuatore == 1 && co2 > 400 && co2 < 1500 && fase == 1) {
        // LED OFF
    }
    else if (light_attuatore == 1 && (co2 < 400 || co2 > 1500) && fase == 1) {
        leds_on(LEDS_GREEN);
    }
    else if (light_attuatore == 0 && co2 > 400 && co2 < 1500 && fase == 1) {
        leds_single_on(LEDS_YELLOW);
    }
    else if (light_attuatore == 0 && (co2 < 400 || co2 > 1500) && fase == 1) {
        leds_on(LEDS_GREEN);
    }
}
extern coap_resource_t res_turnoff;
PROCESS_THREAD(illumination_client, ev, data)
{
    static coap_endpoint_t server_ep;
    static coap_message_t request[1];

    PROCESS_BEGIN();
    while (ev != button_hal_press_event && turnoff == 0 )
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
            cJSON_AddStringToObject(package, "s", "illumination");
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
             printf("sending msg\n");
            COAP_BLOCKING_REQUEST(&server_ep, request, client_chunk_handler_registration);
        }
        printf("REGISTRATION TO THE SERVER COMPLETED\n");
        
        // il parse sarà ora rivolto all'ip del sensore di riferimento
        coap_endpoint_parse(service_ip, strlen(service_ip), &server_ep);
        coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);

        // REGISTRATION PER CO2
        coap_set_header_uri_path(request, service_url_co2);
        coap_obs_request_registration(&server_ep, service_url_co2, handle_notification_co2, NULL);

        // REGISTRATION PER light
        coap_set_header_uri_path(request, service_url_light);
        coap_obs_request_registration(&server_ep, service_url_light, handle_notification_light, NULL);

        // REGISTRATION PER phase
        coap_set_header_uri_path(request, service_url_phase);
        coap_obs_request_registration(&server_ep, service_url_phase, handle_notification_phase, NULL);
        
        coap_activate_resource(&res_turnoff, "turn_off");

        while (turnoff != 1)
        {
            PROCESS_YIELD();
        }
    }
    
    PROCESS_END();
}

package com.example;

import org.eclipse.californium.core.CoapResource;
import org.eclipse.californium.core.coap.CoAP;
import org.eclipse.californium.core.coap.Response;
import org.eclipse.californium.core.server.resources.CoapExchange;
import org.json.JSONObject;

public class RegistrationResource extends CoapResource {

    private DBManager dbManager = new DBManager("jdbc:mysql://localhost:3306/CottonNet", "admin", "admin"); // Database manager instance

    // Constructor for the registration resource
    public RegistrationResource(String name) {
        super(name);
    }

    // Handle POST requests for registration
    @Override
    public void handlePOST(CoapExchange exchange) {
        Response response;
        String nodeAddress = exchange.getSourceAddress().getHostAddress(); // Get the IP address of the node
        String payloadString = exchange.getRequestText(); // Get the payload of the request as a string
        System.out.println("Payload received: " + payloadString + " \nlunghezza: " + payloadString.length());
        System.out.println("IP address: " + nodeAddress + "\n");
        JSONObject json = new JSONObject(payloadString); // Parse the payload as JSON
        String sensorName = (String) json.get("s");
        String sensorType = (String)json.get("t");
        int samplingTime = (int) json.get("c");
        System.out.println("Parsed JSON: " + json);
        
        // Register the sensor or actuator
        if (sensorType.equals("sensor")) {
            dbManager.register(sensorName, nodeAddress, sensorType, samplingTime);
            response = new Response(CoAP.ResponseCode.CREATED);
            exchange.respond(response); 
        } else {
            // Registering an actuator
            dbManager.register(sensorName, nodeAddress, sensorType, samplingTime);
            // Send the IP address of the sensor the actuator refers to in the response
            if (sensorName.equals("illumination")) {
                // Illumination actuator
                dbManager.register(sensorName, nodeAddress, sensorType, samplingTime);
                String sensorReference = dbManager.select("sensor0");
                if(sensorReference.isEmpty()){
                    response = new Response(CoAP.ResponseCode.BAD_REQUEST);
                    exchange.respond(response);
                } else {
                    System.out.println("Sending IP Sensor reference: " + sensorReference + "\n");
                    response = new Response(CoAP.ResponseCode.CONTENT);
                    String coapPrefix = "coap://[";
                    String porta = "]:5683";
                    sensorReference = coapPrefix + sensorReference + porta; 
                    response.setPayload(sensorReference); // Send the sensor's IP address
                    exchange.respond(response);
                }
            } else {
                // Irrigation actuator
                dbManager.register(sensorName, nodeAddress, sensorType, samplingTime);
                String sensorReference = dbManager.select("sensor1");
                if(sensorReference.isEmpty()){
                    response = new Response(CoAP.ResponseCode.BAD_REQUEST);
                    exchange.respond(response);
                } else {
                    System.out.println("Sending IP Sensor reference: " + sensorReference + "\n");
                    response = new Response(CoAP.ResponseCode.CONTENT);
                    String coapPrefix = "coap://[";
                    String porta = "]:5683";
                    sensorReference = coapPrefix + sensorReference + porta; 
                    response.setPayload(sensorReference); // Send the sensor's IP address
                    exchange.respond(response);
                }
            }
        }
        System.out.println("Node at IP: " + nodeAddress + " registered");

        // Start observing the sensor if it's a sensor type
        if (sensorType.equals("sensor")) {
            observe(sensorName, nodeAddress);
            System.out.println("Observation at resource started");
        }
    }

    // Start observing the specified sensor
    private static void observe(String sensorName, String address) {
        CoapObserver obs = new CoapObserver(sensorName, address);
        obs.startObserving();
    }
}
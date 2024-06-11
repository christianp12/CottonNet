package com.example;

import org.eclipse.californium.core.CoapServer;

public class RegistrationServer extends CoapServer {
    public static void main(String[] args) {
        RegistrationServer server = new RegistrationServer(); // Create a new CoAP server instance
        server.add(new RegistrationResource("registration")); // Add the registration resource to the server
        server.start(); // Start the server
        System.out.println("CoAP server is running..."); // Log that the server is running
    }
}

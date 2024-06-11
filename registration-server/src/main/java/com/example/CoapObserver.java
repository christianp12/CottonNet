package com.example;

import org.eclipse.californium.core.CoapClient;
import org.eclipse.californium.core.CoapHandler;
import org.eclipse.californium.core.CoapObserveRelation;
import org.eclipse.californium.core.CoapResponse;
import org.json.JSONObject;

public class CoapObserver implements Runnable {
	private CoapClient client; // CoAP client for communicating with the sensor
	private DBManager dbManager = new DBManager("jdbc:mysql://localhost:3306/CottonNet", "admin", "admin"); // Database manager instance
	private String sensorName; // Name of the sensor being observed
	// private String address;

	// Constructor initializes the CoAP client based on the sensor name and address
	public CoapObserver(String sensorName, String address) {
		if (sensorName.equals("sensor0")) {
			client = new CoapClient("coap://[" + address + "]:5683/observation");
		} else {
			client = new CoapClient("coap://[" + address + "]:5683/soil");
		}
		this.sensorName = sensorName;
		// this.address = address;
	}

	// Starts observing the CoAP resource
	public void startObserving() {
		CoapObserveRelation relation = client.observe(new CoapHandler() {

			// Called when a new CoAP response is received
			@Override
			public void onLoad(CoapResponse response) {
				String responseText = response.getResponseText(); // Get the response payload as a string
				System.out.println("Payload received: " + responseText + " \nlunghezza: " + responseText.length());
				JSONObject json = null;

				try {
					json = new JSONObject(responseText);
					if (sensorName.equals("sensor0")) {
						// Parse and print sensor0 data
						int co2 = json.getInt("c");
						int light = json.getInt("l");
						int phase = json.getInt("p");
						System.out.println(co2 + " " + light + " " + phase);
						// Insert illumination measures into the database
						dbManager.insertIlluminationMeasures(co2, light, phase);
						System.out.println("Dati illlumination Registrati correttamente");
					} else if (sensorName.equals("sensor1")) {
						// Parse and print sensor1 data
						System.out.println("Dati soil Arrivati");
						int moisture = json.getInt("m");
						int temperature = json.getInt("t");
						System.out.println("Dati soil Registrati correttamente");
						System.out.println(temperature + " " + moisture);
						// Insert soil measures into the database
						dbManager.insertSoilMeasures(moisture, temperature);
						System.out.println("Dati sensore Registrati correttamente");
					}
				} catch (Exception e) {
					e.printStackTrace(); // Print stack trace if an exception occurs
				}
			}

			// Called when there is an error with the observation
			@Override
			public void onError() {
				System.out.println("Nothing to observe");
			}
		});
	}

	// The run method to start the observation
	@Override
	public void run() {
		startObserving();
	}
}

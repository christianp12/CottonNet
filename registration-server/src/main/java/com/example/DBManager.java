package com.example;

import java.sql.*;

public class DBManager {
    public String DB_URL; // Database URL
    public String user; // Database user
    public String password; // Database password
    private PreparedStatement pstSelect; // Prepared statement for selecting device address
    private PreparedStatement pstRegister; // Prepared statement for registering a device
    private PreparedStatement pstInsertIlluminationMeasures; // Prepared statement for inserting illumination measures
    private PreparedStatement pstInsertSprinklerMeasures; // Prepared statement for inserting soil measures

    // Constructor initializes the database connection and prepared statements
    public DBManager(String URL, String user, String password) {
        this.DB_URL = URL;
        this.user = user;
        this.password = password;

        try {
            Connection conn = DriverManager.getConnection(DB_URL, this.user, this.password);
            
            // Prepare statement for registering a device, updating address and sampling if the device already exists
            pstRegister = conn.prepareStatement(
                "INSERT INTO devices (name, address, type, sampling) VALUES (?, ?, ?, ?) " +
                "ON DUPLICATE KEY UPDATE address = VALUES(address), sampling = VALUES(sampling);");
            
            // Prepare statement for selecting the address of a device by name
            pstSelect = conn.prepareStatement(" SELECT address FROM devices WHERE name = ? ;");

            // Prepare statement for inserting illumination measures (co2, light, phase)
            pstInsertIlluminationMeasures = conn.prepareStatement(
                //"INSERT INTO illumination (co2, light, phase, timestamp) VALUES (?, ?, ?, ?)");
                "INSERT INTO illumination (co2, light, phase) VALUES (?, ?, ?)");

            // Prepare statement for inserting soil measures (moisture, temperature)
            pstInsertSprinklerMeasures = conn.prepareStatement(
                //"INSERT INTO soil (moisture, temperature, timestamp) VALUES (?, ?, ?)");
                "INSERT INTO soil (moisture, temperature) VALUES (?, ?)");

        } catch (SQLException e) {
            e.printStackTrace(); // Print stack trace if a SQL exception occurs
        }
    }

    // Method to select the address of a device by its name
    public String select(String name) {
        try {
            pstSelect.setString(1, name);
            ResultSet rs = pstSelect.executeQuery();
            if (rs.next()) {
                return rs.getString("address"); // Return the address if found
            }
        } catch (SQLException e) {
            e.printStackTrace(); // Print stack trace if a SQL exception occurs
        }
        return null; // Return null if no address is found or an exception occurs
    }

    // Method to register a device with its name, address, type, and sampling rate
    public void register(String name, String address, String type, int sampling) {
        try {
            pstRegister.setString(1, name);
            pstRegister.setString(2, address);
            pstRegister.setString(3, type);
            pstRegister.setInt(4, sampling);
            
            pstRegister.executeUpdate(); // Execute the update to register the device
        } catch (SQLException e) {
            e.printStackTrace(); // Print stack trace if a SQL exception occurs
        }
    }

    // Method to insert illumination measures (co2, light, phase) into the database
    public void insertIlluminationMeasures(int co2, int light, int phase/*, int timestamp*/) {
        try {
            pstInsertIlluminationMeasures.setInt(1, co2);
            pstInsertIlluminationMeasures.setInt(2, light);
            pstInsertIlluminationMeasures.setInt(3, phase);
            //pstInsertIlluminationMeasures.setInt(4, timestamp);
            
            pstInsertIlluminationMeasures.executeUpdate(); // Execute the update to insert the measures
        } catch (SQLException e) {
            e.printStackTrace(); // Print stack trace if a SQL exception occurs
        }
    }

    // Method to insert soil measures (moisture, temperature) into the database
    public void insertSoilMeasures(int moisture, int temperature /*, int timestamp*/) {
        try {
            pstInsertSprinklerMeasures.setInt(1, moisture);
            pstInsertSprinklerMeasures.setInt(2, temperature);
            //pstInsertSprinklerMeasures.setInt(3, timestamp);
            
            pstInsertSprinklerMeasures.executeUpdate(); // Execute the update to insert the measures
        } catch (SQLException e) {
            e.printStackTrace(); // Print stack trace if a SQL exception occurs
        }
    }
}

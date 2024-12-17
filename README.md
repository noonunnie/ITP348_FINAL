### **Postal Puppy Firmware**  
This firmware project was created using **Particle Developer Tools** and is compatible with all **Particle Devices**.  

---

### **Table of Contents**  
1. [Introduction](#introduction)  
2. [Prerequisites To Use This Repository](#prerequisites-to-use-this-repository)  
3. [Getting Started](#getting-started)  
4. [Postal Puppy Firmware Overview](#postal-puppy-firmware-overview)  
    - [Logging](#logging)  
    - [Setup and Loop](#setup-and-loop)  
    - [Delays and Timing](#delays-and-timing)  
5. [Testing and Debugging](#testing-and-debugging)  
6. [GitHub Actions (CI/CD)](#github-actions-cicd)  
7. [OTA Updates](#ota-updates)  
8. [Support and Feedback](#support-and-feedback)  
9. [Version](#version)  

---

### **Introduction**  
The **Postal Puppy** firmware powers a gift delivery system that assigns recipients, locks a gift box, and grants access through RFID scanning. This repository serves as the starting point for customizing or enhancing the firmware.  

---

### **Prerequisites To Use This Repository**  
To build and deploy the firmware, ensure you have:  
- A **Particle Device** (e.g., Photon, Argon, or Boron).  
- Windows/Mac/Linux for development and flashing the device.  
- Particle Developer Tools installed:  
   - [Particle CLI](https://docs.particle.io/tutorials/device-os/cli/)  
   - [Visual Studio Code with the Particle Workbench plugin](https://docs.particle.io/workbench/).  
- USB connection for flashing and debugging.  
- (Optional) A cup of tea ☕ for stress-free development.  

---

### **Getting Started**  
1. **Device Setup:**  
   - Run the Particle device setup process to ensure up-to-date firmware.  

2. **Open Project:**  
   - Open this repository in Visual Studio Code:  
     **File -> Open Folder**.  

3. **Build and Flash:**  
   - Connect the Particle device to your computer via USB.  
   - Compile and flash the firmware:  
     - **Visual Studio Code:** Run the "Particle: Cloud Flash" or "Particle: Local Flash" command.  
     - **CLI:** Execute:  
       ```bash
       particle flash <device_name> <firmware_file>
       ```  

4. **Monitor Logs:**  
   - View live logging output:  
     - **VS Code:** Command Palette → "Particle: Serial Monitor".  
     - **CLI:**  
       ```bash
       particle serial monitor --follow
       ```  

5. **Particle Cloud Integration:**  
   - Uncomment the relevant code in your `src` folder to enable event publishing.  
   - Monitor real-time events on [Particle Console](https://console.particle.io).  

---

### **Postal Puppy Firmware Overview**  

#### **Logging**  
The firmware includes a logging library for easy debugging. Example:  
```cpp
Log.info("Postal Puppy started");
Log.warn("RFID scan failed");
Log.error("System error detected");
```  
You can adjust log levels to filter messages for testing or deployment.  

---

#### **Setup and Loop**  
The firmware runs using standard Particle framework functions:  
- **setup():** Initializes components (OLED, RFID scanner, LEDs, etc.).  
- **loop():** Main logic for box locking, RFID validation, and access control.  

**Example Workflow:**  
1. Initialize components in `setup()`.  
2. Monitor button inputs and scan RFID cards in `loop()`.  
3. Update OLED messages and LEDs based on events.  

---

#### **Delays and Timing**  
Avoid using `delay()` for better performance. Instead, use non-blocking techniques like timers.  
**Example:**  
```cpp
unsigned long previousMillis = 0;
const long interval = 1000;

if (millis() - previousMillis >= interval) {
    previousMillis = millis();
    Log.info("1 second passed");
}
```  

---

### **Testing and Debugging**  
For testing:  
- Use the **Particle CLI** to monitor logs.  
- Flash in **SAFE MODE** to bypass faulty firmware.  
- Debug hardware connections (buttons, RFID, LEDs) for expected behavior.  

**Tools:**  
- Visual Studio Code Debugger  
- Particle CLI Serial Monitor  

---

### **GitHub Actions (CI/CD)**  
This repository includes a GitHub Actions YAML file that automates:  
- **Firmware compilation** upon each push to the repository.  
- Integration with Particle Cloud.  

You can customize the YAML file for additional build steps or notifications.  

---

### **OTA Updates**  
**Particle OTA (Over-the-Air)** enables remote firmware updates:  
- **Cloud Flash Command:**  
   ```bash
   particle flash <device_name>
   ```  
- Monitor the update process in Particle Console.  

**Advanced Features:**  
- Support for binary assets like images, configurations, or microcontroller firmware.  

---

### **Support and Feedback**  
Need help or want to provide feedback?  
- Join the [Particle Community](https://community.particle.io).  
- Explore Particle Documentation for advanced features.  

---

### **Version**  
**Postal Puppy Firmware** - Version **1.0.0**  

---  
Let the Postal Puppy deliver joy! 🐾✨  

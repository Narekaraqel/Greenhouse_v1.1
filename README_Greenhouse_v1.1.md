# 🌱 StemAr Greenhouse
**Control temperature, humidity, water, lights, and ventilation with ESP32**

---

## 📌 Overview /

This project is designed to **monitor and control an environmental system**, such as a greenhouse or plant incubator. It uses:

- ESP32 microcontroller
- DS18B20 temperature sensor
- Soil moisture sensors
- LCD I2C display (PCF8574)
- Rotary encoder for menu navigation
- Relays for pumps, heater, fan, and LED lights

**Key functions**:
- Automatic temperature control (heater and ventilation fan)
- Watering system for up to 3 pumps
- Soil moisture monitoring
- LED lighting schedule
- Menu interface using rotary encoder
- Factory reset and EEPROM storage for saved settings
- Visual feedback on LCD (temperature, humidity, pumps, fan, light)

---

## 🧰 Required Hardware

| Component | Description / Описание |
|-----------|------------------------|
| ESP32 | Microcontroller board |
| DS18B20 | Temperature sensor |
| Soil moisture sensors | For 3 soil sensors |
| PCF8574 I2C LCD 20x4 | Display |
| Rotary encoder with button | Menu navigation |
| Relays / MOSFETs | For pumps, heater, fan, LED lights |
| Power supply | Appropriate for pumps and relays |

---

## 🔌 Pin Configuration

| Function | Pin / Пин |
|----------|------------|
| Soil Moisture 1 | 36 |
| Soil Moisture 2 | 39 |
| Soil Moisture 3 | 34 |
| Water Low Sensor | 33 |
| DS18B20 Temp Sensor | 19 |
| Heater | 25 |
| LED Light | 26 |
| Pump 1 | 27 |
| Pump 2 | 14 |
| Pump 3 | 13 |
| Ventilation Fan | 18 |
| Encoder CLK | 16 |
| Encoder DT | 4 |
| Encoder SW | 17 |

---

## ⚙️ Features

1. **Temperature Control**
   - Heater turns ON when temperature < min value
   - Heater turns OFF when temperature > max value
   - Ventilation fan operates based on temperature and menu setting

2. **Soil Moisture & Watering**
   - Moisture levels read from 3 sensors
   - Pumps automatically turn ON if moisture < threshold
   - Watering schedule based on menu settings (interval and duration)

3. **LED Light**
   - LED lights turn ON/OFF based on menu-defined hours
   - Adjustable lighting schedule

4. **LCD Display**
   - Shows temperature, humidity, fan, and light status
   - Animated fan icon for ventilation visualization
   - Water indicator if tank is low

5. **Menu & Settings**
   - Navigate menu using encoder
   - Edit parameters: temperature min/max, watering duration, soil humidity threshold, LED hours, fan ON/OFF, pump ON/OFF
   - Save settings in EEPROM
   - Factory reset option

---

## 💾 EEPROM Storage

- The system stores menu values in EEPROM
- Supports persistent settings after reboot
- Factory reset clears EEPROM and restarts the system

---

## 🔧 Software

- Arduino IDE or PlatformIO
- Libraries required:
  - OneWire
  - DallasTemperature
  - LiquidCrystal_PCF8574
  - ESP32Encoder
  - EEPROM

---

## 🖥️ Menu Overview / Меню

| Item | Description / Описание |
|------|-----------------------|
| Temp min | Minimum temperature (°C) |
| Temp max | Maximum temperature (°C) |
| Watering | Watering interval (hours) |
| Watering | Pump ON duration (seconds) |
| Humidity | Soil moisture threshold (%) |
| Light | LED light duration (hours) |
| Fan | Ventilation ON/OFF |
| 1:Pump | Pump 1 ON/OFF |
| 2:Pump | Pump 2 ON/OFF |
| 3:Pump | Pump 3 ON/OFF |
| Pump:run | Manual run for all pumps |
| Reset | Factory reset |

---

## 📟 LCD Icons

- Thermometer: Temperature
- Droplet: Soil moisture
- Fan animation: Ventilation status
- Light icon: LED lights

---

## ⚡ Usage

1. Upload the code to ESP32 using Arduino IDE
2. Connect sensors, relays, and LCD
3. Turn on the power supply
4. The system will start in normal mode, showing environmental data on LCD
5. Press encoder button to enter **menu mode**, rotate to select parameters, press again to edit
6. Save settings automatically in EEPROM

---

## 📝 Notes

- Calibrate **soil moisture sensors**: set `dryValue` and `wetValue` according to your soil
- LED_ON_TIME and LED_OFF_TIME are calculated automatically from menu hours
- Watering OFF/ON intervals are based on menu values
- If water level sensor detects low water, pumps will not activate

---

## 🔧 Troubleshooting

- **No temperature reading**: check DS18B20 wiring, replace sensor if disconnected
- **Pumps not turning on**: check water level, relays, and moisture threshold
- **LED timing incorrect**: adjust hours in menu
- **EEPROM reset**: choose menu item **Reset**, system will restart


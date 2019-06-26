/*
 * 
 *  PCF8563Reader - v1.0
 *  Arduino sketch built to read the date and time on a PCF8563 without changing any data.
 *  Code by Gavin Tryzbiak
 *  
 */

/*
 * 
 *  IMPORTANT:
 *  - The Serial Monitor MUST match "#define BAUD_RATE 115200"
 *    from line 33 in the dropdown menu of the Serial
 *    Monitor! By default it is 115200, but it can be set
 *    lower if your cable has a poor connection, or higher
 *    for quicker text output.
 *     
 *  NOTE:
 *  - You may need to change your register! Do this by
 *    changing "#define I2C_ADDRESS 0x51" from line 34 to whatever
 *    register your chip uses. This sketch does not support
 *    the serparate r/w address version of the chip.
 *  - You may think your chip has separate addresses, but
 *    use an "I2CScanner" sketch to check, as it may
 *    actually be a single-address chip.
 *    
 */

// Libraries
#include <Wire.h>

// Constants
#define BAUD_RATE 115200 // Match this with the number in the dropdown menu in the Serial Monitor
#define I2C_ADDRESS 0x51 // See "NOTE"

#define SECONDS_REGISTER 0x02 // Also includes voltage status in MSB
#define MINUTES_REGISTER 0x03
#define HOURS_REGISTER 0x04
#define DAYS_REGISTER 0x05
#define WEEKDAYS_REGISTER 0x06
#define MONTHS_REGISTER 0x07 // Also includes century in MSB
#define YEARS_REGISTER 0x08

void setup()
{
  Serial.begin(BAUD_RATE);
  Wire.begin();
}

void loop()
{
  displayDateTime();
  Serial.println(F("Enter any character to check the time again."));
  delay(1000);
  while(!Serial.available()); // While nothing is entered, simply wait
  delay(100); // Allow time for all bytes to come in
  while(Serial.available()) // Remove everything from the input
    Serial.read();
}

byte DecToBCD(byte value) // Decimal to Binary Coded Decimal
{
  return ( (value / 10 * 16) + (value % 10) );
}

byte BCDToDec(byte value) // Binary Coded Decimal to Decimal
{
  return ( (value / 16 * 10) + (value % 16) );
}

byte * readFromAddress(byte I2CAddress, word beginningAddress, byte byteCount) // Reads data from I2C registers. Note: variable pointing to this function should eventually be freed!
{
  byte * data = malloc(sizeof(byte)*byteCount);
  byte tempData;
  Wire.beginTransmission(I2CAddress);
  Wire.write(beginningAddress);
  Wire.endTransmission();
  Wire.requestFrom(I2CAddress, byteCount);
  for (byte i = 0; i < byteCount; i++)
    data[i] = Wire.read();
  return data;
}

void displayDateTime() // Displays the current date and time on the PCF8563
{
  byte * data = readFromAddress(I2C_ADDRESS, SECONDS_REGISTER, 7);
  Serial.print(F("\nCurrently, the PCF8563 at address 0x"));
  Serial.print(I2C_ADDRESS, HEX);
  Serial.println(F(" reads..."));

  // Print date
  Serial.print(F("Date: "));
  Serial.print(weekdayFromValue(BCDToDec(data[4] & 0b00000111)));
  Serial.print(F(" ("));
  Serial.print(BCDToDec(data[4] & 0b00000111));
  Serial.print(F("), "));
  Serial.print(monthFromValue(BCDToDec(data[5] & 0b00011111)));
  Serial.print(F(" ("));
  Serial.print(BCDToDec(data[5] & 0b00011111));
  Serial.print(F(") "));
  Serial.print(BCDToDec(data[3] & 0b00111111));
  Serial.print(F(", "));
  Serial.print((data[5] & 0b10000000) < 0b10000000 ? F("20") : F("21"));
  if (BCDToDec(data[6]) < 10)
    Serial.print('0');
  Serial.println(BCDToDec(data[6]));

  // Print time (24 hour format)
  Serial.print(F("Time: "));
  if (BCDToDec(data[2] & 0b00111111) < 10)
    Serial.print('0');
  Serial.print(BCDToDec(data[2] & 0b00111111));
  Serial.print(':');
  if (BCDToDec(data[1] & 0b01111111) < 10)
    Serial.print('0');
  Serial.print(BCDToDec(data[1] & 0b01111111));
  Serial.print(':');
  if (BCDToDec(data[0] & 0b01111111) < 10)
    Serial.print('0');
  Serial.print(BCDToDec(data[0] & 0b01111111));
  
  // Print time (12 hour format)
  Serial.print(F(" ("));
  if (BCDToDec(data[2] & 0b00111111) < 10 && BCDToDec(data[2] & 0b00111111) != 0)
    Serial.print('0');
  Serial.print(BCDToDec(data[2] & 0b00111111) % 12 == 0 ? 12 : (BCDToDec(data[2] & 0b00111111) % 12));
  Serial.print(':');
  if (BCDToDec(data[1] & 0b01111111) < 10)
    Serial.print('0');
  Serial.print(BCDToDec(data[1] & 0b01111111));
  Serial.print(':');
  if (BCDToDec(data[0] & 0b01111111) < 10)
    Serial.print('0');
  Serial.print(BCDToDec(data[0] & 0b01111111));
  Serial.print(BCDToDec(data[2] & 0b00111111) > 12 ? F("PM") : F("AM"));
  Serial.println(')');

  // Print battery status
  Serial.print(F("Battery: "));
  Serial.println((data[0] & 0b10000000) == 0 ? F("Healthy") : F("Low/Unstable"));
  
  free(data);
}

String weekdayFromValue(byte value) // Accepts a number representing a weekday, returns the name of the weekday
{
  switch(value)
  {
    case 0:
      return F("Sunday");
    case 1:
      return F("Monday");
    case 2:
      return F("Tuesday");
    case 3:
      return F("Wednesday");
    case 4:
      return F("Thursday");
    case 5:
      return F("Friday");
    case 6:
      return F("Saturday");
    default:
      return F("Error:UnknownWeekday");
  }
}

String monthFromValue(byte value) // Accepts a number representing a month, returns the name of the month
{
  switch(value)
  {
    case 1:
      return F("January");
    case 2:
      return F("February");
    case 3:
      return F("March");
    case 4:
      return F("April");
    case 5:
      return F("May");
    case 6:
      return F("June");
    case 7:
      return F("July");
    case 8:
      return F("August");
    case 9:
      return F("September");
    case 10:
      return F("October");
    case 11:
      return F("November");
    case 12:
      return F("December");
    default:
      return F("Error:UnknownMonth");
  }
}

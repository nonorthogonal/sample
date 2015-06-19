#include <Servo.h>
#include <Wire.h>
#include <string.h>

#undef int
#include <stdio.h>

Servo rightServo;  // create servo object to control a servo
Servo leftServo;   // create servo object to control a servo

uint8_t outbuf[6];        // array to store arduino output
int cnt = 0;
int ledPin = 13;
int degX = 90;
int degY = 90;
int nutral = 90;

void
setup ()
{
  rightServo.attach(9);  // attaches the servo on pin 9 to the servo object
  leftServo.attach(10);  // attaches the servo on pin 10 to the servo object
  rightServo.write(90);                 // sets the servo position according to the scaled value
  leftServo.write(90);                  // sets the servo position according to the scaled value
  Serial.begin (19200);
//  Serial.print ("Finished setup\n");
  Wire.begin ();        // join i2c bus with address 0x52
  nunchuck_init (); // send the initilization handshake
}

void
nunchuck_init ()
{
  Wire.beginTransmission (0x52);    // transmit to device 0x52
  Wire.send (0x40);        // sends memory address
  Wire.send (0x00);        // sends sent a zero. 
  Wire.endTransmission ();    // stop transmitting
}

void
send_zero ()
{
  Wire.beginTransmission (0x52);    // transmit to device 0x52
  Wire.send (0x00);        // sends one byte
  Wire.endTransmission ();    // stop transmitting
}

void
loop ()
{
  Wire.requestFrom (0x52, 6);    // request data from nunchuck
  while (Wire.available ()) {
    outbuf[cnt] = nunchuk_decode_byte (Wire.receive ());    // receive byte as an integer
    cnt++;
  }

  // If we recieved the 6 bytes, then go print them
  if (cnt >= 5) {
    print ();
  }

  cnt = 0;
  send_zero (); // send the request for next bytes
  delay (50);
}

// Print the input data we have recieved
// accel data is 10 bits long
// so we read 8 bits, then we have to add
// on the last 2 bits.  That is why I
// multiply them by 2 * 2
void
print ()
{
  int joy_x_axis = outbuf[0];
  int joy_y_axis = outbuf[1];
  int accel_x_axis = outbuf[2];
  int accel_y_axis = outbuf[3];
  int accel_z_axis = outbuf[4];

  int z_button = 0;
  int c_button = 0;

  // byte outbuf[5] contains bits for z and c buttons
  // it also contains the least significant bits for the accelerometer data
  // so we have to check each bit of byte outbuf[5]
  if ((outbuf[5] >> 0) & 1) z_button = 1;
  if ((outbuf[5] >> 1) & 1) c_button = 1;

  if ((outbuf[5] >> 2) & 1) accel_x_axis += 2;
  if ((outbuf[5] >> 3) & 1) accel_x_axis += 1;

  if ((outbuf[5] >> 4) & 1) accel_y_axis += 2;
  if ((outbuf[5] >> 5) & 1) accel_y_axis += 1;

  if ((outbuf[5] >> 6) & 1) accel_z_axis += 2;
  if ((outbuf[5] >> 7) & 1) accel_z_axis += 1;

 //For testing.
/*  Serial.print (accel_x_axis, DEC);
 
  Serial.print (accel_y_axis, DEC);

  Serial.print (accel_z_axis, DEC);*/

 // Serial.print (z_button, DEC);

//  Serial.print (c_button, DEC);

 
  if(z_button == 0 && c_button == 0)
  {
    nutral = 90;
  }else if(z_button == 0 && nutral > 40)
  {
    nutral--;
  }else if(c_button == 0 && nutral < 140)
  {
    nutral++;
  }
  Serial.print("R");
  if(nutral <= 99){
    Serial.print("a");
  }
  Serial.print(nutral);
 
  degX = map(joy_x_axis, 25, 224, 140, 40);
  degY = map(joy_y_axis, 28, 219, 140, 40);
  rightServo.write(360 - nutral - degY - degX);                 // sets the servo position according to the scaled value
  leftServo.write(nutral + degY - degX);                  // sets the servo position according to the scaled value 
 
}

// Encode data to format that most wiimote drivers except
// only needed if you use one of the regular wiimote drivers
char
nunchuk_decode_byte (char x)
{
  x = (x ^ 0x17) + 0x17;
  return x;
}


//
//   FILE:  temperature.h
// PURPOSE: convert data from DS18B20 into a temperature value.
//

struct Rational {
  int sign;
  int whole;
  int fract;
};

Rational getTemp(byte data[12])
{
  int low = data[0];
  int high = data[1];
  int treading = (high << 8) + low;
  Rational temp;
  temp.sign = treading & 0x8000; //most significant bit
  if (temp.sign) // negative
  {
    treading = (treading^0xffff) + 1; // two's compliment
  }
  //int c = (treading*100/2);
  int c = (6*treading)+treading/4;

  temp.whole = c / 100;
  temp.fract = c % 100;
  return temp;
}

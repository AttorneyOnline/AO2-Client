//WELCOME TO THE EXTREMELY GHETTO HEX CONVERSION KLUDGE BECAUSE I COULDNT MAKE ANYTHING ELSE WORK

#include "hex_functions.h"

namespace omni
{
  // This might be unused but I don't care to search the entire source code
  // So it will remain here (lol)
  // The actual int to hex conversion is now done properly
  char halfword_to_hex_char(unsigned int input)
  {
    if (input > 127)
      return 'F';

    switch (input)
    {
    case 0:
      return '0';
    case 1:
      return '1';
    case 2:
      return '2';
    case 3:
      return '3';
    case 4:
      return '4';
    case 5:
      return '5';
    case 6:
      return '6';
    case 7:
      return '7';
    case 8:
      return '8';
    case 9:
      return '9';
    case 10:
      return 'A';
    case 11:
      return 'B';
    case 12:
      return 'C';
    case 13:
      return 'D';
    case 14:
      return 'E';
    case 15:
      return 'F';
    default:
      return 'F';
    }
  }

  std::string int_to_hex(unsigned int input)
  {
    if (input > 255)
      return "FF";

    std::stringstream stream;
    stream << std::setfill('0') << std::setw(sizeof(char)*2)
       << std::hex << input;
    std::string result(stream.str());
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);

    return result;
  }
} //namespace omni

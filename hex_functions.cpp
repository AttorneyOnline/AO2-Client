//WELCOME TO THE EXTREME GHETTO HEX CONVERSION KLUDGE BECAUSE I COULDNT MAKE ANYTHING ELSE WORK

#include "hex_operations.h"

namespace omni
{
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

    std::bitset<8> whole_byte(input);
    //240 represents 11110000, our needed bitmask
    uint8_t left_mask_int = 240;
    std::bitset<8> left_mask(left_mask_int);
    std::bitset<8> left_halfword((whole_byte & left_mask) >> 4);
    //likewise, 15 represents 00001111
    uint8_t right_mask_int = 15;
    std::bitset<8> right_mask(right_mask_int);
    std::bitset<8> right_halfword((whole_byte & right_mask));

    unsigned int left = left_halfword.to_ulong();
    unsigned int right = right_halfword.to_ulong();

    //std::cout << "now have have " << left << " and " << right << '\n';

    char a = halfword_to_hex_char(left);
    char b = halfword_to_hex_char(right);

    std::string left_string(1, a);
    std::string right_string(1, b);

    std::string final_byte = left_string + right_string;

    //std::string final_byte = halfword_to_hex_char(left) + "" + halfword_to_hex_char(right);

    return final_byte;
  }
} //namespace omni

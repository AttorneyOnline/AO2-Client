#include "hex_functions.h"

namespace omni
{
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
}

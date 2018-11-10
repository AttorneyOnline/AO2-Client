#include "encryption_functions.h"

#include "hex_functions.h"

QString fanta_encrypt(QString temp_input, unsigned int p_key)
{
  //using standard stdlib types is actually easier here because of implicit char<->int conversion
  //which in turn makes encryption arithmetic easier

  unsigned int key = p_key;
  unsigned int C1 = 53761;
  unsigned int C2 = 32618;

  QVector<uint_fast8_t> temp_result;
  std::string input = temp_input.toUtf8().constData();

  for (unsigned int pos = 0 ; pos < input.size() ; ++pos)
  {
    uint_fast8_t output = input.at(pos) ^ (key >> 8) % 256;
    temp_result.append(output);
    key = (temp_result.at(pos) + key) * C1 + C2;
  }

  std::string result = "";

  for (uint_fast8_t i_int : temp_result)
  {
    result += omni::int_to_hex(i_int);
  }

  QString final_result = QString::fromStdString(result);

  return final_result;
}

QString fanta_decrypt(QString temp_input, unsigned int key)
{
  std::string input = temp_input.toUtf8().constData();

  QVector<unsigned int> unhexed_vector;

  for(unsigned int i=0; i< input.length(); i+=2)
  {
    std::string byte = input.substr(i,2);
    unsigned int hex_int = strtoul(byte.c_str(), nullptr, 16);
    unhexed_vector.append(hex_int);
  }

  unsigned int C1 = 53761;
  unsigned int C2 = 32618;

  std::string result = "";

  for (int pos = 0 ; pos < unhexed_vector.size() ; ++pos)
  {
    unsigned char output = unhexed_vector.at(pos) ^ (key >> 8) % 256;
    result += output;
    key = (unhexed_vector.at(pos) + key) * C1 + C2;
  }

  return QString::fromStdString(result);

}

#include <QTextStream>
#include <QStringList>

#include "path_functions.h"

#include "text_file_functions.h"

QString get_user_theme(){
  QFile config_file(get_base_path() + "config.ini");
  if (!config_file.open(QIODevice::ReadOnly))
      return "default";

  QTextStream in(&config_file);

  while(!in.atEnd())
  {
    QString line = in.readLine();

    if (line.startsWith("theme"))
    {
      QStringList line_elements = line.split("=");

      if (line_elements.size() > 1)
        return line_elements.at(1).trimmed();
    }
  }

  return "default";
}

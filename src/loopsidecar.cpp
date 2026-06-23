#include "loopsidecar.h"

#include <QStringList>

LoopPoints parseLoopSidecarText(const QString &sidecar_text, const std::function<int()> &sample_rate_provider)
{
  LoopPoints out;
  bool seconds_mode = false;
  int sample_rate = 0;
  constexpr int sample_size = 2; // 16-bit
  constexpr int num_channels = 2;

  const QStringList lines = sidecar_text.split("\n");
  for (const QString &line : lines)
  {
    QStringList args = line.split("=");
    if (args.size() < 2)
    {
      continue;
    }
    QString arg = args[0].trimmed();
    QString val = args[1].trimmed();

    if (arg == "seconds")
    {
      seconds_mode = (val == "true");
      continue;
    }

    qint64 ms = 0;
    if (seconds_mode)
    {
      ms = static_cast<qint64>(val.toDouble() * 1000.0);
    }
    else
    {
      if (sample_rate == 0)
      {
        sample_rate = sample_rate_provider ? sample_rate_provider() : 0;
        if (sample_rate == 0)
        {
          continue;
        }
      }
      quint64 bytes = static_cast<quint64>(val.toUInt()) * sample_size * num_channels;
      quint64 frame_bytes = static_cast<quint64>(sample_rate) * sample_size * num_channels;
      ms = static_cast<qint64>(bytes * 1000 / frame_bytes);
    }

    if (arg == "loop_start")
    {
      out.start_ms = ms;
    }
    else if (arg == "loop_length")
    {
      out.end_ms = out.start_ms + ms;
    }
    else if (arg == "loop_end")
    {
      out.end_ms = ms;
    }
  }
  return out;
}

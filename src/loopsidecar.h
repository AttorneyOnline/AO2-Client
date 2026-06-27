#pragma once

#include <QString>

#include <functional>

struct LoopPoints
{
  qint64 start_ms = 0;
  qint64 end_ms = 0;
};

// Parse loop-point metadata from an AO music sidecar (the `.txt` that lives
// next to a music file). Recognises `seconds=(true|false)`, `loop_start=N`,
// `loop_end=N`, `loop_length=N`.
//
// In the legacy form (`seconds=false` or absent) N is a sample count and
// must be divided by the file's sample rate to get a ms timestamp.
// `sample_rate_provider` is invoked lazily — at most once, only if the
// sidecar actually contains a non-seconds entry — and should return the
// file's sample rate, or 0 to signal probe failure. When 0, legacy entries
// are silently ignored.
LoopPoints parseLoopSidecarText(const QString &sidecar_text, const std::function<int()> &sample_rate_provider);

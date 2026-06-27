#include "loopsidecar.h"

#include <QtTest/QTest>

class test_LoopSidecar : public QObject
{
  Q_OBJECT

private:
  // 44100 Hz, 16-bit stereo: 1 second = 44100 samples = 176400 bytes.
  // The parser's legacy form takes sample counts, so a value of 44100 → 1000 ms.
  static constexpr int SAMPLE_RATE = 44100;

  static std::function<int()> fixedRate(int rate)
  {
    return [rate]() {
      return rate;
    };
  }

private Q_SLOTS:
  void emptyInput()
  {
    LoopPoints lp = parseLoopSidecarText("", fixedRate(SAMPLE_RATE));
    QCOMPARE(lp.start_ms, qint64(0));
    QCOMPARE(lp.end_ms, qint64(0));
  }

  void onlyWhitespace()
  {
    LoopPoints lp = parseLoopSidecarText("\n\n   \n", fixedRate(SAMPLE_RATE));
    QCOMPARE(lp.start_ms, qint64(0));
    QCOMPARE(lp.end_ms, qint64(0));
  }

  void secondsFormStartEnd()
  {
    LoopPoints lp = parseLoopSidecarText("seconds=true\nloop_start=2.5\nloop_end=10", fixedRate(SAMPLE_RATE));
    QCOMPARE(lp.start_ms, qint64(2500));
    QCOMPARE(lp.end_ms, qint64(10000));
  }

  void secondsFormStartLength()
  {
    LoopPoints lp = parseLoopSidecarText("seconds=true\nloop_start=2.5\nloop_length=7.5", fixedRate(SAMPLE_RATE));
    QCOMPARE(lp.start_ms, qint64(2500));
    QCOMPARE(lp.end_ms, qint64(10000));
  }

  void legacyFormStartEnd()
  {
    // 44100 samples = 1 second at 44.1 kHz; 88200 samples = 2 seconds.
    LoopPoints lp = parseLoopSidecarText("loop_start=44100\nloop_end=88200", fixedRate(SAMPLE_RATE));
    QCOMPARE(lp.start_ms, qint64(1000));
    QCOMPARE(lp.end_ms, qint64(2000));
  }

  void legacyFormStartLength()
  {
    LoopPoints lp = parseLoopSidecarText("loop_start=22050\nloop_length=22050", fixedRate(SAMPLE_RATE));
    QCOMPARE(lp.start_ms, qint64(500));
    QCOMPARE(lp.end_ms, qint64(1000));
  }

  void explicitSecondsFalseIsLegacy()
  {
    LoopPoints lp = parseLoopSidecarText("seconds=false\nloop_start=44100\nloop_end=88200", fixedRate(SAMPLE_RATE));
    QCOMPARE(lp.start_ms, qint64(1000));
    QCOMPARE(lp.end_ms, qint64(2000));
  }

  void secondsFormSkipsProbe()
  {
    int probeCalls = 0;
    auto provider = [&]() {
      ++probeCalls;
      return SAMPLE_RATE;
    };
    LoopPoints lp = parseLoopSidecarText("seconds=true\nloop_start=1\nloop_end=2", provider);
    QCOMPARE(lp.start_ms, qint64(1000));
    QCOMPARE(lp.end_ms, qint64(2000));
    QCOMPARE(probeCalls, 0);
  }

  void legacyFormProbesAtMostOnce()
  {
    int probeCalls = 0;
    auto provider = [&]() {
      ++probeCalls;
      return SAMPLE_RATE;
    };
    LoopPoints lp = parseLoopSidecarText("loop_start=44100\nloop_length=44100\nloop_end=132300", provider);
    QCOMPARE(lp.start_ms, qint64(1000));
    QCOMPARE(lp.end_ms, qint64(3000)); // loop_end overrides the loop_length computation
    QCOMPARE(probeCalls, 1);
  }

  void probeFailureSilencesLegacyEntries()
  {
    LoopPoints lp = parseLoopSidecarText("loop_start=44100\nloop_end=88200", fixedRate(0));
    QCOMPARE(lp.start_ms, qint64(0));
    QCOMPARE(lp.end_ms, qint64(0));
  }

  void nullProviderTreatedAsFailure()
  {
    LoopPoints lp = parseLoopSidecarText("loop_start=44100\nloop_end=88200", nullptr);
    QCOMPARE(lp.start_ms, qint64(0));
    QCOMPARE(lp.end_ms, qint64(0));
  }

  void malformedLinesSkipped()
  {
    QString input = "this is not a kv line\n"
                    "seconds=true\n"
                    "loop_start=2.5\n"
                    "garbage_without_equals\n"
                    "loop_end=10\n";
    LoopPoints lp = parseLoopSidecarText(input, fixedRate(SAMPLE_RATE));
    QCOMPARE(lp.start_ms, qint64(2500));
    QCOMPARE(lp.end_ms, qint64(10000));
  }

  void unknownKeysIgnored()
  {
    QString input = "seconds=true\n"
                    "loop_start=2.5\n"
                    "something_else=42\n"
                    "loop_end=10\n";
    LoopPoints lp = parseLoopSidecarText(input, fixedRate(SAMPLE_RATE));
    QCOMPARE(lp.start_ms, qint64(2500));
    QCOMPARE(lp.end_ms, qint64(10000));
  }

  void whitespaceAroundKvIsTrimmed()
  {
    QString input = "  seconds = true \n"
                    " loop_start = 2.5\n"
                    "loop_end=  10 \n";
    LoopPoints lp = parseLoopSidecarText(input, fixedRate(SAMPLE_RATE));
    QCOMPARE(lp.start_ms, qint64(2500));
    QCOMPARE(lp.end_ms, qint64(10000));
  }

  void lastLoopStartWins()
  {
    QString input = "seconds=true\n"
                    "loop_start=1\n"
                    "loop_start=5\n"
                    "loop_end=10\n";
    LoopPoints lp = parseLoopSidecarText(input, fixedRate(SAMPLE_RATE));
    QCOMPARE(lp.start_ms, qint64(5000));
    QCOMPARE(lp.end_ms, qint64(10000));
  }

  void secondsToggleMidStreamApplies()
  {
    // Mode change mid-file: loop_start uses legacy (sample count at 44.1k),
    // then `seconds=true` flips the mode, and loop_end uses seconds.
    QString input = "loop_start=44100\n"
                    "seconds=true\n"
                    "loop_end=10\n";
    LoopPoints lp = parseLoopSidecarText(input, fixedRate(SAMPLE_RATE));
    QCOMPARE(lp.start_ms, qint64(1000));
    QCOMPARE(lp.end_ms, qint64(10000));
  }
};

#include "test/test_loopsidecar.moc"

QTEST_APPLESS_MAIN(test_LoopSidecar)

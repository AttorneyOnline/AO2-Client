#include <catch2/catch.hpp>

#include <QStringList>

TEST_CASE("Sort case evidence numerically", "[case]") {
  // Parameters
  QStringList case_evidence = {"1", "10", "11", "2", "3", "4", "5", "6", "7", "8", "9"};
  QStringList case_evidence_sorted = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11"};

  // Sort
  std::sort(case_evidence.begin(), case_evidence.end(),
            [] (const QString &a, const QString &b) {
              return a.toInt() < b.toInt();
            });

  // Test
  REQUIRE(case_evidence == case_evidence_sorted);
}

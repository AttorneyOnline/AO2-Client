#include <QTest>

#include "datatypes.h"

using namespace AttorneyOnline;

namespace tests {
namespace unittests {

class tst_datatypes : public QObject {
  Q_OBJECT

private slots:
    void initTestCase()
    {
        QCOMPARE(true,true);
    }
};

} // namespace unittests
} // namespace tests

QTEST_APPLESS_MAIN(tests::unittests::tst_datatypes)

#include "tst_unittest_datatypes.moc"

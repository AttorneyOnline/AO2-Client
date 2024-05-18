#include "aopacket.h"

#include <QString>
#include <QtTest/QTest>

class test_AOPacket : public QObject
{
  Q_OBJECT

private:
  const QMap<QString, QString> SYMBOL_MAP{
      {"#", "<num>"},
      {"%", "<percent>"},
      {"$", "<dollar>"},
      {"&", "<and>"},
  };

private Q_SLOTS:
  void constructPacket_data()
  {
    QTest::addColumn<QString>("header");
    QTest::addColumn<QStringList>("content");
    QTest::addColumn<QString>("result");

    QTest::newRow("Basic Packet") << "CT" << QStringList{"MY_OOC_NAME", "/doc https://docs.google.com/document/d/123/edit"} << "CT#MY_OOC_NAME#/doc https://docs.google.com/document/d/123/edit#%";
  }

  void constructPacket()
  {
    QFETCH(QString, header);
    QFETCH(QStringList, content);
    QFETCH(QString, result);

    AOPacket packet(header, content);
    QVERIFY(packet.toString(true) == result);
  }

  void encodeDecodeData_data()
  {
    QTest::addColumn<QString>("what");
    QTest::addColumn<QString>("result");
    QTest::addColumn<bool>("encode");

    for (auto it = SYMBOL_MAP.begin(); it != SYMBOL_MAP.end(); ++it)
    {
      QTest::newRow(QString("Encode %1").arg(it.key()).toUtf8()) << it.key() << it.value() << true;
      QTest::newRow(QString("Decode %1").arg(it.value()).toUtf8()) << it.value() << it.key() << false;
    }
  }

  void encodeDecodeData()
  {
    QFETCH(QString, what);
    QFETCH(QString, result);
    QFETCH(bool, encode);

    if (encode)
    {
      QVERIFY(AOPacket::encode(what) == result);
    }
    else
    {
      QVERIFY(AOPacket::decode(what) == result);
    }
  }
};

#include "test/test_aopacket.moc"

QTEST_MAIN(test_AOPacket)

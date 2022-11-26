#include <QObject>
#include <QTest>

#include "datatypes.h"

using namespace AttorneyOnline;

namespace tests {
namespace unittests {

class tst_datatypes : public QObject {
  Q_OBJECT

private slots:
    void headerConversion_data();
    void headerConversion();

    void deskmodConversion_data();
    void deskmodConversion();

    void emoteModifierConversion_data();
    void emoteModifierConversion();

    void shoutModifierConversion_data();
    void shoutModifierConversion();

    void textColorConversion_data();
    void textColorConversion();

    void authenticationConversion_data();
    void authenticationConversion();

    void timerActionConversion_data();
    void timerActionConversion();
};

void tst_datatypes::headerConversion_data()
{
    QTest::addColumn<QString>("incoming_header");
    QTest::addColumn<DataTypes::HEADER>("expected_header");

    QTest::addRow("Invalid Header") << "DECRYPTOR" << DataTypes::HEADER::UNKNOWN;
    QTest::addRow("Misspelled Header") << "I D" << DataTypes::HEADER::UNKNOWN;
    QTest::addRow("Lowercase Header") << "id" << DataTypes::HEADER::ID;
    QTest::addRow("ID Header") << "ID" << DataTypes::HEADER::ID;
    QTest::addRow("PN Header") << "PN" << DataTypes::HEADER::PN;
    QTest::addRow("FL Header") << "FL" << DataTypes::HEADER::FL;
    QTest::addRow("ASS Header") << "ASS" << DataTypes::HEADER::ASS;
    QTest::addRow("SC Header") << "SC" << DataTypes::HEADER::SC;
    QTest::addRow("LE Header") << "LE" << DataTypes::HEADER::LE;
    QTest::addRow("SM Header") << "SM" << DataTypes::HEADER::SM;
    QTest::addRow("DONE Header") << "DONE" << DataTypes::HEADER::DONE;
    QTest::addRow("CHARSCHECK Header") << "CHARSCHECK" << DataTypes::HEADER::CHARSCHECK;
    QTest::addRow("PV Header") << "PV" << DataTypes::HEADER::PV;
    QTest::addRow("MS Header") << "MS" << DataTypes::HEADER::MS;
    QTest::addRow("BN Header") << "BN" << DataTypes::HEADER::BN;
    QTest::addRow("MC Header") << "MC" << DataTypes::HEADER::MC;
    QTest::addRow("HP Header") << "HP" << DataTypes::HEADER::HP;
    QTest::addRow("RT Header") << "RT" << DataTypes::HEADER::RT;
    QTest::addRow("SP Header") << "SP" << DataTypes::HEADER::SP;
    QTest::addRow("SD Header") << "SD" << DataTypes::HEADER::SD;
    QTest::addRow("CT Header") << "CT" << DataTypes::HEADER::CT;
    QTest::addRow("ARUP Header") << "ARUP" << DataTypes::HEADER::ARUP;
    QTest::addRow("FM Header") << "FM" << DataTypes::HEADER::FM;
    QTest::addRow("FA Header") << "FA" << DataTypes::HEADER::FA;
    QTest::addRow("CASEA Header") << "CASEA" << DataTypes::HEADER::CASEA;
    QTest::addRow("AUTH Header") << "AUTH" << DataTypes::HEADER::AUTH;
    QTest::addRow("ZZ Header") << "ZZ" << DataTypes::HEADER::ZZ;
    QTest::addRow("KK Header") << "KK" << DataTypes::HEADER::KK;
    QTest::addRow("KB Header") << "KB" << DataTypes::HEADER::KB;
    QTest::addRow("BD Header") << "BD" << DataTypes::HEADER::BD;
    QTest::addRow("BB Header") << "BB" << DataTypes::HEADER::BB;
    QTest::addRow("CHECK Header") << "CHECK" << DataTypes::HEADER::CHECK;
    QTest::addRow("ST Header") << "ST" << DataTypes::HEADER::ST;
    QTest::addRow("TI Header") << "TI" << DataTypes::HEADER::TI;
    QTest::addRow("JD Header") << "JD" << DataTypes::HEADER::JD;
}

void tst_datatypes::headerConversion()
{
    QFETCH(QString, incoming_header);
    QFETCH(DataTypes::HEADER, expected_header);

    QCOMPARE(expected_header, toDataType<DataTypes::HEADER>(incoming_header));
}

void tst_datatypes::deskmodConversion_data()
{
    QTest::addColumn<QString>("incoming_deskmod");
    QTest::addColumn<DataTypes::DESKMOD>("expected_deskmod");

    QTest::addRow("Chat") << "chat" << DataTypes::DESKMOD::CHAT;
    QTest::addRow("Chat Integer") << "-1" << DataTypes::DESKMOD::CHAT;
    QTest::addRow("Desk Hidden") << "0" << DataTypes::DESKMOD::HIDDEN;
    QTest::addRow("Desk Shown") << "1" << DataTypes::DESKMOD::SHOWN;
    QTest::addRow("Hidden Preanim") << "2" << DataTypes::DESKMOD::HIDEPREANIM;
    QTest::addRow("Shown Preanim") << "3" << DataTypes::DESKMOD::SHOWPREANIM;
    QTest::addRow("Hide Preanim Pair") << "4" << DataTypes::DESKMOD::HIDEPREANIM_PAIR;
    QTest::addRow("Show Preanim No Pair") << "5" << DataTypes::DESKMOD::SHOWPREANIM_NOPAIR;
    QTest::addRow("Invalid Deskmod") << "6" << DataTypes::DESKMOD(6);
}

void tst_datatypes::deskmodConversion()
{
    QFETCH(QString, incoming_deskmod);
    QFETCH(DataTypes::DESKMOD, expected_deskmod);

    QCOMPARE(expected_deskmod, toDataType<DataTypes::DESKMOD>(incoming_deskmod));
}

void tst_datatypes::emoteModifierConversion_data()
{
    QTest::addColumn<QString>("incoming_emoteMod");
    QTest::addColumn<DataTypes::EMOTE_MODIFIER>("expected_emoteMod");

    QTest::addRow("No modifier") << "0" << DataTypes::EMOTE_MODIFIER::NONE;
    QTest::addRow("Preanimation") << "1" << DataTypes::EMOTE_MODIFIER::PREANIM;
    QTest::addRow("Objection") << "2" << DataTypes::EMOTE_MODIFIER::OBJECTION;
    QTest::addRow("Invalid Value 3") << "3" << DataTypes::EMOTE_MODIFIER(3);
    QTest::addRow("Invalid Value 4") << "4" << DataTypes::EMOTE_MODIFIER(4);
    QTest::addRow("Zoom") << "5" << DataTypes::EMOTE_MODIFIER::ZOOM;
    QTest::addRow("Objection Zoom") << "6" << DataTypes::EMOTE_MODIFIER::OBJECT_ZOOM;
}

void tst_datatypes::emoteModifierConversion()
{
    QFETCH(QString, incoming_emoteMod);
    QFETCH(DataTypes::EMOTE_MODIFIER, expected_emoteMod);

    QCOMPARE(expected_emoteMod, toDataType<DataTypes::EMOTE_MODIFIER>(incoming_emoteMod));
}

void tst_datatypes::shoutModifierConversion_data()
{
    QTest::addColumn<QString>("incoming_shoutMod");
    QTest::addColumn<DataTypes::SHOUT_MODIFIER>("expected_shoutMod");

    QTest::addRow("No modifier") << "0" << DataTypes::SHOUT_MODIFIER::NOTHING;
    QTest::addRow("Hold it!") << "1" << DataTypes::SHOUT_MODIFIER::HOLD_IT;
    QTest::addRow("Objection!") << "2" << DataTypes::SHOUT_MODIFIER::OBJECTION;
    QTest::addRow("Take That!") << "3" << DataTypes::SHOUT_MODIFIER::TAKE_THAT;
    QTest::addRow("Custom!") << "4" << DataTypes::SHOUT_MODIFIER::CUSTOM;
}

void tst_datatypes::shoutModifierConversion()
{
    QFETCH(QString, incoming_shoutMod);
    QFETCH(DataTypes::SHOUT_MODIFIER, expected_shoutMod);

    QCOMPARE(expected_shoutMod, toDataType<DataTypes::SHOUT_MODIFIER>(incoming_shoutMod));
}

void tst_datatypes::textColorConversion_data()
{
    QTest::addColumn<QString>("incoming_textColor");
    QTest::addColumn<DataTypes::TEXT_COLOR>("expected_textColor");

    QTest::addRow("White Text") << "0" << DataTypes::TEXT_COLOR::WHITE;
    QTest::addRow("Green Text") << "1" << DataTypes::TEXT_COLOR::GREEN;
    QTest::addRow("Red Text") << "2" << DataTypes::TEXT_COLOR::RED;
    QTest::addRow("Orange Text") << "3" << DataTypes::TEXT_COLOR::ORANGE;
    QTest::addRow("Blue Text") << "4" << DataTypes::TEXT_COLOR::BLUE;
    QTest::addRow("Yellow Text") << "5" << DataTypes::TEXT_COLOR::YELLOW;
    QTest::addRow("Rainbow Text") << "6" << DataTypes::TEXT_COLOR::RAINBOW;
}

void tst_datatypes::textColorConversion()
{
    QFETCH(QString, incoming_textColor);
    QFETCH(DataTypes::TEXT_COLOR, expected_textColor);

    QCOMPARE(expected_textColor, toDataType<DataTypes::TEXT_COLOR>(incoming_textColor));
}

void tst_datatypes::authenticationConversion_data()
{
    QTest::addColumn<QString>("incoming_authenticationState");
    QTest::addColumn<DataTypes::AUTHENTICATION>("expected_authenticationState");

    QTest::addRow("Logout") << "-1" << DataTypes::AUTHENTICATION::LOGOUT;
    QTest::addRow("Authentication Failed") << "0" << DataTypes::AUTHENTICATION::LOGIN_FAIL;
    QTest::addRow("Authentication Success") << "1" << DataTypes::AUTHENTICATION::LOGIN_SUCCESS;
}

void tst_datatypes::authenticationConversion()
{
    QFETCH(QString, incoming_authenticationState);
    QFETCH(DataTypes::AUTHENTICATION, expected_authenticationState);

    QCOMPARE(expected_authenticationState, toDataType<DataTypes::AUTHENTICATION>(incoming_authenticationState));
}

void tst_datatypes::timerActionConversion_data()
{
    QTest::addColumn<QString>("incoming_timerAction");
    QTest::addColumn<DataTypes::TIMER_ACTION>("expected_timerAction");

    QTest::addRow("Start timer") << "0" << DataTypes::TIMER_ACTION::START;
    QTest::addRow("Pause timer") << "1" << DataTypes::TIMER_ACTION::PAUSE;
    QTest::addRow("Show timer") << "2" << DataTypes::TIMER_ACTION::SHOW;
    QTest::addRow("Hide timer") << "3" << DataTypes::TIMER_ACTION::HIDE;
}

void tst_datatypes::timerActionConversion()
{
    QFETCH(QString, incoming_timerAction);
    QFETCH(DataTypes::TIMER_ACTION, expected_timerAction);

    QCOMPARE(expected_timerAction, toDataType<DataTypes::TIMER_ACTION>(incoming_timerAction));
}

} // namespace unittests
} // namespace tests

QTEST_APPLESS_MAIN(tests::unittests::tst_datatypes)

#include "tst_unittest_datatypes.moc"
#include "moc_datatypes.cpp"

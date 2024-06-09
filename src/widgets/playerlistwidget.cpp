#include "playerlistwidget.h"
#include "qjsondocument.h"
#include <QJsonDocument>

PlayerListWidget::PlayerListWidget(QWidget *parent)
    : QTreeWidget(parent)
{}

void PlayerListWidget::handleNetworkCommand(QString json_body)
{
  QJsonParseError l_error;
  QJsonDocument l_doc = QJsonDocument::fromJson(json_body.toUtf8(), &l_error);
  if (l_error.error != QJsonParseError::NoError || !l_doc.isObject())
  {
    qDebug() << "Unable to parse network command due to parsing error or invalid structure." << Qt::endl << "Parsing error:" << l_error.errorString();
    return;
  }

  NetCommand l_command = QVariant::fromValue(l_doc["command"]).value<NetCommand>();

  switch (l_command)
  {
  case ADD:

    break;
  case UPDATE:
    break;
  case REMOVE:
    break;
  case CLEAR:
    break;
  default:
    break;
  }
}

#include "aobutton.h"

#include "options.h"

AOButton::AOButton(AOApplication *ao_app, QWidget *parent)
    : QPushButton(parent)
    , ao_app(ao_app)
{
  m_movie = new QMovie(this);

  connect(m_movie, &QMovie::frameChanged, this, [this] {
    this->setIcon(m_movie->currentPixmap().scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    this->setIconSize(QSize(this->width(), this->height()));
  });
}

AOButton::~AOButton()
{
  deleteMovie();
}

void AOButton::setImage(QString image_name)
{
  deleteMovie();

  QString file_path = ao_app->get_image(image_name, Options::getInstance().theme(), Options::getInstance().subTheme(), ao_app->default_theme, QString(), QString(), QString(), !Options::getInstance().animatedThemeEnabled());
  if (file_path.isEmpty())
  {
    setStyleSheet(QString());
    setIcon(QIcon());
  }
  else
  {
    setText(QString());
    setStyleSheet("QPushButton { background-color: transparent; border: 0px }");

    if (Options::getInstance().animatedThemeEnabled())
    {
      m_movie = new QMovie;
      m_movie->setFileName(file_path);

      connect(m_movie, &QMovie::frameChanged, this, &AOButton::handleNextFrame);

      m_movie->start();
    }
    else
    {
      updateIcon(QPixmap(file_path));
    }
  }
}

void AOButton::deleteMovie()
{
  if (m_movie)
  {
    disconnect(m_movie, &QMovie::frameChanged, this, &AOButton::handleNextFrame);

    m_movie->stop();
    m_movie->deleteLater();
    m_movie = nullptr;
  }
}

void AOButton::handleNextFrame()
{
  updateIcon(m_movie->currentPixmap());
}

void AOButton::updateIcon(QPixmap icon)
{
  const QSize current_size = size();
  setIcon(icon.scaled(current_size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
  setIconSize(current_size);
}

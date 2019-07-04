#ifndef AOROOMCONTROLS_H
#define AOROOMCONTROLS_H

#include "aoapplication.h"

#include <QProgressBar>
#include <QPushButton>
#include <QWidget>

class AORoomControls : public QWidget
 {
   Q_OBJECT
 public:
   explicit AORoomControls(QWidget *parent, AOApplication *p_ao_app);

 private:
   AOApplication *ao_app;

   QPushButton *ui_witness_testimony;
   QPushButton *ui_cross_examination;
   QPushButton *ui_not_guilty;
   QPushButton *ui_guilty;

   QProgressBar *ui_def_health;
   QPushButton *ui_def_plus;
   QPushButton *ui_def_minus;

   QProgressBar *ui_pro_health;
   QPushButton *ui_pro_plus;
   QPushButton *ui_pro_minus;

 signals:
   void requestHealthChange(HEALTH_TYPE type, int value);
   void wtce(WTCE_TYPE type);

 public slots:
   void setHealth(HEALTH_TYPE type, int value);
   void toggleJudgeControls(bool visible);

 private slots:
   void on_witnessTestimony_clicked();
   void on_crossExamination_clicked();
   void on_notGuilty_clicked();
   void on_guilty_clicked();

   void on_defPlus_clicked();
   void on_defMinus_clicked();
   void on_proPlus_clicked();
   void on_proMinus_clicked();
};

#endif // AOROOMCONTROLS_H

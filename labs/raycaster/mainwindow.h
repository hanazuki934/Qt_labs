#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QKeyEvent>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPaintEvent>
#include <QRadioButton>
#include <QButtonGroup>
#include <QVector>
#include <QWidget>
#include <optional>
#include <vector>
#include <QDebug>

#include "controller.h"

class MainWindow : public QWidget {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
      void keyPressEvent(QKeyEvent *event) override;

private:
    Controller controller_;

    enum class Mode { Polygons, Light };

    Mode mode_ = Mode::Polygons;
   // QPixmap canvas;
    QButtonGroup *ModeButtonGroup_ = nullptr;
    QRadioButton *PolygonsButton_ = nullptr;
    QRadioButton *LightButton_ = nullptr;
};

#endif // MAINWINDOW_H

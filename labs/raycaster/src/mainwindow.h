#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QRadioButton>
#include <QButtonGroup>
#include <QWidget>
#include <QVBoxLayout>

#include "controller.h"
#include "canva.h"

class MainWindow : public QWidget {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
      void keyPressEvent(QKeyEvent *event) override;

private:
    Controller controller_;

    enum class Mode { Polygons, Light, StaticLights };

    Mode mode_ = Mode::Polygons;
    Canva* canva_ = nullptr;
    QButtonGroup *ModeButtonGroup_ = nullptr;
    QRadioButton *PolygonsButton_ = nullptr;
    QRadioButton *LightButton_ = nullptr;
    QRadioButton *StaticLightsButton_ = nullptr;
    QVBoxLayout *layout = nullptr;
    QHBoxLayout *mode_layout = nullptr;

};

#endif // MAINWINDOW_H

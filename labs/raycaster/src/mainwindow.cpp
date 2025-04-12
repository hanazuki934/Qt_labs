#include "mainwindow.h"
#include <qevent.h>
#include <qnamespace.h>

MainWindow::MainWindow(QWidget *parent) : QWidget(parent), mode_layout(new QHBoxLayout()) {
    setWindowTitle("3d raycaster for 2d games");

    layout = new QVBoxLayout(this);


    PolygonsButton_ = new QRadioButton("Polygons Mode", this);
    LightButton_ = new QRadioButton("Light Mode", this);
    StaticLightsButton_ = new QRadioButton("Static lights Mode", this);

    ModeButtonGroup_ = new QButtonGroup(this);
    ModeButtonGroup_->addButton(PolygonsButton_, static_cast<int>(Mode::Polygons));
    ModeButtonGroup_->addButton(LightButton_, static_cast<int>(Mode::Light));
    ModeButtonGroup_->addButton(StaticLightsButton_, static_cast<int>(Mode::StaticLights));

    PolygonsButton_->setChecked(true);

    mode_layout->addWidget(PolygonsButton_);
    mode_layout->addWidget(LightButton_);
    mode_layout->addWidget(StaticLightsButton_);

    canva_ = new Canva(controller_, this);

    connect(ModeButtonGroup_, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked),
            [this](QAbstractButton *button) {
                if (button->text() == "Polygons Mode") {
                    mode_ = Mode::Polygons;
                }
                else if (button->text() == "Light Mode") {
                    mode_ = Mode::Light;
                }
                else if (button->text() == "Static lights Mode") {
                    mode_ = Mode::StaticLights;
                }
                canva_->SetMode(static_cast<int>(mode_));
                update();
            });

    layout->addLayout(mode_layout);
    layout->addWidget(canva_);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->modifiers() & Qt::ControlModifier && event->key() == Qt::Key_M) {
        if (mode_ == Mode::Polygons) {
            LightButton_->setChecked(true);
            PolygonsButton_->setChecked(false);
            mode_ = Mode::Light;
        } else if (mode_ == Mode::Light) {
            LightButton_->setChecked(false);
            StaticLightsButton_->setChecked(true);
            mode_ = Mode::StaticLights;
        } else {
            StaticLightsButton_->setChecked(false);
            PolygonsButton_->setChecked(true);
            mode_ = Mode::Polygons;
        }
        canva_->SetMode(static_cast<int>(mode_));
    }
    if (event->modifiers() & Qt::ControlModifier && event->key() == Qt::Key_Delete) {
        controller_.Clear();
    }
    if (event->modifiers() & Qt::ControlModifier && event->key() == Qt::Key_D) {
        if (mode_ == Mode::Polygons || mode_ == Mode::Light) {
            canva_->FinishPolygon();
            controller_.DeleteLastPolygon();
        }
        if (mode_ == Mode::StaticLights) {
            controller_.DeleteLastAdditionalLightSource();
        }
    }
    QWidget::keyPressEvent(event);
}

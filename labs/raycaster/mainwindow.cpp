#include "mainwindow.h"
#include <qevent.h>
#include <qnamespace.h>

MainWindow::MainWindow(QWidget *parent) : QWidget(parent) {
    setWindowTitle("3d raycaster for terraria");
    //resize(800, 600);

    layout = new QVBoxLayout(this);
    mode_layout = new QHBoxLayout();

    PolygonsButton_ = new QRadioButton("Polygons Mode", this);
    LightButton_ = new QRadioButton("Light Mode", this);

    ModeButtonGroup_ = new QButtonGroup(this);
    ModeButtonGroup_->addButton(PolygonsButton_, static_cast<int>(Mode::Polygons));
    ModeButtonGroup_->addButton(LightButton_, static_cast<int>(Mode::Light));

    PolygonsButton_->setChecked(true);

    mode_layout->addWidget(PolygonsButton_);
    mode_layout->addWidget(LightButton_);

    canva_ = new Canva(controller_, this);

    connect(ModeButtonGroup_, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked),
            [this](QAbstractButton *button) {
                if (button->text() == "Polygons Mode") {
                    mode_ = Mode::Polygons;
                } else {
                    mode_ = Mode::Light;
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
        } else {
            LightButton_->setChecked(false);
            PolygonsButton_->setChecked(true);
            mode_ = Mode::Polygons;
        }
        canva_->SetMode(static_cast<int>(mode_));
    }
    QWidget::keyPressEvent(event);
}

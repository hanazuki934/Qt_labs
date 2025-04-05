#include "mainwindow.h"
#include <qevent.h>
#include <qnamespace.h>

const std::vector<Polygon> &Controller::GetPolygons() const {
    return polygons_;
}

void Controller::AddPolygon(const Polygon &polygon) {
    polygons_.push_back(polygon);
}

void Controller::AddVertexToLastPolygon(const QPointF &new_vertex) {
    polygons_.back().AddVertex(new_vertex);
}

void Controller::UpdateLastPolygon(const QPointF &new_vertex) {
    polygons_.back().UpdateLastVertex(new_vertex);
}

QPointF Controller::GetLightSource() const {
    return light_source_;
}

void Controller::SetLightSource(const QPointF &light_source) {
    light_source_ = light_source;
}

MainWindow::MainWindow(QWidget *parent) : QWidget(parent) {
    setWindowTitle("3d raytracer for terraria");
    resize(800, 600);

    QVBoxLayout *layout = new QVBoxLayout(this);
    QHBoxLayout *mode_layout = new QHBoxLayout();

    PolygonsButton_ = new QRadioButton("Polygons Mode", this);
    LightButton_ = new QRadioButton("Light Mode", this);

    ModeButtonGroup_ = new QButtonGroup(this);
    ModeButtonGroup_->addButton(PolygonsButton_, static_cast<int>(Mode::Polygons));
    ModeButtonGroup_->addButton(LightButton_, static_cast<int>(Mode::Light));

    PolygonsButton_->setChecked(true);

    mode_layout->addWidget(PolygonsButton_);
    mode_layout->addWidget(LightButton_);

    /*  canvas = QPixmap(size());
      canvas.fill(Qt::white);*/

    connect(ModeButtonGroup_, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked),
            [this](QAbstractButton *button) {
                if (button->text() == "Polygons Mode") {
                    mode_ = Mode::Polygons;
                } else {
                    mode_ = Mode::Light;
                }
                //qDebug() << (mode_ == Mode::Light);
                update();
            });

    layout->addLayout(mode_layout);

    Polygon border;
    border.AddVertex(QPointF(0, 0));
    border.AddVertex(QPointF(width(), 0));
    border.AddVertex(QPointF(width(), height()));
    border.AddVertex(QPointF(0, height()));
    controller_.AddPolygon(border);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->modifiers() & Qt::ControlModifier && event->key() == Qt::Key_M) {
        if (mode_ == Mode::Polygons) {
            LightButton_->setChecked(false);
            PolygonsButton_->setChecked(true);
            mode_ = Mode::Light;
        } else {
            LightButton_->setChecked(true);
            PolygonsButton_->setChecked(false);
            mode_ = Mode::Polygons;
        }
    }
    QWidget::keyPressEvent(event);
}

#include "canva.h"
#include "controller.h"
#include "polygon.h"

#include <QColorDialog>
#include <QCursor>
#include <QGraphicsEllipseItem>
#include <QGraphicsView>
#include <QMessageBox>
#include <QMouseEvent>
#include <QTimer>
#include <QWidget>

Canva::Canva(Controller &controller, QWidget *parent)
    : QGraphicsView(parent), controller_(controller), colorDialog_(new QColorDialog()), messageBox_(new QMessageBox()) {
    scene_ = new QGraphicsScene(this);
    setScene(scene_);
    scene_->setBackgroundBrush(QBrush(Qt::gray));
    setRenderHint(QPainter::Antialiasing);
    setMinimumSize(800, 600);
    setSceneRect(0, 0, 800, 600);

    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, &Canva::renderImage);
    timer_->start(1);

    Polygon screen_polygon;
    screen_polygon.AddVertex(QPointF(0, 0));
    screen_polygon.AddVertex(QPointF(1000, 0));
    screen_polygon.AddVertex(QPointF(1000, 700));
    screen_polygon.AddVertex(QPointF(0, 700));
    controller_.AddPolygon(screen_polygon);

    drawPolygons();
    setMouseTracking(true);
}

void Canva::FinishPolygon() {
    if (isDrawingPolygon_) {
        const std::vector<QPointF> &vertices = controller_.GetPolygons().back().GetVertices();
        scene_->addLine(vertices.back().x(), vertices.back().y(),
                        vertices.front().x(), vertices.front().y(), QPen(Qt::darkRed));
        isDrawingPolygon_ = false;
        currentPolygonLines_.clear();
    }
}

void Canva::SetMode(int mode) {
    mode_ = static_cast<Mode>(mode);
    if (mode_ != Mode::Polygons) {
        if (isDrawingPolygon_) {
            FinishPolygon();
        }
    }
}

void Canva::mousePressEvent(QMouseEvent *event) {
    QPointF pos = mapToScene(event->pos());
    if (mode_ == Mode::Polygons) {
        if (event->button() == Qt::LeftButton) {
            if (!isDrawingPolygon_) {
                controller_.AddPolygon(Polygon({pos}));
                isDrawingPolygon_ = true;
            } else {
                const std::vector<QPointF> &vertices = controller_.GetPolygons().back().GetVertices();

                QGraphicsLineItem *line = scene_->addLine(
                    vertices.back().x(), vertices.back().y(),
                    pos.x(), pos.y(), QPen(Qt::darkRed));
                currentPolygonLines_.push_back(line);
                controller_.AddVertexToLastPolygon(pos);
            }
        } else if (event->button() == Qt::RightButton && isDrawingPolygon_) {
            FinishPolygon();
        }
    }
    if (mode_ == Mode::StaticLights && event->button() == Qt::LeftButton) {
        timer_->stop();
        const QColorDialog colorDialog;
        const QColor color = QColorDialog::getColor();
        controller_.AddAdditionalLightSource(pos, color);
        timer_->start(1);
    }
}

void Canva::drawPolygons() const {
    for (const Polygon &poly: controller_.GetPolygons()) {
        const std::vector<QPointF> &vertices = poly.GetVertices();
        if (vertices.size() > 1) {
            for (size_t i = 0; i < vertices.size() - 1; ++i) {
                scene_->addLine(vertices[i].x(), vertices[i].y(),
                                vertices[i + 1].x(), vertices[i + 1].y(), QPen(Qt::darkRed));
            }
            if (!isDrawingPolygon_ || &poly != &controller_.GetPolygons().back()) {
                scene_->addLine(vertices.back().x(), vertices.back().y(),
                                vertices.front().x(), vertices.front().y(), QPen(Qt::darkRed));
            }
        }
    }
}

void Canva::renderImage() {
    timer_->stop();
    scene_->clear();
    drawPolygons();
    QPointF cursor_pos = mapToScene(mapFromGlobal(QCursor::pos()));
    if (mode_ == Mode::Polygons) {
        for (int i = 0; i < controller_.GetAdditionalLightSource().size(); i++) {
            QColor color = controller_.GetAdditionalLightSource()[i].second;
            color.setAlpha(255);
            lightSourceItems_.push_back(scene_->addEllipse(0, 0, 5, 5, QPen(color),
                                                           QBrush(color)));
            lightSourceItems_.back()->setPos(controller_.GetAdditionalLightSource()[i].first);
            lightSourceItems_.back()->setVisible(true);
        }
    }
    if (mode_ == Mode::Light) {
        controller_.SetLightSource(cursor_pos);
        controller_.SetLightSources();

        Polygon light_area = controller_.CreateLightArea(controller_.GetLightSource());
        renderLightArea(light_area, QColor(139, 0, 0, 28));
        for (int i = 0; i < controller_.GetLightSources().size(); i++) {
            light_area = controller_.CreateLightArea(controller_.GetLightSources()[i]);
            renderLightArea(light_area, QColor(139, 0, 0, 28));
        }
        for (int i = 0; i < controller_.GetAdditionalLightSource().size(); i++) {
            QColor color = controller_.GetAdditionalLightSource()[i].second;
            color.setAlpha(28);
            light_area = controller_.CreateLightArea(controller_.GetAdditionalLightSource()[i].first);
            renderLightArea(light_area, color);
        }


        lightSourceItem_ = scene_->addEllipse(-5, -5, 3, 3, QPen(QColor(255, 255, 255, 120)),
                                              QBrush(QColor(255, 255, 255, 120)));
        lightSourceItem_->setPos(cursor_pos);
        lightSourceItem_->setVisible(true);

        for (int i = 0; i < controller_.GetLightSources().size(); i++) {
            lightSourceItems_.push_back(scene_->addEllipse(-5, -5, 3, 3, QPen(QColor(255, 255, 255, 120)),
                                                           QBrush(QColor(255, 255, 255, 120))));
            lightSourceItems_.back()->setPos(controller_.GetLightSources()[i]);
            lightSourceItems_.back()->setVisible(true);
        }

        for (int i = 0; i < controller_.GetAdditionalLightSource().size(); i++) {
            QColor color = controller_.GetAdditionalLightSource()[i].second;
            color.setAlpha(255);
            lightSourceItems_.push_back(scene_->addEllipse(0, 0, 5, 5, QPen(color),
                                                           QBrush(color)));
            lightSourceItems_.back()->setPos(controller_.GetAdditionalLightSource()[i].first);
            lightSourceItems_.back()->setVisible(true);
        }

    } else {
        if (isDrawingPolygon_) {
            QPointF last_vertex = controller_.GetPolygons().back().GetVertices().back();
            scene_->addLine(last_vertex.x(), last_vertex.y(), cursor_pos.x(), cursor_pos.y(),
                            QPen(QColor(0, 0, 0, 100)));
        }
    }
    if (mode_ == Mode::StaticLights) {
        for (int i = 0; i < controller_.GetAdditionalLightSource().size(); i++) {
            QColor color = controller_.GetAdditionalLightSource()[i].second;
            color.setAlpha(60);
            Polygon light_area = controller_.CreateLightArea(controller_.GetAdditionalLightSource()[i].first);
            renderLightArea(light_area, color);
        }

        for (int i = 0; i < controller_.GetAdditionalLightSource().size(); i++) {
            QColor color = controller_.GetAdditionalLightSource()[i].second;
            color.setAlpha(255);
            lightSourceItems_.push_back(scene_->addEllipse(0, 0, 5, 5, QPen(color),
                                                           QBrush(color)));
            lightSourceItems_.back()->setPos(controller_.GetAdditionalLightSource()[i].first);
            lightSourceItems_.back()->setVisible(true);
        }
    }
    timer_->start(1);
}

void Canva::renderLightArea(const Polygon &area, QColor color) {
    const std::vector<QPointF> &vertices = area.GetVertices();
    if (vertices.size() < 3) {
        return;
    }

    QPainterPath path;
    path.moveTo(vertices[0]);
    for (int i = 1; i < vertices.size(); ++i) {
        path.lineTo(vertices[i]);
    }

    path.closeSubpath();

    lightItem_ = new QGraphicsPathItem(path);
    lightItem_->setBrush(QBrush(color));
    lightItem_->setPen(QPen(color, 1));
    scene_->addItem(lightItem_);
}

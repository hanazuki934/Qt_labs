#include "canva.h"
#include <QGraphicsLineItem>
#include <QGraphicsEllipseItem>
#include <QCursor>

Canva::Canva(Controller &controller, QWidget *parent)
    : QGraphicsView(parent), controller_(controller) {
    scene_ = new QGraphicsScene(this);
    setScene(scene_);
    scene_->setBackgroundBrush(QBrush(Qt::gray));
    setRenderHint(QPainter::Antialiasing);
    setMinimumSize(800, 600);
    setSceneRect(0, 0, 800, 600);

    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, &Canva::renderImage);
    timer_->start(1);

    Polygon screenPolygon;
    screenPolygon.AddVertex(QPointF(0, 0));
    screenPolygon.AddVertex(QPointF(800, 0));
    screenPolygon.AddVertex(QPointF(800, 600));
    screenPolygon.AddVertex(QPointF(0, 600));
    controller_.AddPolygon(screenPolygon);

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
    if (mode_ == Mode::Light) {
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
    if (mode_ == Mode::Light) {
        QPointF cursorPos = mapToScene(mapFromGlobal(QCursor::pos()));
            controller_.SetLightSource(cursorPos);
            lightSourceItem_ = scene_->addEllipse(-5, -5, 10, 10, QPen(Qt::red), QBrush(Qt::red));
            lightSourceItem_->setPos(cursorPos);
            lightSourceItem_->setVisible(true);
            std::vector<Ray> rays = controller_.CastRays();
            controller_.IntersectRays(&rays);
            controller_.RemoveAdjacentRays(&rays);
            Polygon lightArea = controller_.CreateLightArea();
            renderLightArea(lightArea);
            for (auto ray: rays) {
                scene_->addLine(ray.GetBegin().x(), ray.GetBegin().y(), ray.GetEnd().x(), ray.GetEnd().y(), QPen(Qt::darkRed));
            }
    }
    timer_->start(1);
}

void Canva::renderLightArea(const Polygon& area) {
    const std::vector<QPointF>& vertices = area.GetVertices();
    if (vertices.size() < 3) {
        return;
    }

    QPainterPath path;
    path.moveTo(vertices[0]);
    for (size_t i = 1; i < vertices.size(); ++i) {
        path.lineTo(vertices[i]);
    }

    path.closeSubpath();

    QGraphicsPathItem* light_item = new QGraphicsPathItem(path);
    light_item->setBrush(QBrush(QColor(255, 255, 0, 100)));
    light_item->setPen(QPen(Qt::yellow, 1));
    scene_->addItem(light_item);
}

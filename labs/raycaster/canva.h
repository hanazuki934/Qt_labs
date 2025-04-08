#ifndef CANVA_H
#define CANVA_H

#include "controller.h"
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QTimer>

class Canva : public QGraphicsView {
    Q_OBJECT

public:
    explicit Canva(Controller &controller, QWidget *parent = nullptr);

    enum class Mode { Polygons, Light };

    void SetMode(int mode);

    bool IsDrawingPolygon() const { return isDrawingPolygon_; }

    void FinishPolygon();

    Mode mode() const { return mode_; }

protected:
    void mousePressEvent(QMouseEvent *event) override;
private slots:
    void renderImage();

private:
    Controller &controller_;
    QGraphicsScene *scene_{};
    Mode mode_ = Mode::Polygons;
    bool isDrawingPolygon_ = false;
    QTimer* timer_;
    std::vector<QGraphicsLineItem *> currentPolygonLines_{};
    QGraphicsEllipseItem *lightSourceItem_ = nullptr;

    void drawPolygons() const;

    void renderLightArea(const Polygon& area);

};

#endif // CANVA_H

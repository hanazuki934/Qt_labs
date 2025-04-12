#ifndef CANVA_H
#define CANVA_H

#include "controller.h"
#include "polygon.h"

#include <QGraphicsView>
#include <QMouseEvent>
#include <QTimer>
#include <QColorDialog>
#include <QMessageBox>

class Canva : public QGraphicsView {
    Q_OBJECT

public:
    explicit Canva(Controller &controller, QWidget *parent = nullptr);

    enum class Mode { Polygons, Light, StaticLights };

    void SetMode(int mode);

    [[nodiscard]] bool IsDrawingPolygon() const { return isDrawingPolygon_; }

    void FinishPolygon();

    [[nodiscard]] Mode mode() const { return mode_; }

protected:
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void renderImage();

private:
    Controller &controller_;
    QGraphicsScene *scene_{};
    Mode mode_ = Mode::Polygons;
    bool isDrawingPolygon_ = false;
    QTimer *timer_{};
    std::vector<QGraphicsLineItem *> currentPolygonLines_{};
    QGraphicsEllipseItem *lightSourceItem_ = nullptr;
    QGraphicsPathItem *lightItem_ = nullptr;
    std::vector<QGraphicsEllipseItem *> lightSourceItems_{};
    QColorDialog *colorDialog_ = nullptr;
    QMessageBox *messageBox_ = nullptr;

    void drawPolygons() const;

    void renderLightArea(const Polygon &area, QColor color);
};

#endif // CANVA_H

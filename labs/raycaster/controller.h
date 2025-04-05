#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "polygon.h"

class Controller {
public:
    [[nodiscard]] const std::vector<Polygon> &GetPolygons() const;

    void AddPolygon(const Polygon &);

    void AddVertexToLastPolygon(const QPointF &new_vertex);

    void UpdateLastPolygon(const QPointF &new_vertex);

    [[nodiscard]] QPointF GetLightSource() const;

    void SetLightSource(const QPointF &light_source);

private:
    std::vector<Polygon> polygons_ = {};
    QPointF light_source_ = QPointF(0, 0);
};

#endif //CONTROLLER_H

#ifndef POLYGON_H
#define POLYGON_H

#include "ray.h"

#include <QPointF>
#include <vector>

class Polygon {
public:
    explicit Polygon(const std::vector<QPointF> &vertices = {});

    [[nodiscard]] const std::vector<QPointF> &GetVertices() const;

    void AddVertex(const QPointF &vertex);

    void UpdateLastVertex(const QPointF &new_vertex);

    std::optional<QPointF> IntersectRay(const Ray &ray) const;

private:
    std::vector<QPointF> vertices_{};
};

#endif //POLYGON_H

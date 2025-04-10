#include "polygon.h"
#include <optional>

Polygon::Polygon(const std::vector<QPointF> &vertices) : vertices_(vertices) {
}

const std::vector<QPointF> &Polygon::GetVertices() const {
    return vertices_;
}

void Polygon::AddVertex(const QPointF &vertex) {
    vertices_.push_back(vertex);
}

void Polygon::UpdateLastVertex(const QPointF &new_vertex) {
    vertices_.back() = new_vertex;
}

std::optional<QPointF> Polygon::IntersectRay(const Ray& ray) const {
    std::optional<QPointF> nearest_intersection = std::nullopt;
    double min_distance_squared = std::numeric_limits<double>::max();
    const QPointF rayBegin = ray.GetBegin();
    const QPointF rayEnd = ray.GetEnd();
    const QPointF rayDir = rayEnd - rayBegin;

    for (size_t i = 0; i < vertices_.size(); ++i) {
        QPointF v1 = vertices_[i];
        QPointF v2 = (i + 1 == vertices_.size()) ? vertices_.front() : vertices_[i + 1];
        QPointF edge = v2 - v1;


        QPointF delta = v1 - rayBegin;

        double denom = rayDir.x() * edge.y() - rayDir.y() * edge.x();
        if (std::abs(denom) < 1e-10) {
            continue;
        }

        double u = (delta.x() * edge.y() - delta.y() * edge.x()) / denom;
        double t = (delta.x() * rayDir.y() - delta.y() * rayDir.x()) / denom;

        if (u >= 0 && t >= 0 && t <= 1) {
            QPointF intersection = rayBegin + u * rayDir;

            QPointF diff = intersection - rayBegin;
            double distance_squared = diff.x() * diff.x() + diff.y() * diff.y();

            if (distance_squared < min_distance_squared) {
                min_distance_squared = distance_squared;
                nearest_intersection = intersection;
            }
        }
    }

    return nearest_intersection;
}

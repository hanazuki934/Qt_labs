#include "controller.h"
#include "polygon.h"
#include "ray.h"

#include <QColor>
#include <QDebug>
#include <QPointF>
#include <QtMath>
#include <algorithm>

const std::vector<Polygon> &Controller::GetPolygons() {
    return polygons_;
}

void Controller::SetPolygon(const int i, const Polygon &polygon) {
    polygons_[i] = polygon;
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

void Controller::DeleteLastPolygon() {
    if (polygons_.size() == 1) {
        return;
    }
    polygons_.pop_back();
}

QPointF Controller::GetLightSource() const {
    return light_source_;
}

void Controller::SetLightSource(const QPointF &light_source) {
    light_source_ = light_source;
}

std::vector<QPointF> Controller::GetLightSources() const {
    return light_sources_;
}

void Controller::AddAdditionalLightSource(const QPointF &light_source, QColor color) {
    additional_sources_.push_back({light_source, color});
}

std::vector<std::pair<QPointF, QColor> > Controller::GetAdditionalLightSource() const {
    return additional_sources_;
}

void Controller::DeleteLastAdditionalLightSource() {
    if (additional_sources_.size() == 0) {
        return;
    }
    additional_sources_.pop_back();
}


void Controller::SetLightSources() {
    constexpr int kNumAdditionalSources = 10;
    constexpr double kAngleStep = 2 * M_PI / kNumAdditionalSources;

    light_sources_.clear();

    light_sources_.push_back(light_source_);

    for (int i = 0; i < kNumAdditionalSources; ++i) {
        constexpr double kRadius = 10.0;
        const double angle = i * kAngleStep;
        const double x = light_source_.x() + kRadius * std::cos(angle);
        const double y = light_source_.y() + kRadius * std::sin(angle);
        light_sources_.push_back(QPointF(x, y));
    }
}

void Controller::ClearPolygons() {
    polygons_.clear();
}

std::vector<Ray> Controller::CastRays(const QPointF light_source) {
    std::vector<Ray> rays = {};
    for (auto polygon: polygons_) {
        for (auto vertex: polygon.GetVertices()) {
            auto ray1 = Ray{light_source, vertex};
            Ray ray2 = ray1.Rotate(0.0001);
            Ray ray3 = ray1.Rotate(-0.0001);
            rays.push_back(ray1);
            rays.push_back(ray2);
            rays.push_back(ray3);
        }
    }
    return rays;
}

void Controller::Clear() {
    polygons_.clear();
    light_sources_.clear();
    additional_sources_.clear();
    light_source_ = QPointF();
}

void Controller::IntersectRays(std::vector<Ray> *rays) const {
    for (auto &ray: *rays) {
        std::vector<QPointF> intersections;
        for (const auto &polygon: polygons_) {
            auto intersection = polygon.IntersectRay(ray);
            if (intersection.has_value()) {
                intersections.push_back(intersection.value());
            }
        }

        QPointF new_end = ray.GetEnd();
        double min_distance = std::numeric_limits<double>::max();
        const QPointF ray_begin = ray.GetBegin();

        for (const auto &intersection: intersections) {
            double distance = Dst(ray_begin, intersection);
            if (distance < min_distance) {
                min_distance = distance;
                new_end = intersection;
            }
        }

        if (intersections.empty()) {
            constexpr double max_distance = 10000.0;
            const double angle = ray.GetAngle();
            new_end = QPointF(
                ray_begin.x() + max_distance * std::cos(angle),
                ray_begin.y() + max_distance * std::sin(angle)
            );
        }

        ray.SetEnd(new_end);
    }
}

void Controller::RemoveAdjacentRays(std::vector<Ray> *rays) const {
    if (!rays || rays->empty()) {
        return;
    }

    constexpr double kDistanceThreshold = 5.0;
    std::vector<Ray> filtered_rays;

    std::vector<bool> used(rays->size(), false);

    for (int i = 0; i < rays->size(); ++i) {
        if (used[i]) {
            continue;
        }

        filtered_rays.push_back((*rays)[i]);
        used[i] = true;

        for (int j = i + 1; j < rays->size(); ++j) {
            if (used[j]) {
                continue;
            }

            double dist = Dst((*rays)[i].GetEnd(), (*rays)[j].GetEnd());
            if (dist < kDistanceThreshold) {
                used[j] = true;
            }
        }
    }

    *rays = std::move(filtered_rays);
}

Polygon Controller::CreateLightArea(const QPointF light_source) {
    std::vector<Ray> rays = CastRays(light_source);
    IntersectRays(&rays);
    RemoveAdjacentRays(&rays);
    std::ranges::sort(rays, [](const Ray &ray1, const Ray &ray2) { return ray1.GetAngle() < ray2.GetAngle(); });
    std::vector<QPointF> vertices(0);
    for (auto ray: rays) {
        vertices.push_back(ray.GetEnd());
    }
    return Polygon(vertices);
}

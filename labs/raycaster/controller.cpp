#include "controller.h"
#include <algorithm>

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

void Controller::ClearPolygons() {
    polygons_.clear();
}

std::vector<Ray> Controller::CastRays() {
    std::vector<Ray> rays = {};
    for (auto polygon: polygons_) {
        for (auto vertex: polygon.GetVertices()) {
            Ray ray1 = Ray{light_source_, vertex};
            //Ray ray2 = ray1.Rotate(0.1);
            Ray ray2 = ray1.Rotate(0.0001);
            //Ray ray3 = ray1.Rotate(2 * M_PI - 0.1);
            Ray ray3 = ray1.Rotate(-0.0001);
            rays.push_back(ray1);
            rays.push_back(ray2);
            rays.push_back(ray3);
        }
    }
    return rays;
}

void Controller::IntersectRays(std::vector<Ray> *rays) {
    for (auto& ray: *rays) {
        std::vector<QPointF> vertices(0);
        for (auto polygon: polygons_) {
            auto intersection = polygon.IntersectRay(ray);
            if (intersection.has_value()) {
                vertices.push_back(intersection.value());
            }
        }
        QPointF new_end = ray.GetEnd();
        for (int i = 0; i < vertices.size(); i++) {
            if (Dst(light_source_, vertices[i]) <= Dst(light_source_, new_end)) {
                new_end = vertices[i];
            }
        }
        ray.SetEnd(new_end);
    }
}

void Controller::RemoveAdjacentRays(std::vector<Ray>* rays) const {
    if (!rays || rays->empty()) {
        return;
    }

    const double distance_threshold = 2.0;
    std::vector<Ray> filtered_rays;

    std::vector<bool> used(rays->size(), false);

    for (size_t i = 0; i < rays->size(); ++i) {
        if (used[i]) {
            continue;
        }

        filtered_rays.push_back((*rays)[i]);
        used[i] = true;

        for (size_t j = i + 1; j < rays->size(); ++j) {
            if (used[j]) {
                continue;
            }

            double dist = Dst((*rays)[i].GetEnd(), (*rays)[j].GetEnd());
            if (dist < distance_threshold) {
                used[j] = true;
            }
        }
    }

    *rays = std::move(filtered_rays);
}

Polygon Controller::CreateLightArea() {
    std::vector<Ray> rays = CastRays();
    std::sort(rays.begin(), rays.end(), [](const Ray &ray1, const Ray &ray2) {return ray1.GetAngle() < ray2.GetAngle();});
    std::vector<QPointF> vertices(0);
    for (auto ray: rays) {
        vertices.push_back(ray.GetEnd());
    }
    return Polygon(vertices);
}

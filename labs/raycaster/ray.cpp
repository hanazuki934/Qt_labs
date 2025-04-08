#include "ray.h"
#include <cmath>
#include <QPointF>

Ray::Ray(const QPointF &begin, const QPointF &end, const double angle) : begin_(begin), end_(end), angle_(angle) {
    const double dx = end_.x() - begin_.x();
    const double dy = end_.y() - begin_.y();
    angle_ = std::atan2(dy, dx);
    if (angle_ < 0) {
        angle_ += 2 * M_PI;
    }
}

QPointF Ray::GetBegin() const {
    return begin_;
}

QPointF Ray::GetEnd() const {
    return end_;
}

double Ray::GetAngle() const {
    return angle_;
}

void Ray::SetBegin(const QPointF &begin) {
    begin_ = begin;
}

void Ray::SetEnd(const QPointF& new_end) {
    end_ = new_end;
    double dx = end_.x() - begin_.x();
    double dy = end_.y() - begin_.y();
    angle_ = std::atan2(dy, dx);
    if (angle_ < 0) {
        angle_ += 2 * M_PI;
    }
}

void Ray::SetAngle(double angle) {
    angle_ = angle;
}

Ray Ray::Rotate(double angle) const {
    const double dx = end_.x() - begin_.x();
    const double dy = end_.y() - begin_.y();

    const double cos_angle = std::cos(angle);
    const double sin_angle = std::sin(angle);

    const double new_dx = dx * cos_angle - dy * sin_angle;
    const double new_dy = dx * sin_angle + dy * cos_angle;

    const QPointF new_end(
        begin_.x() + new_dx,
        begin_.y() + new_dy
    );

    return Ray{begin_, new_end, angle_ + angle};
}

double Dst(QPointF a, QPointF b) {
    const double dx = b.x() - a.x();
    const double dy = b.y() - a.y();
    return std::sqrt((dx * dx) + (dy * dy));
}
#ifndef RAY_H
#define RAY_H

#include <QPointF>

class Ray {
public:
    Ray() = default;

    Ray(const QPointF &begin, const QPointF &end, double angle = 0.0);

    [[nodiscard]] QPointF GetBegin() const;

    [[nodiscard]] QPointF GetEnd() const;

    [[nodiscard]] double GetAngle() const;

    void SetBegin(const QPointF &begin);

    void SetEnd(const QPointF &end);

    void SetAngle(double angle);

    [[nodiscard]] Ray Rotate(double delta_angle) const;

private:
    QPointF begin_{};
    QPointF end_{};
    double angle_{};
};

double Dst(QPointF a, QPointF b);

#endif //RAY_H

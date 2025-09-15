#ifndef CANVAS_HPP
#define CANVAS_HPP

#include <QWidget>
#include <QPixmap>

class Canvas : public QWidget {
    Q_OBJECT
public:
    explicit Canvas(QWidget *parent = nullptr);
    int angle() const { return m_angle; }
    void setAngle(int a) { m_angle = a; }
protected:
    void paintEvent(QPaintEvent *) override;
private:
    int m_angle = 0;
    QPixmap m_tex;
};


#endif // CANVAS_HPP
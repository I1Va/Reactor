#ifndef CANVAS_HPP
#define CANVAS_HPP

#include <QApplication>
#include <QMainWindow>
#include <QFrame>
#include <QVBoxLayout>
#include <QPainter>
#include <QPixmap>
#include <QWidget>


class Canvas : public QWidget {
    Q_OBJECT
    QPixmap m_tex;

public:
    explicit Canvas(const QString &texPath, QWidget *parent = nullptr)
        : QWidget(parent), m_tex(texPath) {
        setMinimumSize(300, 200);
    }
protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        // draw white background for the canvas area
        if (!m_tex.isNull())
            p.drawPixmap(rect(), m_tex);  // stretch
        else
            p.fillRect(rect(), Qt::white);

    }
};


class TexturedFrame : public QFrame {
    Q_OBJECT

    QPixmap m_tex;
    int m_border;
public:
    // texPath: "texture.png" (filesystem) or ":/images/texture.png" (resource)
    explicit TexturedFrame(const QString &texPath, int border = 12, QWidget *parent = nullptr)
        : QFrame(parent), m_tex(texPath), m_border(border)
    {
        // layout owned by this frame and margins reserve the textured border
        auto *lyt = new QVBoxLayout(this);
        lyt->setContentsMargins(m_border, m_border, m_border, m_border);
        lyt->setSpacing(0);
    }

protected:
    void paintEvent(QPaintEvent *ev) override {
        QPainter p(this);
        if (!m_tex.isNull()) {
            // tile the texture across the whole frame
            p.drawPixmap(rect(), m_tex);
        } else {
            qWarning("Texture image not loaded!");
            // fallback background  
            p.fillRect(rect(), palette().window());
        }
        // draw the QFrame decoration (borders) if you want them
        QFrame::paintEvent(ev);
    }
};


#endif // CANVAS_HPP
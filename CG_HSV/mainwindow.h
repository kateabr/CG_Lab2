#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <stdexcept>
#include <QtCharts>
#include <QtWidgets>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QWidget {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

private slots:
    void on_load_button_clicked();

    void on_horizontalSlider_1_sliderMoved(int position);

    void on_horizontalSlider_2_sliderMoved(int position);

    void on_horizontalSlider_3_sliderMoved(int position);

    void on_pushButton_clicked();

private:
    Ui::MainWindow* ui;
    QChart* hist;
    QBarSet* histBars;
    QBarSeries* histBarsSeries;
    QBarCategoryAxis* axisX;
    QGraphicsScene* sceneOrig;
    QGraphicsScene* sceneHSV;
    QGraphicsPixmapItem* pixmapItemOrig;
    QGraphicsPixmapItem* pixmapItemHSV;
    QChartView* chartView;
    QImage imgOrig;
    QImage imgHSV;
    bool isLoaded = false;
};

struct HSV;

struct RGB {
    int r, g, b;

    RGB(int r, int g, int b) : r(r), g(g), b(b)
    { }

    RGB(QRgb c) {
        r = (c & 0x00FF0000) >> 16;
        g = (c & 0x0000FF00) >> 8;
        b = c & 0x000000FF;
    }

    inline int max() const {
        return std::max(r, std::max(g, b));
    }

    inline int min() const {
        return std::min(r, std::min(g, b));
    }

    HSV to_hsv() const;
};

struct HSV {
    double h, s, v;

    HSV(double h, double s, double v) : h(h), s(s), v(v)
    { }

    double & operator[](int i) {
        switch (i) {
            case 0: return h;
            case 1: return s;
            case 2: return v;
        }
        throw std::out_of_range("index is out of range");
    }

    RGB to_rgb() const;
};

#endif // MAINWINDOW_H

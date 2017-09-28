#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

    void on_comboBox_currentIndexChanged(int index);

private:
    Ui::MainWindow* ui;
    QChart* hist;
    QBarSet* histBars;
    QBarSeries* histBarsSeries;
    QBarCategoryAxis* axisX;
    QGraphicsScene* sceneOrig;
    QGraphicsScene* sceneChannel;
    QGraphicsPixmapItem* pixmapItemOrig;
    QGraphicsPixmapItem* pixmapItemChannel;
    QChartView* chartView;
    QImage imgOrig;
    bool isLoaded = false;
};

#endif // MAINWINDOW_H

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

private:
    Ui::MainWindow* ui;
    QBarSet* histBars = new QBarSet("Bars");
    QBarSeries* histBarsSeries = new QBarSeries();
};

#endif // MAINWINDOW_H

#include <cmath>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    sceneOrig = new QGraphicsScene(nullptr);
    sceneHSV = new QGraphicsScene(nullptr);
    pixmapItemOrig = new QGraphicsPixmapItem();
    pixmapItemHSV = new QGraphicsPixmapItem();

    ui->graphics_orig->setScene(sceneOrig);
    ui->graphics_hsv->setScene(sceneHSV);
    sceneOrig->addItem(pixmapItemOrig);
    sceneHSV->addItem(pixmapItemHSV);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete sceneOrig;
    delete sceneHSV;
    delete pixmapItemOrig;
    delete pixmapItemHSV;
}

void showImage(QGraphicsView * view, QGraphicsPixmapItem * pixmapItem,
               QImage const & img)
{
    pixmapItem->setVisible(true);
    QPixmap pixmap = QPixmap::fromImage(img);
    pixmapItem->setPixmap(pixmap);
    view->scene()->setSceneRect(0, 0, pixmap.width(), pixmap.height());
    view->fitInView(pixmapItem, Qt::KeepAspectRatio);
}

HSV RGB::to_hsv() const
{
    double h, s, v;

    double r = static_cast<double>(this->r) / 255;
    double g = static_cast<double>(this->g) / 255;
    double b = static_cast<double>(this->b) / 255;

    double max = static_cast<double>(this->max()) / 255;
    double min = static_cast<double>(this->min()) / 255;

    if (max == min)
        h = 0;
    else if (g >= b)
        h = 60 * static_cast<double>(g - b) / (max - min);
    else if (g < b)
        h = 60 * static_cast<double>(g - b) / (max - min) + 360;
    else if (max == g)
        h = 60 * static_cast<double>(b - r) / (max - min) + 120;
    else if (max == b)
        h = 60 * static_cast<double>(r - g) / (max - min) + 240;

    if (max == 0)
        s = 0;
    else
        s = 1 - min / max;

    v = std::max(r, std::max(g, b));

    return HSV(h, s, v);
}

RGB HSV::to_rgb() const
{
    int hi = static_cast<int>(floor(h / 60)) % 6;

    double f = h / 60 - floor(h / 60);
    int p = lround(255 * v * (1 - s));
    int q = lround(255 * v * (1 - f * s));
    int t = lround(255 * v * (1 - (1 - f) * s));
    int v1 = lround(255 * v);

    int r, g, b;
    switch (hi) {
        case 0: r = v1, g = t, b = p; break;
        case 1: r = q, g = v1, b = p; break;
        case 2: r = p, g = v1, b = t; break;
        case 3: r = p, g = q, b = v1; break;
        case 4: r = t, g = p, b = v1; break;
        case 5: r = v1, g = p, b = q; break;
    }

    return RGB(r, g, b);
}

void MainWindow::on_load_button_clicked()
{
    QString filename = QFileDialog::getOpenFileName
            (this, "Load Image", "./", "*.jpg");
    if (!filename.isEmpty()) {
        QImageReader reader(filename);
        reader.setAutoTransform(true);
        imgOrig = reader.read().convertToFormat(QImage::Format_ARGB32);        
        isLoaded = true;

        on_horizontalSlider_1_sliderMoved(ui->horizontalSlider_1->value());
    }
}

void MainWindow::on_pushButton_clicked()
{
    QString filename = QFileDialog::getSaveFileName
            (this, "Save Image", "./", "*.jpg");
    if (!filename.isEmpty())
        QImageWriter(filename).write(imgHSV);
}

void MainWindow::on_horizontalSlider_1_sliderMoved(int position)
{
    if (!isLoaded)
        return;
    imgHSV = imgOrig.copy();
    quint64 pixCnt = imgOrig.width() * imgOrig.height();
    QRgb* iCp = (QRgb*)imgHSV.bits();

    double h_sl = ui->horizontalSlider_1->value() - 180;
    double s_sl = static_cast<double>
            (ui->horizontalSlider_2->value() - 50) / 100;
    double v_sl = static_cast<double>
            (ui->horizontalSlider_3->value() - 50) / 100;

    for (quint64 p = 0; p < pixCnt; ++p) {
        HSV hsv = RGB(iCp[p]).to_hsv();

        hsv.h += h_sl;
        hsv.s += s_sl;
        hsv.v += v_sl;
        for (int sl = 0; sl < 3; ++sl) {
            if (hsv[sl] < 0)
                hsv[sl] = !sl ? hsv[sl] + 360 : 0;
            else if (hsv[sl] > (!sl ? 360 : 1))
                hsv[sl] = !sl ? hsv[sl] - 360 : 1;
        }

        RGB rgb = hsv.to_rgb();
        iCp[p] = QColor(rgb.r, rgb.g, rgb.b).rgb();
    }

    showImage(ui->graphics_orig, pixmapItemOrig, imgOrig);
    showImage(ui->graphics_hsv, pixmapItemHSV, imgHSV);
}

void MainWindow::on_horizontalSlider_2_sliderMoved(int position)
{
    on_horizontalSlider_1_sliderMoved(position);
}

void MainWindow::on_horizontalSlider_3_sliderMoved(int position)
{
    on_horizontalSlider_1_sliderMoved(position);
}

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    hist = new QChart();
    histBars = new QBarSet("Bars");
    histBarsSeries = new QBarSeries();
    axisX = new QBarCategoryAxis();
    sceneOrig = new QGraphicsScene(nullptr);
    sceneChannel = new QGraphicsScene(nullptr);
    pixmapItemOrig = new QGraphicsPixmapItem();
    pixmapItemChannel = new QGraphicsPixmapItem();
    chartView = new QChartView(hist);

    ui->comboBox->addItems(QStringList({"Red", "Green", "Blue"}));

    ui->graphics_orig->setScene(sceneOrig);
    ui->graphics_channel->setScene(sceneChannel);
    sceneOrig->addItem(pixmapItemOrig);
    sceneChannel->addItem(pixmapItemChannel);

    histBarsSeries->append(histBars);
    hist->addSeries(histBarsSeries);
    hist->setTitle("Intensity histogram");
    hist->setAnimationOptions(QChart::SeriesAnimations);
    QStringList categories;
    for (int i = 0; i < 256; ++i)
        categories << QString::number(i);

    axisX->append(categories);
    hist->createDefaultAxes();
    hist->setAxisX(axisX, histBarsSeries);
    hist->legend()->setVisible(false);
    hist->axisX()->setLabelsVisible(false);
    hist->axisY()->setLabelsVisible(false);
    hist->axisY()->setMin(0);
    hist->axisY()->setMax(1);

    chartView->setRenderHint(QPainter::Antialiasing);
    ui->frame_hist->layout()->addWidget(chartView);    

    chartView->setBackgroundRole(QPalette::Base);
    chartView->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete hist;
    delete histBarsSeries;
    delete histBars;
    delete axisX;
    delete sceneOrig;
    delete sceneChannel;
    delete pixmapItemOrig;
    delete pixmapItemChannel;
    delete chartView;
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

inline int toRed(quint64 p, QRgb * iCp)
{
    int c = (iCp[p] & 0x00FF0000) >> 16;
    iCp[p] = QColor(c, 0, 0).rgb();
    return c;
}

inline int toGreen(quint64 p, QRgb * iCp)
{
    int c = (iCp[p] & 0x0000FF00) >> 8;
    iCp[p] = QColor(0, c, 0).rgb();
    return c;
}

inline int toBlue(quint64 p, QRgb * iCp)
{
    int c = iCp[p] & 0x000000FF;
    iCp[p] = QColor(0, 0, c).rgb();
    return c;
}

void MainWindow::on_load_button_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Load Image", "./", "*.jpg");
    if (!filename.isEmpty()) {
        QImageReader reader(filename);
        reader.setAutoTransform(true);
        imgOrig = reader.read().convertToFormat(QImage::Format_ARGB32);
        isLoaded = true;

        on_comboBox_currentIndexChanged(ui->comboBox->currentIndex());
    }
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    if (!isLoaded)
        return;

    QImage imgChannel = imgOrig.copy();

    quint64 pixCnt = imgOrig.width() * imgOrig.height();

    int vals[256]{};
    QRgb* iCp = (QRgb*)imgChannel.bits();
    switch (index) {
        case 0:
          for (quint64 p = 0; p < pixCnt; ++p)
              ++vals[toRed(p, iCp)];
          break;
        case 1:
          for (quint64 p = 0; p < pixCnt; ++p)
              ++vals[toGreen(p, iCp)];
          break;
        case 2:
          for (quint64 p = 0; p < pixCnt; ++p)
              ++vals[toBlue(p, iCp)];
          break;
    }

    if (histBars->count() > 0)
        histBars->remove(0, 256);
    double max = *std::max_element(vals, vals + 256);
    for (int i = 0; i < 256; ++i)
        *histBars << vals[i] / max;

    showImage(ui->graphics_orig, pixmapItemOrig, imgOrig);
    showImage(ui->graphics_channel, pixmapItemChannel, imgChannel);
}

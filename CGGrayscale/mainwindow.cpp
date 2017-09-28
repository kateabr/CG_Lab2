#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->label_orig->setBackgroundRole(QPalette::Base);
    ui->label_orig->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    ui->label_orig->setScaledContents(true);

    ui->label_diff->setBackgroundRole(QPalette::Base);
    ui->label_diff->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    ui->label_diff->setScaledContents(true);

    ui->label_equal->setBackgroundRole(QPalette::Base);
    ui->label_equal->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    ui->label_equal->setScaledContents(true);

    ui->label_intensity->setBackgroundRole(QPalette::Base);
    ui->label_intensity->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    ui->label_intensity->setScaledContents(true);

    QChart* hist = new QChart();
    histBarsSeries->append(histBars);
    hist->addSeries(histBarsSeries);
    hist->setTitle("Intensity histogram");
    hist->setAnimationOptions(QChart::SeriesAnimations);
    QStringList categories;
    for (int i = 0; i < 256; ++i)
        categories << QString::number(i);

    QBarCategoryAxis* axisX = new QBarCategoryAxis();
    axisX->append(categories);
    hist->createDefaultAxes();
    hist->setAxisX(axisX, histBarsSeries);
    hist->legend()->setVisible(false);
    hist->axisX()->setLabelsVisible(false);
    hist->axisY()->setLabelsVisible(false);
    hist->axisY()->setMin(0);
    hist->axisY()->setMax(1);

    QChartView* chartView = new QChartView(hist);
    chartView->setRenderHint(QPainter::Antialiasing);
    ui->frame_hist->layout()->addWidget(chartView);

    chartView->setBackgroundRole(QPalette::Base);
    chartView->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete histBarsSeries;
    delete histBars;
}

void MainWindow::on_load_button_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Load Image", "./", "*.jpg");
    if (!filename.isEmpty()) {
        QImageReader reader(filename);
        reader.setAutoTransform(true);
        const QImage imgOrig = reader.read().convertToFormat(QImage::Format_ARGB32);
        QImage imgIntesity = imgOrig.copy();
        QImage imgEqual = imgOrig.copy();
        QImage imgDiff = imgOrig.copy();

        quint64 pixCnt = imgOrig.width() * imgOrig.height();

        int vals[256]{};
        int r, g, b, c, cc, cdiff;
        QRgb* iEp = (QRgb*)imgEqual.bits();
        QRgb* iIp = (QRgb*)imgIntesity.bits();
        QRgb* iDp = (QRgb*)imgDiff.bits();
        for (quint64 p = 0; p < pixCnt; p++) {
            r = (iEp[p] & 0x00FF0000) >> 16;
            g = (iEp[p] & 0x0000FF00) >> 8;
            b = (iEp[p] & 0x000000FF);
            c = (r + g + b) / 3;
            iEp[p] = QColor(c, c, c).rgb();
            cc = 0.2126 * r + 0.7152 * g + 0.0722 * b;
            iIp[p] = QColor(cc, cc, cc).rgb();
            cdiff = 255 - qAbs(c - cc);
            iDp[p] = QColor(cdiff, cdiff, cdiff).rgb();
            vals[cc] += 1;
        }

        if (histBars->count() > 0)
            histBars->remove(0, 256);
        double max = *std::max_element(vals, vals + 256);
        for (int i = 0; i < 256; ++i)
            *histBars << vals[i] / max;

        ui->label_orig->setPixmap(QPixmap::fromImage(imgOrig));

        ui->label_diff->setPixmap(QPixmap::fromImage(imgDiff));

        ui->label_equal->setPixmap(QPixmap::fromImage(imgEqual));

        ui->label_intensity->setPixmap(QPixmap::fromImage(imgIntesity));
    }
}

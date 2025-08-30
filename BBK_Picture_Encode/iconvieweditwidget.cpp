#include "iconvieweditwidget.h"
#include "ui_iconvieweditwidget.h"
#include "gam_type.h"

IconViewEditWidget::IconViewEditWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::IconViewEditWidget)
{
    ui->setupUi(this);

    static QTranslator translator;
    translator.load(":/translations/zh_CN.qm");
    qApp->installTranslator(&translator);
    ui->retranslateUi(this);

    adjustSize();

    buildDateStr = QLocale(QLocale::English).toDate(QString(__DATE__).replace("  ", " 0"), "MMM dd yyyy").toString("yyyyMMdd");
    buildTimeStr = QLocale(QLocale::English).toTime(QString(__TIME__), "hh:mm:ss").toString("hhmm");

    setWindowTitle(tr("BBK_Picture_Encode ") + buildDateStr + ' ' + buildTimeStr);

    connect(ui->iconViewEdit,   &IconViewEdit::brushTypeChanged,    this, &IconViewEditWidget::on_brushTypeChanged);

    connect(ui->openFile,       &QPushButton::clicked,              this, &IconViewEditWidget::on_pushButton_clicked);
    connect(ui->brushBtn,       &QPushButton::clicked,              this, &IconViewEditWidget::on_pushButton_clicked);
    connect(ui->resetView,      &QPushButton::clicked,              this, &IconViewEditWidget::on_pushButton_clicked);
    connect(ui->saveFile,       &QPushButton::clicked,              this, &IconViewEditWidget::on_pushButton_clicked);

    connect(ui->transChk,       &QCheckBox::clicked,                this, &IconViewEditWidget::on_pictureEncode_configChanged);
    connect(ui->transColorBtn,  &ColorButton::colorChanged,         this, &IconViewEditWidget::on_pictureEncode_configChanged);
    connect(ui->otsuRB,         &QRadioButton::clicked,             this, &IconViewEditWidget::on_pictureEncode_configChanged);
    connect(ui->wellnerRB,      &QRadioButton::clicked,             this, &IconViewEditWidget::on_pictureEncode_configChanged);
    connect(ui->iconWidth,      SIGNAL(valueChanged(int)),          this, SLOT(on_pictureEncode_configChanged()));
    connect(ui->iconHeight,     SIGNAL(valueChanged(int)),          this, SLOT(on_pictureEncode_configChanged()));
    connect(ui->iconHorCnt,     SIGNAL(valueChanged(int)),          this, SLOT(on_pictureEncode_configChanged()));
    connect(ui->iconVerCnt,     SIGNAL(valueChanged(int)),          this, SLOT(on_pictureEncode_configChanged()));
    connect(ui->iconCnt,        SIGNAL(valueChanged(int)),          this, SLOT(on_pictureEncode_configChanged()));

    QSettings settings("QtProject", "BBK_Picture_Encode");
    ui->transChk->setChecked(settings.value("transChk").toBool());
    ui->transColorBtn->SetColor(settings.value("last_trans_color").toInt());
    // ui->iconIdx->setValue(settings.value("iconIdx").toInt());
    ui->iconWidth->setValue(settings.value("iconWidth").toInt());
    ui->iconHeight->setValue(settings.value("iconHeight").toInt());
    ui->iconHorCnt->setValue(settings.value("iconHorCnt").toInt());
    ui->iconVerCnt->setValue(settings.value("iconVerCnt").toInt());
    ui->iconCnt->setValue(settings.value("iconCnt").toInt());
}

IconViewEditWidget::~IconViewEditWidget()
{
    QSettings settings("QtProject", "BBK_Picture_Encode");
    settings.setValue("transChk", ui->transChk->isChecked());
    settings.setValue("last_trans_color", ui->transColorBtn->GetColor().rgba());
    // settings.setValue("iconIdx", ui->iconIdx->value());
    settings.setValue("iconWidth", ui->iconWidth->value());
    settings.setValue("iconHeight", ui->iconHeight->value());
    settings.setValue("iconHorCnt", ui->iconHorCnt->value());
    settings.setValue("iconVerCnt", ui->iconVerCnt->value());
    settings.setValue("iconCnt", ui->iconCnt->value());
    delete ui;
}

void IconViewEditWidget::on_brushTypeChanged(IconViewEdit::BrushType brushType)
{
    switch(brushType)
    {
    case IconViewEdit::BrushType_Black:
        ui->brushBtn->setText(tr("Black"));
        break;

    case IconViewEdit::BrushType_White:
        ui->brushBtn->setText(tr("White"));
        break;

    case IconViewEdit::BrushType_Transparent:
        ui->brushBtn->setText(tr("Transparent"));
        break;

    default:
        break;
    }
}

void IconViewEditWidget::on_iconType_currentIndexChanged(int index)
{
    ui->iconSubType->clear();

    switch(index)
    {
    case 0:
        ui->iconSubType->addItem(tr("Scene TILE"));
        currSuffix = "TIL(*.til)";
        break;

    case 1:
        ui->iconSubType->addItem(tr("Player"));
        ui->iconSubType->addItem(tr("NPC"));
        ui->iconSubType->addItem(tr("Enemy"));
        ui->iconSubType->addItem(tr("Scene Object"));
        currSuffix = "ACP(*.acp)";
        break;

    case 2:
        ui->iconSubType->addItem(tr("Hat"));
        ui->iconSubType->addItem(tr("Clothes"));
        ui->iconSubType->addItem(tr("Shoes"));
        ui->iconSubType->addItem(tr("Cloak"));
        ui->iconSubType->addItem(tr("Wristlet"));
        ui->iconSubType->addItem(tr("Accessory"));
        ui->iconSubType->addItem(tr("Weapon"));
        ui->iconSubType->addItem(tr("Concealed Weapon"));
        ui->iconSubType->addItem(tr("Medicine"));
        ui->iconSubType->addItem(tr("Panacea"));
        ui->iconSubType->addItem(tr("Elixir"));
        ui->iconSubType->addItem(tr("Dope"));
        ui->iconSubType->addItem(tr("Escape"));
        ui->iconSubType->addItem(tr("Drama"));
        currSuffix = "GDP(*.gdp)";
        break;

    case 3:
        ui->iconSubType->addItem(tr("Effect"));
        currSuffix = "PIC(*.pic)";
        break;

    case 4:
        ui->iconSubType->addItem(tr("Profile"));
        ui->iconSubType->addItem(tr("IF Display"));
        ui->iconSubType->addItem(tr("Fight Player"));
        ui->iconSubType->addItem(tr("Fight Background"));
        ui->iconSubType->addItem(tr("Drama Display"));
        currSuffix = "PIC(*.pic)";
        break;
    }
}

void IconViewEditWidget::on_iconViewIdx_valueChanged(int index)
{
    ui->iconViewEdit->SetIconIndex(index - 1);
}

void IconViewEditWidget::on_pushButton_clicked()
{
    QPushButton *btn = static_cast<QPushButton *>(sender());
    if(btn == ui->openFile) {
        QSettings settings("QtProject", "BBK_Picture_Encode");
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), settings.value("last_img_dir").toString(), "Image(*.bmp *.png *.jpg *.tif)");
        if(!fileName.isEmpty() && QFile(fileName).exists())
        {
            settings.setValue("last_img_dir", QFileInfo(fileName).absoluteDir().absolutePath());
            settings.sync();
            srcImage.load(fileName);
            if(srcImage.isNull()) {
                return;
            }
            ui->iconViewEdit->SetTransColor(ui->transColorBtn->GetColor());
            ui->iconCnt->setRange(1, ui->iconHorCnt->value() * ui->iconVerCnt->value());
            ui->iconViewIdx->setRange(1, ui->iconCnt->value());
            ui->iconViewEdit->SetHeader(ui->iconWidth->value(), ui->iconHeight->value(), ui->iconHorCnt->value(), ui->iconVerCnt->value(), ui->iconCnt->value(), ui->transChk->isChecked() ? 2 : 1);
            if(ui->otsuRB->isChecked()) {
                ui->iconViewEdit->LoadImageBinaryAll(srcImage, ui->gaussianBlur->value(), IconViewEdit::BinAlg_Otsu);
            }
            else if(ui->wellnerRB->isChecked()) {
                ui->iconViewEdit->LoadImageBinaryAll(srcImage, ui->gaussianBlur->value(), IconViewEdit::BinAlg_Wellner);
            }
            ui->iconViewEdit->SetIconIndex(ui->iconViewIdx->value() - 1);
        }
    }
    else if(btn == ui->brushBtn) {
        if(btn->text() == tr("Black")) {
            btn->setText(tr("White"));
            ui->iconViewEdit->SetBrushType(IconViewEdit::BrushType_White);
        }
        else if(btn->text() == tr("White")) {
            if(ui->iconViewEdit->GetIconTransparency() < 2) {
                btn->setText(tr("Black"));
                ui->iconViewEdit->SetBrushType(IconViewEdit::BrushType_Black);
            }
            else if(ui->iconViewEdit->GetIconTransparency() < 3) {
                btn->setText(tr("Transparent"));
                ui->iconViewEdit->SetBrushType(IconViewEdit::BrushType_Transparent);
            }
        }
        else {
            btn->setText(tr("Black"));
            ui->iconViewEdit->SetBrushType(IconViewEdit::BrushType_Black);
        }
    }
    else if(btn == ui->resetView) {
        ui->iconViewEdit->ResetView();
    }
    else if(btn == ui->saveFile) {
        QSettings settings("QtProject", "BBK_Picture_Encode");
        QFile imgFile(QFileDialog::getSaveFileName(this, tr("Save File"), settings.value("last_img_dir").toString(), currSuffix));
        if(!imgFile.open(QFile::WriteOnly)) {
            return;
        }

        uint8_t *imgData = new uint8_t[ICON_DATA_MAX_SIZE];
        if(imgData == nullptr) {
            return;
        }

        imgData[0] = ui->iconSubType->currentIndex() + 1;
        imgData[1] = ui->iconIdx->value();
        imgData[2] = ui->iconViewEdit->GetIconWidth();
        imgData[3] = ui->iconViewEdit->GetIconHeight();
        imgData[4] = ui->iconViewEdit->GetIconCount();
        imgData[5] = ui->iconViewEdit->GetIconTransparency();
        ui->iconViewEdit->WriteBack(imgData, calc_effects_icon_size(reinterpret_cast<effects_icon_head_t *>(imgData)));

        QDataStream gamStream(&imgFile);
        gamStream.writeRawData((const char *)imgData, calc_effects_icon_size(reinterpret_cast<effects_icon_head_t *>(imgData)));

        imgFile.close();

        delete[] imgData;

        ui->iconIdx->setValue(ui->iconIdx->value() + 1);
    }
}

void IconViewEditWidget::on_gaussianBlur_valueChanged(int value)
{
    if(srcImage.isNull()) {
        return;
    }

    if(ui->otsuRB->isChecked()) {
        ui->iconViewEdit->LoadImageBinary(srcImage, value, IconViewEdit::BinAlg_Otsu);
    }
    else if(ui->wellnerRB->isChecked()) {
        ui->iconViewEdit->LoadImageBinary(srcImage,  value, IconViewEdit::BinAlg_Wellner);
    }
}

void IconViewEditWidget::on_pictureEncode_configChanged(void)
{
    if(srcImage.isNull()) {
        return;
    }
    ui->iconViewEdit->SetTransColor(ui->transColorBtn->GetColor());
    ui->iconCnt->setRange(1, ui->iconHorCnt->value() * ui->iconVerCnt->value());
    ui->iconViewIdx->setRange(1, ui->iconCnt->value());
    ui->iconViewEdit->SetHeader(ui->iconWidth->value(), ui->iconHeight->value(), ui->iconHorCnt->value(), ui->iconVerCnt->value(), ui->iconCnt->value(), ui->transChk->isChecked() ? 2 : 1);
    if(ui->otsuRB->isChecked()) {
        ui->iconViewEdit->LoadImageBinaryAll(srcImage, ui->gaussianBlur->value(), IconViewEdit::BinAlg_Otsu);
    }
    else if(ui->wellnerRB->isChecked()) {
        ui->iconViewEdit->LoadImageBinaryAll(srcImage, ui->gaussianBlur->value(), IconViewEdit::BinAlg_Wellner);
    }
    ui->iconViewEdit->SetIconIndex(ui->iconViewIdx->value() - 1);
}

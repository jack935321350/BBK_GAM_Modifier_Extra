#include "mainwidget.h"
#include "ui_mainwidget.h"

MainWidget::MainWidget(QWidget *parent) : QWidget(parent), ui(new Ui::MainWidget)
{
    ui->setupUi(this);

    _buildDateTimeStr = QLocale(QLocale::English).toDateTime(QString(__DATE__ " " __TIME__).replace("  ", " 0"), "MMM dd yyyy hh:mm:ss").toString("yyyyMMdd_hhmm");

    setWindowTitle(tr("BBK_Resource_Integrate_") + _buildDateTimeStr);

    connect(&stateTimer, SIGNAL(timeout()), this, SLOT(on_stateTimer_timeout()));

    QSettings settings("QtProject", "BBK_Resource_Integrate");
    ui->rcDir->setText(settings.value("last_res_dir").toString());
    ui->sizeOptChk->setChecked(settings.value("size_opt").toBool());
    ui->libName->setText(settings.value("lib_name").toString());
}

MainWidget::~MainWidget()
{
    QSettings settings("QtProject", "BBK_Resource_Integrate");
    settings.setValue("size_opt", ui->sizeOptChk->isChecked());
    settings.setValue("lib_name", ui->libName->text());
    settings.sync();

    delete ui;
}

void MainWidget::SetBlockSize(uint8_t block, uint16_t size)
{
    _rcLib[_blockSize * block + 0x0c] = (size & 0xff);
    _rcLib[_blockSize * block + 0x0d] = ((size >> 8) & 0xff);
}

void MainWidget::SetBlockArgs(uint16_t rcIndex, ResType resType, uint8_t subType, uint8_t resIndex)
{
    _rcLib[_rcBlockBase + rcIndex * 3] = resType;
    _rcLib[_rcBlockBase + rcIndex * 3 + 1] = subType;
    _rcLib[_rcBlockBase + rcIndex * 3 + 2] = resIndex;
}

void MainWidget::SetBlockOffset(uint16_t rcIndex, uint8_t block, uint16_t offset)
{
    _rcLib[_rcOffsetBase + rcIndex * 3] = block;
    _rcLib[_rcOffsetBase + rcIndex * 3 + 1] = (offset & 0xff);
    _rcLib[_rcOffsetBase + rcIndex * 3 + 2] = ((offset >> 8) & 0xff);
}

void MainWidget::Append_Block(ResType resType)
{
    QByteArray block;
    block.resize(_blockSize);
    block.fill(-1, _blockSize);

    QByteArray blockTag = ResTag[resType];
    if(resType == Res_PIC) {
        blockTag = "SUN";
    }

    block[0] = blockTag[0];
    block[1] = blockTag[1];
    block[2] = blockTag[2];
    memset(block.data() + 3, 0, 0x0d);

    _rcLib.append(block);
    _currBlock++;
    _currBlockOffset = _rcBlockBase;
}

bool libResIndexCompare(const LibRes &a, const LibRes &b) {
    if(uint8_t(a.data[0]) < uint8_t(b.data[0])) {
        return true;
    }
    else if(uint8_t(a.data[0]) == uint8_t(b.data[0]) && uint8_t(a.data[1]) < uint8_t(b.data[1])) {
        return true;
    }
    return false;
}

bool libResSizeCompare(const LibRes &a, const LibRes &b) {
    if(a.data.size() < b.data.size()) {
        return false;
    }
    return true;
}

void MainWidget::Integrate_Block(ResType resType)
{
    _libResVec.clear();

    QString resDirPath = ui->rcDir->text() + '/' + ResTag[resType];
    if(!QDir(resDirPath).exists()) {
        QDir(ui->rcDir->text()).mkdir(ResTag[resType]);
    }

    QDir resDir(resDirPath);
    QStringList resFilters;
    resFilters << "*." + ResTag[resType] << "*." + ResTag[resType].toLower();
    resDir.setNameFilters(resFilters);
    QStringList resEntries = resDir.entryList();
    for(const QString &resEntry : qAsConst(resEntries)) {
        QFile resFile(resDir.absoluteFilePath(resEntry));
        if(!resFile.open(QFile::ReadOnly)) {
            continue;
        }
        LibRes libRes;
        libRes.fileName = resEntry;
        libRes.data = resFile.readAll();
        _libResVec.append(libRes);
        resFile.close();
    }

    if(_libResVec.isEmpty()) {
        return;
    }

    for(int i = 0; i < _libResVec.size(); i++) {
        for(int j = i; j < _libResVec.size(); j++) {
            if(i == j) {
                continue;
            }
            if(_libResVec[i].data[0] == _libResVec[j].data[0] && _libResVec[i].data[1] == _libResVec[j].data[1]) {
                QMessageBox::critical(this, ResTag[resType],
                    "<font color=red>" + _libResVec[i].fileName + "</font>" + tr(" & ") + "<font color=red>" + _libResVec[j].fileName + "</font>" + tr(" Res Index Same!"));
                _errorTag = true;
            }
        }
    }

    if(_lastResType != resType) {
        Append_Block(resType);
        _lastResType = resType;
    }

    if(ui->sizeOptChk->isChecked()) {

        std::sort(_libResVec.begin(), _libResVec.end(), libResSizeCompare);

        QVector<BlkInfo> blkInfoVec;

        BlkInfo blkInfo;

        blkInfo.blkIndex = _currBlock;
        blkInfo.blkOffset = _currBlockOffset;
        blkInfoVec.append(blkInfo);

        while (_libResVec.size() > 0) {

            bool blkEnough = false;
            uint8_t bestBlkIndex = 0;
            uint16_t bestBlkOffset = _rcBlockBase;

            for(int i = 0; i < blkInfoVec.size(); i++) {
                if(_libResVec.front().data.size() <= (_blockSize - blkInfoVec[i].blkOffset)) {
                    uint16_t blkOffset = blkInfoVec[i].blkOffset + _libResVec.front().data.size();
                    if(blkOffset > bestBlkOffset) {
                        bestBlkIndex = i;
                        bestBlkOffset = blkOffset;
                    }
                    blkEnough = true;
                }
            }

            if(blkEnough) {

                SetBlockArgs(_rcIndex, resType, _libResVec.front().data[0], _libResVec.front().data[1]);
                SetBlockOffset(_rcIndex, blkInfoVec[bestBlkIndex].blkIndex, blkInfoVec[bestBlkIndex].blkOffset);

                for(int j = 0; j < _libResVec.front().data.size(); j++) {
                    _rcLib[_blockSize * blkInfoVec[bestBlkIndex].blkIndex + blkInfoVec[bestBlkIndex].blkOffset + j] = _libResVec.front().data[j];
                }

                blkInfoVec[bestBlkIndex].blkOffset += _libResVec.front().data.size();
                _libResVec.removeFirst();
                _rcIndex++;

                SetBlockSize(0, _rcIndex * 3);
                SetBlockSize(blkInfoVec[bestBlkIndex].blkIndex, blkInfoVec[bestBlkIndex].blkOffset);

            } else {

                Append_Block(resType);
                blkInfo.blkIndex = _currBlock;
                blkInfo.blkOffset = _currBlockOffset;
                blkInfoVec.append(blkInfo);

            }
        }

    } else {

        std::sort(_libResVec.begin(), _libResVec.end(), libResIndexCompare);

        for(int i = 0; i < _libResVec.size(); i++) {

            if(_libResVec[i].data.size() > (_blockSize - _currBlockOffset)) {
                Append_Block(resType);
            }

            SetBlockArgs(_rcIndex, resType, _libResVec[i].data[0], _libResVec[i].data[1]);
            SetBlockOffset(_rcIndex, _currBlock, _currBlockOffset);

            for(int j = 0; j < _libResVec[i].data.size(); j++) {
                _rcLib[_blockSize * _currBlock + _currBlockOffset + j] = _libResVec[i].data[j];
            }

            _currBlockOffset += _libResVec[i].data.size();
            _rcIndex++;

            SetBlockSize(0, _rcIndex * 3);
            SetBlockSize(_currBlock, _currBlockOffset);

        }

    }

    _libResVec.clear();
}

void MainWidget::on_integrateResBtn_clicked()
{
    _rcLib.clear();
    _rcLib.resize(_blockSize);
    _rcLib.fill(-1, _blockSize);

    _rcLib[0] = 'L';
    _rcLib[1] = 'I';
    _rcLib[2] = 'B';

    QTextCodec *gbkCodec = QTextCodec::codecForName("GBK");
    QByteArray libName = gbkCodec->fromUnicode(ui->libName->text());
    memcpy_s(_rcLib.data() + 3, 9, libName.data(), libName.size());

    memset(_rcLib.data() + 3 + libName.size(), 0, 11 - libName.size());

    _rcIndex = 0;
    _currBlock = 0;
    _currBlockOffset = _blockSize;

    _errorTag = false;

    _lastResType = Res_LIB;

    Integrate_Block(Res_GUT);
    Integrate_Block(Res_MAP);
    Integrate_Block(Res_ARS);
    Integrate_Block(Res_MRS);
    Integrate_Block(Res_SRS);
    Integrate_Block(Res_GRS);
    Integrate_Block(Res_TIL);
    Integrate_Block(Res_ACP);
    Integrate_Block(Res_GDP);
    Integrate_Block(Res_GGJ);
    Integrate_Block(Res_PIC);
    Integrate_Block(Res_MLR);

    if(_errorTag) {
        return;
    }

    QSettings settings("QtProject", "BBK_Resource_Integrate");

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save"), settings.value("last_dir").toString() + '/' + settings.value("last_file").toString(), "LIB(*.lib)");

    QFile libFile(fileName);
    if(!libFile.open(QFile::WriteOnly)) {
        return;
    }

    QDataStream libStream(&libFile);
    libStream.writeRawData(_rcLib.data(), _rcLib.size());
    libFile.close();

    settings.setValue("last_dir", QFileInfo(fileName).absoluteDir().path());
    settings.setValue("last_file", QFileInfo(fileName).fileName());
    settings.sync();

    ui->stateLbl->setText(u8"<font color=green>√</font>");
    stateTimer.start(1500);
}

void MainWidget::on_libName_textChanged(const QString &str)
{
    Q_UNUSED(str)

    int len8Bit = ui->libName->text().toLocal8Bit().length();
    if(len8Bit > 8) {
        ui->libName->setCursorPosition(ui->libName->text().size());
        do {
            ui->libName->backspace();
            len8Bit = ui->libName->text().toLocal8Bit().length();
        } while(len8Bit > 8);
    }
}

void MainWidget::on_stateTimer_timeout()
{
    stateTimer.stop();
    ui->stateLbl->clear();
}

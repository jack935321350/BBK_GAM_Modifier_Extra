#include "gut_engine.h"

GutEngine::GutEngine(QObject *parent, uint8_t *data, uint32_t len) : QObject(parent)
{
    _GutData = (uint8_t *)malloc(GUT_DATA_MAX_SIZE);
    _LabelFlag = (bool *)malloc(65535 * sizeof(bool));
    _JumpTable = (uint16_t *)malloc(255 * sizeof(uint16_t));
    if(_GutData == nullptr || _LabelFlag == nullptr || _JumpTable == nullptr || data == nullptr) {
        if(_GutData != nullptr) {
            free(_GutData);
        }
        if(_LabelFlag != nullptr) {
            free(_LabelFlag);
        }
        if(_JumpTable != nullptr) {
            free(_JumpTable);
        }
        ErrorProcess();
        return;
    }

    memset(_GutData, 0, GUT_DATA_MAX_SIZE);
    memset(_LabelFlag, false, 65535);
    memset(_JumpTable, 0, 255);

    memcpy_s(_GutData, len, data, len);
    _GutSize = len;

    _drama_head = reinterpret_cast<drama_head_t *>(_GutData);

    _LabelFlagSize = _drama_head->data_size;

    _GutPtr = _GutData + sizeof(drama_head_t);
}

GutEngine::~GutEngine()
{
    if(_GutData != nullptr) {
        free(_GutData);
    }
    if(_LabelFlag != nullptr) {
        free(_LabelFlag);
    }
    if(_JumpTable != nullptr) {
        free(_JumpTable);
    }
}

bool GutEngine::Process()
{
    try {

        SetJumpTable();

        _OutBA.clear();

        _OutBA.append(QString("@ Datetime: %1\n\n\n").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).toLatin1());

        QVector<GutCmd> gutCmds;

        while(_GutPtr < (_GutData + _GutSize)) {

            uint8_t currByte = ReadBYTE();

            uint16_t currAddr = static_cast<uint16_t>(_GutPtr - _GutData - sizeof(drama_head_t) + 1);

            GutCmd cmd;
            cmd.setAddr(currAddr);

            QByteArray result;
            result.append(GetTagName(currByte));

            if(currByte == GutCmd::Callback || currByte == GutCmd::Return) {
                result.append('\n');
            }

            QByteArray tagParam = GetTagParam(currByte);

            if(!tagParam.isEmpty()) {
                result.append(' ');
                result.append(FormatParam(tagParam));
            }

            cmd.setText(result);

            gutCmds.append(cmd);

        }

        for (const GutCmd &cmd : qAsConst(gutCmds)) {

            uint16_t addr = cmd.getAddr();

            if(_LabelFlag[addr]) {
                _OutBA.append(QString("\nlabel_%1:\n").arg(addr).toLatin1());
            }

            _OutBA.append(cmd.getText());
            _OutBA.append('\n');

        }

        _OutBA.append('\n');


        if(_JumpTableSize > 0) {

            _OutBA.append("@ ----- GutEvent -----\n");

            for(uint8_t i = 0; i < _JumpTableSize; i++) {

                uint16_t jump = _JumpTable[i];

                if(jump > 0) {
                    _OutBA.append(GetTagName(GutCmd::GutEvent));
                    _OutBA.append(QString(" %1 label_%2\n").arg(i + 1).arg(jump).toLatin1());
                }
            }
        }

        return true;

    } catch (...) {

        ErrorProcess();
        return false;

    }
}

QByteArray GutEngine::GetOutBA()
{
    return _OutBA;
}

uint8_t GutEngine::ReadBYTE()
{
    uint8_t retVal = *(uint8_t *)_GutPtr;
    _GutPtr += sizeof(uint8_t);
    return retVal;
}
uint16_t GutEngine::ReadWORD()
{
    uint16_t retVal = *(uint16_t *)_GutPtr;
    _GutPtr += sizeof(uint16_t);
    return retVal;
}

uint32_t GutEngine::ReadDWORD()
{
    uint32_t retVal = *(uint32_t *)_GutPtr;
    _GutPtr += sizeof(uint32_t);
    return retVal;
}

void GutEngine::ErrorProcess()
{

}

void GutEngine::SetLabelFlag(uint16_t index)
{
    if(index >= _LabelFlagSize) {
        ErrorProcess();
    }
    _LabelFlag[index] = true;
}

void GutEngine::SetJumpTable()
{
    _JumpTableSize = ReadBYTE();
    uint16_t addr = 0;
    for(uint8_t i = 0; i < _JumpTableSize; i++) {
        addr = ReadWORD();
        if(addr > 0) {
            if(addr < 5) {
                ErrorProcess();
            }
            SetLabelFlag(addr);
        }
        _JumpTable[i] = addr;
    }
}

QByteArray GutEngine::FormatParam(const QByteArray &param)
{
    QByteArray result = "";
    uint16_t addr = 0, wValue = 0;
    uint8_t c = 0;

    for(int i = 0; i < param.size(); i++) {

        if(!result.isEmpty()) {
            result.append(' ');
        }

        switch(param.at(i))
        {
        case 'A':
            addr = ReadWORD();
            SetLabelFlag(addr);
            result.append(QString("label_%1").arg(addr).toLatin1());
            break;

        case 'C':
            result.append("\"");
            do {
                c = ReadBYTE();
                if(c > 0) {
                    result.append(c);
                }
            } while(c > 0);
            result.append("\"");
            break;

        case 'E':
            addr = _JumpTable[ReadBYTE()];
            SetLabelFlag(addr);
            result.append(QString("label_%1").arg(addr).toLatin1());
            break;

        case 'L':
            result.append(QString::number(ReadDWORD()).toLatin1());
            break;

        case 'N':
            result.append(QString::number(ReadWORD()).toLatin1());
            break;

        case 'U':
            result.append("\"");
            wValue = ReadWORD();
            result.append(QString::number((wValue >> 8) & 0xff).toLatin1());
            result.append(QString("%1").arg(wValue & 0xff, 3, 10, QLatin1Char('0')).toLatin1());
            while(*_GutPtr > 0) {
                result.append(' ');
                wValue = ReadWORD();
                result.append(QString::number((wValue >> 8) & 0xff).toLatin1());
                result.append(QString("%1").arg(wValue & 0xff, 3, 10, QLatin1Char('0')).toLatin1());
            }
            result.append("\"");
            _GutPtr++;
            break;

        default:
            break;
        }
    }

    return result;
}

QByteArray GutEngine::GetTagName(uint8_t index)
{
    if(index >= GutCmd::END) {
        ErrorProcess();
        return "UnknownName";
    }
    return TAG_TEXT[index];
}

QByteArray GutEngine::GetTagParam(uint8_t index)
{
    if(index >= GutCmd::END) {
        ErrorProcess();
        return "UnknownParam";
    }
    return TAG_PARAM[index];
}

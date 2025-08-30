#ifndef GUT_ENGINE_H
#define GUT_ENGINE_H

#include <QDebug>
#include <QObject>
#include <QVector>
#include <QByteArray>
#include <QDateTime>

#include  "gam_type.h"

#define GUT_DATA_MAX_SIZE   1048576

class GutCmd {
public:
    GutCmd() {
        _addr = 0xffff;
        _text.clear();
    }

    ~GutCmd() {

    }

    GutCmd(const GutCmd &right) {
        _addr = right._addr;
        _text = right._text;
    }

    GutCmd &operator = (const GutCmd &right) {
        this->_addr = right._addr;
        this->_text = right._text;
        return *this;
    }

    void setAddr(uint16_t addr) {
        _addr = addr;
    }

    uint16_t getAddr() const {
        return _addr;
    }

    void setText(const QByteArray &text) {
        _text = text;
    }

    QByteArray getText() const {
        return _text;
    }

    enum TAG_ID : uint8_t {
        Music = 0,
        LoadMap,
        CreateActor,
        DeleteNpc,
        MapEvent,
        ActorEvent,
        Move,
        ActorMove,
        ActorSpeed,
        Callback,
        Goto,
        If,
        Set,
        Say,
        StartChapter,
        ScreenR,
        ScreenS,
        ScreenA,
        Event,
        Money,
        Gameover,
        IfCmp,
        Add,
        Sub,
        SetControlId,
        GutEvent,
        SetEvent,
        ClrEvent,
        Buy,
        FaceToFace,
        Movie,
        Choice,
        CreateBox,
        DeleteBox,
        GainGoods,
        InitFight,
        FightEnable,
        FightDisenable,
        CreateNpc,
        EnterFight,
        DeleteActor,
        GainMoney,
        UseMoney,
        SetMoney,
        LearnMagic,
        Sale,
        NpcMoveMod,
        Message,
        DeleteGoods,
        ResumeActorHp,
        ActorLayerUp,
        BoxOpen,
        DelAllNpc,
        NpcStep,
        SetSceneName,
        ShowSceneName,
        ShowScreen,
        UseGoods,
        AttribTest,
        AttribSet,
        AttribAdd,
        ShowGut,
        UseGoodsNum,
        Randrade,
        Menu,
        TestMoney,
        CallChapter,
        DisCmp,
        Return,
        TimeMsg,
        DisableSave,
        EnableSave,
        GameSave,
        SetEventTimer,
        EnableShowPos,
        DisableShowPos,
        SetTo,
        TestGoodsNum,
        SetFightMiss,
        SetArmsToss,
        END
    };

private:
    uint16_t _addr;
    QByteArray _text;
};

class GutEngine : QObject
{
    Q_OBJECT

public:
    GutEngine(QObject *parent = nullptr, uint8_t *data = nullptr, uint32_t len = 0);
    ~GutEngine();
    bool Process();
    QByteArray GetOutBA();


private:
    uint8_t         *_GutData = nullptr;
    int32_t         _GutSize = 0;

    bool            *_LabelFlag = nullptr;
    uint16_t        _LabelFlagSize = 0;

    uint16_t        *_JumpTable = nullptr;
    uint8_t         _JumpTableSize = 0;

    uint8_t         *_GutPtr = nullptr;

    drama_head_t    *_drama_head = nullptr;

    QByteArray      _OutBA;

    uint8_t         ReadBYTE();
    uint16_t        ReadWORD();
    uint32_t        ReadDWORD();

    void            ErrorProcess();
    void            SetLabelFlag(uint16_t index);
    void            SetJumpTable();

    QByteArray      FormatParam(const QByteArray &param);
    QByteArray      GetTagName(uint8_t index);
    QByteArray      GetTagParam(uint8_t index);

    GutEngine(const GutEngine &) = delete;
    GutEngine &operator = (const GutEngine &) = delete;

    const QVector<QByteArray> TAG_TEXT = {
        "Music",
        "LoadMap",
        "CreateActor",
        "DeleteNpc",
        "MapEvent",
        "ActorEvent",
        "Move",
        "ActorMove",
        "ActorSpeed",
        "Callback",
        "Goto",
        "If",
        "Set",
        "Say",
        "StartChapter",
        "ScreenR",
        "ScreenS",
        "ScreenA",
        "Event",
        "Money",
        "Gameover",
        "IfCmp",
        "Add",
        "Sub",
        "SetControlId",
        "GutEvent",
        "SetEvent",
        "ClrEvent",
        "Buy",
        "FaceToFace",
        "Movie",
        "Choice",
        "CreateBox",
        "DeleteBox",
        "GainGoods",
        "InitFight",
        "FightEnable",
        "FightDisenable",
        "CreateNpc",
        "EnterFight",
        "DeleteActor",
        "GainMoney",
        "UseMoney",
        "SetMoney",
        "LearnMagic",
        "Sale",
        "NpcMoveMod",
        "Message",
        "DeleteGoods",
        "ResumeActorHp",
        "ActorLayerUp",
        "BoxOpen",
        "DelAllNpc",
        "NpcStep",
        "SetSceneName",
        "ShowSceneName",
        "ShowScreen",
        "UseGoods",
        "AttribTest",
        "AttribSet",
        "AttribAdd",
        "ShowGut",
        "UseGoodsNum",
        "Randrade",
        "Menu",
        "TestMoney",
        "CallChapter",
        "DisCmp",
        "Return",
        "TimeMsg",
        "DisableSave",
        "EnableSave",
        "GameSave",
        "SetEventTimer",
        "EnableShowPos",
        "DisableShowPos",
        "SetTo",
        "TestGoodsNum",
        "SetFightMiss",
        "SetArmsToss",
        ""
    };

    const QVector<QByteArray> TAG_PARAM = {
        "NN",
        "NNNN",
        "NNN",
        "N",
        "NNNNNN",
        "NA",
        "NNN",
        "NNNNNN",
        "NN",
        "",
        "A",
        "NA",
        "NN",
        "NC",
        "NN",
        "N",
        "NN",
        "N",
        "NA",
        "N",
        "",
        "NNA",
        "NN",
        "NN",
        "N",
        "NE",
        "N",
        "N",
        "U",
        "NN",
        "NNNNN",
        "CCA",
        "NNNN",
        "N",
        "NN",
        "NNNNNNNNNNN",
        "",
        "",
        "NNNN",
        "NNNNNNNNNNNNNAA",
        "N",
        "L",
        "L",
        "L",
        "NNN",
        "",
        "NN",
        "C",
        "NNA",
        "NN",
        "NN",
        "N",
        "",
        "NNN",
        "C",
        "",
        "",
        "NNA",
        "NNNAA",
        "NNN",
        "NNN",
        "NNC",
        "NNNA",
        "NA",
        "NC",
        "LA",
        "NN",
        "NNAA",
        "",
        "NC",
        "",
        "",
        "",
        "NN",
        "",
        "",
        "NN",
        "NNNAA",
        "N",
        "N",
        ""
    };

};

#endif // GUT_ENGINE_H

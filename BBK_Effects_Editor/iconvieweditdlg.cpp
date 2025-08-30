#include "iconvieweditdlg.h"
#include "ui_iconvieweditdlg.h"

IconViewEditDlg::IconViewEditDlg(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::IconViewEditDlg)
{
    ui->setupUi(this);

    ui->dlgBtnBox->button(QDialogButtonBox::Reset)->setText(tr("Reset"));
    ui->dlgBtnBox->button(QDialogButtonBox::Save)->setText(tr("Save"));
    ui->dlgBtnBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    connect(ui->iconViewEdit,           &IconViewEdit::brushTypeChanged,    this, &IconViewEditDlg::on_brushTypeChanged);
    connect(ui->brushBtn,               &QPushButton::clicked,              this, &IconViewEditDlg::on_pushButton_clicked);
}

IconViewEditDlg::~IconViewEditDlg()
{
    delete ui;
}

void IconViewEditDlg::UpdateView(uint8_t *data, uint32_t len)
{
    ui->iconViewEdit->UpdateView(data, len);
    setWindowTitle(QString("%1 x %2").arg(ui->iconViewEdit->GetIconWidth()).arg(ui->iconViewEdit->GetIconHeight()));
}

void IconViewEditDlg::WriteBack(uint8_t *data, uint32_t len)
{
    ui->iconViewEdit->WriteBack(data, len);
}

void IconViewEditDlg::on_brushTypeChanged(IconViewEdit::BrushType brushType)
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

void IconViewEditDlg::on_iconIndex_valueChanged(int index)
{
    ui->iconViewEdit->SetIconIndex(index - 1);
}

void IconViewEditDlg::on_pushButton_clicked()
{
    QPushButton *btn = static_cast<QPushButton *>(sender());
    if(btn == ui->brushBtn) {
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
}

void IconViewEditDlg::on_dlgBtnBox_clicked(QAbstractButton *button)
{
    if(button == ui->dlgBtnBox->button(QDialogButtonBox::Reset)) {
        ui->iconViewEdit->ResetView();
    }
    else if(button == ui->dlgBtnBox->button(QDialogButtonBox::Save)) {
        accept();
    }
    else if(button == ui->dlgBtnBox->button(QDialogButtonBox::Cancel)) {
        reject();
    }
}

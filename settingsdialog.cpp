#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QSettings>
#include <QFileDialog>

SettingsDialog::SettingsDialog(const QString &current_compiler_path, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    ui->compiler_path_input->setText(current_compiler_path);
}


void SettingsDialog::save()
{
    QSettings settings("Compiladores", "IDE");
    settings.setValue("compiler_path", ui->compiler_path_input->text());


}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::on_compiler_path_browse_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Selecionar executável do compilador"),
        ui->compiler_path_input->text(),
        tr("Executáveis (*.exe);;Todos os Arquivos (*)")
        );

    if(!filePath.isEmpty()){
        ui->compiler_path_input->setText(filePath);
    }
}


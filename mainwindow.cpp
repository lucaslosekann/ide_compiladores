#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include <QDebug>
#include <QSettings>
#include <QFile>
#include <QProcess>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->splitter->setSizes({600, 200});
    ui->splitter->setStretchFactor(0, 3);
    ui->splitter->setStretchFactor(1, 1);


    compilerProcess.setProcessChannelMode(
        QProcess::MergedChannels
    );

    connect(
        &compilerProcess,
        &QProcess::readyReadStandardOutput,
        this,
        [this] {
            const QString text = QString::fromLocal8Bit(
                compilerProcess.readAllStandardOutput()
                );

            ui->compiler_output->appendPlainText(text);
            ui->compile_button->setEnabled(true);
        }
    );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_Preferences_triggered()
{
    QSettings settings("Compiladores", "IDE");

    SettingsDialog dialog(settings.value("compiler_path", "").toString(), this);

    if (dialog.exec() == QDialog::Accepted) {
        dialog.save();
    }
}



void MainWindow::on_compile_button_clicked()
{
    ui->compile_button->setEnabled(false);
    ui->compiler_output->appendPlainText("\n>>Iniciando Compilação<<\n");


    if (!buildDirectory.isValid()) {
        ui->compiler_output->appendPlainText(
            "ERRO: Não foi possível criar o diretório temporário."
            );
        ui->compile_button->setEnabled(true);
        return;
    }

    const QString sourcePath =
        buildDirectory.filePath("main");

    QFile sourceFile(sourcePath);


    if (!sourceFile.open(QIODevice::WriteOnly |
                         QIODevice::Truncate)) {
        ui->compiler_output->appendPlainText(
            "ERRO: Não foi possível criar o arquivo:\n" +
            sourceFile.errorString()
            );
        ui->compile_button->setEnabled(true);
        return;
    }

    const QByteArray sourceCode =
        ui->code_editor->toPlainText().toUtf8();

    if (sourceFile.write(sourceCode) != sourceCode.size()) {
        ui->compiler_output->appendPlainText(
            "ERRO: Não foi possível escrever todo o código fonte."
            );
        ui->compile_button->setEnabled(true);
        return;
    }

    sourceFile.close();

    //ui->compiler_output->appendPlainText("Código fonte salvo em:\n" +
    //                                     sourcePath + "\n");

    QSettings settings("Compiladores", "IDE");
    QString compiler_path = settings.value("compiler_path", "").toString();

    if(compiler_path.isEmpty()){
        ui->compiler_output->appendPlainText("ERRO: Adicione um caminho para o compilador em Editar>Preferências");
        ui->compile_button->setEnabled(true);
        return;
    }

    compilerProcess.start(
        compiler_path,
        QStringList() << sourcePath
    );

}


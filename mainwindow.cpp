#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include <QDebug>
#include <QSettings>
#include <QFile>
#include <QProcess>
#include <QHeaderView>
#include <QTableWidgetItem>

namespace {
const QStringList SYMBOL_TABLE_COLUMNS = {
    "id", "tipo", "modalidade", "ini", "usada", "escopo",
    "param", "pos", "vet", "matriz", "ref", "func"
};
const QString SYMBOL_LINE_PREFIX = "@TS|";
}


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

    setupSymbolTable();

    connect(
        &compilerProcess,
        &QProcess::readyReadStandardOutput,
        this,
        [this] {
            compilerOutputBuffer += compilerProcess.readAllStandardOutput();
        }
    );

    connect(
        &compilerProcess,
        &QProcess::finished,
        this,
        [this] {
            compilerOutputBuffer += compilerProcess.readAllStandardOutput();
            showCompilerOutput();
            ui->compile_button->setEnabled(true);
        }
    );

    connect(
        &compilerProcess,
        &QProcess::errorOccurred,
        this,
        [this](QProcess::ProcessError error) {
            if (error == QProcess::FailedToStart) {
                ui->compiler_output->appendPlainText(
                    "ERRO: Não foi possível executar o compilador. Verifique o caminho em Editar>Preferências."
                    );
                ui->compile_button->setEnabled(true);
            }
        }
    );
}

void MainWindow::setupSymbolTable()
{
    ui->symbol_table->setColumnCount(SYMBOL_TABLE_COLUMNS.size());
    ui->symbol_table->setHorizontalHeaderLabels(SYMBOL_TABLE_COLUMNS);
    ui->symbol_table->verticalHeader()->setVisible(false);
    ui->symbol_table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->symbol_table->horizontalHeader()->setStretchLastSection(true);
}

void MainWindow::showCompilerOutput()
{
    const QStringList lines = QString::fromUtf8(compilerOutputBuffer).split('\n');
    compilerOutputBuffer.clear();

    QStringList messages;
    ui->symbol_table->setRowCount(0);

    for (const QString &line : lines) {
        if (!line.startsWith(SYMBOL_LINE_PREFIX)) {
            messages << line;
            continue;
        }

        const QStringList fields = line.mid(SYMBOL_LINE_PREFIX.size()).split('|');
        const int row = ui->symbol_table->rowCount();
        ui->symbol_table->insertRow(row);

        for (int column = 0; column < fields.size() && column < SYMBOL_TABLE_COLUMNS.size(); column++) {
            auto *item = new QTableWidgetItem(fields[column]);
            item->setTextAlignment(Qt::AlignCenter);
            ui->symbol_table->setItem(row, column, item);
        }
    }

    ui->compiler_output->appendPlainText(messages.join('\n').trimmed());
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
    ui->compiler_output->clear();
    ui->symbol_table->setRowCount(0);
    compilerOutputBuffer.clear();
    ui->output_tabs->setCurrentWidget(ui->output_tab);
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
        ui->code_editor->toPlainText().toLatin1();

    if (sourceFile.write(sourceCode) != sourceCode.size()) {
        ui->compiler_output->appendPlainText(
            "ERRO: Não foi possível escrever todo o código fonte."
            );
        ui->compile_button->setEnabled(true);
        return;
    }

    sourceFile.close();

    QSettings settings("Compiladores", "IDE");
    QString compiler_path = settings.value("compiler_path", "").toString();

    if(compiler_path.isEmpty()){
        ui->compiler_output->appendPlainText("ERRO: Adicione um caminho para o compilador em Editar>Preferências");
        ui->compile_button->setEnabled(true);
        return;
    }

    compilerProcess.start(
        compiler_path,
        QStringList() << "--ide" << sourcePath
    );

}


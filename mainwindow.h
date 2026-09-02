#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTemporaryDir>
#include <QProcess>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_action_Preferences_triggered();

    void on_compile_button_clicked();

private:
    QProcess compilerProcess;
    QTemporaryDir buildDirectory;

    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H

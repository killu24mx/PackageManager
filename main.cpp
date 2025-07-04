
#include <QApplication>
#include "guimanager.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GuiManager guiManager;

    guiManager.showGui();




    return a.exec();
}

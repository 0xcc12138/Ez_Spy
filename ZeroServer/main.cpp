#include "m_Window.h"
#include <QtWidgets/QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    m_Window w;
    w.show();

    return a.exec();
}

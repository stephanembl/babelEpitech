#ifndef MYAPPLICATION_H
#define MYAPPLICATION_H

#include <QApplication>

class MyApplication : public QApplication
{
public:
    MyApplication(int ac, char **av)
        : QApplication(ac, av)
    {}
    ~MyApplication() {};
};

#endif // MYGUI_H

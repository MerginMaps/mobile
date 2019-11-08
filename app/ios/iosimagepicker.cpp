#include "iosimagepicker.h"
#include "ioshandler.h"

IOSImagePicker::IOSImagePicker(QObject *parent) : QObject(parent)
{

}

void IOSImagePicker::test()
{
    IOSHandler::objectiveC_Call();
}

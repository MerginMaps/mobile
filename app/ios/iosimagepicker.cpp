#include "iosimagepicker.h"
#include "ioshandler.h"

IOSImagePicker::IOSImagePicker(QObject *parent) : QObject(parent)
{

}

void IOSImagePicker::test()
{
    //IOSHandler* systemHandler = IOSHandler::instance();

    //systemHandler->
    IOSHandler::objectiveC_Call();
}

//int IOSImagePicker::someMethod(void *objectiveCObject, void *aParameter)
//{
//    // To invoke an Objective-C method from C++, use
//       // the C trampoline function
//       return MyObjectDoSomethingWith (objectiveCObject, aParameter);
//}

Rules for a Qml [new design] developers
- main folder input/app/qmlV2/
- all components, pages, map items, ... in its subfolder
- all items begins MM, for example MMButton
- using required key before property, if it makes sense
- using property alias, if it makes sense
- using qmlfmt tool for Qml formatting, see qmltool section
- using GalleryApp for creating/updating qml files, see GalleryApp section

qmltool
- git clone https://github.com/jesperhh/qmlfmt.git
- cd qmlfmt 
- git submodule update --init qt-creator
- mkdir build  
- cd build
- export Qt6_DIR="/your_path/Qt/6.5.2/macos/lib/cmake/Qt6"
- export Qt6GuiTools_DIR="/your_path/Qt/6.5.2/macos/lib/cmake/Qt6GuiTools"
- export Qt6Test_DIR="/your_path/Qt/6.5.2/macos/lib/cmake/Qt6Test" 
- cmake ..
- make
- ./qmlfmt -wi2 path_to_qml_dir

GalleryApp
- located in input/GalleryApp/
- uses qml files from input/app/qmlV2/
- as wrapper for qml files see input/GalleryApp/qml/
- works hot reload (tested on Mac), if any problem see script in HotReload class

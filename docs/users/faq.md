# FAQ

Here is a list of common issues you might experience:

**What is an Input project?**

An Input project consists of a folder (wiht subfolders) with **one** QGIS project file and all its associated layers. The QGIS project is linked with relative path (the default option in QGIS) to all the layers.

**Why am not able to see my project in the list (under Input > Projects > My Projects**

It is likely that you have no QGIS project (the extension for QGIS project is qgs or qgz file) or more than one project in your folder. To have a valid Input project you need **one and only one** QGIS project in your folder.

**I have loaded my project in Input, but I cannot see all my layers**

You need to package all your layers with the folder uploaded to Mergin. They should be also linked accordingly to the packaged folder. For example, if you have a project where it has links to your shared server layers, those layers will not appear in Input.

**When I press Record, Input tells me there is no survey layers in the project**

- Ensure you have all your survey layers uploaded with the project
- Ensure your survey layer is not marked as Read-only (Project Properties > Data Sources)

**When I synchronise my data, I get some files with 'conf' in their names**

Input supports collaborative editing, if you use Geopackage file format for your survey layer. If you use for example ESRI Shapefile for your survey layer, Input is not able to detect changes from other users and overwrites them. It will keep a copy of the overwritten files with 'conf' in the name. So, in summary, **always use Geopackage for your survey layer**.

**I use Geopackage for my survey layer, but I still see 'conf' files**

 Input does not support changing of the data schema. So, if you delete or add columns to your survey layer, Input cannot detect the change. Therefore, it is always recommended to stick with the same attribute table structure once you have set up your survey layer.
 
 **I cannot synchronise changes from the Mergin server**
 
 - Make sure you have data connectivity.
 - Ensure you have not exceeded or reached your storage allowance
 
 

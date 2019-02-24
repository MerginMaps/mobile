# Preparing project


Project preparation is done in QGIS. For more information about loading
layers, styling the data and creating map themes, visit QGIS
documentation page. In addition, Input uses some of the features within
the project to help with visualisation, data capturing and interrogation
of the data. Below are the key parts used by Input:

## Background layers

If you have internet connectivity, you can use a WM(T)S or XYZ layer as
your background map. QGIS can also reads local XYZ tiles.

If you are using a vector layer for background layer:

-   Ensure to use a simplified geometry version of your data. This will help with smooth map navigation.

-   All vector layers not intended to be used as survey layers, have to be set as read-only. To make a vector layer read-only in QGIS, from the main menu select Project \> Properties. Within the window, select Data Source tab and select the Read-only option for the layers you do not want to be used as survey layer in Input.

## Survey layer

Vector layers (currently points and lines) can be used as survey layer
in Input. You can apply style and set up the forms according to the QGIS
documentation.

## Layer properties
The following properties of a layer in QGIS project will be additionally used in Input:

### Settings for Input preview panel
Similar to Google map, when a feature is identified on the map a preview panel will appear. To enable a layer being identified, the option should be enabled in QGIS desktop, under **Project properties**. For setting the preview panel, you need to open ** Layer Properties ** and select ** Display ** tab.

- title - this will use "Display Name" (in layer properties > Display tab) which can be a field name or an expression.

- content - there are three options for what to show in content area: 1. field values, 2. image, 3. HTML. This will use "HTML Map Tip" (in layer properties > Display tab). QGIS always interprets the content of map tip as being HTML, however Input extends the syntax to allow two more modes. If the map tip is not specified, Input will try to use the first three fields and show their attribute values.

#### Field values

Sample map tip content that will show "description" and "time" field values:

```
# fields
description
time
```

If the map tip content has `# fields` marker on the first line, the following lines will be understood as field names that should be listed in the preview.
At most three fields will be shown. Expressions are not allowed.

#### Image

Sample map tip content that will cause an image to be show specified by file path in field "image_1" (containing path relative to the project directory):

```
# image
file:///[%@project_folder%]/[% "image_1" %]
```

If the map tip has `# image` marker on the first line, the following line is understood as the URL for the image. It can be a regular file on the file system, but it could be even a remote image from network. Expressions embedded in the image URL will be evaluated (enclosed in `[% 1+1 %]`).

#### HTML

Sample map tip content that will show render as HTML page:

```
<p><strong>Notes:</strong>[% "notes" %]</p>
```

If the map tip does not contain any special marker, it is assumed that the map tip is HTML content. Only a limited subset of HTML is supported - see https://doc.qt.io/qt-5/richtext-html-subset.html

Example of the preview panel in Input and QGIS tooltip set up can be seen below:

<img src="images/qgis_properties_display.png" alt="Display setting in QGIS" style="width: 50%; height: 50%"/>​

<img src="images/input_preview_panel.png" alt="Preview panel in Input based on Display settings in QGIS" style="width: 25%; height: 25%"/>​

### Forms

To be able to attach photos to a survey feature, you will need to have an attribute column in your survey layer. Within the form setting, ensure to set Widget type to Attachment.

Make use of Value Map widget in your forms to simplify filling the forms during survey.

## Project settings

Having Map Themes, will help switching between different background layers in Input (e.g. one map theme set up with aerial photo and another with cartography rasters.)

Ensure the paths are set to Relative under Project Properties. All paths to the project data in Input is relative to the project location.

Input does not include all the SVGs within QGIS. Therefore, if you are using SVGs for your layer styling, ensure those are copied across to the project folder.

Input currently cannot read QGZ project at the moment. Ensure to save your project as QGS file.

An example of Project Properties in QGIS for survey layers and identifiable layers can be seen below:

<img src="images/qgis_project_properties.png" alt="Preview panel in Input based on Display settings in QGIS" style="width: 50%; height: 50%"/>​

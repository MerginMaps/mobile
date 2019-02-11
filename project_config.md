
# How to prepare QGIS projects for Input


## Preview Panel

When a feature is identified, the preview panel is shown:

- title - this will use "Display Name" (in layer properties > Display tab) which can be a field name or an expression.

- content - there are three options for what to show in content area: 1. field values, 2. image, 3. HTML. This will use "HTML Map Tip" (in layer properties > Display tab). QGIS always interprets the content of map tip as being HTML, however Input extends the syntax to allow two more modes. If the map tip is not specified, Input will try to use the first three fields and show their attribute values.

### Field values

Sample map tip content that will show "description" and "time" field values:

```
# fields
description
time
```

If the map tip content has `# fields` marker on the first line, the following lines will be understood as field names that should be listed in the preview.
At most three fields will be shown. Expressions are not allowed.

### Image

Sample map tip content that will cause an image to be show specified by file path in field "image_1" (containing path relative to the project directory):

```
# image
file:///[%@project_folder%]/[% "image_1" %]
```

If the map tip has `# image` marker on the first line, the following line is understood as the URL for the image. It can be a regular file on the file system, but it could be even a remote image from network. Expressions embedded in the image URL will be evaluated (enclosed in `[% 1+1 %]`).

### HTML

Sample map tip content that will show render as HTML page:

```
<p><strong>Notes:</strong>[% "notes" %]</p>
```

If the map tip does not contain any special marker, it is assumed that the map tip is HTML content. Only a limited subset of HTML is supported - see https://doc.qt.io/qt-5/richtext-html-subset.html

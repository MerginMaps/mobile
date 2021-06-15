<!DOCTYPE qgis PUBLIC 'http://mrcc.com/qgis.dtd' 'SYSTEM'>
<qgis version="3.14.15-Pi" labelsEnabled="0" styleCategories="Symbology|Labeling|Fields|Forms">
  <renderer-v2 type="categorizedSymbol" forceraster="0" symbollevels="0" attr="Class" enableorderby="0">
    <categories>
      <category label="B52" render="true" symbol="0" value="B52"/>
      <category label="Biplane" render="true" symbol="1" value="Biplane"/>
      <category label="Jet" render="true" symbol="2" value="Jet"/>
    </categories>
    <symbols>
      <symbol type="marker" name="0" alpha="1" force_rhr="0" clip_to_extent="1">
        <layer enabled="1" pass="0" locked="0" class="SvgMarker">
          <prop k="angle" v="0"/>
          <prop k="color" v="189,189,189,255"/>
          <prop k="fixedAspectRatio" v="0"/>
          <prop k="horizontal_anchor_point" v="1"/>
          <prop k="name" v="gpsicons/plane.svg"/>
          <prop k="offset" v="0,0"/>
          <prop k="offset_map_unit_scale" v="3x:0,0,0,0,0,0"/>
          <prop k="offset_unit" v="MM"/>
          <prop k="outline_color" v="0,0,0,255"/>
          <prop k="outline_width" v="0.2"/>
          <prop k="outline_width_map_unit_scale" v="3x:0,0,0,0,0,0"/>
          <prop k="outline_width_unit" v="MM"/>
          <prop k="scale_method" v="diameter"/>
          <prop k="size" v="11"/>
          <prop k="size_map_unit_scale" v="3x:0,0,0,0,0,0"/>
          <prop k="size_unit" v="MM"/>
          <prop k="vertical_anchor_point" v="1"/>
          <data_defined_properties>
            <Option type="Map">
              <Option type="QString" name="name" value=""/>
              <Option type="Map" name="properties">
                <Option type="Map" name="angle">
                  <Option type="bool" name="active" value="true"/>
                  <Option type="QString" name="field" value="Heading"/>
                  <Option type="int" name="type" value="2"/>
                </Option>
              </Option>
              <Option type="QString" name="type" value="collection"/>
            </Option>
          </data_defined_properties>
        </layer>
      </symbol>
      <symbol type="marker" name="1" alpha="1" force_rhr="0" clip_to_extent="1">
        <layer enabled="1" pass="0" locked="0" class="SvgMarker">
          <prop k="angle" v="0"/>
          <prop k="color" v="124,124,124,255"/>
          <prop k="fixedAspectRatio" v="0"/>
          <prop k="horizontal_anchor_point" v="1"/>
          <prop k="name" v="transport/transport_airport.svg"/>
          <prop k="offset" v="0,0"/>
          <prop k="offset_map_unit_scale" v="3x:0,0,0,0,0,0"/>
          <prop k="offset_unit" v="MM"/>
          <prop k="outline_color" v="0,0,0,255"/>
          <prop k="outline_width" v="0.2"/>
          <prop k="outline_width_map_unit_scale" v="3x:0,0,0,0,0,0"/>
          <prop k="outline_width_unit" v="MM"/>
          <prop k="scale_method" v="diameter"/>
          <prop k="size" v="18"/>
          <prop k="size_map_unit_scale" v="3x:0,0,0,0,0,0"/>
          <prop k="size_unit" v="MM"/>
          <prop k="vertical_anchor_point" v="1"/>
          <data_defined_properties>
            <Option type="Map">
              <Option type="QString" name="name" value=""/>
              <Option type="Map" name="properties">
                <Option type="Map" name="angle">
                  <Option type="bool" name="active" value="true"/>
                  <Option type="QString" name="field" value="Heading"/>
                  <Option type="int" name="type" value="2"/>
                </Option>
              </Option>
              <Option type="QString" name="type" value="collection"/>
            </Option>
          </data_defined_properties>
        </layer>
      </symbol>
      <symbol type="marker" name="2" alpha="1" force_rhr="0" clip_to_extent="1">
        <layer enabled="1" pass="0" locked="0" class="SvgMarker">
          <prop k="angle" v="0"/>
          <prop k="color" v="178,223,138,255"/>
          <prop k="fixedAspectRatio" v="0"/>
          <prop k="horizontal_anchor_point" v="1"/>
          <prop k="name" v="transport/transport_airport2.svg"/>
          <prop k="offset" v="0,0"/>
          <prop k="offset_map_unit_scale" v="3x:0,0,0,0,0,0"/>
          <prop k="offset_unit" v="MM"/>
          <prop k="outline_color" v="0,0,0,255"/>
          <prop k="outline_width" v="0.2"/>
          <prop k="outline_width_map_unit_scale" v="3x:0,0,0,0,0,0"/>
          <prop k="outline_width_unit" v="MM"/>
          <prop k="scale_method" v="diameter"/>
          <prop k="size" v="11"/>
          <prop k="size_map_unit_scale" v="3x:0,0,0,0,0,0"/>
          <prop k="size_unit" v="MM"/>
          <prop k="vertical_anchor_point" v="1"/>
          <data_defined_properties>
            <Option type="Map">
              <Option type="QString" name="name" value=""/>
              <Option type="Map" name="properties">
                <Option type="Map" name="angle">
                  <Option type="bool" name="active" value="true"/>
                  <Option type="QString" name="field" value="Heading"/>
                  <Option type="int" name="type" value="2"/>
                </Option>
              </Option>
              <Option type="QString" name="type" value="collection"/>
            </Option>
          </data_defined_properties>
        </layer>
      </symbol>
    </symbols>
    <rotation/>
    <sizescale/>
  </renderer-v2>
  <blendMode>0</blendMode>
  <featureBlendMode>0</featureBlendMode>
  <fieldConfiguration>
    <field name="Class">
      <editWidget type="ValueMap">
        <config>
          <Option type="Map">
            <Option type="Map" name="map">
              <Option type="QString" name="B52" value="B52"/>
              <Option type="QString" name="Biplane" value="Biplane"/>
              <Option type="QString" name="Jet" value="Jet"/>
            </Option>
          </Option>
        </config>
      </editWidget>
    </field>
    <field name="Heading">
      <editWidget type="TextEdit">
        <config>
          <Option type="Map">
            <Option type="QString" name="IsMultiline" value="0"/>
            <Option type="QString" name="UseHtml" value="0"/>
          </Option>
        </config>
      </editWidget>
    </field>
    <field name="Importance">
      <editWidget type="TextEdit">
        <config>
          <Option type="Map">
            <Option type="QString" name="IsMultiline" value="0"/>
            <Option type="QString" name="UseHtml" value="0"/>
          </Option>
        </config>
      </editWidget>
    </field>
    <field name="Pilots">
      <editWidget type="TextEdit">
        <config>
          <Option type="Map">
            <Option type="QString" name="IsMultiline" value="0"/>
            <Option type="QString" name="UseHtml" value="0"/>
          </Option>
        </config>
      </editWidget>
    </field>
    <field name="Cabin Crew">
      <editWidget type="TextEdit">
        <config>
          <Option type="Map">
            <Option type="QString" name="IsMultiline" value="0"/>
            <Option type="QString" name="UseHtml" value="0"/>
          </Option>
        </config>
      </editWidget>
    </field>
    <field name="Staff">
      <editWidget type="TextEdit">
        <config>
          <Option type="Map">
            <Option type="QString" name="IsMultiline" value="0"/>
            <Option type="QString" name="UseHtml" value="0"/>
          </Option>
        </config>
      </editWidget>
    </field>
  </fieldConfiguration>
  <aliases>
    <alias name="" field="Class" index="0"/>
    <alias name="" field="Heading" index="1"/>
    <alias name="" field="Importance" index="2"/>
    <alias name="" field="Pilots" index="3"/>
    <alias name="" field="Cabin Crew" index="4"/>
    <alias name="" field="Staff" index="5"/>
  </aliases>
  <excludeAttributesWMS/>
  <excludeAttributesWFS/>
  <defaults>
    <default field="Class" expression="" applyOnUpdate="0"/>
    <default field="Heading" expression="" applyOnUpdate="0"/>
    <default field="Importance" expression="" applyOnUpdate="0"/>
    <default field="Pilots" expression="" applyOnUpdate="0"/>
    <default field="Cabin Crew" expression="" applyOnUpdate="0"/>
    <default field="Staff" expression="" applyOnUpdate="0"/>
  </defaults>
  <constraints>
    <constraint field="Class" unique_strength="0" exp_strength="0" constraints="0" notnull_strength="0"/>
    <constraint field="Heading" unique_strength="0" exp_strength="0" constraints="0" notnull_strength="0"/>
    <constraint field="Importance" unique_strength="0" exp_strength="0" constraints="0" notnull_strength="0"/>
    <constraint field="Pilots" unique_strength="0" exp_strength="0" constraints="0" notnull_strength="0"/>
    <constraint field="Cabin Crew" unique_strength="0" exp_strength="0" constraints="0" notnull_strength="0"/>
    <constraint field="Staff" unique_strength="0" exp_strength="0" constraints="0" notnull_strength="0"/>
  </constraints>
  <constraintExpressions>
    <constraint desc="" field="Class" exp=""/>
    <constraint desc="" field="Heading" exp=""/>
    <constraint desc="" field="Importance" exp=""/>
    <constraint desc="" field="Pilots" exp=""/>
    <constraint desc="" field="Cabin Crew" exp=""/>
    <constraint desc="" field="Staff" exp=""/>
  </constraintExpressions>
  <expressionfields/>
  <editform tolerant="1"></editform>
  <editforminit/>
  <editforminitcodesource>0</editforminitcodesource>
  <editforminitfilepath>/Users/peter/Projects/quick/input/qgsquick/from_qgis/src/quickgui/app/qgis-data</editforminitfilepath>
  <editforminitcode><![CDATA[# -*- coding: utf-8 -*-
"""
QGIS forms can have a Python function that is called when the form is
opened.

Use this function to add extra logic to your forms.

Enter the name of the function in the "Python Init function"
field.
An example follows:
"""
from qgis.PyQt.QtWidgets import QWidget

def my_form_open(dialog, layer, feature):
	geom = feature.geometry()
	control = dialog.findChild(QWidget, "MyLineEdit")
]]></editforminitcode>
  <featformsuppress>0</featformsuppress>
  <editorlayout>tablayout</editorlayout>
  <attributeEditorForm>
    <attributeEditorContainer visibilityExpressionEnabled="0" name="General" visibilityExpression="" showLabel="1" columnCount="1" groupBox="1">
      <attributeEditorField name="Class" index="0" showLabel="1"/>
      <attributeEditorField name="Heading" index="1" showLabel="1"/>
      <attributeEditorField name="Importance" index="2" showLabel="1"/>
    </attributeEditorContainer>
    <attributeEditorContainer visibilityExpressionEnabled="0" name="People" visibilityExpression="" showLabel="1" columnCount="1" groupBox="1">
      <attributeEditorField name="Pilots" index="3" showLabel="1"/>
      <attributeEditorField name="Cabin Crew" index="4" showLabel="1"/>
      <attributeEditorField name="Staff" index="5" showLabel="1"/>
    </attributeEditorContainer>
  </attributeEditorForm>
  <editable>
    <field name="Cabin Crew" editable="1"/>
    <field name="Class" editable="1"/>
    <field name="Heading" editable="1"/>
    <field name="Importance" editable="1"/>
    <field name="Pilots" editable="1"/>
    <field name="Staff" editable="1"/>
  </editable>
  <labelOnTop>
    <field name="Cabin Crew" labelOnTop="0"/>
    <field name="Class" labelOnTop="0"/>
    <field name="Heading" labelOnTop="0"/>
    <field name="Importance" labelOnTop="0"/>
    <field name="Pilots" labelOnTop="0"/>
    <field name="Staff" labelOnTop="0"/>
  </labelOnTop>
  <dataDefinedFieldProperties/>
  <widgets/>
  <layerGeometryType>0</layerGeometryType>
</qgis>

<!DOCTYPE qgis PUBLIC 'http://mrcc.com/qgis.dtd' 'SYSTEM'>
<qgis maxScale="1" labelsEnabled="0" minScale="1e+08" simplifyDrawingHints="0" simplifyAlgorithm="0" simplifyLocal="1" readOnly="0" simplifyDrawingTol="1" version="3.10.0-A Coruña" simplifyMaxScale="1" styleCategories="AllStyleCategories" hasScaleBasedVisibilityFlag="0">
  <flags>
    <Identifiable>1</Identifiable>
    <Removable>1</Removable>
    <Searchable>1</Searchable>
  </flags>
  <renderer-v2 type="categorizedSymbol" attr="Class" symbollevels="0" forceraster="0" enableorderby="0">
    <categories>
      <category value="B52" symbol="0" label="B52" render="true"/>
      <category value="Biplane" symbol="1" label="Biplane" render="true"/>
      <category value="Jet" symbol="2" label="Jet" render="true"/>
    </categories>
    <symbols>
      <symbol force_rhr="0" type="marker" clip_to_extent="1" name="0" alpha="1">
        <layer class="SvgMarker" pass="0" enabled="1" locked="0">
          <prop v="0" k="angle"/>
          <prop v="189,189,189,255" k="color"/>
          <prop v="0" k="fixedAspectRatio"/>
          <prop v="1" k="horizontal_anchor_point"/>
          <prop v="gpsicons/plane.svg" k="name"/>
          <prop v="0,0" k="offset"/>
          <prop v="3x:0,0,0,0,0,0" k="offset_map_unit_scale"/>
          <prop v="MM" k="offset_unit"/>
          <prop v="0,0,0,255" k="outline_color"/>
          <prop v="0.2" k="outline_width"/>
          <prop v="3x:0,0,0,0,0,0" k="outline_width_map_unit_scale"/>
          <prop v="MM" k="outline_width_unit"/>
          <prop v="diameter" k="scale_method"/>
          <prop v="11" k="size"/>
          <prop v="3x:0,0,0,0,0,0" k="size_map_unit_scale"/>
          <prop v="MM" k="size_unit"/>
          <prop v="1" k="vertical_anchor_point"/>
          <data_defined_properties>
            <Option type="Map">
              <Option type="QString" value="" name="name"/>
              <Option type="Map" name="properties">
                <Option type="Map" name="angle">
                  <Option type="bool" value="true" name="active"/>
                  <Option type="QString" value="Heading" name="field"/>
                  <Option type="int" value="2" name="type"/>
                </Option>
              </Option>
              <Option type="QString" value="collection" name="type"/>
            </Option>
          </data_defined_properties>
        </layer>
      </symbol>
      <symbol force_rhr="0" type="marker" clip_to_extent="1" name="1" alpha="1">
        <layer class="SvgMarker" pass="0" enabled="1" locked="0">
          <prop v="0" k="angle"/>
          <prop v="124,124,124,255" k="color"/>
          <prop v="0" k="fixedAspectRatio"/>
          <prop v="1" k="horizontal_anchor_point"/>
          <prop v="transport/transport_airport.svg" k="name"/>
          <prop v="0,0" k="offset"/>
          <prop v="3x:0,0,0,0,0,0" k="offset_map_unit_scale"/>
          <prop v="MM" k="offset_unit"/>
          <prop v="0,0,0,255" k="outline_color"/>
          <prop v="0.2" k="outline_width"/>
          <prop v="3x:0,0,0,0,0,0" k="outline_width_map_unit_scale"/>
          <prop v="MM" k="outline_width_unit"/>
          <prop v="diameter" k="scale_method"/>
          <prop v="18" k="size"/>
          <prop v="3x:0,0,0,0,0,0" k="size_map_unit_scale"/>
          <prop v="MM" k="size_unit"/>
          <prop v="1" k="vertical_anchor_point"/>
          <data_defined_properties>
            <Option type="Map">
              <Option type="QString" value="" name="name"/>
              <Option type="Map" name="properties">
                <Option type="Map" name="angle">
                  <Option type="bool" value="true" name="active"/>
                  <Option type="QString" value="Heading" name="field"/>
                  <Option type="int" value="2" name="type"/>
                </Option>
              </Option>
              <Option type="QString" value="collection" name="type"/>
            </Option>
          </data_defined_properties>
        </layer>
      </symbol>
      <symbol force_rhr="0" type="marker" clip_to_extent="1" name="2" alpha="1">
        <layer class="SvgMarker" pass="0" enabled="1" locked="0">
          <prop v="0" k="angle"/>
          <prop v="178,223,138,255" k="color"/>
          <prop v="0" k="fixedAspectRatio"/>
          <prop v="1" k="horizontal_anchor_point"/>
          <prop v="transport/transport_airport2.svg" k="name"/>
          <prop v="0,0" k="offset"/>
          <prop v="3x:0,0,0,0,0,0" k="offset_map_unit_scale"/>
          <prop v="MM" k="offset_unit"/>
          <prop v="0,0,0,255" k="outline_color"/>
          <prop v="0.2" k="outline_width"/>
          <prop v="3x:0,0,0,0,0,0" k="outline_width_map_unit_scale"/>
          <prop v="MM" k="outline_width_unit"/>
          <prop v="diameter" k="scale_method"/>
          <prop v="11" k="size"/>
          <prop v="3x:0,0,0,0,0,0" k="size_map_unit_scale"/>
          <prop v="MM" k="size_unit"/>
          <prop v="1" k="vertical_anchor_point"/>
          <data_defined_properties>
            <Option type="Map">
              <Option type="QString" value="" name="name"/>
              <Option type="Map" name="properties">
                <Option type="Map" name="angle">
                  <Option type="bool" value="true" name="active"/>
                  <Option type="QString" value="Heading" name="field"/>
                  <Option type="int" value="2" name="type"/>
                </Option>
              </Option>
              <Option type="QString" value="collection" name="type"/>
            </Option>
          </data_defined_properties>
        </layer>
      </symbol>
    </symbols>
    <rotation/>
    <sizescale/>
  </renderer-v2>
  <customproperties>
    <property key="dualview/previewExpressions">
      <value>COALESCE( "Class", '&lt;NULL>' )</value>
    </property>
    <property key="embeddedWidgets/count" value="0"/>
    <property key="variableNames"/>
    <property key="variableValues"/>
  </customproperties>
  <blendMode>0</blendMode>
  <featureBlendMode>0</featureBlendMode>
  <layerOpacity>1</layerOpacity>
  <SingleCategoryDiagramRenderer diagramType="Pie" attributeLegend="1">
    <DiagramCategory scaleDependency="Area" backgroundAlpha="255" minScaleDenominator="1" maxScaleDenominator="1e+08" barWidth="5" penAlpha="255" penColor="#000000" enabled="0" labelPlacementMethod="XHeight" diagramOrientation="Up" height="15" opacity="1" lineSizeType="MM" minimumSize="0" rotationOffset="270" sizeType="MM" backgroundColor="#ffffff" sizeScale="3x:0,0,0,0,0,0" lineSizeScale="3x:0,0,0,0,0,0" penWidth="0" scaleBasedVisibility="0" width="15">
      <fontProperties style="" description="Ubuntu,13,-1,5,50,0,0,0,0,0"/>
      <attribute field="" label="" color="#000000"/>
    </DiagramCategory>
  </SingleCategoryDiagramRenderer>
  <DiagramLayerSettings priority="0" zIndex="0" showAll="1" linePlacementFlags="2" obstacle="0" placement="0" dist="0">
    <properties>
      <Option type="Map">
        <Option type="QString" value="" name="name"/>
        <Option type="Map" name="properties">
          <Option type="Map" name="show">
            <Option type="bool" value="true" name="active"/>
            <Option type="QString" value="Class" name="field"/>
            <Option type="int" value="2" name="type"/>
          </Option>
        </Option>
        <Option type="QString" value="collection" name="type"/>
      </Option>
    </properties>
  </DiagramLayerSettings>
  <geometryOptions removeDuplicateNodes="0" geometryPrecision="0">
    <activeChecks/>
    <checkConfiguration/>
  </geometryOptions>
  <fieldConfiguration>
    <field name="Class">
      <editWidget type="ValueMap">
        <config>
          <Option type="Map">
            <Option type="Map" name="map">
              <Option type="QString" value="B52" name="B52"/>
              <Option type="QString" value="Biplane" name="Biplane"/>
              <Option type="QString" value="Jet" name="Jet"/>
            </Option>
          </Option>
        </config>
      </editWidget>
    </field>
    <field name="Heading">
      <editWidget type="TextEdit">
        <config>
          <Option type="Map">
            <Option type="QString" value="0" name="IsMultiline"/>
            <Option type="QString" value="0" name="UseHtml"/>
          </Option>
        </config>
      </editWidget>
    </field>
    <field name="Importance">
      <editWidget type="TextEdit">
        <config>
          <Option type="Map">
            <Option type="QString" value="0" name="IsMultiline"/>
            <Option type="QString" value="0" name="UseHtml"/>
          </Option>
        </config>
      </editWidget>
    </field>
    <field name="Pilots">
      <editWidget type="TextEdit">
        <config>
          <Option type="Map">
            <Option type="QString" value="0" name="IsMultiline"/>
            <Option type="QString" value="0" name="UseHtml"/>
          </Option>
        </config>
      </editWidget>
    </field>
    <field name="Cabin Crew">
      <editWidget type="TextEdit">
        <config>
          <Option type="Map">
            <Option type="QString" value="0" name="IsMultiline"/>
            <Option type="QString" value="0" name="UseHtml"/>
          </Option>
        </config>
      </editWidget>
    </field>
    <field name="Staff">
      <editWidget type="TextEdit">
        <config>
          <Option type="Map">
            <Option type="QString" value="0" name="IsMultiline"/>
            <Option type="QString" value="0" name="UseHtml"/>
          </Option>
        </config>
      </editWidget>
    </field>
  </fieldConfiguration>
  <aliases>
    <alias field="Class" name="" index="0"/>
    <alias field="Heading" name="" index="1"/>
    <alias field="Importance" name="" index="2"/>
    <alias field="Pilots" name="" index="3"/>
    <alias field="Cabin Crew" name="" index="4"/>
    <alias field="Staff" name="" index="5"/>
  </aliases>
  <excludeAttributesWMS/>
  <excludeAttributesWFS/>
  <defaults>
    <default applyOnUpdate="0" expression="" field="Class"/>
    <default applyOnUpdate="0" expression="" field="Heading"/>
    <default applyOnUpdate="0" expression="" field="Importance"/>
    <default applyOnUpdate="0" expression="" field="Pilots"/>
    <default applyOnUpdate="0" expression="" field="Cabin Crew"/>
    <default applyOnUpdate="0" expression="" field="Staff"/>
  </defaults>
  <constraints>
    <constraint constraints="0" field="Class" unique_strength="0" exp_strength="0" notnull_strength="0"/>
    <constraint constraints="0" field="Heading" unique_strength="0" exp_strength="0" notnull_strength="0"/>
    <constraint constraints="0" field="Importance" unique_strength="0" exp_strength="0" notnull_strength="0"/>
    <constraint constraints="0" field="Pilots" unique_strength="0" exp_strength="0" notnull_strength="0"/>
    <constraint constraints="0" field="Cabin Crew" unique_strength="0" exp_strength="0" notnull_strength="0"/>
    <constraint constraints="0" field="Staff" unique_strength="0" exp_strength="0" notnull_strength="0"/>
  </constraints>
  <constraintExpressions>
    <constraint field="Class" desc="" exp=""/>
    <constraint field="Heading" desc="" exp=""/>
    <constraint field="Importance" desc="" exp=""/>
    <constraint field="Pilots" desc="" exp=""/>
    <constraint field="Cabin Crew" desc="" exp=""/>
    <constraint field="Staff" desc="" exp=""/>
  </constraintExpressions>
  <expressionfields/>
  <attributeactions>
    <defaultAction key="Canvas" value="{00000000-0000-0000-0000-000000000000}"/>
  </attributeactions>
  <attributetableconfig actionWidgetStyle="dropDown" sortOrder="0" sortExpression="">
    <columns>
      <column type="field" name="Class" width="-1" hidden="0"/>
      <column type="field" name="Heading" width="-1" hidden="0"/>
      <column type="field" name="Importance" width="-1" hidden="0"/>
      <column type="field" name="Pilots" width="-1" hidden="0"/>
      <column type="field" name="Cabin Crew" width="-1" hidden="0"/>
      <column type="field" name="Staff" width="-1" hidden="0"/>
      <column type="actions" width="-1" hidden="1"/>
    </columns>
  </attributetableconfig>
  <conditionalstyles>
    <rowstyles/>
    <fieldstyles/>
  </conditionalstyles>
  <storedexpressions/>
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
    <attributeEditorContainer visibilityExpressionEnabled="0" columnCount="1" groupBox="0" name="General" showLabel="1" visibilityExpression="">
      <attributeEditorField name="Class" showLabel="1" index="0"/>
      <attributeEditorField name="Heading" showLabel="1" index="1"/>
      <attributeEditorField name="Importance" showLabel="1" index="2"/>
    </attributeEditorContainer>
    <attributeEditorContainer visibilityExpressionEnabled="0" columnCount="1" groupBox="0" name="People" showLabel="1" visibilityExpression="">
      <attributeEditorField name="Pilots" showLabel="1" index="3"/>
      <attributeEditorField name="Cabin Crew" showLabel="1" index="4"/>
      <attributeEditorField name="Staff" showLabel="1" index="5"/>
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
  <widgets/>
  <previewExpression>COALESCE( "Class", '&lt;NULL>' )</previewExpression>
  <mapTip></mapTip>
  <layerGeometryType>0</layerGeometryType>
</qgis>

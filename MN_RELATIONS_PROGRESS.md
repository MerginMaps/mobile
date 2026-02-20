# N-M Relations View-Only Support — Progress

## Goal

Display many-to-many related features in `MMFormRelationEditor` (text/bubble mode) as if it were a simple 1:N relation. The intermediate join table is hidden from the user. This is **view-only** (no creating/editing via the join table).

**Data flow:**
```
Parent feature → (mRelation: parent → join_table) → join features → (mNmRelation: join_table → child) → child features shown in form
```

---

## Tasks

| # | Description | Files | Status | Commit |
|---|-------------|-------|--------|--------|
| 1 | Add `nmRelation` to `FormItem` | `attributedata.h`, `attributedata.cpp` | Pending | — |
| 2 | Handle n-m in `AttributeController` (detect instead of skip) | `attributecontroller.cpp` | Pending | — |
| 3 | Add `NmRelation` model role to `AttributeFormModel` | `attributeformmodel.h`, `attributeformmodel.cpp` | Pending | — |
| 4 | Teach `RelationFeaturesModel` two-step n-m lookup | `relationfeaturesmodel.h`, `relationfeaturesmodel.cpp` | Pending | — |
| 5 | QML: pass nm-relation to editor, hide Add button | `MMFormPage.qml`, `MMFormRelationEditor.qml` | Pending | — |
| 6 | Build verification & runtime test | — | Pending | — |
| 7 | Code formatting pass (`format_cpp.bash`) | — | Pending | — |

---

## Technical Notes

- **QGIS API**: `QgsAttributeEditorRelation::nmRelationId()` (available since QGIS 3.16, project uses 3.40.3)
- **Detection**: Replace widget-config hack with `relationField->nmRelationId()` and look up via `QgsProject::instance()->relationManager()->relation(id)`
- **Two-step lookup in `RelationFeaturesModel`**:
  1. Query join table features using `mRelation.getRelatedFeaturesRequest(parent)`
  2. Extract child FK values (`mNmRelation.fieldPairs().first().referencingField()`)
  3. Filter child layer by `"pk_field" IN (val1, val2, ...)`
- **View-only**: Hide "Add feature" button in `MMFormRelationEditor` when `rmodel.isNmRelation` is true
- **Symmetric**: Works from both sides of the n-m relation (parent→child and child→parent) as QGIS configures n-m widgets per-layer

## Verification Checklist

- [ ] Build compiles without errors
- [ ] 1:N relations still work (no regression)
- [ ] N-M form shows child layer features (not join table features)
- [ ] N-M form from child layer shows parent layer features (symmetric)
- [ ] "Add feature" button is hidden in n-m editors
- [ ] Tapping a bubble opens the correct child/parent feature form
- [ ] `format_cpp.bash` passes cleanly

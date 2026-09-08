/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef TESTATTRIBUTECONTROLLER_H
#define TESTATTRIBUTECONTROLLER_H

#include <QObject>

class TestAttributeController: public QObject
{
    Q_OBJECT
  private slots:
    void init(); // will be called before each testfunction is executed.
    void cleanup(); // will be called after every testfunction.

    void noFields();
    void twoFieldsAutoLayout();
    void twoTabsDragAndDropLayout();
    void twoGroupsDragAndDropLayout();
    void tabsAndFieldsMixed();
    void testValidationMessages();
    void testExpressions();
    void testRawValue();
    void testFieldsOutsideForm();
    void testPhotoRenaming();

    //! A collision suffix must be inserted before the real extension, even if the name has a dot
    void testPhotoRenamingCollisionWithDotInName();

    //! A reused photo must be renamed with the new feature's own expression value, not the old one
    void testPhotoReuseRenamesWithFreshExpressionValue();

    //! A reused photo must not let a field-level Default Value expression overwrite it
    void testDefaultValueDoesNotOverwriteReusedPhoto();

    //! When there's nothing to reuse (empty value), the Default Value expression must still apply
    void testDefaultValueAppliesWhenNothingReused();

    //! Saving the same feature twice must not rename an already-renamed photo again
    void testPhotoRenamingNotRepeatedOnResave();

    //! Discarding a draft feature must delete the clone made for a reused photo
    void testDiscardReusedPhotoCopyOnRollback();

    //! Replacing a reused photo before saving must delete the now-orphaned clone
    void testDiscardReusedPhotoCopyOnReplace();

    //! Reusing a photo must create an independent file, not just copy the path string
    void testReusedPhotoIsIndependentFile();

    void testHtmlAndTextWidgets();
    void testVirtualFields();

    /**
     * Test that if any photo sketches exist, saving form will save them to original photo. Specifically test
     * the sketches play nicely with renaming expressions and metadata gets copied too.
     */
    void testPhotoSketchingSave();

    /**
     * Test that prefillRelationReferenceField sets the rawValue of the FK field in the child controller
     * to the parent feature's referenced field value (FID).
     */
    void testPrefillRelationReferenceField();
};

#endif // TESTATTRIBUTECONTROLLER_H

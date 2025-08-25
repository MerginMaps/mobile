# ########################################################################################
# Mergin Maps Tests
# ########################################################################################

macro (add_mm_test testname)
  add_test(NAME ${testname} COMMAND Input --${testname})
endmacro ()

set(MM_TESTS
    testLinks
    testUtils
    testAttributePreviewController
    testMerginApi
    testAttributeController
    testIdentifyKit
    testPosition
    testRememberAttributesController
    testScaleBarKit
    testFormEditors
    testModels
    testCoreUtils
    testImageUtils
    testMapTools
    testLayerTree
    testActiveProject
    testProjectChecksumCache
    testVariablesManager
    testSketching
)

foreach (test ${MM_TESTS})
  add_mm_test(${test})
endforeach ()

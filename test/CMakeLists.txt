# ########################################################################################
# Mergin Maps Tests
# ########################################################################################

macro (add_mm_test testname)
  set(extra_args ${ARGN})
  list(LENGTH extra_args extra_count)
  if (${extra_count} GREATER 0)
    list(
      JOIN
      extra_args
      " "
      qtest_args
    )
  else ()
    set(qtest_args "")
  endif ()

  add_test(NAME ${testname} COMMAND Input --${testname} ${qtest_args})
endmacro ()

set(MM_TESTS
    testLinks
    testUtils
    testAttributePreviewController
    "testMerginApi -maxwarnings 0"
    testPurchasing
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
)

foreach (test ${MM_TESTS})
  string(
    REPLACE " "
            ";"
            test_args
            ${test}
  )
  add_mm_test(${test_args})
endforeach ()

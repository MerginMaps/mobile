# ########################################################################################
# Mergin Maps Tests
# ########################################################################################

macro (add_mm_test testname)
  set(extra_args ${ARGN})

  # Did we get any optional qtest args?
  list(LENGTH extra_args extra_count)
  if (${extra_count} GREATER 0)
    list(
      GET
      extra_args
      0
      optional_arg
    )
    set(qtest_args ${optional_arg})
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

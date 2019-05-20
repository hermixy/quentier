macro(COLLECT_SOURCES_FOR_TRANSLATION SOURCES FORMS)
  get_property(QUENTIER_TRANSLATABLE_SOURCES GLOBAL PROPERTY QUENTIER_TRANSLATABLE_SOURCES)
  foreach(SOURCE IN LISTS SOURCES)
    list(APPEND QUENTIER_TRANSLATABLE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE}")
  endforeach()
  set_property(GLOBAL PROPERTY QUENTIER_TRANSLATABLE_SOURCES ${QUENTIER_TRANSLATABLE_SOURCES})

  get_property(QUENTIER_TRANSLATABLE_FORMS GLOBAL PROPERTY QUENTIER_TRANSLATABLE_FORMS)
  foreach(FORM IN LISTS FORMS)
    list(APPEND QUENTIER_TRANSLATABLE_FORMS "${CMAKE_CURRENT_SOURCE_DIR}/${FORM}")
  endforeach()
  set_property(GLOBAL PROPERTY QUENTIER_TRANSLATABLE_FORMS ${QUENTIER_TRANSLATABLE_FORMS})
endmacro()
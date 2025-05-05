set(SOURCES
        AbstractAnalysisModule.cpp
        AbstractAnalysisModule.h
        AbstractSynthesisProgramGenerator.cpp
        AbstractSynthesisProgramGenerator.h
        ClassificationAnalysisModule.cpp
        ClassificationAnalysisModule.h
        Constants.h
        DoubleRangeWidget.cpp
        DoubleRangeWidget.h
        ProxyScatterItemModel.cpp
        ProxyScatterItemModel.h
        Range.cpp
        Range.h
        StaticAnalysisModule.cpp
        StaticAnalysisModule.h
        SimulationAnalysisModule.cpp
        SimulationAnalysisModule.h
        SynthesisConfigurator.cpp
        SynthesisConfigurator.h
        SynthesisController.cpp
        SynthesisController.h
        SynthesisModel.cpp
        SynthesisModel.h
        SynthesisPlugin.cpp
        SynthesisPlugin.h
        SynthesisWorker.cpp
        SynthesisWorker.h
        SynthesisWorkerController.cpp
        SynthesisWorkerController.h
        SynthesisEditorWidget.cpp
        SynthesisEditorWidget.h
        SynthesisDocument.cpp
        SynthesisDocument.h
        SynthesisEditor.cpp
        SynthesisEditor.h
        DecomposeTask.cpp
        DecomposeTask.h
        StaticAnalysisModule.cpp
        StaticAnalysisModule.h
        SimulationAnalysisModule.cpp
        SimulationAnalysisModule.h
        ClassificationAnalysisModule.cpp
        ClassificationAnalysisModule.h
)

qt_add_plugin(
        ${TARGET_NAME}_synthesis_plugin STATIC
        ${SOURCES}
        CLASS_NAME SynthesisPlugin
        PLUGIN_TYPE "org.gpne.plugin.synthesis"
)
target_link_libraries(${TARGET_NAME}_synthesis_plugin PUBLIC ${TARGET_NAME}_editor_plugin Qt::Concurrent)
target_include_directories(${TARGET_NAME}_synthesis_plugin PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/..>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
)
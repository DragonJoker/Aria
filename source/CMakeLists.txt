if( APPLE )
	set( GTK_FOUND TRUE )
	set( GTK_LIBRARIES
		"-framework AppKit"
		"-framework QuartzCore"
		"-framework Metal"
	)
elseif ( WIN32 )
	set( GTK_FOUND TRUE )
else()
	find_package( GTK3 QUIET )
	find_package( X11 QUIET )
	if ( GTK3_FOUND )
		set( GTK_FOUND ${GTK3_FOUND} )
		set( GTK_INCLUDE_DIRS ${GTK3_INCLUDE_DIRS} )
		set( GTK_LIBRARIES ${GTK3_LIBRARIES} )
	else ()
		set( GTK_FOUND FALSE )
	endif ()
endif()

if ( MSVC )
	find_package( wxWidgets COMPONENTS core base adv aui stc )
else ()
	find_package( wxWidgets COMPONENTS core base adv aui stc )
endif ()

if ( wxWidgets_FOUND AND GTK_FOUND )
	set( wxCharts_HDR_FILES
		wxCharts/include/wx/charts/wxareachart.h
		wxCharts/include/wx/charts/wxareachartctrl.h
		wxCharts/include/wx/charts/wxareachartdatasetoptions.h
		wxCharts/include/wx/charts/wxareachartoptions.h
		wxCharts/include/wx/charts/wxbarchart.h
		wxCharts/include/wx/charts/wxbarchartctrl.h
		wxCharts/include/wx/charts/wxbarchartdatasetoptions.h
		wxCharts/include/wx/charts/wxbarchartoptions.h
		wxCharts/include/wx/charts/wxboxplot.h
		wxCharts/include/wx/charts/wxboxplotctrl.h
		wxCharts/include/wx/charts/wxboxplotdatasetoptions.h
		wxCharts/include/wx/charts/wxboxplotoptions.h
		wxCharts/include/wx/charts/wxbubblechart.h
		wxCharts/include/wx/charts/wxbubblechartctrl.h
		wxCharts/include/wx/charts/wxbubblechartoptions.h
		wxCharts/include/wx/charts/wxcandlestickchart.h
		wxCharts/include/wx/charts/wxcandlestickchartctrl.h
		wxCharts/include/wx/charts/wxcandlestickchartoptions.h
		wxCharts/include/wx/charts/wxchart.h
		wxCharts/include/wx/charts/wxchartbackground.h
		wxCharts/include/wx/charts/wxchartbackgroundoptions.h
		wxCharts/include/wx/charts/wxchartcommonoptions.h
		wxCharts/include/wx/charts/wxchartctrl.h
		wxCharts/include/wx/charts/wxchartfontoptions.h
		wxCharts/include/wx/charts/wxchartmultitooltip.h
		wxCharts/include/wx/charts/wxchartmultitooltipoptions.h
		wxCharts/include/wx/charts/wxchartobservers.h
		wxCharts/include/wx/charts/wxchartoptions.h
		wxCharts/include/wx/charts/wxchartpadding.h
		wxCharts/include/wx/charts/wxcharts.h
		wxCharts/include/wx/charts/wxchartsarc.h
		wxCharts/include/wx/charts/wxchartsarcoptions.h
		wxCharts/include/wx/charts/wxchartsaxis.h
		wxCharts/include/wx/charts/wxchartsaxisoptions.h
		wxCharts/include/wx/charts/wxchartsbrushoptions.h
		wxCharts/include/wx/charts/wxchartscategoricalaxis.h
		wxCharts/include/wx/charts/wxchartscategoricaldata.h
		wxCharts/include/wx/charts/wxchartscategoricaldataobserver.h
		wxCharts/include/wx/charts/wxchartscircle.h
		wxCharts/include/wx/charts/wxchartscircleoptions.h
		wxCharts/include/wx/charts/wxchartsdataset.h
		wxCharts/include/wx/charts/wxchartsdatasetid.h
		wxCharts/include/wx/charts/wxchartsdatasettheme.h
		wxCharts/include/wx/charts/wxchartsdoubledataset.h
		wxCharts/include/wx/charts/wxchartselement.h
		wxCharts/include/wx/charts/wxchartsgrid.h
		wxCharts/include/wx/charts/wxchartsgridmapping.h
		wxCharts/include/wx/charts/wxchartsgridoptions.h
		wxCharts/include/wx/charts/wxchartslabel.h
		wxCharts/include/wx/charts/wxchartslabelgroup.h
		wxCharts/include/wx/charts/wxchartslabeloptions.h
		wxCharts/include/wx/charts/wxchartslegendctrl.h
		wxCharts/include/wx/charts/wxchartslegenditem.h
		wxCharts/include/wx/charts/wxchartslegendline.h
		wxCharts/include/wx/charts/wxchartslegendlineoptions.h
		wxCharts/include/wx/charts/wxchartslegendoptions.h
		wxCharts/include/wx/charts/wxchartslicedata.h
		wxCharts/include/wx/charts/wxchartsnumericalaxis.h
		wxCharts/include/wx/charts/wxchartsohlcdata.h
		wxCharts/include/wx/charts/wxchartspenoptions.h
		wxCharts/include/wx/charts/wxchartspoint.h
		wxCharts/include/wx/charts/wxchartspointoptions.h
		wxCharts/include/wx/charts/wxchartsprecisiontheme.h
		wxCharts/include/wx/charts/wxchartspresentationtheme.h
		wxCharts/include/wx/charts/wxchartsradialgrid.h
		wxCharts/include/wx/charts/wxchartsradialgridoptions.h
		wxCharts/include/wx/charts/wxchartsrectangle.h
		wxCharts/include/wx/charts/wxchartsrectangleoptions.h
		wxCharts/include/wx/charts/wxchartstheme.h
		wxCharts/include/wx/charts/wxchartsthemefactory.h
		wxCharts/include/wx/charts/wxchartsthemeid.h
		wxCharts/include/wx/charts/wxchartstimeaxis.h
		wxCharts/include/wx/charts/wxchartsutilities.h
		wxCharts/include/wx/charts/wxcharttooltip.h
		wxCharts/include/wx/charts/wxcharttooltipoptions.h
		wxCharts/include/wx/charts/wxcharttooltipprovider.h
		wxCharts/include/wx/charts/wxcolumnchart.h
		wxCharts/include/wx/charts/wxcolumnchartctrl.h
		wxCharts/include/wx/charts/wxcolumnchartdatasetoptions.h
		wxCharts/include/wx/charts/wxcolumnchartoptions.h
		wxCharts/include/wx/charts/wxcombinationchart.h
		wxCharts/include/wx/charts/wxcombinationchartctrl.h
		wxCharts/include/wx/charts/wxcombinationchartoptions.h
		wxCharts/include/wx/charts/wxdoughnutandpiechartbase.h
		wxCharts/include/wx/charts/wxdoughnutandpiechartoptionsbase.h
		wxCharts/include/wx/charts/wxdoughnutchart.h
		wxCharts/include/wx/charts/wxdoughnutchartctrl.h
		wxCharts/include/wx/charts/wxdoughnutchartoptions.h
		wxCharts/include/wx/charts/wxhistogram.h
		wxCharts/include/wx/charts/wxhistogramctrl.h
		wxCharts/include/wx/charts/wxhistogramoptions.h
		wxCharts/include/wx/charts/wxlinechart.h
		wxCharts/include/wx/charts/wxlinechartctrl.h
		wxCharts/include/wx/charts/wxlinechartdatasetoptions.h
		wxCharts/include/wx/charts/wxlinechartoptions.h
		wxCharts/include/wx/charts/wxmath2dplot.h
		wxCharts/include/wx/charts/wxmath2dplotctrl.h
		wxCharts/include/wx/charts/wxmath2dplotoptions.h
		wxCharts/include/wx/charts/wxohlcchart.h
		wxCharts/include/wx/charts/wxohlcchartctrl.h
		wxCharts/include/wx/charts/wxohlcchartoptions.h
		wxCharts/include/wx/charts/wxpiechart.h
		wxCharts/include/wx/charts/wxpiechartctrl.h
		wxCharts/include/wx/charts/wxpiechartoptions.h
		wxCharts/include/wx/charts/wxpolarareachart.h
		wxCharts/include/wx/charts/wxpolarareachartctrl.h
		wxCharts/include/wx/charts/wxpolarareachartoptions.h
		wxCharts/include/wx/charts/wxradarchart.h
		wxCharts/include/wx/charts/wxradarchartctrl.h
		wxCharts/include/wx/charts/wxradarchartoptions.h
		wxCharts/include/wx/charts/wxscatterplot.h
		wxCharts/include/wx/charts/wxscatterplotctrl.h
		wxCharts/include/wx/charts/wxscatterplotoptions.h
		wxCharts/include/wx/charts/wxstackedbarchart.h
		wxCharts/include/wx/charts/wxstackedbarchartctrl.h
		wxCharts/include/wx/charts/wxstackedbarchartdatasetoptions.h
		wxCharts/include/wx/charts/wxstackedbarchartoptions.h
		wxCharts/include/wx/charts/wxstackedcolumnchart.h
		wxCharts/include/wx/charts/wxstackedcolumnchartctrl.h
		wxCharts/include/wx/charts/wxstackedcolumnchartdatasetoptions.h
		wxCharts/include/wx/charts/wxstackedcolumnchartoptions.h
		wxCharts/include/wx/charts/wxtimeserieschart.h
		wxCharts/include/wx/charts/wxtimeserieschartctrl.h
		wxCharts/include/wx/charts/wxtimeserieschartoptions.h
	)
	set( wxCharts_SRC_FILES
		wxCharts/src/wxareachart.cpp
		wxCharts/src/wxareachartctrl.cpp
		wxCharts/src/wxareachartdatasetoptions.cpp
		wxCharts/src/wxareachartoptions.cpp
		wxCharts/src/wxbarchart.cpp
		wxCharts/src/wxbarchartctrl.cpp
		wxCharts/src/wxbarchartdatasetoptions.cpp
		wxCharts/src/wxbarchartoptions.cpp
		wxCharts/src/wxboxplot.cpp
		wxCharts/src/wxboxplotctrl.cpp
		wxCharts/src/wxboxplotdatasetoptions.cpp
		wxCharts/src/wxboxplotoptions.cpp
		wxCharts/src/wxbubblechart.cpp
		wxCharts/src/wxbubblechartctrl.cpp
		wxCharts/src/wxbubblechartoptions.cpp
		wxCharts/src/wxcandlestickchart.cpp
		wxCharts/src/wxcandlestickchartctrl.cpp
		wxCharts/src/wxcandlestickchartoptions.cpp
		wxCharts/src/wxchart.cpp
		wxCharts/src/wxchartbackground.cpp
		wxCharts/src/wxchartbackgroundoptions.cpp
		wxCharts/src/wxchartcommonoptions.cpp
		wxCharts/src/wxchartctrl.cpp
		wxCharts/src/wxchartfontoptions.cpp
		wxCharts/src/wxchartmultitooltip.cpp
		wxCharts/src/wxchartmultitooltipoptions.cpp
		wxCharts/src/wxchartoptions.cpp
		wxCharts/src/wxchartpadding.cpp
		wxCharts/src/wxchartsarc.cpp
		wxCharts/src/wxchartsarcoptions.cpp
		wxCharts/src/wxchartsaxis.cpp
		wxCharts/src/wxchartsaxisoptions.cpp
		wxCharts/src/wxchartsbrushoptions.cpp
		wxCharts/src/wxchartscategoricalaxis.cpp
		wxCharts/src/wxchartscategoricaldata.cpp
		wxCharts/src/wxchartscategoricaldataobserver.cpp
		wxCharts/src/wxchartscircle.cpp
		wxCharts/src/wxchartscircleoptions.cpp
		wxCharts/src/wxchartsdataset.cpp
		wxCharts/src/wxchartsdatasetid.cpp
		wxCharts/src/wxchartsdatasettheme.cpp
		wxCharts/src/wxchartsdoubledataset.cpp
		wxCharts/src/wxchartselement.cpp
		wxCharts/src/wxchartsgrid.cpp
		wxCharts/src/wxchartsgridmapping.cpp
		wxCharts/src/wxchartsgridoptions.cpp
		wxCharts/src/wxchartslabel.cpp
		wxCharts/src/wxchartslabelgroup.cpp
		wxCharts/src/wxchartslabeloptions.cpp
		wxCharts/src/wxchartslegendctrl.cpp
		wxCharts/src/wxchartslegenditem.cpp
		wxCharts/src/wxchartslegendline.cpp
		wxCharts/src/wxchartslegendlineoptions.cpp
		wxCharts/src/wxchartslegendoptions.cpp
		wxCharts/src/wxchartslicedata.cpp
		wxCharts/src/wxchartsnumericalaxis.cpp
		wxCharts/src/wxchartsohlcdata.cpp
		wxCharts/src/wxchartspenoptions.cpp
		wxCharts/src/wxchartspoint.cpp
		wxCharts/src/wxchartspointoptions.cpp
		wxCharts/src/wxchartsprecisiontheme.cpp
		wxCharts/src/wxchartspresentationtheme.cpp
		wxCharts/src/wxchartsradialgrid.cpp
		wxCharts/src/wxchartsradialgridoptions.cpp
		wxCharts/src/wxchartsrectangle.cpp
		wxCharts/src/wxchartsrectangleoptions.cpp
		wxCharts/src/wxchartstheme.cpp
		wxCharts/src/wxchartsthemefactory.cpp
		wxCharts/src/wxchartsthemeid.cpp
		wxCharts/src/wxchartstimeaxis.cpp
		wxCharts/src/wxchartsutilities.cpp
		wxCharts/src/wxcharttooltip.cpp
		wxCharts/src/wxcharttooltipoptions.cpp
		wxCharts/src/wxcharttooltipprovider.cpp
		wxCharts/src/wxcolumnchart.cpp
		wxCharts/src/wxcolumnchartctrl.cpp
		wxCharts/src/wxcolumnchartdatasetoptions.cpp
		wxCharts/src/wxcolumnchartoptions.cpp
		wxCharts/src/wxcombinationchart.cpp
		wxCharts/src/wxcombinationchartctrl.cpp
		wxCharts/src/wxcombinationchartoptions.cpp
		wxCharts/src/wxdoughnutandpiechartbase.cpp
		wxCharts/src/wxdoughnutandpiechartoptionsbase.cpp
		wxCharts/src/wxdoughnutchart.cpp
		wxCharts/src/wxdoughnutchartctrl.cpp
		wxCharts/src/wxdoughnutchartoptions.cpp
		wxCharts/src/wxhistogram.cpp
		wxCharts/src/wxhistogramctrl.cpp
		wxCharts/src/wxhistogramoptions.cpp
		wxCharts/src/wxlinechart.cpp
		wxCharts/src/wxlinechartctrl.cpp
		wxCharts/src/wxlinechartdatasetoptions.cpp
		wxCharts/src/wxlinechartoptions.cpp
		wxCharts/src/wxmath2dplot.cpp
		wxCharts/src/wxmath2dplotctrl.cpp
		wxCharts/src/wxmath2dplotoptions.cpp
		wxCharts/src/wxohlcchart.cpp
		wxCharts/src/wxohlcchartctrl.cpp
		wxCharts/src/wxohlcchartoptions.cpp
		wxCharts/src/wxpiechart.cpp
		wxCharts/src/wxpiechartctrl.cpp
		wxCharts/src/wxpiechartoptions.cpp
		wxCharts/src/wxpolarareachart.cpp
		wxCharts/src/wxpolarareachartctrl.cpp
		wxCharts/src/wxpolarareachartoptions.cpp
		wxCharts/src/wxradarchart.cpp
		wxCharts/src/wxradarchartctrl.cpp
		wxCharts/src/wxradarchartoptions.cpp
		wxCharts/src/wxscatterplot.cpp
		wxCharts/src/wxscatterplotctrl.cpp
		wxCharts/src/wxscatterplotoptions.cpp
		wxCharts/src/wxstackedbarchart.cpp
		wxCharts/src/wxstackedbarchartctrl.cpp
		wxCharts/src/wxstackedbarchartdatasetoptions.cpp
		wxCharts/src/wxstackedbarchartoptions.cpp
		wxCharts/src/wxstackedcolumnchart.cpp
		wxCharts/src/wxstackedcolumnchartctrl.cpp
		wxCharts/src/wxstackedcolumnchartdatasetoptions.cpp
		wxCharts/src/wxstackedcolumnchartoptions.cpp
		wxCharts/src/wxtimeserieschart.cpp
		wxCharts/src/wxtimeserieschartctrl.cpp
		wxCharts/src/wxtimeserieschartoptions.cpp
	)
	source_group( "Header Files"
		FILES
			${wxCharts_HDR_FILES}
	)
	source_group( "Source Files"
		FILES
			${wxCharts_SRC_FILES}
	)
	add_library( wxCharts STATIC
		${wxCharts_HDR_FILES}
		${wxCharts_SRC_FILES}
	)
	add_library( aria::wxCharts
		ALIAS
			wxCharts
	)
	target_include_directories( wxCharts
		PRIVATE
			${CMAKE_CURRENT_SOURCE_DIR}/wxCharts/include/wx/charts
		PUBLIC
			${wxWidgets_INCLUDE_DIRS}
			${GTK_INCLUDE_DIRS}
		INTERFACE
			${CMAKE_CURRENT_SOURCE_DIR}/wxCharts/include
	)

	add_subdirectory( Aria )
else ()
	if ( NOT wxWidgets_FOUND )
		message( SEND_ERROR "no (missing wxWidgets)" )
	endif ()
	if ( NOT GTK2_FOUND )
		message( SEND_ERROR "no (missing gtk2)" )
	endif ()
endif ()
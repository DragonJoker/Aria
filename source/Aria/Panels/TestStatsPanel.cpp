#include "TestStatsPanel.hpp"

#include "DiffImage.hpp"
#include "Database/DatabaseTest.hpp"
#include "Database/TestDatabase.hpp"

#include <wx/sizer.h>
#include <wx/bitmap.h>
#include <wx/combobox.h>
#include <wx/dcclient.h>
#include <wx/filename.h>
#include <wx/charts/wxchartslegendctrl.h>
#include <wx/charts/wxlinechartctrl.h>

namespace aria
{
	namespace
	{
		wxPanel * createPanel( wxWindow * parent
			, wxPoint const & position
			, wxChartsCategoricalData::ptr chartData
			, wxChartsLegendData legendData
			, wxLineChartOptions const & options )
		{
			auto size = parent->GetClientSize();
			size.y /= 2;
			auto result = new wxPanel{ parent, wxID_ANY, position, size };
			result->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
			result->SetForegroundColour( PANEL_FOREGROUND_COLOUR );

			auto legend = new wxChartsLegendCtrl{ result, wxID_ANY, legendData, {}, { 100, size.y } };
			legend->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
			legend->SetForegroundColour( PANEL_FOREGROUND_COLOUR );

			auto chart = new wxLineChartCtrl{ result, wxID_ANY, chartData, wxCHARTSLINETYPE_STRAIGHT, options, { 100, 0 }, { size.x - 100, size.y } };
			chart->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
			chart->SetForegroundColour( PANEL_FOREGROUND_COLOUR );

			wxBoxSizer * sizer{ new wxBoxSizer{ wxHORIZONTAL } };
			sizer->Add( legend, wxSizerFlags{}.Expand() );
			sizer->Add( chart, wxSizerFlags{ 1 }.Expand() );
			result->SetSizerAndFit( sizer );

			return result;
		}
	}

	TestStatsPanel::TestStatsPanel( wxWindow * parent
		, wxWindowID id
		, wxSize const & size
		, TestDatabase & database )
		: wxPanel{ parent, id, {}, size }
		, m_database{ database }
	{
		SetBackgroundColour( BORDER_COLOUR );
		SetForegroundColour( PANEL_FOREGROUND_COLOUR );
	}

	void TestStatsPanel::refresh()
	{
		if ( m_totalPanel )
		{
			RemoveChild( m_totalPanel );
			delete m_totalPanel;
			m_totalPanel = nullptr;

			RemoveChild( m_framePanel );
			delete m_framePanel;
			m_framePanel = nullptr;
		}

		auto & testRun = **m_test;
		auto times = m_database.listTestTimes( *testRun.test, testRun.renderer );
		wxVector< wxDouble > total;
		wxVector< wxDouble > avg;
		wxVector< wxDouble > last;
		wxVector< wxString > cats;

		for ( auto & time : times )
		{
			cats.push_back( time.first.FormatISODate() + " " + time.first.FormatISOTime() );
			total.push_back( time.second.total.count() / 1000.0 );
			avg.push_back( time.second.avg.count() / 1000.0 );
			last.push_back( time.second.last.count() / 1000.0 );
		}

		wxLineChartOptions options;
		auto & gridOptions = options.GetGridOptions();

		auto size = GetClientSize();
		size.y /= 2;
		wxChartsDoubleDataset::ptr totalDataSet( new wxChartsDoubleDataset( _( "Total times" ), total ) );
		auto totalTimesData = wxChartsCategoricalData::make_shared( cats );
		totalTimesData->AddDataset( totalDataSet );
		wxChartsLegendData totalLegendData( totalTimesData->GetDatasets() );
		m_totalPanel = createPanel( this, {}, totalTimesData, totalLegendData, options );

		wxChartsDoubleDataset::ptr avgDataSet( new wxChartsDoubleDataset( _( "Avg. frame times" ), avg ) );
		wxChartsDoubleDataset::ptr lastDataSet( new wxChartsDoubleDataset( _( "Last frame times" ), last ) );
		auto frameTimesData = wxChartsCategoricalData::make_shared( cats );
		frameTimesData->AddDataset( avgDataSet );
		frameTimesData->AddDataset( lastDataSet );
		wxChartsLegendData frameLegendData( frameTimesData->GetDatasets() );
		m_framePanel = createPanel( this, { 0, size.y }, frameTimesData, frameLegendData, options );

		wxBoxSizer * sizer{ new wxBoxSizer{ wxVERTICAL } };
		sizer->Add( m_totalPanel, wxSizerFlags{ 1 }.Border( wxALL, 0 ).Expand() );
		sizer->Add( m_framePanel, wxSizerFlags{ 1 }.Border( wxALL, 0 ).Expand() );
		sizer->SetSizeHints( this );
		SetSizer( sizer );
	}

	void TestStatsPanel::setTest( DatabaseTest & test )
	{
		m_test = &test;
		refresh();
	}
}

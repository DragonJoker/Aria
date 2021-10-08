#include "TestStatsPanel.hpp"

#include "DiffImage.hpp"
#include "Database/DatabaseTest.hpp"
#include "Database/TestDatabase.hpp"

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/charts/wxchartslegendctrl.h>
#include <wx/charts/wxlinechartctrl.h>

namespace aria
{
	namespace
	{
		static constexpr int LabelHeight = 20;
		static constexpr int LegendWidth = 100;

		wxPanel * createPanel( wxWindow * parent
			, wxString const & name
			, wxPoint const & position
			, wxChartsCategoricalData::ptr chartData
			, wxChartsLegendData legendData )
		{
			auto size = parent->GetClientSize();
			size.y /= 2;
			auto result = new wxPanel{ parent, wxID_ANY, position, size };
			result->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
			result->SetForegroundColour( PANEL_FOREGROUND_COLOUR );

			auto labelPanel = new wxPanel{ result, wxID_ANY, {}, { size.x, LabelHeight } };
			labelPanel->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
			labelPanel->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
			auto label = new wxStaticText{ labelPanel, wxID_ANY, name, {}, { size.x - 5, LabelHeight } };
			label->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
			label->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
			wxBoxSizer * labelSizer{ new wxBoxSizer{ wxHORIZONTAL } };
			labelSizer->Add( label, wxSizerFlags{ 1 }.Border( wxLEFT, 5 ).Align( wxALIGN_CENTRE_VERTICAL ) );
			labelPanel->SetSizerAndFit( labelSizer );

			size.y -= LabelHeight;
			auto chartPanel = new wxPanel{ result, wxID_ANY, { 0, LabelHeight }, size };
			chartPanel->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
			chartPanel->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
			auto legend = new wxChartsLegendCtrl{ chartPanel, wxID_ANY, legendData, {}, { LegendWidth, size.y } };
			legend->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
			legend->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
			auto chart = new wxLineChartCtrl{ chartPanel, wxID_ANY, chartData, wxCHARTSLINETYPE_STRAIGHT, { LegendWidth, 0 }, { size.x - LegendWidth, size.y } };
			chart->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
			chart->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
			wxBoxSizer * chartSizer{ new wxBoxSizer{ wxHORIZONTAL } };
			chartSizer->Add( legend, wxSizerFlags{}.Expand() );
			chartSizer->Add( chart, wxSizerFlags{ 1 }.Expand() );
			chartPanel->SetSizerAndFit( chartSizer );

			wxBoxSizer * sizer{ new wxBoxSizer{ wxVERTICAL } };
			sizer->Add( labelPanel, wxSizerFlags{}.Expand() );
			sizer->Add( chartPanel, wxSizerFlags{ 1 }.Expand() );
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

		auto size = GetClientSize();
		size.y /= 2;
		wxChartsDoubleDataset::ptr totalDataSet( new wxChartsDoubleDataset( _( "Total" ), total, " ms" ) );
		auto totalTimesData = wxChartsCategoricalData::make_shared( cats );
		totalTimesData->AddDataset( totalDataSet );
		wxChartsLegendData totalLegendData( totalTimesData->GetDatasets() );
		m_totalPanel = createPanel( this, _( "Total times" ), {}, totalTimesData, totalLegendData );

		wxChartsDoubleDataset::ptr avgDataSet( new wxChartsDoubleDataset( _( "Average" ), avg, " ms" ) );
		wxChartsDoubleDataset::ptr lastDataSet( new wxChartsDoubleDataset( _( "Last" ), last, " ms" ) );
		auto frameTimesData = wxChartsCategoricalData::make_shared( cats );
		frameTimesData->AddDataset( avgDataSet );
		frameTimesData->AddDataset( lastDataSet );
		wxChartsLegendData frameLegendData( frameTimesData->GetDatasets() );
		m_framePanel = createPanel( this, _( "Frame times" ), { 0, size.y }, frameTimesData, frameLegendData );

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
